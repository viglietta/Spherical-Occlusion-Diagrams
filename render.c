#include "main.h"

bool savingSVG=false;
FILE *SVGfile;
int SVGr=0,SVGg=0,SVGb=0;

double drawingPolyLine=false;

#define wins2 ((wins-1)/2)
#define sphererad (wins2*7.0/8.0)

bool doubleDisplay=false;
bool drawSphere=true;
bool drawHidden=true;
bool drawVertices=true;
bool drawPoles=false;
bool drawMeridian=false;
bool drawClosest=false;
bool drawAuxiliary=false;
bool drawMercator=false;
int drawTypes=0;

point eye,up,right;

void setWindowSize(){
    SDL_SetWindowSize(window,wins+(doubleDisplay?wins:0),wins);
}

void toggleDoubleDisplay(){
    doubleDisplay=!doubleDisplay;
    setWindowSize();
}

bool sphere2screen(point a,double *x,double *y){
    if(drawMercator){
        point p;
        p.x=a.x; p.y=0; p.z=a.z;
        normalize(&p);
        point q={1,0,0};
        double angle=angleU(p,q);
        if(a.z<0)angle=-angle;
        *x=wins-sphererad*angle*2/M_PI;
        *y=wins2+sphererad*(acos(a.y)*2/M_PI-1);
        return true;
    }
    else{
        *x=wins2+sphererad*dotProduct(right,a);
        *y=wins2-sphererad*dotProduct(up,a);
        return dotProduct(eye,a)>=0.0;
    }
}

bool poly2screen(point a,double *x,double *y){
    *x=wins+wins2+sphererad*dotProduct(right,a);
    *y=wins2-sphererad*dotProduct(up,a);
    return dotProduct(eye,a)>=0.0;
}

bool screen2sphere(int x,int y,point *a,bool front){
    if(drawMercator){
        double angle1=-(x-wins)/(sphererad*2/M_PI);
        double angle2=((y-wins2)/sphererad+1)*M_PI/2;
        a->x=cos(angle1)*sin(angle2);
        a->y=cos(angle2);
        a->z=sin(angle1)*sin(angle2);
        return true;
    }
    else{
        bool inside=true;
        double xx=(x-wins2)/sphererad;
        double yy=-(y-wins2)/sphererad;
        double zz=0.0;
        double d=1.0-(xx*xx+yy*yy);
        if(d>=0.0)zz=sqrt(d);
        else inside=false;
        if(!front)zz=-zz;
        a->x=xx*right.x+yy*up.x+zz*eye.x;
        a->y=xx*right.y+yy*up.y+zz*eye.y;
        a->z=xx*right.z+yy*up.z+zz*eye.z;
        if(inside)normalize(a);
        return inside;
    }
}

void resetRotation() {
    eye.x=0; eye.y=0; eye.z=1;
    up.x=0; up.y=1; up.z=0;
    right.x=1; right.y=0; right.z=0;
}

void rotateView(double x,double y){
    double angle=sqrt(x*x+y*y)*ROTATION_SPEED*M_PI/180.0;
    point axis;
    axis.x=right.x*x+up.x*y;
    axis.y=right.y*x+up.y*y;
    axis.z=right.z*x+up.z*y;
    crossProduct(axis,eye,&axis);
    normalize(&axis);
    rotateAroundAxis(axis,angle,&eye);
    normalize(&eye);
    rotateAroundAxis(axis,angle,&up);
    normalize(&up);
    crossProduct(up,eye,&right);
    normalize(&right);
    crossProduct(eye,right,&up);
    normalize(&up);
}

void drawColor(int r,int g,int b){
    if(savingSVG){SVGr=r;SVGg=g;SVGb=b;}
    else SDL_SetRenderDrawColor(renderer,r,g,b,255);
}

void SVGcolor(){
    fprintf(SVGfile,"#%02X%02X%02X",SVGr,SVGg,SVGb);
}

void drawSquare(double x,double y,double w,bool full){
    if(savingSVG){
        fprintf(SVGfile,"<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"",(x-w)*SVG_SCALE,(y-w)*SVG_SCALE,w*2.0*SVG_SCALE,w*2.0*SVG_SCALE);
        if(full){
            fprintf(SVGfile," stroke=\"none\" fill=\"");
            SVGcolor();
            fprintf(SVGfile,"\"");
        }
        else{
            fprintf(SVGfile," stroke=\"");
            SVGcolor();
            fprintf(SVGfile,"\" fill=\"none\" stroke-width=\"%.2fpt\"",SVG_STROKE1);
        }
        fprintf(SVGfile,"/>\n");
    }
    else{
        SDL_Rect r;
        r.x=round(x-w); r.y=round(y-w);
        r.w=r.h=round(2.0*w);
        if(full)SDL_RenderFillRect(renderer,&r);
        else SDL_RenderDrawRect(renderer,&r);
    }
}

void drawCircle(double x,double y,double r,bool full){
    if(savingSVG){
        fprintf(SVGfile,"<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\"",x*SVG_SCALE,y*SVG_SCALE,r*SVG_SCALE);
        if(full){
            fprintf(SVGfile," stroke=\"none\" fill=\"");
            SVGcolor();
            fprintf(SVGfile,"\"");
        }
        else{
            fprintf(SVGfile," stroke=\"");
            SVGcolor();
            fprintf(SVGfile,"\" fill=\"none\" stroke-width=\"%.2fpt\"",SVG_STROKE1);
        }
        fprintf(SVGfile,"/>\n");
    }
    else drawSquare(x,y,r,full);
}

void drawCross(int x,int y,int w){
    SDL_RenderDrawLine(renderer,x-w,y-w,x+w,y+w);
    SDL_RenderDrawLine(renderer,x-w,y+w,x+w,y-w);
    SDL_RenderDrawLine(renderer,x,y+w,x,y-w);
    SDL_RenderDrawLine(renderer,x-w,y,x+w,y);
}

void drawThickLine(int x1,int y1,int x2,int y2){
    SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
    SDL_RenderDrawLine(renderer,x1+1,y1,x2+1,y2);
    SDL_RenderDrawLine(renderer,x1,y1+1,x2,y2+1);
    SDL_RenderDrawLine(renderer,x1-1,y1,x2-1,y2);
    SDL_RenderDrawLine(renderer,x1,y1-1,x2,y2-1);
}

void endPolyLine(bool thick){
    if(!drawingPolyLine)return;
    drawingPolyLine=false;
    if(savingSVG){
        fprintf(SVGfile,"\" stroke=\"");
        SVGcolor();
        fprintf(SVGfile,"\" fill=\"none\" stroke-width=\"%.2fpt\"/>\n",thick?SVG_STROKE2:SVG_STROKE1);
    }
}

void drawLine(double x1,double y1,double x2,double y2,bool thick,bool poly){
    if(drawMercator && absolute(x1-x2)>sphererad*2){
        if(savingSVG)endPolyLine(thick);
        return;
    }
    if(savingSVG){
        if(poly){
            if(!drawingPolyLine)fprintf(SVGfile,"<polyline points=\"%.2f,%.2f",x1*SVG_SCALE,y1*SVG_SCALE);
            fprintf(SVGfile," %.2f,%.2f",x2*SVG_SCALE,y2*SVG_SCALE);
            drawingPolyLine=true;
        }
        else{
            fprintf(SVGfile,"<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" fill=\"none\" stroke=\"",x1*SVG_SCALE,y1*SVG_SCALE,x2*SVG_SCALE,y2*SVG_SCALE);
            SVGcolor();
            fprintf(SVGfile,"\" stroke-width=\"%.2fpt\" stroke-linecap=\"round\"/>\n",thick?SVG_STROKE2:SVG_STROKE1);
        }
    }
    else if(thick)drawThickLine(round(x1),round(y1),round(x2),round(y2));
    else SDL_RenderDrawLine(renderer,round(x1),round(y1),round(x2),round(y2));
}

void renderPoint(endpoint *e,bool front){
    double x,y;
    bool f=sphere2screen(e->p,&x,&y);
    if(f==front){
        drawColor(front?0:128,front?0:128,front?0:128);
        drawCircle(x,y,savingSVG?SVG_DOT:DOT_SIZE,true);
        if(e->i==-1){
            drawColor(255,255,255);
            drawCircle(x,y,savingSVG?(SVG_DOT*0.5):(DOT_SIZE-2),true);
        }
    }
}

void renderTrace(trace *t,bool end,bool front){
    double x1,y1,x2,y2;
    bool f;
    while(traceNext(t)){
        x1=x2; y1=y2;
        f=sphere2screen(t->p,&x2,&y2);
        if(t->i>0){
            if(f==front)drawLine(x1,y1,x2,y2,front,true);
            else endPolyLine(front);
        }
    }
    if(end)endPolyLine(front);
}

void drawTriangle(double x1,double y1,double x2,double y2,double x3,double y3,bool thick){
    if(!savingSVG)return;
    fprintf(SVGfile,"<polygon points=\"%.2f,%.2f",x1*SVG_SCALE,y1*SVG_SCALE);
    fprintf(SVGfile," %.2f,%.2f",x2*SVG_SCALE,y2*SVG_SCALE);
    fprintf(SVGfile," %.2f,%.2f",x3*SVG_SCALE,y3*SVG_SCALE);
    fprintf(SVGfile,"\" stroke=\"");
    SVGcolor();
    fprintf(SVGfile,"\" fill=\"");
    SVGcolor();
    fprintf(SVGfile,"\" stroke-width=\"%.2fpt\" stroke-linejoin=\"miter\" stroke-miterlimit=\"8\"/>\n",(thick?SVG_STROKE2:SVG_STROKE1)/2);
}

void renderTriangle(point p1,point p2,point p3,bool front){
    double x1,y1,x2,y2,x3,y3;
    if(sphere2screen(p3,&x1,&y1)!=front)return;
    sphere2screen(p2,&x2,&y2);
    sphere2screen(p1,&x3,&y3);
    drawTriangle(x1,y1,x2,y2,x3,y3,front);
}

void renderArrow(point p,int type,bool front){
    double alpha1=0.05;
    double alpha2=savingSVG?0.015:0.02;
    switch(type){
    case 1:
        if(front)drawColor(0,192,0);
        else drawColor(64,255,64);
        break;
    case -1:
        if(front)drawColor(255,0,0);
        else drawColor(255,128,128);
        break;
    default:
        return;
    }
    if(drawMercator){
        if(!front)return;
        double scale=0.75;
        double x,y,x1,y1,y2;
        sphere2screen(p,&x,&y);
        x1=sphererad*alpha2*scale;
        y1=sphererad*alpha1*scale;
        y2=savingSVG?y1/4:y1;
        if(type<0){y1=-y1;y2=-y2;}
        drawLine(x,y+y1,x,y-y2,true,false);
        if(savingSVG)drawTriangle(x-x1,y,x+x1,y,x,y-y1,true);
        else{
            drawLine(x-x1,y,x,y-y1,true,false);
            drawLine(x+x1,y,x,y-y1,true,false);
        }
    }
    else{
        trace t;
        point p1={0,1,0},p2={0,-1,0};
        traceInitAngle(&t,p,type>0?p1:p2,-alpha1,savingSVG?alpha1/4:alpha1);
        renderTrace(&t,true,front);
        point q1,q2,k,k2,r;
        normalVector(p,p1,&k);
        normalVector(p,k,&k2);
        copyPoint(p,&q1);
        rotateAroundAxis(k2,alpha2,&q1);
        if(savingSVG){
            copyPoint(q1,&r);
            copyPoint(p,&q2);
            rotateAroundAxis(k,type>0?alpha1:-alpha1,&q2);
        }
        else{
            copyPoint(t.p,&q2);
            traceInit(&t,q1,q2,q2);
            renderTrace(&t,false,front);
        }
        copyPoint(p,&q1);
        rotateAroundAxis(k2,-alpha2,&q1);
        if(savingSVG)renderTriangle(r,q1,q2,front);
        else{
            traceInit(&t,q2,q1,q1);
            renderTrace(&t,true,front);
        }
    //    if(savingSVG){
    //        traceInit(&t,q1,r,r);
    //        renderTrace(&t,true,front);
    //    }
    }
}

void renderLine(int i,bool arrows,bool front){
    trace t;
    if(i==n-1 && constructing){
        if(arrows)return;
        drawColor(255,front?0:128,front?0:128);
    }
    else if(!arrows){
        if(drawTypes==1){
            switch(lines[i].type){
            case 1:
                if(front)drawColor(0,192,0);
                else drawColor(64,255,64);
                break;
            case -1:
                if(front)drawColor(255,0,0);
                else drawColor(255,128,128);
                break;
            default:
                drawColor(front?0:128,front?0:128,255);
            }
        }
        else drawColor(front?0:128,front?0:128,255);
    }
    traceInit(&t,lines[i].p1->p,lines[i].p2->p,lines[i].p2->p);
    if(arrows){while(traceNext(&t))if(t.i%10==5)renderArrow(t.p,lines[i].type,front);}
    else renderTrace(&t,true,front);
}

void renderLines(bool front){
    for(int i=0;i<n;i++)renderLine(i,false,front);
    if(drawVertices){
        for(int i=0;i<n;i++){
            renderPoint(lines[i].p1,front);
            renderPoint(lines[i].p2,front);
        }
    }
    if(drawTypes==2)for(int i=0;i<n;i++)renderLine(i,true,front);
}

void renderPoles(bool front){
    if(drawMercator)return;
    double x1,y1,x2,y2;
    point p1={0,1,0},p2={0,-1,0};
    bool f1=sphere2screen(p1,&x1,&y1);
    bool f2=sphere2screen(p2,&x2,&y2);
    //drawColor(216,216,0);
    drawColor(255,214,64);
    if(!f1&&!front)drawCircle(x1,y1,savingSVG?SVG_DOT:DOT_SIZE,true);
    if(!f2&&!front)drawCircle(x2,y2,savingSVG?SVG_DOT:DOT_SIZE,true);
    //drawColor(192,192,0);
    drawColor(255,150,0);
    if(f1&&front)drawCircle(x1,y1,savingSVG?SVG_DOT:DOT_SIZE,true);
    if(f2&&front)drawCircle(x2,y2,savingSVG?SVG_DOT:DOT_SIZE,true);
}

void renderPolarAxis(){
    if(drawMercator)return;
    double x1,y1,x2,y2;
    point p1={0,1,0},p2={0,-1,0};
    sphere2screen(p1,&x1,&y1);
    sphere2screen(p2,&x2,&y2);
    //drawColor(216,216,0);
    drawColor(255,214,64);
    drawLine(x1,y1,x2,y2,false,false);
}

void renderMeridian(bool front){
    if(drawMercator){
        if(!front)return;
        double x,y1,y2;
        sphere2screen(mousep,&x,&y1);
        y1=wins2-sphererad;
        y2=wins2+sphererad;
        drawColor(255,150,0);
        drawLine(x,y1,x,y2,false,false);
    }
    else{
        trace t;
        point p1={0,1,0},p2={0,-1,0};
        if(front)drawColor(255,150,0);
        else drawColor(255,214,64);
        traceInit(&t,p1,mousep,p2);
        renderTrace(&t,true,front);
    }
}

void renderArrows(bool front){
    point pole={0,1,0};
    if(mousep.y>0)pole.y=-pole.y;
    point q1,q2,r1,r2;
    double a1,a2,at;
    for(int i=0;i<n;i++){
        if(abs(lines[i].type)!=1)continue;
        copyPoint(lines[i].p1->p,&q1);
        copyPoint(lines[i].p2->p,&q2);
        intersectArcs(q1,q2,pole,mousep,&r1,&r2);
        if((r1.x<0 && mousep.x>0)||(r1.x>0 && mousep.x<0)||(r1.z<0 && mousep.z>0)||(r1.z>0 && mousep.z<0))copyPoint(r2,&r1);
        a1=angleU(q1,r1);
        a2=angleU(r1,q2);
        at=angleU(q1,q2);
        if(a1>at || a2>at)continue;
        renderArrow(r1,lines[i].type,front);
    }
}

void renderDisplayGraph(bool front){
    drawColor(front?0:128,front?0:128,255);
    double x1,y1,x2,y2;
    point p,q,r,s;
    for(int i=0;i<displayGraph->m;i++){
        copyPoint(displayGraph->v[displayGraph->e[2*i]],&p);
        copyPoint(displayGraph->v[displayGraph->e[2*i+1]],&q);
        int k=displayGraph->e[displayGraph->cw[2*i]^1];
        copyPoint(displayGraph->v[k],&r);
        q.x-=p.x; q.y-=p.y; q.z-=p.z;
        r.x-=p.x; r.y-=p.y; r.z-=p.z;
        normalVector(r,q,&s);
        bool f1=poly2screen(s,&x1,&y1);
        k=displayGraph->e[displayGraph->ccw[2*i]^1];
        copyPoint(displayGraph->v[k],&r);
        r.x-=p.x; r.y-=p.y; r.z-=p.z;
        normalVector(q,r,&s);
        bool f2=poly2screen(s,&x1,&y1);
        if((f1||f2)==front){
            poly2screen(displayGraph->v[displayGraph->e[2*i]],&x1,&y1);
            poly2screen(displayGraph->v[displayGraph->e[2*i+1]],&x2,&y2);
            drawLine(x1,y1,x2,y2,front,false);
        }
    }
}

void Render(){
    double x1,y1,x2=0.0,y2=0.0;
    if(!savingSVG){
        drawColor(255,255,255);
        SDL_RenderClear(renderer);
    }
    if(!drawMercator && drawSphere){
        drawColor(128,128,128);
        if(savingSVG)drawCircle(wins2,wins2,sphererad,false);
        else{
            int m=grain*2;
            for(int i=0;i<=m;i++){
                x1=x2; y1=y2;
                x2=wins2+sphererad*cos(2*M_PI*i/m);
                y2=wins2+sphererad*sin(2*M_PI*i/m);
                if(i>0)drawLine(x1,y1,x2,y2,false,false);
            }
        }
    }
    if(drawMeridian)renderMeridian(false);
    if(drawHidden){
        if(drawPoles)renderPoles(false);
        if(drawAuxiliary)renderLinesAux(false);
        renderLines(false);
        if(drawTypes==3)renderArrows(false);
    }
    if(drawPoles){
        if(!drawHidden)renderPoles(false);
        renderPolarAxis();
    }
    if(drawMeridian)renderMeridian(true);
    if(drawPoles)renderPoles(true);
    if(drawAuxiliary)renderLinesAux(true);
    renderLines(true);
    if(drawTypes==3)renderArrows(true);
    if(!drawMercator && doubleDisplay && displayGraph!=NULL){
        if(drawHidden)renderDisplayGraph(false);
        renderDisplayGraph(true);
    }
    if(!savingSVG){
        drawColor(255,0,0);
        if(iSelected>=0){
            endpoint *e=fSelected? lines[iSelected].p1 : lines[iSelected].p2;
            sphere2screen(e->p,&x1,&y1);
            drawCircle(x1,y1,DOT_SIZE,true);
        }
        if(drawClosest && iClosest>=0){
            sphere2screen(pClosest,&x1,&y1);
            drawSquare(x1,y1,DOT_SIZE,false);
        }
        SDL_RenderPresent(renderer);
    }
}

void saveSVG(){
    savingSVG=true;
    SVGfile=fopen("screenshot.svg","w");
    fprintf(SVGfile,"<svg version=\"1.1\">\n");
    int winsT=wins;
    wins=1601;
    Render();
    wins=winsT;
    fprintf(SVGfile,"</svg>\n");
    fclose(SVGfile);
    savingSVG=false;
}
