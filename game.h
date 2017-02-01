#ifndef GAME
#define GAME

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

typedef enum {
  BLACK = 0,
  WHITE = 1,
  RED,
  GREEN,
  BLUE,
} Colour;

typedef enum {
  RECT = 1,
  SPHERE,
  TRIANGLE
} ShapeType;

typedef struct {
  ShapeType type;
  float *values;
  Colour colour;
} Shape;

typedef struct ShapeList {
  Shape *shape;
  struct ShapeList *next;
} ShapeList;

typedef struct {
  ShapeList *shapeList;
} Layer;


/* *********** */
/* implemented */
/* *********** */
ShapeList *createShapeList();
void freeShapeList(ShapeList *shapeList);
void appendShape(ShapeList *shapeList, Shape *shape);
void addShapeList(ShapeList *shapeList, Shape *shape);
void drawShapeList(ShapeList *shapeList);

Shape *createRect(float x, float y, float z,
                float w, float h, float l);
Shape *createTriangle(float x, float y, float z,
                      float w, float h, float l);
/* drawTriangle will draw triangle to the screen */
void drawTriangle(Shape *triangle);
void drawShape(Shape *shape);
void freeShape(Shape *shape);
void getTriangleValues(Shape *t, float *x, float *y, float *z, float *w, float *h);
void setTriangleValues(Shape *t, float x, float y, float z, float w, float h);
/* translate will add x, y, z to shape's x, y, and z */
void translateShape(Shape *shape, float x, float y, float z);
/* ******************* */
/* not implemented yet */
/* ******************* */
void prependShape(ShapeList *shapeList, Shape *shape);
void getShapeListLength(ShapeList *shapeList);
void insertShapeAtIndex(ShapeList *shapeList, Shape *shape, int i);
void removeShapeAtIndex(ShapeList *shapeList, int i);

Layer *createLayer();
void freeLayer(Layer *layer);
void drawLayer(Layer *layer);

Shape *createSphere(float x, float y, float z,
                  float w, float h, float l);
void drawRect(Shape *rect);
void drawSphere(Shape *sphere);
#endif /* GAME */
