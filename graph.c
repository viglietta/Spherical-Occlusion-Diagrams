#include "main.h"

graph *newGraph(){
    graph *g=malloc(sizeof(graph));
    g->v=NULL;
    g->e=g->cw=g->ccw=NULL;
    g->n=g->m=0;
    return g;
}

void freeGraph(graph *g){
    if(g==NULL)return;
    free(g->v);
    free(g->e);
    free(g->cw);
    free(g->ccw);
    free(g);
}

void addGraphNode(graph *g,double x,double y,double z,bool b){
    g->v=realloc(g->v,sizeof(point)*(g->n+1));
    g->v[g->n].x=x; g->v[g->n].y=y; g->v[g->n].z=z;
    if(b)normalize(g->v+g->n);
    g->n++;
}

void graphNormalizeNodes(graph *g){
    for(int i=0;i<g->n;i++)normalize(g->v+i);
}

void graphNormalizeScale(graph *g){
    double m=-1,mm;
    for(int i=0;i<g->n;i++){
        mm=norm(g->v[i]);
        if(mm>m)m=mm;
    }
    m=1/m;
    for(int i=0;i<g->n;i++)scale(g->v+i,m);
}

void addGraphEdge(graph *g,int a,int b){
    g->e=realloc(g->e,sizeof(int)*2*(g->m+1));
    g->e[2*g->m]=a; g->e[2*g->m+1]=b;
    g->m++;
}

void addGraphEdgesDist(graph *g,double d1,double d2){
    double d1s=d1*d1;
    double d2s=d2*d2;
    for(int i=0;i<g->n;i++){
        for(int j=i+1;j<g->n;j++){
            double d=dist2(g->v[i],g->v[j]);
            if(d1s<=d && d<=d2s)addGraphEdge(g,i,j);
        }
    }
}

graph *copyGraph(graph *g){
    graph *h=newGraph();
    h->n=g->n; h->m=g->m;
    h->v=malloc(sizeof(point)*g->n);
    h->e=malloc(sizeof(int)*2*g->m);
    memcpy(h->v,g->v,sizeof(point)*g->n);
    memcpy(h->e,g->e,sizeof(int)*2*g->m);
    if(g->cw){
        h->cw=malloc(sizeof(int)*2*g->m);
        memcpy(h->cw,g->cw,sizeof(int)*2*g->m);
    }
    if(g->ccw){
        h->ccw=malloc(sizeof(int)*2*g->m);
        memcpy(h->ccw,g->ccw,sizeof(int)*2*g->m);
    }
    return h;
}

void graphComputeCwCcw(graph *g){
    if(g->cw)free(g->cw);
    g->cw=malloc(sizeof(int)*2*g->m);
    if(g->ccw)free(g->ccw);
    g->ccw=malloc(sizeof(int)*2*g->m);
    graph *h=copyGraph(g);
    graphNormalizeNodes(h);
    for(int i=0;i<2*h->m;i++){
        int k=h->e[i];
        point p;
        copyPoint(h->v[k],&p);
        double mi=10,ma=-10;
        int ii=i,ia=i;
        point n1,n2;
        normalVector(p,h->v[h->e[i^1]],&n1);
        for(int j=0;j<2*h->m;j++){
            if(j==i || h->e[j]!=k)continue;
            normalVector(p,h->v[h->e[j^1]],&n2);
            double mm=angleCCW(n1,n2,p);
            if(mm<mi){mi=mm; ii=j;}
            if(mm>ma){ma=mm; ia=j;}
        }
        g->cw[i]=ia;
        g->ccw[i]=ii;
    }
    freeGraph(h);
}

int *graphBipartition(graph *g){
    int *p=malloc(sizeof(bool)*g->n);
    for(int i=0;i<g->n;i++)p[i]=-1;
    int toAssign=g->n;
    bool changed=false;
    int unassigned=0;
    while(toAssign>0){
        if(changed)changed=false;
        else{
            p[unassigned]=0;
            toAssign--;
        }
        for(int i=0;i<g->m;i++){
            int a=g->e[2*i];
            int b=g->e[2*i+1];
            if(p[a]==-1&&p[b]!=-1){
                p[a]=1-p[b];
                toAssign--;
                changed=true;
            }
            else if(p[a]!=-1&&p[b]==-1){
                p[b]=1-p[a];
                toAssign--;
                changed=true;
            }
            if(p[a]==-1)unassigned=a;
            if(p[b]==-1)unassigned=b;
        }
    }
    return p;
}

graph *truncateGraph(graph *g,double d,bool relative){ // ToDo: rewrite using g->cw
    graph *h=newGraph();
    point p,q;
    int c=0;
    int *rv=malloc(sizeof(int)*g->n);
    int *re=malloc(sizeof(int)*g->m);
    for(int i=0;i<g->m;i++)re[i]=-1;
    for(int i=0;i<g->n;i++){
        for(int j=0;j<2*g->m;j++){
            if(g->e[j]==i){
                copyPoint(g->v[i],&p);
                if(relative)sphereInterpR(p,g->v[g->e[j^1]],d,&p);
                else sphereInterpA(p,g->v[g->e[j^1]],d,&p);
                addGraphNode(h,p.x,p.y,p.z,false);
                if(re[j/2]==-1)re[j/2]=c;
                else addGraphEdge(h,re[j/2],c);
                c++;
            }
        }
        rv[i]=c;
    }
    free(re);
    double *a=malloc(sizeof(double)*c);
    int n1,n2=0;
    for(int i=0;i<g->n;i++){
        n1=n2;
        n2=rv[i];
        copyPoint(h->v[n1],&p);
        projectOnPlane(&p,g->v[i]);
        normalize(&p);
        for(int j=n1;j<n2;j++){
            copyPoint(h->v[j],&q);
            projectOnPlane(&q,g->v[i]);
            normalize(&q);
            a[j]=angleCCW(p,q,g->v[i]);
        }
        int l=n1;
        double m=0;
        for(int k=1;k<n2-n1;k++){
            int ll=n1;
            double mm=10.0;
            for(int j=n1;j<n2;j++){
                if(a[j]>m&&a[j]<mm&&j!=l){
                    ll=j;
                    mm=a[j];
                }
            }
            addGraphEdge(h,l,ll);
            l=ll;
            m=mm;
        }
        addGraphEdge(h,l,n1);
    }
    free(a);
    free(rv);
    return h;
}

//double swirlHelper(double a,double d){
//    return d;
////    double r=a/(2.0*M_PI);
////    r/=0.3;
////    return d/(r*r*r);
//}

graph *swirlifyGraph(graph *g,double d,bool relative){ // ToDo: rewrite using g->cw and g->ccw
    graph *h=newGraph();
    point p,q;
    int l,ll;
    int *rv=malloc(sizeof(int)*g->n);
    int *re=malloc(sizeof(int)*g->m);
    int *rh=malloc(sizeof(int)*g->m*2);
    int *rr=malloc(sizeof(int)*g->m*2);
    for(int i=0;i<g->m;i++)re[i]=-1;
    for(int i=0;i<g->n;i++){
        for(int j=0;j<2*g->m;j++){
            if(g->e[j]==i){
                copyPoint(g->v[i],&p);
                sphereInterpR(p,g->v[g->e[j^1]],0.25,&p);
                if(re[j/2]==-1)re[j/2]=h->n;
                else{
                    addGraphEdge(h,re[j/2],h->n);
                    rh[re[j/2]]=i;
                    rh[h->n]=g->e[j^1];
                    rr[re[j/2]]=h->n;
                    rr[h->n]=re[j/2];
                }
                addGraphNode(h,p.x,p.y,p.z,false);
            }
        }
        rv[i]=h->n;
    }
    free(re);

    int *s=graphBipartition(g);
    double *a=malloc(sizeof(double)*h->n);
    int *pv=malloc(sizeof(int)*h->n);
    int n1,n2=0;
    for(int i=0;i<g->n;i++){
        n1=n2;
        n2=rv[i];
        copyPoint(h->v[n1],&p);
        projectOnPlane(&p,g->v[i]);
        normalize(&p);
        a[n1]=0;
        for(int j=n1+1;j<n2;j++){
            copyPoint(h->v[j],&q);
            projectOnPlane(&q,g->v[i]);
            normalize(&q);
            a[j]=angleCCW(p,q,g->v[i]);
            if(s[i])a[j]=M_PI*2.0-a[j];
        }
        l=n1;
        double m=0;
        copyPoint(g->v[i],&p);
        for(int k=1;k<n2-n1;k++){
            ll=n1;
            double mm=10.0;
            for(int j=n1;j<n2;j++){
                if(a[j]>m&&a[j]<mm&&j!=l){
                    ll=j;
                    mm=a[j];
                }
            }
            pv[l]=ll;
            copyPoint(g->v[rh[ll]],&q);
            if(relative)sphereInterpR(p,q,d,h->v+l);
            else sphereInterpA(p,q,d,h->v+l);
            l=ll;
            m=mm;
        }
        pv[l]=n1;
        copyPoint(g->v[rh[n1]],&q);
        if(relative)sphereInterpR(p,q,d,h->v+l);
        else sphereInterpA(p,q,d,h->v+l);
    }
    free(a);
    free(s);

//    for(int k=0;k<1;k++){
//        for(int i=0;i<h->m;i++){
//            copyPoint(h->v[h->e[2*i]],&p);
//            copyPoint(h->v[h->e[2*i+1]],&q);
//            sphereInterpA(p,q,d,h->v+h->e[2*i]);
//            sphereInterpA(q,p,d,h->v+h->e[2*i+1]);
//        }
//        n2=0;
//        for(int i=0;i<g->n;i++){
//            n1=n2;
//            n2=rv[i];
//            l=n1;
//            copyPoint(h->v[n1],&p);
//            for(int j=n1;j<n2;j++){
//                ll=pv[l];
//                copyPoint(ll==n1?p:h->v[ll],h->v+l);
//                l=ll;
//            }
//        }
//    }

    for(int i=0;i<h->m;i++){
        point p2,q2,r1,r2;
        int v1=h->e[2*i];
        int v2=rr[v1];
        copyPoint(h->v[v1],&p);
        copyPoint(h->v[v2],&q);
        int u1=pv[v1];
        int u2=rr[u1];
        copyPoint(h->v[u1],&p2);
        copyPoint(h->v[u2],&q2);
        intersectArcs(p,q,p2,q2,&r1,&r2);
        double d1=dist2(p,r1);
        double d2=dist2(p,r2);
        copyPoint(d1<d2?r1:r2,h->v+v1);
        u1=pv[v2];
        u2=rr[u1];
        copyPoint(h->v[u1],&p2);
        copyPoint(h->v[u2],&q2);
        intersectArcs(p,q,p2,q2,&r1,&r2);
        d1=dist2(q,r1);
        d2=dist2(q,r2);
        copyPoint(d1<d2?r1:r2,h->v+v2);
    }

    free(pv);
    free(rr);
    free(rh);
    free(rv);
    return h;
}
