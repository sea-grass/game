#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include "game.h"
#include <pthread.h>
#include <string.h>

#define GRAV 9.81

/* dx, dy, dz cannot be updated without first locking updatePosition */
pthread_mutex_t updatePosition;
float dx = 0, dy = 0, dz = 0;

/* vpx, vpy, vpz - viewpoint coordinates in the world */
float vpx = 0.0, vpy = 0.0, vpz = 0.0;

/* worker watches dx, dy, dz for changes and updates player coordinates */
pthread_t worker,
          debug;
pthread_attr_t attr;

/* a list of shapes to be drawn to screen */
ShapeList *shapeList;
Shape *triangle;

int width = 0, height = 0;

/* helper function to get time in seconds, with milliseconds */
float startTime;
float gettime();

/* helper function to sleep for less than a second */
void sleepfor(float s);

typedef enum {
  KEY_UP = 0,
  KEY_DOWN
} KeyEventType;
enum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  CTRL = 4,
  SPACE = 5
};
/* keys is used to determine the state of the arrow keys at any time */
KeyEventType keys[6] = { KEY_UP, KEY_UP, KEY_UP, KEY_UP, KEY_UP, KEY_UP };

void initThreads(void);
void initGlut(void);
void init(void);

void reshape(int w, int h);
void display(void);
void update(void);
void keyboard(int key, int x, int y);
void _keyboard(unsigned char key, int x, int y);
void keyboardUp(int key, int x, int y);
void _keyboardUp(unsigned char key, int x, int y);

/* the worker thread */
void *Worker(void *_data);
void *Debug(void *_data);

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Hello world");  
  glutSpecialFunc(keyboard);
  glutKeyboardFunc(_keyboard);
  glutSpecialUpFunc(keyboardUp);
  glutKeyboardUpFunc(_keyboardUp);
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

/* sleep for less than a second, hurrah! */
void sleepfor(float s) {
  struct timespec tim, tim2;

  tim.tv_sec = (int) s;
  s = s - tim.tv_sec;
  // note: not sure if I calculated this right... come back later
  tim.tv_nsec = s * (10^9);

  nanosleep(&tim, &tim2);
}

void *Worker(void *_data) {
  float tid;
  float prevtime, currtime;
  float dt;
  float _dx = 0.0, _dy = 0.0, _dz = 0.0;
  float dif = 0.9;

  tid = *((float*)(_data));
  fprintf(stderr, "I am worker %2lf ready to update the triangle!\n", tid);

  while (1) {
    currtime = gettime();
    if (prevtime == 0) prevtime = currtime;
    dt = currtime - prevtime;

    if (_dx != 0 || _dy != 0 || _dx != 0) _dx = _dy = _dz = 0;
    pthread_mutex_lock(&updatePosition);
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
    /* if there are changes in dx or dy, absorb them and scale by time elapsed */
    if (dx != 0) {
      _dx = dx * dt;
      dx = 0.0;
    }
    if (dy != 0) {
      _dy = dy * dt;
      dy = 0.0;
    }

    pthread_mutex_unlock(&updatePosition);

    //fprintf(stderr, "translate(%2.3lf, %2.3lf, %2.3lf)\n", _dx, _dy, _dz);
    translateShape(triangle, _dx, _dy, _dz);

    prevtime = currtime;
    sleepfor(0.1); /* sleep for 0.1 seconds */
  }
  pthread_exit(NULL);
}

void *Debug(void *_data)  {
  float tid;
  char *buf;
  char *shape, *x, *y, *w, *h;

  tid = *((float*)(_data));
  buf = malloc(sizeof(char)*1000);

  fprintf(stdout, "Hello, I am the debugger %2.3lf!\n", tid);

  while ((fgets(buf, 999, stdin)) != NULL) {
    /* remove the newline */
    buf[strlen(buf) - 1] = 0;

    if (strcmp(buf, "help") == 0) {
      fprintf(stdout, "Help\n----\n<shape> <x>,<y> <w> <h>\n");
    } else {
      /* tokenize by spaces */
      shape = strtok_r(buf, " ", &buf);
      x = strtok_r(NULL, ",", &buf);
      y = strtok_r(NULL, " ", &buf);
      w = strtok_r(NULL, " ", &buf);
      h = strtok_r(NULL, " ", &buf);
      if (x != NULL && y != NULL && w != NULL && h != NULL) {
        fprintf(stdout, "We got %s: (%s,%s) (%s,%s)\n", shape, x, y, w, h);
        addShapeList(shapeList, createTriangle(atof(x), atof(y), -10.0,
                                               atof(w), atof(h), 0.0));
      }
    }

    fprintf(stdout, "%s is funny\n", buf);
  }

  fprintf(stdout, "Done!\n");

  free(buf);
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
    case ' ':
      keys[SPACE] = KEY_DOWN;
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
    case ' ':
      keys[SPACE] = KEY_UP;
      break;
  }
}
void _keyboard(unsigned char key, int x, int y) {
  keyboard((int)key, x, y);
}
void _keyboardUp(unsigned char key, int x, int y) {
  keyboardUp((int)key, x, y);
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
  rc = pthread_create(&debug, &attr, Debug, (void *)&tid);
  if (rc != 0) {
    fprintf(stderr, "Uh oh...\n");
  }
}

void initGlut() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
}

void init() {
  shapeList = createShapeList();
  triangle = createTriangle(0.0, 0.0, -10.0, 1.0, 1.0, 0.0);
  addShapeList(shapeList, triangle);

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

  drawShapeList(shapeList);

  glutSwapBuffers();
}
void update() {
  float y;

  getTriangleValues(triangle, NULL, &y, NULL, NULL, NULL);
  if (y > -2.0) {
    pthread_mutex_lock(&updatePosition);
    dy += -GRAV; /* gravity LOL */
    pthread_mutex_unlock(&updatePosition);
  }

  glutPostRedisplay();
}
