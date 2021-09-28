#include "main.h"

double absolute(double x){
    if(x<0.0)return -x;
    return x;
}

bool checkZero(point p){
    if(absolute(p.x)+absolute(p.y)+absolute(p.z)==0.0)return true;
    return false;
}

void copyPoint(point a,point *b){
    b->x=a.x;
    b->y=a.y;
    b->z=a.z;
}

double dotProduct(point u,point v){
    return u.x*v.x+u.y*v.y+u.z*v.z;
}

void crossProduct(point u,point v,point *w){
    point p;
    p.x=u.y*v.z-u.z*v.y;
    p.y=u.z*v.x-u.x*v.z;
    p.z=u.x*v.y-u.y*v.x;
    copyPoint(p,w);
}

void rotateAroundAxis(point v,double angle,point *w){
    double c=cos(angle);
    double s=sin(angle);
    point rot1,rot2,rot3,p;
    rot1.x=v.x*v.x*(1.0-c)+c; rot1.y=v.x*v.y*(1.0-c)-v.z*s; rot1.z=v.x*v.z*(1.0-c)+v.y*s;
    rot2.x=v.x*v.y*(1.0-c)+v.z*s; rot2.y=v.y*v.y*(1.0-c)+c; rot2.z=v.y*v.z*(1.0-c)-v.x*s;
    rot3.x=v.x*v.z*(1.0-c)-v.y*s; rot3.y=v.y*v.z*(1.0-c)+v.x*s; rot3.z=v.z*v.z*(1.0-c)+c;
    p.x=dotProduct(rot1,*w);
    p.y=dotProduct(rot2,*w);
    p.z=dotProduct(rot3,*w);
    copyPoint(p,w);
}

void scale(point *v,double a){
    v->x*=a; v->y*=a; v->z*=a;
}

double norm(point v){
    return sqrt(dotProduct(v,v));
}

double dist2(point a,point b){
    a.x-=b.x; a.y-=b.y; a.z-=b.z;
    return dotProduct(a,a);
}

double dist(point a,point b){
    return sqrt(dist2(a,b));
}

void normalize(point *v){
    if(checkZero(*v))return;
    scale(v,1.0/norm(*v));
}

double angleU(point a,point b){
    double c=dotProduct(a,b);
    if(c<-1.0)return M_PI;
    if(c>1.0)return 0.0;
    return acos(c);
}

double angleCCW(point a,point b,point n){
    double c=dotProduct(a,b);
    if(c<-1.0)return M_PI;
    if(c>1.0)return 0.0;
    point d;
    crossProduct(a,b,&d);
    double s=dotProduct(n,d);
    if(s<-1.0)return M_PI*1.5;
    if(s>1.0)return M_PI*0.5;
    double r=atan2(s,c);
    return r<0.0?r+M_PI*2.0:r;
}

double angleO(point o,point a,point b){
    a.x-=o.x; a.y-=o.y; a.z-=o.z;
    b.x-=o.x; b.y-=o.y; b.z-=o.z;
    return angleU(a,b);
}

void normalVector(point a,point b,point *c){
    crossProduct(a,b,c);
    if(checkZero(*c)){
        point aa={1,0,0};
        crossProduct(a,aa,c);
    }
    if(checkZero(*c)){
        point aa={0,1,0};
        crossProduct(a,aa,c);
    }
    normalize(c);
}

void projectOnPlane(point *v,point n){
    scale(&n,dotProduct(*v,n));
    v->x-=n.x; v->y-=n.y; v->z-=n.z;
}

void sphereInterpR(point a,point b,double t,point *m){
    point k;
    normalVector(a,b,&k);
    copyPoint(a,m);
    rotateAroundAxis(k,angleU(a,b)*t,m);
    normalize(m);
}

void sphereInterpA(point a,point b,double t,point *m){
    point k;
    normalVector(a,b,&k);
    copyPoint(a,m);
    rotateAroundAxis(k,t,m);
    normalize(m);
}

void intersectArcs(point p1,point p2,point q1,point q2,point *r1,point *r2){
    point n1,n2;
    normalVector(p1,p2,&n1);
    normalVector(q1,q2,&n2);
    normalVector(n1,n2,r1);
    r2->x=-r1->x; r2->y=-r1->y; r2->z=-r1->z;
}
