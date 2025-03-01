#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "graphics/graphics.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct Particle
{
    double pos_dx;
    double pos_dy;
    double m_diff;
    double vel_dx;
    double vel_dy;
    double b_diff;
} particle_t;

typedef struct {
    double * restrict pos_dx;  // Array of x positions
    double * restrict pos_dy;  // Array of y positions
    double * restrict vel_dx;  // Array of x velocities
    double * restrict vel_dy;  // Array of y velocities
    double * restrict m_diff;  // Array of mass differences (or masses)
    double * restrict b_diff;
    double * restrict Fx;
    double * restrict Fy;
}  particles_soa_t;


const float epsilon = 1e-3, circleRadius=0.004, circleColor=0, L=1, W=1;
const int windowWidth=800;

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

int read_doubles_from_file(int n, double* p, const char* fileName) {
  FILE* input_file = fopen(fileName, "rb");
  if(!input_file) {
    printf("read_doubles_from_file error: failed to open input file '%s'.\n", fileName);
    return -1;
  }
  fseek(input_file, 0L, SEEK_END);
  size_t fileSize = ftell(input_file);
  fseek(input_file, 0L, SEEK_SET);
  if(fileSize != n * sizeof(double)) {
    printf("read_doubles_from_file error: size of input file '%s' does not match the given n.\n", fileName);
    printf("For n = %d the file size is expected to be (n * sizeof(double)) = %lu but the actual file size is %lu.\n",
	   n, n * sizeof(double), fileSize);
    return -1;
  }

  size_t noOfItemsRead = fread(p, sizeof(char), fileSize, input_file);
  if(noOfItemsRead != fileSize) {
    printf("read_doubles_from_file error: failed to read file contents into buffer.\n");
    return -1;
  }

  if(fclose(input_file) != 0) {
    printf("read_doubles_from_file error: error closing input file.\n");
    return -1;
  }
  return 0;
}

int write_doubles_to_file(int n, double* p, const char* fileName) {
    FILE* output_file = fopen(fileName, "wb");
    if(!output_file) {
        printf("failed to open file in write byte mode\n");
        return -1;
    }
    size_t bytes_size = n * sizeof(double);
    size_t bytes_written = fwrite(p, sizeof(char), bytes_size, output_file);
    if (bytes_written != bytes_size){
        printf("failed to write buffer content to file\n");
        return -1;
    }
    if(fclose(output_file) != 0) {
        printf("failed to close output file.\n");
        return -1;
    }
    return 0;
}

void calculateParticleMotion(particle_t* particles, int N, float delta_t){
    const float G = -100.0f/N;
    int i, j;
    for (i = 0; i < N; i++){
        double pos_x_i = particles[i].pos_dx;
        double pos_y_i = particles[i].pos_dy;
        double Fx = 0;
        double Fy = 0;
        for (j = 0; j < N; j++){
            if (i==j) continue;

            double r_x = pos_x_i - particles[j].pos_dx;
            double r_y = pos_y_i - particles[j].pos_dy;
            double r_squared = r_x*r_x + r_y*r_y;
            double rij = sqrt(r_squared) + epsilon;
            double inv_dist3 = 1.0 / (rij * rij * rij);
            double factor = particles[j].m_diff * inv_dist3;
            Fx += r_x * factor;
            Fy += r_y * factor;
        }
        particles[i].vel_dx += delta_t*Fx*G;
        particles[i].vel_dy += delta_t*Fy*G;
    }

    for (i = 0; i < N; i++){
        particles[i].pos_dx += delta_t*particles[i].vel_dx;
        particles[i].pos_dy += delta_t*particles[i].vel_dy;
    }
}

// void calculateParticleMotionOptimized(particle_t* particles, int N, float delta_t) {
//     // The gravitational constant factor multiplied by delta_t.
//     const double Gdelta = (-100.0 * delta_t) / N;
//     // Block size tuned to your cache (adjust as needed)
//     const int BLOCK_SIZE = 32;

//     // Loop over blocks of i indices.
//     for (int ib = 0; ib < N; ib += BLOCK_SIZE) {
//         int i_end = (ib + BLOCK_SIZE < N) ? (ib + BLOCK_SIZE) : N;
//         int i_count = i_end - ib;

//         // Temporary buffers for the current i-block.
//         double iPosX[BLOCK_SIZE], iPosY[BLOCK_SIZE];
//         double iForceX[BLOCK_SIZE], iForceY[BLOCK_SIZE];

//         // Load positions for the i-block and clear force accumulators.
//         for (int i = 0; i < i_count; i++) {
//             iPosX[i]   = particles[ib + i].pos_dx;
//             iPosY[i]   = particles[ib + i].pos_dy;
//             iForceX[i] = 0.0;
//             iForceY[i] = 0.0;
//         }

//         // Loop over all j blocks.
//         for (int jb = 0; jb < N; jb += BLOCK_SIZE) {
//             int j_end = (jb + BLOCK_SIZE < N) ? (jb + BLOCK_SIZE) : N;
//             int j_count = j_end - jb;

//             // Temporary buffers for the current j-block.
//             double jPosX[BLOCK_SIZE], jPosY[BLOCK_SIZE], jMass[BLOCK_SIZE];

//             // Load j-block data.
//             for (int j = 0; j < j_count; j++) {
//                 jPosX[j] = particles[jb + j].pos_dx;
//                 jPosY[j] = particles[jb + j].pos_dy;
//                 jMass[j] = particles[jb + j].m_diff;
//             }

//             // If the i- and j-blocks are the same, we must skip self-interaction.
//             if (ib == jb) {
//                 for (int i = 0; i < i_count; i++) {
//                     double ix = iPosX[i];
//                     double iy = iPosY[i];
//                     double fx = 0.0, fy = 0.0;
//                     for (int j = 0; j < j_count; j++) {
//                         if (i == j) continue;  // skip self-interaction
//                         double dx = ix - jPosX[j];
//                         double dy = iy - jPosY[j];
//                         double rsq = dx * dx + dy * dy;
//                         double inv_r = 1.0 / (sqrt(rsq) + epsilon);
//                         double inv_r3 = inv_r * inv_r * inv_r;
//                         double factor = jMass[j] * inv_r3;
//                         fx += dx * factor;
//                         fy += dy * factor;
//                     }
//                     iForceX[i] += fx;
//                     iForceY[i] += fy;
//                 }
//             } else {
//                 // For different i- and j-blocks we can avoid the self-check.
//                 for (int i = 0; i < i_count; i++) {
//                     double ix = iPosX[i];
//                     double iy = iPosY[i];
//                     double fx = 0.0, fy = 0.0;
//                     for (int j = 0; j < j_count; j++) {
//                         double dx = ix - jPosX[j];
//                         double dy = iy - jPosY[j];
//                         double rsq = dx * dx + dy * dy;
//                         double inv_r = 1.0 / (sqrt(rsq) + epsilon);
//                         double inv_r3 = inv_r * inv_r * inv_r;
//                         double factor = jMass[j] * inv_r3;
//                         fx += dx * factor;
//                         fy += dy * factor;
//                     }
//                     iForceX[i] += fx;
//                     iForceY[i] += fy;
//                 }
//             }
//         }
//         // Now update the particles in the i-block.
//         for (int i = 0; i < i_count; i++) {
//             int idx = ib + i;
//             particles[idx].vel_dx += Gdelta * iForceX[i];
//             particles[idx].vel_dy += Gdelta * iForceY[i];
//             particles[idx].pos_dx += delta_t * particles[idx].vel_dx;
//             particles[idx].pos_dy += delta_t * particles[idx].vel_dy;
//         }
//     }
// }

// void calculateParticleMotionOptimized2(particle_t* particles, int N, float delta_t) {
//     // Precompute the gravitational constant scaled by delta_t.
//     const double Gdelta = (-100.0 * delta_t) / N;

//     // Choose a block size that is expected to fit in L1 or L2 cache.
//     const int BLOCK_SIZE = 64;  // Tune this parameter to your hardware.

//     // Loop over blocks for the "i" index.
//     for (int ib = 0; ib < N; ib += BLOCK_SIZE) {
//         int i_end = (ib + BLOCK_SIZE < N) ? (ib + BLOCK_SIZE) : N;
//         int i_count = i_end - ib;

//         // Temporary arrays to hold positions and force accumulators for the i-block.
//         double iPosX[BLOCK_SIZE], iPosY[BLOCK_SIZE];
//         double iForceX[BLOCK_SIZE], iForceY[BLOCK_SIZE];

//         // Load i-block data and initialize force accumulators.
//         for (int i = 0; i < i_count; i++) {
//             iPosX[i]   = particles[ib + i].pos_dx;
//             iPosY[i]   = particles[ib + i].pos_dy;
//             iForceX[i] = 0.0;
//             iForceY[i] = 0.0;
//         }

//         // Loop over blocks for the "j" index.
//         for (int jb = 0; jb < N; jb += BLOCK_SIZE) {
//             int j_end = (jb + BLOCK_SIZE < N) ? (jb + BLOCK_SIZE) : N;
//             int j_count = j_end - jb;

//             // Temporary arrays to hold positions and masses for the j-block.
//             double jPosX[BLOCK_SIZE], jPosY[BLOCK_SIZE], jMass[BLOCK_SIZE];

//             // Load j-block data.
//             for (int j = 0; j < j_count; j++) {
//                 jPosX[j] = particles[jb + j].pos_dx;
//                 jPosY[j] = particles[jb + j].pos_dy;
//                 jMass[j] = particles[jb + j].m_diff;
//             }

//             // Compute interactions between the current i-block and j-block.
//             for (int i = 0; i < i_count; i++) {
//                 double ix = iPosX[i];
//                 double iy = iPosY[i];
//                 double fx = 0.0, fy = 0.0;

//                 for (int j = 0; j < j_count; j++) {
//                     // For the same block, skip self-interaction.
//                     if (ib == jb && i == j)
//                         continue;

//                     double dx = ix - jPosX[j];
//                     double dy = iy - jPosY[j];
//                     double rsq = dx * dx + dy * dy;

//                     // Compute the inverse distance cubed.
//                     double inv_r = 1.0 / (sqrt(rsq) + epsilon);
//                     double inv_r3 = inv_r * inv_r * inv_r;

//                     double factor = jMass[j] * inv_r3;

//                     fx += dx * factor;
//                     fy += dy * factor;
//                 }
//                 // Accumulate forces from all j-blocks.
//                 iForceX[i] += fx;
//                 iForceY[i] += fy;
//             }
//         }

//         // Update the velocities and positions for the particles in the i-block.
//         for (int i = 0; i < i_count; i++) {
//             int idx = ib + i;
//             particles[idx].vel_dx += Gdelta * iForceX[i];
//             particles[idx].vel_dy += Gdelta * iForceY[i];
//             particles[idx].pos_dx += delta_t * particles[idx].vel_dx;
//             particles[idx].pos_dy += delta_t * particles[idx].vel_dy;
//         }
//     }
// }

void calculateParticleMotionOptimizedNoBuffer(particle_t* particles, int N, float delta_t) {
    const double Gdelta = (-100.0 * delta_t) / N;
    const int BLOCK_SIZE = 64;  // Tune this based on your cache size

    // Process force computations in blocks.
    for (int ib = 0; ib < N; ib += BLOCK_SIZE) {
        int i_end = (ib + BLOCK_SIZE < N) ? (ib + BLOCK_SIZE) : N;

        for (int i = ib; i < i_end; i++) {
            double pos_x_i = particles[i].pos_dx;
            double pos_y_i = particles[i].pos_dy;
            double Fx = 0.0, Fy = 0.0;

            // Process j in blocks.
            for (int jb = 0; jb < N; jb += BLOCK_SIZE) {
                int j_end = (jb + BLOCK_SIZE < N) ? (jb + BLOCK_SIZE) : N;

                for (int j = jb; j < j_end; j++) {
                    if (i == j)
                        continue;  // Skip self-interaction

                    double dx = pos_x_i - particles[j].pos_dx;
                    double dy = pos_y_i - particles[j].pos_dy;
                    double rsq = dx * dx + dy * dy;
                    double inv_r = 1.0 / (sqrt(rsq) + epsilon);
                    double inv_r3 = inv_r * inv_r * inv_r;
                    double factor = particles[j].m_diff * inv_r3;

                    Fx += dx * factor;
                    Fy += dy * factor;
                }
            }
            // Accumulate computed force into velocity.
            particles[i].vel_dx += Gdelta * Fx;
            particles[i].vel_dy += Gdelta * Fy;
        }
    }

    // Update positions using the new velocities.
    for (int i = 0; i < N; i++) {
        particles[i].pos_dx += delta_t * particles[i].vel_dx;
        particles[i].pos_dy += delta_t * particles[i].vel_dy;
    }
}


void calculateParticleMotionOptimizedSoA(particles_soa_t * restrict particles, int N, float delta_t) {
    const double Gdelta = (-100.0 * delta_t) / N;
    const int BLOCK_SIZE = 64;  // Adjust based on your cache size

    // Compute forces and update velocities.
    for (int ib = 0; ib < N; ib += BLOCK_SIZE) {
        int i_end = MIN(ib + BLOCK_SIZE, N);
        for (int i = ib; i < i_end; i++) {
            double pos_x_i = particles->pos_dx[i];
            double pos_y_i = particles->pos_dy[i];
            double Fx = 0.0, Fy = 0.0;

            // Process j in blocks.
            for (int jb = 0; jb < N; jb += BLOCK_SIZE) {
                int j_end = MIN(jb + BLOCK_SIZE, N);
                for (int j = jb; j < j_end; j++) {
                    if (i == j)
                        continue;  // Skip self-interaction

                    double dx = pos_x_i - particles->pos_dx[j];
                    double dy = pos_y_i - particles->pos_dy[j];
                    double rsq = dx * dx + dy * dy;
                    double inv_r = 1.0 / (sqrt(rsq) + epsilon);
                    double inv_r3 = inv_r * inv_r * inv_r;
                    double factor = particles->m_diff[j] * inv_r3;
                    Fx += dx * factor;
                    Fy += dy * factor;
                }
            }
            particles->vel_dx[i] += Gdelta * Fx;
            particles->vel_dy[i] += Gdelta * Fy;
        }
    }

    // Update positions using the new velocities.
    for (int i = 0; i < N; i++) {
        particles->pos_dx[i] += delta_t * particles->vel_dx[i];
        particles->pos_dy[i] += delta_t * particles->vel_dy[i];
    }
}

void calculateParticleMotionOptimizedSoA1(particles_soa_t * restrict particles, int N, float delta_t) {
    // Here we use the gravitational constant as given. (The negative sign accounts for the attractive force.)
    const double Gdelta = (-100.0 * delta_t) / N;
    const int BLOCK_SIZE = 64;  // Adjust based on your cache size

    for (int i = 0; i < N; i++) {
        particles->Fx[i] = 0.0;
        particles->Fy[i] = 0.0;
    }

    // Use two-level blocking and process each unique pair (i,j) with j > i.
    for (int ib = 0; ib < N; ib += BLOCK_SIZE) {
        int i_end = MIN(ib + BLOCK_SIZE, N);
        for (int i = ib; i < i_end; i++) {
            double pos_x_i = particles->pos_dx[i];
            double pos_y_i = particles->pos_dy[i];
            // Loop over j in blocks, starting from i+1 to avoid self-interaction.
            for (int jb = i + 1; jb < N; jb += BLOCK_SIZE) {
                int j_end = MIN(jb + BLOCK_SIZE, N);
                for (int j = jb; j < j_end; j++) {
                    double dx = pos_x_i - particles->pos_dx[j];
                    double dy = pos_y_i - particles->pos_dy[j];
                    double r = sqrt(dx * dx + dy * dy);
                    double denom = r + epsilon;
                    double inv_r3 = 1.0 / (denom * denom * denom);

                    // Compute the pairwise factor.
                    double Fxij = dx * inv_r3;
                    double Fyij = dy * inv_r3;

                    // Update force on particle i with contribution from particle j.
                    particles->Fx[i] += particles->m_diff[j] * Fxij;
                    particles->Fy[i] += particles->m_diff[j] * Fyij;

                    // Update force on particle j (opposite sign, scaled by m[i]).
                    particles->Fx[j] -= particles->m_diff[i] * Fxij;
                    particles->Fy[j] -= particles->m_diff[i] * Fyij;
                }
            }
        }
    }

    // Update velocities and positions using the accumulated forces.
    for (int i = 0; i < N; i++) {
        particles->vel_dx[i] += Gdelta * particles->Fx[i];
        particles->vel_dy[i] += Gdelta * particles->Fy[i];
        particles->pos_dx[i] += delta_t * particles->vel_dx[i];
        particles->pos_dy[i] += delta_t * particles->vel_dy[i];
    }

}

int main(int argc, const char* argv[]) {
    int N, nsteps;
    const char* filename;
    char graphics;
    float delta_t;
    particle_t * particles;
    particles_soa_t particles_a;

    if (argc != 6) {
        printf("Give 5 input args: N filename nsteps delta_t graphics\n");
        return -1;
    }

    N = atoi(argv[1]);
    filename = argv[2];
    nsteps = atoi(argv[3]);
    delta_t = atof(argv[4]);
    graphics = (char)atoi(argv[5]);

    double buffer[6*N];
    if(read_doubles_from_file(6*N, buffer, filename) != 0){
        printf("Error reading file '%s'\n", filename);
        return -1;
    }
    particles = (particle_t*)malloc(sizeof(particle_t)*N);
    particles_a.pos_dx = (double*)malloc(sizeof(double)*N);
    particles_a.pos_dy = (double*)malloc(sizeof(double)*N);
    particles_a.vel_dx = (double*)malloc(sizeof(double)*N);
    particles_a.vel_dy = (double*)malloc(sizeof(double)*N);
    particles_a.m_diff = (double*)malloc(sizeof(double)*N);
    particles_a.b_diff = (double*)malloc(sizeof(double)*N);
    particles_a.Fx = (double *) malloc(N * sizeof(double));
    particles_a.Fy = (double *) malloc(N * sizeof(double));

    for (size_t i = 0; i < N; i++){
        particles[i].pos_dx = buffer[i*6+0];
        particles_a.pos_dx[i] = buffer[i*6+0];
        particles[i].pos_dy = buffer[i*6+1];
        particles_a.pos_dy[i] = buffer[i*6+1];
        particles[i].m_diff = buffer[i*6+2];
        particles_a.m_diff[i] = buffer[i*6+2];
        particles[i].vel_dx = buffer[i*6+3];
        particles_a.vel_dx[i] = buffer[i*6+3];
        particles[i].vel_dy = buffer[i*6+4];
        particles_a.vel_dy[i] = buffer[i*6+4];
        particles[i].b_diff = buffer[i*6+5];
        particles_a.b_diff[i] = buffer[i*6+5];
    }

    if (graphics == 1){
        InitializeGraphics((char*)argv[0],windowWidth,windowWidth);
        SetCAxes(0,1);

        for (int k = 0; k < nsteps; k++){
            /* Call graphics routines. */
            ClearScreen();
            for (int i = 0; i < N; i++){
                DrawCircle(particles[i].pos_dx, particles[i].pos_dy, L, W, circleRadius, circleColor);
            }
            Refresh();
            calculateParticleMotion(particles, N, delta_t);
            /* Sleep a short while to avoid screen flickering. */
            usleep(3000);
        }
        FlushDisplay();
        CloseDisplay();
    }
    else{
        double startTime = get_wall_seconds();
        for (int k = 0; k < nsteps; k++){
            calculateParticleMotion(particles, N, delta_t);
        }
        double secondsTaken = get_wall_seconds() - startTime;
        printf("normal secondsTaken = %f\n", secondsTaken);
        startTime = get_wall_seconds();
        for (int k = 0; k < nsteps; k++){
            calculateParticleMotionOptimizedSoA(&particles_a, N, delta_t);
        }
        secondsTaken = get_wall_seconds() - startTime;
        printf("optimized SOA secondsTaken = %f\n", secondsTaken);
        startTime = get_wall_seconds();
        for (int k = 0; k < nsteps; k++){
            calculateParticleMotionOptimizedSoA1(&particles_a, N, delta_t);
        }
        secondsTaken = get_wall_seconds() - startTime;
        printf("optimized SOA1 secondsTaken = %f\n", secondsTaken);
    }

    for (size_t i = 0; i < N; i++){
        buffer[i*6+0] = particles[i].pos_dx;
        buffer[i*6+1] = particles[i].pos_dy;
        buffer[i*6+2] = particles[i].m_diff;
        buffer[i*6+3] = particles[i].vel_dx;
        buffer[i*6+4] = particles[i].vel_dy;
        buffer[i*6+5] = particles[i].b_diff;
    }

    for (size_t i = 0; i < N; i++){
        buffer[i*6+0] = particles_a.pos_dx[i];
        buffer[i*6+1] = particles_a.pos_dy[i];
        buffer[i*6+2] = particles_a.m_diff[i];
        buffer[i*6+3] = particles_a.vel_dx[i];
        buffer[i*6+4] = particles_a.vel_dy[i];
        buffer[i*6+5] = particles_a.b_diff[i];
    }

    if(write_doubles_to_file(6*N, buffer, "result.gal")!= 0){
        printf("Error writing file '%s'\n", "result.gal");
    }
    free(particles);
    free(particles_a.m_diff);
    free(particles_a.pos_dx);
    free(particles_a.pos_dy);
    free(particles_a.vel_dx);
    free(particles_a.vel_dy);
    free(particles_a.Fx);
    free(particles_a.Fy);

    return 0;
}
