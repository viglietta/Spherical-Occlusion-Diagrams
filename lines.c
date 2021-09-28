#include "main.h"

int n=0;
line *lines=NULL;
graph *displayGraph=NULL;

int nSaved=0;
line *linesSaved=NULL;
graph *displayGraphSaved=NULL;

double snapDistance=0.02;

double closestToLine(point p,int i,point *q){
    point *a=&lines[i].p1->p;
    point *b=&lines[i].p2->p;
    point k;
    normalVector(*a,*b,&k);
    scale(&k,dotProduct(k,p));
    q->x=p.x-k.x;
    q->y=p.y-k.y;
    q->z=p.z-k.z;
    normalize(q);
    if(checkZero(*q))copyPoint(*a,q);
    double aa=angleU(*a,*q);
    double bb=angleU(*b,*q);
    double cc=angleU(*a,*b);
    if(aa>cc || bb>cc || aa+bb>M_PI){
        if(dist2(p,*a)<dist2(p,*b)){
            copyPoint(*a,q);
            return 0.0;
        }
        copyPoint(*b,q);
        return 1.0;
    }
    if(cc==0.0)return 0.0;
    return aa/cc;
}

double closestToLines(point p,int ex,point *q,int *index){
    double md=-1.0,mt=0.0;
    *index=-1;
    point w;
    for(int i=0;i<n;i++){
        if(i==ex)continue;
        double t=closestToLine(p,i,&w);
        double d=dist2(p,w);
        if(md<0.0 || d<md){
            md=d;
            mt=t;
            copyPoint(w,q);
            *index=i;
        }
    }
    return mt;
}

void recomputeEndpoint(endpoint *p){
    if(p->i<0)return;
    point *a=&lines[p->i].p1->p;
    point *b=&lines[p->i].p2->p;
    point k;
    normalVector(*a,*b,&k);
    copyPoint(*a,&p->p);
    rotateAroundAxis(k,angleU(*a,*b)*p->t,&p->p);
}

double connectEndpoint(int i,bool first,bool snap){
    double d=-1.0;
    endpoint *e=first? lines[i].p1 : lines[i].p2;
    point q;
    int index;
    double t=closestToLines(e->p,i,&q,&index);
    e->i=-1; e->t=0.0;
    if(index>=0){
        d=dist2(q,e->p);
        if(d>snapDistance*snapDistance)d=-1.0;
        else{
            e->t=t;
            e->i=index;
            if(snap)copyPoint(q,&e->p);
        }
    }
    return d;
}

double connectLine(int i,bool snap){
    double d1=connectEndpoint(i,true,snap);
    double d2=connectEndpoint(i,false,snap);
    return d1>d2?d1:d2;
}

void connectLines(bool snap){
    int c=1024;
    double md;
    do{
        md=-1.0;
        for(int i=0;i<n;i++){
            double d=connectLine(i,snap);
            if(d>md)md=d;
        }
    }while(snap && md>ADJUST_TOLERANCE*ADJUST_TOLERANCE && --c>0);
}

endpoint *newPointC(double x,double y,double z,bool b){
    endpoint *p=malloc(sizeof(endpoint));
    p->p.x=x;
    p->p.y=y;
    p->p.z=z;
    p->i=-1;
    p->t=0.0;
    if(b)normalize(&p->p);
    return p;
}

endpoint *newPointP(point a){
    endpoint *p=malloc(sizeof(endpoint));
    copyPoint(a,&p->p);
    p->i=-1;
    p->t=0.0;
    return p;
}

endpoint *newPointR(int i,double t){
    endpoint *p=malloc(sizeof(endpoint));
    p->i=i;
    p->t=t;
    recomputeEndpoint(p);
    return p;
}

endpoint *newPointE(endpoint *e){
    endpoint *p=malloc(sizeof(endpoint));
    copyPoint(e->p,&p->p);
    p->i=e->i;
    p->t=e->t;
    return p;
}

void newLine(endpoint *p1,endpoint *p2){
    n++;
    lines=realloc(lines,sizeof(line)*n);
    lines[n-1].p1=p1;
    lines[n-1].p2=p2;
}

void graphToLines(graph *g,bool b){
    for(int i=0;i<g->m;i++)newLine(newPointP(g->v[g->e[2*i]]),newPointP(g->v[g->e[2*i+1]]));
    connectLines(b);
}

void switchLines(int i1,int i2){
    endpoint *p;
    if(i1==i2)return;
    for(int i=0;i<n;i++){
        p=lines[i].p1;
        if(p->i==i1)p->i=i2;
        else if(p->i==i2)p->i=i1;
        p=lines[i].p2;
        if(p->i==i1)p->i=i2;
        else if(p->i==i2)p->i=i1;
    }
    p=lines[i1].p1;
    lines[i1].p1=lines[i2].p1;
    lines[i2].p1=p;
    p=lines[i1].p2;
    lines[i1].p2=lines[i2].p2;
    lines[i2].p2=p;
}

void deleteLine(int index){
    endpoint *p;
    switchLines(index,n-1);
    for(int i=0;i<n;i++){
        p=lines[i].p1;
        if(p->i==n-1){ p->i=-1; p->t=0.0; }
        p=lines[i].p2;
        if(p->i==n-1){ p->i=-1; p->t=0.0; }
    }
    free(lines[n-1].p1);
    free(lines[n-1].p2);
    n--;
    lines=realloc(lines,sizeof(line)*n);
}

void freeLines(){
    for(int i=0;i<n;i++){
        free(lines[i].p1);
        free(lines[i].p2);
    }
    free(lines);
    lines=NULL;
    n=0;
    freeGraph(displayGraph);
    displayGraph=NULL;
}

void freeLinesSaved(){
    for(int i=0;i<nSaved;i++){
        free(linesSaved[i].p1);
        free(linesSaved[i].p2);
    }
    free(linesSaved);
    linesSaved=NULL;
    nSaved=0;
    freeGraph(displayGraphSaved);
    displayGraphSaved=NULL;
}

void saveLines(int untilN){
    freeLinesSaved();
    nSaved=untilN;
    linesSaved=malloc(sizeof(line)*nSaved);
    for(int i=0;i<nSaved;i++){
        linesSaved[i].p1=newPointE(lines[i].p1);
        linesSaved[i].p2=newPointE(lines[i].p2);
    }
    displayGraphSaved=displayGraph==NULL?NULL:copyGraph(displayGraph);
}

void restoreLines(){
    int nTemp=n;
    line *linesTemp=lines;
    graph *displayGraphTemp=displayGraph;
    n=nSaved;
    lines=linesSaved;
    displayGraph=displayGraphSaved;
    nSaved=nTemp;
    linesSaved=linesTemp;
    displayGraphSaved=displayGraphTemp;
}

void rotateLines(double x,double y) {
    double angle=sqrt(x*x+y*y)*ROTATION_SPEED*M_PI/180.0;
    point axis;
    axis.x=right.x*x+up.x*y;
    axis.y=right.y*x+up.y*y;
    axis.z=right.z*x+up.z*y;
    crossProduct(axis,eye,&axis);
    normalize(&axis);
    for(int i=0;i<n;i++){
        rotateAroundAxis(axis,angle,&lines[i].p1->p);
        normalize(&lines[i].p1->p);
        rotateAroundAxis(axis,angle,&lines[i].p2->p);
        normalize(&lines[i].p2->p);
    }
}

double distEndpoint(int x,int y,point p){
    double xx,yy;
    if(sphere2screen(p,&xx,&yy))return (x-xx)*(x-xx)+(y-yy)*(y-yy);
    else return -1.0;
}

double findClosestEndpoint(int x,int y,int *index,bool *first){
    int mi=-1;
    bool mf=true;
    double md=-1.0,d;
    for(int i=0;i<n;i++){
        d=distEndpoint(x,y,lines[i].p1->p);
        if(d>=0.0 && (mi==-1 || d<md)){ mi=i; mf=true; md=d; }
        d=distEndpoint(x,y,lines[i].p2->p);
        if(d>=0.0 && (mi==-1 || d<md)){ mi=i; mf=false; md=d; }
    }
    if(mi==-1 || md>SELECT_DISTANCE*SELECT_DISTANCE){
        *index=-1;
        *first=true;
        return -1.0;
    }
    *index=mi;
    *first=mf;
    return md;
}

void computeLineTypes(){
    point m;
    double c;
    line *l;
    for(int i=0;i<n;i++){
        normalVector(lines[i].p1->p,lines[i].p2->p,&lines[i].k);
        if(lines[i].k.y<0.0)scale(&lines[i].k,-1.0);
        lines[i].type=0;
    }
    for(int i=0;i<n;i++){
        sphereInterpR(lines[i].p1->p,lines[i].p2->p,0.5,&m);
        if(lines[i].p1->i>=0){
            l=&lines[lines[i].p1->i];
            if(lines[i].p1->i==lines[i].p2->i)l->type=2;
            c=dotProduct(l->k,m);
            if(c>0.0){
                if(l->type==0)l->type=1;
                else if(l->type==-1)l->type=2;
            }
            else if(c<0.0){
                if(l->type==0)l->type=-1;
                else if(l->type==1)l->type=2;
            }
            else l->type=2;
        }
        if(lines[i].p2->i>=0){
            l=&lines[lines[i].p2->i];
            c=dotProduct(l->k,m);
            if(c>0.0){
                if(l->type==0)l->type=1;
                else if(l->type==-1)l->type=2;
            }
            else if(c<0.0){
                if(l->type==0)l->type=-1;
                else if(l->type==1)l->type=2;
            }
            else l->type=2;
        }
    }
}

void extendLine(int index,double a1,double a2){
    point k;
    normalVector(lines[index].p1->p,lines[index].p2->p,&k);
    if(a1!=0){
        rotateAroundAxis(k,-a1,&lines[index].p1->p);
        normalize(&lines[index].p1->p);
    }
    if(a2!=0){
        rotateAroundAxis(k,a2,&lines[index].p2->p);
        normalize(&lines[index].p2->p);
    }
}
