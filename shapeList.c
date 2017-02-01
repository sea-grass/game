#include "game.h"

ShapeList *createShapeList() {
  ShapeList *shapeList;

  shapeList = malloc(sizeof(ShapeList));
  shapeList->shape = NULL; /* root node always has a null shape */
  shapeList->next = NULL;
  return shapeList;
}
void appendShapeList(ShapeList *shapeList, Shape *shape) {
  ShapeList *curr;
  ShapeList *node;

  curr = shapeList;
  if (curr == NULL) return;

  while (curr->next != NULL) {
    curr = curr->next;
  }

  node = malloc(sizeof(ShapeList));
  node->shape = shape;
  node->next = NULL;

  curr->next = node;
}
void addShapeList(ShapeList *shapeList, Shape *shape) {
  appendShapeList(shapeList, shape);
}

/* frees shapeList nodes recursively, as it goes along */
void freeShapeList(ShapeList *shapeList) {
  ShapeList *curr = NULL, *next = NULL;
  if (shapeList == NULL) return;

  curr = shapeList;
  if (curr->shape != NULL) {
    freeShape(curr->shape);
  }
  next = curr->next;
  free(shapeList);
  freeShapeList(next);
}

void drawShapeList(ShapeList *shapeList) {
  ShapeList *curr;
  if (shapeList == NULL) return;

  curr = shapeList;
  if (curr->shape != NULL) drawShape(curr->shape);
  drawShapeList(curr->next);
}
