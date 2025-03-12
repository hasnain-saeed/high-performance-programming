#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

const double epsilon = 1e-3;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nthreads;
    int waiting;
    int state;
} barrier_t;

typedef struct ParticleSystem {
    double* pos_dx;
    double* pos_dy;
    double* m_diff;
    double* vel_dx;
    double* vel_dy;
    double* b_diff;
    int n_particles;
} particle_system_t;

typedef struct thread_data thread_data_t;

typedef struct {
    particle_system_t* system;
    int nsteps;
    double delta;
    int nthreads;
    barrier_t barrier;
    volatile int next_index;
    pthread_mutex_t next_index_mutex;
    thread_data_t* thread_args;
} simulation_context_t;

struct thread_data {
    int thread_id;
    simulation_context_t* context;
    double* thread_tmp_fx;
    double* thread_tmp_fy;
};

void barrier_init(barrier_t *barrier, int nthreads) {
    barrier->nthreads = nthreads;
    barrier->waiting = 0;
    barrier->state = 0;
    pthread_mutex_init(&(barrier->mutex), NULL);
    pthread_cond_init(&(barrier->cond), NULL);
}

void barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&(barrier->mutex));
    int mystate = barrier->state;
    barrier->waiting++;
    if (barrier->waiting == barrier->nthreads) {
        barrier->state = 1-mystate;
        barrier->waiting = 0;
        pthread_cond_broadcast(&(barrier->cond));
    }
    while (mystate == barrier->state) {
        pthread_cond_wait(&(barrier->cond), &(barrier->mutex));
    }
    pthread_mutex_unlock(&(barrier->mutex));
}

void barrier_destroy(barrier_t *barrier) {
    pthread_mutex_destroy(&(barrier->mutex));
    pthread_cond_destroy(&(barrier->cond));
}

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
    return seconds;
}

void* galsim_worker(void* arg) {
    thread_data_t* tdata = (thread_data_t*) arg;
    simulation_context_t* context = tdata->context;
    particle_system_t* system = context->system;
    int nsteps = context->nsteps;
    int nthreads = context->nthreads;
    int N = system->n_particles;
    double delta = context->delta;
    const double Gdelta = -100.0*delta/N;
    int tid = tdata->thread_id;

    int start = (N*tid)/nthreads;
    int end = (N*(tid+1))/nthreads;

    const int chunk = 1;

    for (int step=0; step<nsteps; step++) {

        memset(tdata->thread_tmp_fx, 0.0, N*sizeof(double));
        memset(tdata->thread_tmp_fy, 0.0, N*sizeof(double));

        if (tid == 0) {
            context->next_index = 0;
        }
        barrier_wait(&context->barrier);

        while (1) {
            int i_start, i_end;
            // each thread picks up chunk size iterations and proceeds to accumulate forces
            // in their corresponding local force arrays.
            pthread_mutex_lock(&context->next_index_mutex);
            i_start = context->next_index;
            i_end = i_start + chunk;
            if (i_end > N) {
                i_end = N;
            }
            context->next_index = i_end;
            pthread_mutex_unlock(&context->next_index_mutex);
            if (i_start >= N)
                break;

            for (int i=i_start; i<i_end; i++){

                double pos_x_i = system->pos_dx[i];
                double pos_y_i = system->pos_dy[i];
                double m_i = system->m_diff[i];
                double tmp_fx_i = 0.0;
                double tmp_fy_i = 0.0;

                #pragma GCC ivdep
                for (int j = i + 1; j < N; j++) {
                    double dx = pos_x_i - system->pos_dx[j];
                    double dy = pos_y_i - system->pos_dy[j];
                    double r2 = dx * dx + dy * dy;
                    double denom = sqrt(r2) + epsilon;
                    double d3 = denom * denom * denom;
                    double inv_r3 = 1.0 / d3;
                    double fx = dx * inv_r3;
                    double fy = dy * inv_r3;
                    tmp_fx_i += system->m_diff[j] * fx;
                    tmp_fy_i += system->m_diff[j] * fy;
                    tdata->thread_tmp_fx[j] -= m_i * fx;
                    tdata->thread_tmp_fy[j] -= m_i * fy;
                }
                tdata->thread_tmp_fx[i] += tmp_fx_i;
                tdata->thread_tmp_fy[i] += tmp_fy_i;
            }
        }
        barrier_wait(&context->barrier);

        for (int i = start; i < end; i++) {
            double sum_fx = 0.0, sum_fy = 0.0;
            for (int t = 0; t < nthreads; t++) {
                sum_fx += context->thread_args[t].thread_tmp_fx[i];
                sum_fy += context->thread_args[t].thread_tmp_fy[i];
            }
            system->vel_dx[i] += Gdelta * sum_fx;
            system->vel_dy[i] += Gdelta * sum_fy;
            system->pos_dx[i] += delta * system->vel_dx[i];
            system->pos_dy[i] += delta * system->vel_dy[i];
        }
        barrier_wait(&context->barrier);
    }
    return NULL;
}

void create_particle_system(particle_system_t* system, int N) {
    system->n_particles = N;
    system->pos_dx = (double*)malloc(N * sizeof(double));
    system->pos_dy = (double*)malloc(N * sizeof(double));
    system->m_diff = (double*)malloc(N * sizeof(double));
    system->vel_dx = (double*)malloc(N * sizeof(double));
    system->vel_dy = (double*)malloc(N * sizeof(double));
    system->b_diff = (double*)malloc(N * sizeof(double));
}

void free_particle_system(particle_system_t* system) {
    free(system->pos_dx);
    free(system->pos_dy);
    free(system->m_diff);
    free(system->vel_dx);
    free(system->vel_dy);
    free(system->b_diff);
}

int main(int argc, const char* argv[]) {
    if (argc != 7) {
        printf("Usage: %s N filename nsteps delta_t graphics nthreads\n", argv[0]);
        return -1;
    }
    const int N = atoi(argv[1]);
    const char* filename = argv[2];
    const int nsteps = atoi(argv[3]);
    const double delta = atof(argv[4]);
    const int graphics = atoi(argv[5]);
    const int nthreads = atoi(argv[6]);

    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening %s\n", filename);
        return -1;
    }
    double* buffer = malloc(6 * N * sizeof(double));
    if (fread(buffer, sizeof(double), 6 * N, file) != 6 * N) {
        printf("Error reading data\n");
        fclose(file);
        free(buffer);
        return -1;
    }
    fclose(file);

    particle_system_t system;
    create_particle_system(&system, N);
    for (int i = 0; i < N; i++) {
        system.pos_dx[i] = buffer[i*6+0];
        system.pos_dy[i] = buffer[i*6+1];
        system.m_diff[i] = buffer[i*6+2];
        system.vel_dx[i] = buffer[i*6+3];
        system.vel_dy[i] = buffer[i*6+4];
        system.b_diff[i] = buffer[i*6+5];
    }

    if (graphics == 0) {
        double startTime = get_wall_seconds();

        simulation_context_t context;
        context.system = &system;
        context.nsteps = nsteps;
        context.delta  = delta;
        context.nthreads = nthreads;
        context.next_index = 0;
        context.thread_args = malloc(nthreads * sizeof(thread_data_t));
        pthread_mutex_init(&context.next_index_mutex, NULL);
        barrier_init(&context.barrier, nthreads);
        pthread_t* threads = malloc(nthreads * sizeof(pthread_t));

        for (int t=0; t<nthreads; t++) {
            context.thread_args[t].thread_id = t;
            context.thread_args[t].context = &context;
            context.thread_args[t].thread_tmp_fx = malloc(N * sizeof(double));
            context.thread_args[t].thread_tmp_fy = malloc(N * sizeof(double));
            pthread_create(&threads[t], NULL, galsim_worker, (void*)&context.thread_args[t]);
        }

        for (int t = 0; t < nthreads; t++) {
            pthread_join(threads[t], NULL);
        }

        barrier_destroy(&context.barrier);
        pthread_mutex_destroy(&context.next_index_mutex);
        for (int t = 0; t < nthreads; t++) {
            free(context.thread_args[t].thread_tmp_fx);
            free(context.thread_args[t].thread_tmp_fy);
        }
        free(context.thread_args);
        free(threads);
        double secondsTaken = get_wall_seconds() - startTime;
        printf("normal secondsTaken = %f\n", secondsTaken);
    }

    for (int i = 0; i < N; i++) {
        buffer[i*6+0] = system.pos_dx[i];
        buffer[i*6+1] = system.pos_dy[i];
        buffer[i*6+2] = system.m_diff[i];
        buffer[i*6+3] = system.vel_dx[i];
        buffer[i*6+4] = system.vel_dy[i];
        buffer[i*6+5] = system.b_diff[i];
    }
    file = fopen("result.gal", "wb");
    if (!file || fwrite(buffer, sizeof(double), 6 * N, file) != 6 * N) {
        printf("Error writing file\n");
    }
    if (file) fclose(file);

    free(buffer);
    free_particle_system(&system);
    return 0;
}
