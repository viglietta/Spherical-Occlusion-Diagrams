typedef struct{
    point k,s,p;
    double a;
    int i,m;
}trace;

void traceInit(trace *t,point a,point b,point c);
void traceInitAngle(trace *t,point a,point b,double angle1,double angle2);
bool traceNext(trace *t);
