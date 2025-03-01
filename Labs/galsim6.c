#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "graphics/graphics.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 64
#endif

typedef struct Particle
{
    double pos_dx;
    double pos_dy;
    double m_diff;
    double vel_dx;
    double vel_dy;
    double b_diff;
    double Fx;
    double Fy;
} particle_t;

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
    const double Gdelta = (-100.0 * delta_t) / N;

    // Initialize forces
    #pragma GCC ivdep
    for (int i = 0; i < N; i++) {
        particles[i].Fx = 0.0;
        particles[i].Fy = 0.0;
    }

    // Only buffer the most frequently accessed data
    double block_fx[BLOCK_SIZE];
    double block_fy[BLOCK_SIZE];

    for(int ii = 0; ii < N; ii += BLOCK_SIZE){
        const int imax = MIN(ii + BLOCK_SIZE, N);

        // Zero out block forces
        #pragma GCC ivdep
        for(int k = 0; k < (imax - ii); k++) {
            block_fx[k] = 0.0;
            block_fy[k] = 0.0;
        }

        for (int i = ii; i < imax; i++){
            const double pos_x_i = particles[i].pos_dx;
            const double pos_y_i = particles[i].pos_dy;
            const double m_i = particles[i].m_diff;
            double fx_acc = 0.0;
            double fy_acc = 0.0;

            for(int jj = i + 1; jj < N; jj += BLOCK_SIZE){
                const int jmax = MIN(jj + BLOCK_SIZE, N);

                #pragma GCC ivdep
                for (int j = jj; j < jmax; j++){
                    const double dx = pos_x_i - particles[j].pos_dx;
                    const double dy = pos_y_i - particles[j].pos_dy;
                    const double r = sqrt(dx * dx + dy * dy);
                    const double denom = r + epsilon;
                    const double inv_r3 = 1.0 / (denom * denom * denom);
                    const double Fxij = dx * inv_r3;
                    const double Fyij = dy * inv_r3;

                    const double fx_j = m_i * Fxij;
                    const double fy_j = m_i * Fyij;
                    const double fx_i = particles[j].m_diff * Fxij;
                    const double fy_i = particles[j].m_diff * Fyij;

                    fx_acc += fx_i;
                    fy_acc += fy_i;
                    particles[j].Fx -= fx_j;
                    particles[j].Fy -= fy_j;
                }
            }

            // Accumulate forces for block i
            const int local_i = i - ii;
            block_fx[local_i] += fx_acc;
            block_fy[local_i] += fy_acc;
        }

        // Write back accumulated forces
        #pragma GCC ivdep
        for(int k = 0; k < (imax - ii); k++) {
            particles[ii + k].Fx += block_fx[k];
            particles[ii + k].Fy += block_fy[k];
        }
    }

    // Update velocities and positions
    #pragma GCC ivdep
    for (int i = 0; i < N; i++) {
        particles[i].vel_dx += Gdelta * particles[i].Fx;
        particles[i].vel_dy += Gdelta * particles[i].Fy;
        particles[i].pos_dx += delta_t * particles[i].vel_dx;
        particles[i].pos_dy += delta_t * particles[i].vel_dy;
    }
}

int main(int argc, const char* argv[]) {
    int N, nsteps;
    const char* filename;
    char graphics;
    float delta_t;
    particle_t * particles;

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

    for (size_t i = 0; i < N; i++){
        particles[i].pos_dx = buffer[i*6+0];
        particles[i].pos_dy = buffer[i*6+1];
        particles[i].m_diff = buffer[i*6+2];
        particles[i].vel_dx = buffer[i*6+3];
        particles[i].vel_dy = buffer[i*6+4];
        particles[i].b_diff = buffer[i*6+5];
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
        printf("secondsTaken = %f\n", secondsTaken);
    }

    for (size_t i = 0; i < N; i++){
        buffer[i*6+0] = particles[i].pos_dx;
        buffer[i*6+1] = particles[i].pos_dy;
        buffer[i*6+2] = particles[i].m_diff;
        buffer[i*6+3] = particles[i].vel_dx;
        buffer[i*6+4] = particles[i].vel_dy;
        buffer[i*6+5] = particles[i].b_diff;
    }

    if(write_doubles_to_file(6*N, buffer, "result.gal")!= 0){
        printf("Error writing file '%s'\n", "result.gal");
    }
    free(particles);

    return 0;
}
