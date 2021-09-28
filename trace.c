#include "main.h"

void traceInit(trace *t,point a,point b,point c){
    normalVector(a,b,&t->k);
    copyPoint(a,&t->s);
    t->a=angleU(a,c);
    t->m=t->a/M_PI*grain; if(t->m<1)t->m=1;
    t->i=-1;
}

void traceInitAngle(trace *t,point a,point b,double angle1,double angle2){
    normalVector(a,b,&t->k);
    copyPoint(a,&t->s);
    rotateAroundAxis(t->k,angle1,&t->s);
    normalize(&t->s);
    t->a=angle2-angle1;
    t->m=t->a/M_PI*grain; if(t->m<1)t->m=1;
    t->i=-1;
}

bool traceNext(trace *t){
    t->i++;
    if(t->i>t->m)return false;
    copyPoint(t->s,&t->p);
    rotateAroundAxis(t->k,t->a*t->i/t->m,&t->p);
    normalize(&t->p);
    return true;
}
