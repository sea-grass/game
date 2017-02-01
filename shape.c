#include "game.h"

void freeShape(Shape *shape) {
  free(shape->values);
  free(shape);
}

Shape *createRect(float x, float y, float z, float w, float h, float l) {
  Shape *rect;

  rect = malloc(sizeof(Shape));

  rect->type = RECT;

  rect->values = malloc(sizeof(float) * 6);

  rect->values[0] = x;
  rect->values[1] = y;
  rect->values[2] = z;
  rect->values[3] = w;
  rect->values[4] = h;
  rect->values[5] = l;
  return rect;
}

Shape *createTriangle(float x, float y, float z, float w, float h, float l) {
  Shape *triangle;

  triangle = malloc(sizeof(Shape));

  triangle->type = TRIANGLE;
  triangle->values = malloc(sizeof(float) * 6);

  triangle->values[0] = x;
  triangle->values[1] = y;
  triangle->values[2] = z;
  triangle->values[3] = w;
  triangle->values[4] = h;
  triangle->values[5] = l;
  
  return triangle;
}

void setTriangleValues(Shape *t, float x, float y, float z, float w, float h) {
  t->values[0] = x;
  t->values[1] = y;
  t->values[2] = z;
  t->values[3] = w;
  t->values[4] = h;
}
void getTriangleValues(Shape *t, float *x, float *y, float *z, float *w, float *h) {
  if (x != NULL) *x = t->values[0];
  if (y != NULL) *y = t->values[1];
  if (z != NULL) *z = t->values[2];
  if (w != NULL) *w = t->values[3];
  if (h != NULL) *h = t->values[4];
}
void drawTriangle(Shape *t) {
  float points[3][3];
  float x, y, z, w, h;
  getTriangleValues(t, &x, &y, &z, &w, &h);

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
    glVertex3f(points[0][0], points[0][1], points[0][2]);
    glVertex3f(points[1][0], points[1][1], points[1][2]);
    glVertex3f(points[2][0], points[2][1], points[2][2]);
  glEnd();
}
void printTriangle(Shape *triangle) {
  float x, y, z;
  if (triangle == NULL || triangle->type != TRIANGLE) return;

  getTriangleValues(triangle, &x, &y, &z, NULL, NULL);

  fprintf(stderr, "t: %2.3lf, %2.3lf, %2.3lf\n", x, y, z);
}
void drawShape(Shape *shape) {
  if (shape == NULL) return;

  switch (shape->type) {
    case TRIANGLE:
      drawTriangle(shape);
      break;
    case RECT:
      //drawRect(shape);
      break;
    case SPHERE:
      //drawSphere(shape);
      break;
  }
}

void translateShape(Shape *shape, float x, float y, float z) {
  if (shape == NULL || shape->values == NULL) return;

  shape->values[0] += x;
  shape->values[1] += y;
  shape->values[2] += z;
}
