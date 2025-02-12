#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "graphics/graphics.h"

typedef struct Particle
{
    double pos_dx;
    double pos_dy;
    double m_diff;
    double vel_dx;
    double vel_dy;
    double b_diff;
} particle_t;

const float epsilon = 1e-3, circleRadius=0.005, circleColor=0, L=1, W=1;
const int windowWidth=800;

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

void keep_within_box(double* xA, double* yA) {
  if(*xA > 1)
    *xA = 0;
  if(*yA > 1)
    *yA = 0;
}

int read_doubles_from_file(int n, double* p, const char* fileName) {
  /* Open input file and determine its size. */
  FILE* input_file = fopen(fileName, "rb");
  if(!input_file) {
    printf("read_doubles_from_file error: failed to open input file '%s'.\n", fileName);
    return -1;
  }
  /* Get filesize using fseek() and ftell(). */
  fseek(input_file, 0L, SEEK_END);
  size_t fileSize = ftell(input_file);
  /* Now use fseek() again to set file position back to beginning of the file. */
  fseek(input_file, 0L, SEEK_SET);
  if(fileSize != n * sizeof(double)) {
    printf("read_doubles_from_file error: size of input file '%s' does not match the given n.\n", fileName);
    printf("For n = %d the file size is expected to be (n * sizeof(double)) = %lu but the actual file size is %lu.\n",
	   n, n * sizeof(double), fileSize);
    return -1;
  }
  /* Read contents of input_file into buffer. */
  size_t noOfItemsRead = fread(p, sizeof(char), fileSize, input_file);
  if(noOfItemsRead != fileSize) {
    printf("read_doubles_from_file error: failed to read file contents into buffer.\n");
    return -1;
  }
  /* OK, now we have the file contents in the buffer.
     Since we are finished with the input file, we can close it now. */
  if(fclose(input_file) != 0) {
    printf("read_doubles_from_file error: error closing input file.\n");
    return -1;
  }
  /* Return 0 to indicate success. */
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

void calculateParticleMotion(particle_t* particles, int N, int nsteps, float delta_t){
    float G = -100/N;
    double Gm, rij, rij_x, rij_y,factor, Fx, Fy, ax, ay;
    for (int k = 0; k < nsteps; k++){
        for (int i = 0; i < N; i++){
            Gm = G * particles[i].m_diff;
            Fx = 0,Fy = 0;
            for (int j = 0; j < N; j++){
                if (i==j) continue;
                rij_x = particles[i].pos_dx - particles[j].pos_dx;
                rij_y = particles[i].pos_dy - particles[j].pos_dy;
                rij = sqrt(rij_x*rij_x + rij_y*rij_y) + epsilon;
                factor = particles[j].m_diff/(rij*rij*rij);
                Fx += rij_x*factor;
                Fy += rij_y*factor;
            }
            ax = Fx*Gm/particles[i].m_diff;
            ay = Fy*Gm/particles[i].m_diff;
            particles[i].vel_dx += delta_t*ax;
            particles[i].vel_dy += delta_t*ay;
            particles[i].pos_dx += delta_t*particles[i].vel_dx;
            particles[i].pos_dy += delta_t*particles[i].vel_dy;
        }
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

        printf("Hit q to quit.\n");
        while(!CheckForQuit()) {
            /* Call graphics routines. */
            ClearScreen();
            for (size_t i = 0; i < N; i++){
                DrawCircle(particles[i].pos_dx, particles[i].pos_dy, L, W, circleRadius, circleColor);
            }
            Refresh();
            calculateParticleMotion(particles, N, nsteps, delta_t);
            /* Sleep a short while to avoid screen flickering. */
            usleep(3000);
        }
        FlushDisplay();
        CloseDisplay();
    }
    double startTime = get_wall_seconds();
    calculateParticleMotion(particles, N, nsteps, delta_t);
    double secondsTaken = get_wall_seconds() - startTime;
    printf("secondsTaken = %f\n", secondsTaken);


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
