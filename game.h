#ifndef GAME
#define GAME
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

#ifdef linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

typedef enum {
  BLACK = 0,
  WHITE = 1,
  RED,
  GREEN,
  BLUE,
} Colour;

typedef enum {
  RECT = 1
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

Shape *createRect(float x, float y, float z,
                float w, float h, float l);
Shape *createSphere(float x, float y, float z,
                  float w, float h, float l);
void freeShape(Shape *shape);
void drawRect(Shape *rect);
void drawSphere(Shape *sphere);
void drawShape(Shape *shape);

ShapeList *createShapeList();
void prependShape(ShapeList *shapeList, Shape *shape);
void appendShape(ShapeList *shapeList, Shape *shape);
void getShapeListLength(ShapeList *shapeList);
void insertShapeAtIndex(ShapeList *shapeList, Shape *shape, int i);
void removeShapeAtIndex(ShapeList *shapeList, int i);
void freeShapeList(ShapeList *shapeList);
void drawShapeList(ShapeList *shapeList);

Layer *createLayer();
void freeLayer(Layer *layer);
void drawLayer(Layer *layer);

#endif /* GAME */
