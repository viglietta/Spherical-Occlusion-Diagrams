#include "main.h"

int nLineAux=0;
int nPointAux=0;
trace *lineAux=NULL;
point *pointAux=NULL;

void createLineAux(double x,double y,double z,double a,double b){
    lineAux=realloc(lineAux,sizeof(trace)*(nLineAux+1));
    lineAux[nLineAux].k.x=x; lineAux[nLineAux].k.y=y; lineAux[nLineAux].k.z=z;
    normalVector(lineAux[nLineAux].k,lineAux[nLineAux].k,&lineAux[nLineAux].s);
    rotateAroundAxis(lineAux[nLineAux].k,a*M_PI*2.0,&lineAux[nLineAux].s);
    normalize(&lineAux[nLineAux].s);
    lineAux[nLineAux].a=(b-a)*M_PI*2.0;
    lineAux[nLineAux].m=lineAux[nLineAux].a/M_PI*grain; if(lineAux[nLineAux].m<1)lineAux[nLineAux].m=1;
    nLineAux++;
}

void createPointAux(int i,double a){
    pointAux=realloc(pointAux,sizeof(trace)*(nPointAux+1));
    normalVector(lineAux[i].k,lineAux[i].k,pointAux+nPointAux);
    rotateAroundAxis(lineAux[i].k,a*M_PI*2.0,pointAux+nPointAux);
    normalize(pointAux+nPointAux);
    nPointAux++;
}

void initAux(){
//    for(int i=0;i<REPLICAS;i++){
//        double x=cos(M_PI*i/REPLICAS);
//        double y=sin(M_PI*i/REPLICAS);
//        createLineAux(x,y,0,0,1);
//    }
    //createLineAux(0,0,1,0,1);

createLineAux(0,1,1,0,1);
createLineAux(0,-1,1,0,1);

//    createLineAux(0,0,1,0,0.6);
//    createLineAux(0,1,1,0,0.6);
//    createLineAux(0,1,0,0,1);
//    createLineAux(1,0,0,0,1);
//    createPointAux(0,0);
//    createPointAux(0,0.25);
//    createPointAux(0,0.5);
//    createPointAux(0,0.75);
//    createPointAux(1,0);
//    createPointAux(1,0.5);
}

void renderPointAux(int i,bool front){
    double x,y;
    bool f=sphere2screen(pointAux[i],&x,&y);
    if(f==front)drawCircle(x,y,savingSVG?SVG_DOT:DOT_SIZE,true);
}

void renderLineAux(int i,bool front){
    double x1,y1,x2,y2;
    lineAux[i].i=-1;
    while(traceNext(lineAux+i)){
        x1=x2; y1=y2;
        bool f=sphere2screen(lineAux[i].p,&x2,&y2);
        if((lineAux[i].i>0)&&(f==front))drawLine(x1,y1,x2,y2,front,true);
        else endPolyLine(front);
    }
    endPolyLine(front);
}

void renderLinesAux(bool front){
    drawColor(front?0:128,front?0:128,front?0:128);
    for(int i=0;i<nLineAux;i++)renderLineAux(i,front);
    for(int i=0;i<nPointAux;i++)renderPointAux(i,front);
}
