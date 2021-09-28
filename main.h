#ifndef __MAIN_H
#define __MAIN_H

#include <SDL2/SDL.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define false 0
#define true 1
#define bool int

#define ROTATION_SPEED (0.15)
#define SELECT_DISTANCE (20)
#define ADJUST_TOLERANCE (0.0001)

#define REPLICAS (2)
#define ALTERNATE_REPLICAS

typedef struct{
    double x,y,z;
}point;

typedef struct{
    point p;
    int i;
    double t;
}endpoint;

typedef struct{
    endpoint *p1,*p2;
    point k;
    int type;
}line;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern int wins;

extern char *filename;

void loadDiagram(const char *filename,bool loadView);
void saveDiagram(const char *filename);

#include "mathematics.h"
#include "graph.h"
#include "lines.h"
#include "diagrams.h"
#include "trace.h"
#include "render.h"
#include "auxiliary.h"
#include "events.h"

#endif
