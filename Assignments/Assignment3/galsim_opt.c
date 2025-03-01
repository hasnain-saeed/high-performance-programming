#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct ParticleSystem {
    double* pos_dx;
    double* pos_dy;
    double* m_diff;
    double* vel_dx;
    double* vel_dy;
    double* b_diff;
    double* tmp_fx;
    double* tmp_fy;
    int n_particles;
} particle_system_t;

const double epsilon = 1e-3;

void calculateParticleMotion(particle_system_t* system, double delta) {
    const int N = system->n_particles;
    const double Gdelta = -100.0 * delta / N;

    memset(system->tmp_fx, 0.0, N*sizeof(double));
    memset(system->tmp_fy, 0.0, N*sizeof(double));

    for (int i = 0; i < N; i++){
        const double pos_x_i = system->pos_dx[i];
        const double pos_y_i = system->pos_dy[i];
        const double m_i = system->m_diff[i];
        double tmp_fx_i = 0.0;
        double tmp_fy_i = 0.0;

        #pragma GCC ivdep
        for (int j = i + 1; j < N; j++) {
            const double dx = pos_x_i - system->pos_dx[j];
            const double dy = pos_y_i - system->pos_dy[j];
            const double r2 = dx * dx + dy * dy;
            const double denom = sqrt(r2) + epsilon;
            const double d3 = denom * denom * denom;
            const double inv_r3 = 1.0 / d3;
            const double fx = dx * inv_r3;
            const double fy = dy * inv_r3;
            tmp_fx_i += system->m_diff[j] * fx;
            tmp_fy_i += system->m_diff[j] * fy;
            system->tmp_fx[j] -= m_i * fx;
            system->tmp_fy[j] -= m_i * fy;
        }
        system->tmp_fx[i] += tmp_fx_i;
        system->tmp_fy[i] += tmp_fy_i;
    }

    // update velocities and positions.
    #pragma GCC ivdep
    for (int i = 0; i < N; i++){
        system->vel_dx[i] += Gdelta * system->tmp_fx[i];
        system->vel_dy[i] += Gdelta * system->tmp_fy[i];
        system->pos_dx[i] += delta * system->vel_dx[i];
        system->pos_dy[i] += delta * system->vel_dy[i];
    }
}

void create_particle_system(particle_system_t* system, int N) {
    system->n_particles = N;
    system->pos_dx = (double*)malloc(N * sizeof(double));
    system->pos_dy = (double*)malloc(N * sizeof(double));
    system->m_diff = (double*)malloc(N * sizeof(double));
    system->vel_dx = (double*)malloc(N * sizeof(double));
    system->vel_dy = (double*)malloc(N * sizeof(double));
    system->b_diff = (double*)malloc(N * sizeof(double));
    system->tmp_fx = (double*)malloc(N * sizeof(double));
    system->tmp_fy = (double*)malloc(N * sizeof(double));
}

void free_particle_system(particle_system_t* system) {
    free(system->pos_dx);
    free(system->pos_dy);
    free(system->m_diff);
    free(system->vel_dx);
    free(system->vel_dy);
    free(system->b_diff);
    free(system->tmp_fx);
    free(system->tmp_fy);
}

int main(int argc, const char* argv[]) {
    if (argc != 6) {
        printf("Usage: %s N filename nsteps delta_t graphics\n", argv[0]);
        return -1;
    }

    const int N = atoi(argv[1]);
    const char* filename = argv[2];
    const int nsteps = atoi(argv[3]);
    const double delta = atof(argv[4]);
    const char graphics = atoi(argv[5]);

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
    if (graphics == 0){
        for (int k = 0; k < nsteps; k++) {
            calculateParticleMotion(&system, delta);
        }
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
