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
    double block_i_x[BLOCK_SIZE];
    double block_i_y[BLOCK_SIZE];
    double block_i_m[BLOCK_SIZE];
    double block_j_x[BLOCK_SIZE];
    double block_j_y[BLOCK_SIZE];
    double block_j_m[BLOCK_SIZE];
    double block_fx[BLOCK_SIZE];
    double block_fy[BLOCK_SIZE];

    for(int ii = 0; ii < N; ii += BLOCK_SIZE){
        const int imax = MIN(ii + BLOCK_SIZE, N);
        const int iblocksize = imax - ii;

        // load block i data into cache
        #pragma GCC ivdep
        for(int k = 0; k < iblocksize; k++) {
            block_i_x[k] = particles[ii + k].pos_dx;
            block_i_y[k] = particles[ii + k].pos_dy;
            block_i_m[k] = particles[ii + k].m_diff;
            block_fx[k] = 0.0;
            block_fy[k] = 0.0;
        }

        for(int i = 0; i < iblocksize; i++){
            const double pos_x_i = block_i_x[i];
            const double pos_y_i = block_i_y[i];
            const double m_i = block_i_m[i];
            double fx_acc = 0.0;
            double fy_acc = 0.0;

            for(int jj = ii + i + 1; jj < N; jj += BLOCK_SIZE){
                const int jmax = MIN(jj + BLOCK_SIZE, N);
                const int jblocksize = jmax - jj;
                // load block j data into cache
                #pragma GCC ivdep
                for(int k = 0; k < jblocksize; k++) {
                    block_j_x[k] = particles[jj + k].pos_dx;
                    block_j_y[k] = particles[jj + k].pos_dy;
                    block_j_m[k] = particles[jj + k].m_diff;
                }

                // process interactions between blocks
                #pragma GCC ivdep
                for(int j = 0; j < jblocksize; j++){
                    const double dx = pos_x_i - particles[j].pos_dx;
                    const double dy = pos_y_i - particles[j].pos_dy;
                    const double r = dx * dx + dy * dy;
                    const double denom = sqrt(r) + epsilon;
                    const double d3 = denom * denom * denom;
                    const double inv_r3 = 1.0 / d3;
                    const double Fxij = dx * inv_r3;
                    const double Fyij = dy * inv_r3;

                    // calculate forces
                    const double fx_j = m_i * Fxij;
                    const double fy_j = m_i * Fyij;
                    const double fx_i = block_j_m[j] * Fxij;
                    const double fy_i = block_j_m[j] * Fyij;

                    // accumulate forces
                    fx_acc += fx_i;
                    fy_acc += fy_i;

                    // update forces for particle j directly
                    particles[jj + j].Fx -= fx_j;
                    particles[jj + j].Fy -= fy_j;
                }
            }

            // update accumulated forces for particle i
            block_fx[i] += fx_acc;
            block_fy[i] += fy_acc;
        }

        // write back accumulated forces for block i
        #pragma GCC ivdep
        for(int k = 0; k < iblocksize; k++) {
            particles[ii + k].Fx += block_fx[k];
            particles[ii + k].Fy += block_fy[k];
        }
    }

    // update velocities and positions
    #pragma GCC ivdep
    for (int i = 0; i < N; i++) {
        particles[i].vel_dx += Gdelta * particles[i].Fx;
        particles[i].vel_dy += Gdelta * particles[i].Fy;
        particles[i].pos_dx += delta_t * particles[i].vel_dx;
        particles[i].pos_dy += delta_t * particles[i].vel_dy;
        particles[i].Fx = 0.0;
        particles[i].Fy = 0.0;
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

    for (int i = 0; i < N; i++){
        particles[i].pos_dx = buffer[i*6+0];
        particles[i].pos_dy = buffer[i*6+1];
        particles[i].m_diff = buffer[i*6+2];
        particles[i].vel_dx = buffer[i*6+3];
        particles[i].vel_dy = buffer[i*6+4];
        particles[i].b_diff = buffer[i*6+5];
        particles[i].Fx = 0.0;
        particles[i].Fy = 0.0;
    }

    if (graphics == 1){
        InitializeGraphics((char*)argv[0],windowWidth,windowWidth);
        SetCAxes(0,1);

        for (int k = 0; k < nsteps; k++){
            ClearScreen();
            for (int i = 0; i < N; i++){
                DrawCircle(particles[i].pos_dx, particles[i].pos_dy, L, W, circleRadius, circleColor);
            }
            Refresh();
            calculateParticleMotion(particles, N, delta_t);
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

    for (int i = 0; i < N; i++){
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
