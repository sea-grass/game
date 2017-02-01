#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <GL/glut.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

pthread_t worker;
pthread_attr_t attr;
pthread_mutex_t updatePosition;
float dx = 0, dy = 0, dz = 0;

int width = 0, height = 0;
float startTime;
float gettime();

float triangle[5] = { 0.0, 0.0, -10.0, 1.0, 1.0 };
void translate(float x, float y, float z);
void getTriangleValues(float* x, float* y, float* z, float* w, float* h);
void printTriangle(void) {
  float x, y, z;
  getTriangleValues(&x, &y, &z, NULL, NULL);

  fprintf(stderr, "t: %2.3lf, %2.3lf, %2.3lf\n", x, y, z);
}

typedef enum {
  KEY_UP = 0,
  KEY_DOWN
} KeyEventType;
typedef struct {
  unsigned char key;
  KeyEventType type;
} KeyEvent;
typedef struct {
  KeyEvent *keys;
} KeyEventList;

enum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3
};
KeyEventType keys[4] = { KEY_UP, KEY_UP, KEY_UP, KEY_UP };
//KeyEventType keys[126 - 32];

void initThreads(void);
void initGlut(void);
void init(void);
void reshape(int w, int h);
void display(void);
void update(void);
void keyboard(int key, int x, int y);
void keyboardUp(int key, int x, int y);
void pushKey(unsigned char key, float time);
void drawTriangle();
void updateTriangle(float p1[3], float p2[3], float p3[3]);
void *Worker(void *_data);
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Hello world");  
  glutSpecialFunc(keyboard);
  glutSpecialUpFunc(keyboardUp);
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(update);

  init();
  glutMainLoop();
  return 0;
};

float gettime() {
  struct timeval now;

  gettimeofday(&now, NULL);
  if (startTime == 0)
    startTime = (float)(now.tv_sec + (float)now.tv_usec/1000000.0);
  return (float)(now.tv_sec + (float)now.tv_usec/1000000.0 - startTime);
}
void *Worker(void *_data) {
  float tid;
  float prevtime, currtime;
  float dt;
  float _dx = 0.0, _dy = 0.0, _dz = 0.0;
  float dif = 0.9;
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = 100000000L;

  tid = *((float*)(_data));
  fprintf(stderr, "I am worker %2lf ready to update the triangle!\n", tid);

  while (1) {
    currtime = gettime();
    if (prevtime == 0) prevtime = currtime;
    dt = currtime - prevtime;

    pthread_mutex_lock(&updatePosition);
    if (_dx != 0 || _dy != 0 || _dx != 0) _dx = _dy = _dz = 0;
    if (keys[UP] == KEY_DOWN) {
      dy += dif;
    }
    if (keys[DOWN] == KEY_DOWN) {
      dy += -dif;
    }
    if (keys[LEFT] == KEY_DOWN) {
      dx += -dif;
    }
    if (keys[RIGHT] == KEY_DOWN) {
      dx += dif;
    }
    if (dx != 0) {
      _dx = dx * dt;
      fprintf(stderr, "dy is: %lf", dx);
      dx = 0.0;
      fprintf(stderr, " and then %lf\n", dx);
      //fprintf(stderr, "Moving %lf hori, %lf\n", _dx, dt);
    }
    if (dy != 0) {
      _dy = dy * dt;
      fprintf(stderr, "dy is: %lf", dy);
      dy = 0.0;
      fprintf(stderr, " and then %lf\n", dy);
      //fprintf(stderr, "Moving %lf vert, %lf\n", _dy, dt);
    }
    pthread_mutex_unlock(&updatePosition);

    //fprintf(stderr, "Something about TIME: %ld from %ld\n", currtime, prevtime);
    //fprintf(stderr, "translate(%2.3lf, %2.3lf, %2.3lf)\n", _dx, _dy, _dz);
    translate(_dx, _dy, _dz);

    prevtime = currtime;
    nanosleep(&tim, &tim2);
  }
  pthread_exit(NULL);
}
void keyboard(int key, int x, int y) {

  switch(key) {
    case GLUT_KEY_UP:
      keys[UP] = KEY_DOWN;
      break;
    case GLUT_KEY_DOWN:
      keys[DOWN] = KEY_DOWN;
      break;
    case GLUT_KEY_LEFT:
      keys[LEFT] = KEY_DOWN;
      break;
    case GLUT_KEY_RIGHT:
      keys[RIGHT] = KEY_DOWN;
      break;
  }
}
void keyboardUp(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP:
      keys[UP] = KEY_UP;
      break;
    case GLUT_KEY_DOWN:
      keys[DOWN] = KEY_UP;
      break;
    case GLUT_KEY_LEFT:
      keys[LEFT] = KEY_UP;
      break;
    case GLUT_KEY_RIGHT:
      keys[RIGHT] = KEY_UP;
      break;
  }
}

void translate(float x, float y, float z) {
  triangle[0] += x;
  triangle[1] += y;
  triangle[2] += z;
}

void initThreads() {
  int rc; /* return code */
  float tid = 0;
  pthread_attr_init(&attr);
  //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&updatePosition, NULL);
  rc = pthread_create(&worker, &attr, Worker, (void *)&tid);
  if (rc != 0) {
    fprintf(stderr, "Uh oh...\n");
  }
  fprintf(stderr, "Created worker %lf\n", tid);
}
void initGlut() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
}
void init() {
  initThreads();
  initGlut();
}

void reshape(int w, int h) {
  width = w;
  height = h;
  if (height < width) width = height;
  if (width < height) height = width;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.1,
            0.1,
            (float)(-h) / (10.0 * (float)w),
            (float)h / (10.0 * (float)w),
            0.5,
            1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  //drawShapeList();
  drawTriangle();
/*
  glBegin(GL_TRIANGLES); //Begin triangle coordinates
  //Triangle
  glVertex3f(x[0][0], x[0][1], x[0][2]);
  glVertex3f(-1.0f, 1.5f, -5.0f);
  glVertex3f(-1.5f, 0.5f, -5.0f);
  glEnd(); //End triangle coordinates
*/
  glutSwapBuffers();
}
void update() {
  float y;
  printTriangle();
  fprintf(stderr, "w: %3d, h: %3d\n", width, height);

  getTriangleValues(NULL, &y, NULL, NULL, NULL);
  if (y > -2.0) {
    pthread_mutex_lock(&updatePosition);
    dy += -0.1; /* gravity LOL */
    pthread_mutex_unlock(&updatePosition);
  }
  pthread_mutex_lock(&updatePosition);
  //fprintf(stderr, "dx: %2.3lf, dy %2.3lf, dz %2.3lf\n", dx, dy, dz);
  pthread_mutex_unlock(&updatePosition);
  glutPostRedisplay();
}
void setTriangleValues(float x, float y, float z, float w, float h) {
  triangle[0] = x;
  triangle[1] = y;
  triangle[2] = z;
  triangle[3] = w;
  triangle[4] = h;
}
void getTriangleValues(float *x, float *y, float *z, float *w, float *h) {
  if (x != NULL) *x = triangle[0];
  if (y != NULL) *y = triangle[1];
  if (z != NULL) *z = triangle[2];
  if (w != NULL) *w = triangle[3];
  if (h != NULL) *h = triangle[4];
}
void drawTriangle() {
  int i;
  float points[3][3];
  float x, y, z, w, h;
  getTriangleValues(&x, &y, &z, &w, &h);

  points[0][0] = x;
  points[0][1] = y;
  points[0][2] = z;
  points[1][0] = x + (w / 2);
  points[1][1] = y + h;
  points[1][2] = z;
  points[2][0] = x + w;
  points[2][1] = y;
  points[2][2] = z;

  glBegin(GL_TRIANGLES);
  for (i = 0; i < 3; i++) {
    glVertex3f(points[i][0], points[i][1], points[i][2]);
  }
  glEnd();
}
