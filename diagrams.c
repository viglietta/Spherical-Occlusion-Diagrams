#include "main.h"

void swirlHelper(graph *g,double s){
    graphComputeCwCcw(g);

    graph *gg=copyGraph(g);
    graphNormalizeNodes(gg);
    graph *h=swirlifyGraph(gg,s,false);
    freeGraph(gg);

    graphToLines(h,false);
    freeGraph(h);
}

void swirl(graph *g,double s){
    freeLines();
    swirlHelper(g,s);
    graphNormalizeScale(g);
    displayGraph=g;
}

void truncSwirl(graph *g,double t,double s){
    freeLines();
    graphComputeCwCcw(g);

    graph *gg=copyGraph(g);
    graphNormalizeNodes(gg);
    graph *h=truncateGraph(gg,t,true);
    freeGraph(gg);

    graphNormalizeScale(g);
    displayGraph=g;
    swirlHelper(h,s);
    freeGraph(h);
}

void buildPrism(int m){
//    freeLines();
//    graph *g=newGraph();
//
//    double eps=0.1;
//    double h=1-eps;
//    double w=3;
//    double d=1+eps;
//
//    addGraphNode(g,w,h,d,false);
//    addGraphNode(g,-w,h,d,false);
//    addGraphNode(g,-w,-h,d,false);
//    addGraphNode(g,w,-h,d,false);
//    addGraphEdge(g,0,1);
//    addGraphEdge(g,1,2);
//    addGraphEdge(g,2,3);
//    addGraphEdge(g,3,0);
//
//    addGraphNode(g,w,h,-d,false);
//    addGraphNode(g,-w,h,-d,false);
//    addGraphNode(g,-w,-h,-d,false);
//    addGraphNode(g,w,-h,-d,false);
//    addGraphEdge(g,4,5);
//    addGraphEdge(g,5,6);
//    addGraphEdge(g,6,7);
//    addGraphEdge(g,7,4);
//
//    addGraphNode(g,d,w,h,false);
//    addGraphNode(g,d,-w,h,false);
//    addGraphNode(g,d,-w,-h,false);
//    addGraphNode(g,d,w,-h,false);
//    addGraphEdge(g,8,9);
//    addGraphEdge(g,9,10);
//    addGraphEdge(g,10,11);
//    addGraphEdge(g,11,8);
//
//    addGraphNode(g,-d,w,h,false);
//    addGraphNode(g,-d,-w,h,false);
//    addGraphNode(g,-d,-w,-h,false);
//    addGraphNode(g,-d,w,-h,false);
//    addGraphEdge(g,12,13);
//    addGraphEdge(g,13,14);
//    addGraphEdge(g,14,15);
//    addGraphEdge(g,15,12);
//
//    addGraphNode(g,h,d,w,false);
//    addGraphNode(g,h,d,-w,false);
//    addGraphNode(g,-h,d,-w,false);
//    addGraphNode(g,-h,d,w,false);
//    addGraphEdge(g,16,17);
//    addGraphEdge(g,17,18);
//    addGraphEdge(g,18,19);
//    addGraphEdge(g,19,16);
//
//    addGraphNode(g,h,-d,w,false);
//    addGraphNode(g,h,-d,-w,false);
//    addGraphNode(g,-h,-d,-w,false);
//    addGraphNode(g,-h,-d,w,false);
//    addGraphEdge(g,20,21);
//    addGraphEdge(g,21,22);
//    addGraphEdge(g,22,23);
//    addGraphEdge(g,23,20);
//
//    graphNormalizeNodes(g);
//    graphToLines(g,false);
//    freeGraph(g);

    double d=3;
    if(m<=4)d=(sqrt(2)/2)*(1+(m-1)*0.3);
    if(m==2)d=sqrt(2)/2;

    graph *g=newGraph();
    for(int i=0;i<2*m;i++){
        double x=cos(M_PI*i/m);
        double y=sin(M_PI*i/m);
        addGraphNode(g,x,y,d,false);
        addGraphNode(g,x,y,-d,false);
        addGraphEdge(g,2*i,2*i+1);
        addGraphEdge(g,2*i,(2*i+2)%(4*m));
        addGraphEdge(g,2*i+1,(2*i+3)%(4*m));
    }
    double k=0.015;
    if(m==2)k=0.2;
    if(m==3)k=0.05;
    swirl(g,k);
}

void buildTwistedPrism(int m){
    double a=0.7;
    double d=1;
    //if(m<=4)d=(sqrt(2)/2)*(1+(m-1)*0.3);

    graph *g=newGraph();
    for(int i=0;i<2*m;i++){
        double x=cos(M_PI*i/m);
        double y=sin(M_PI*i/m);
        addGraphNode(g,x,y,d,false);
        x=cos(M_PI*i/m+M_PI*a);
        y=sin(M_PI*i/m+M_PI*a);
        addGraphNode(g,x,y,-d,false);
        addGraphEdge(g,2*i,2*i+1);
        addGraphEdge(g,2*i,(2*i+2)%(4*m));
        addGraphEdge(g,2*i+1,(2*i+3)%(4*m));
    }
//    freeLines();
//    graphToLines(g,false);
    swirl(g,0.05);
}

void buildTAntiprism(int m){
    double d=1;
    graph *g=newGraph();
    for(int i=0;i<2*m;i++){
        double x=cos(M_PI*i/m);
        double y=sin(M_PI*i/m);
        addGraphNode(g,x,y,i%2?d:-d,false);
        addGraphEdge(g,i,(i+1)%(2*m));
        addGraphEdge(g,i,(i+2)%(2*m));
    }
    truncSwirl(g,0.25,0.02);
}

void buildTBipyramid(int m){
    graph *g=newGraph();
    for(int i=0;i<2*m;i++){
        double x=cos(M_PI*i/m);
        double y=sin(M_PI*i/m);
        addGraphNode(g,x,y,0,false);
        addGraphEdge(g,i,(i+1)%(2*m));
        addGraphEdge(g,i,2*m);
        addGraphEdge(g,i,2*m+1);
    }
    addGraphNode(g,0,0,1,false);
    addGraphNode(g,0,0,-1,false);
    truncSwirl(g,0.25,0.02);
}

void buildTrapezohedron(int m){ // dual of antiprism
    double d=0.5;
    graph *g=newGraph();
    for(int i=0;i<2*m;i++){
        double x=cos(M_PI*i/m);
        double y=sin(M_PI*i/m);
        addGraphNode(g,x,y,i%2?d:-d,false);
        addGraphEdge(g,i,(i+1)%(2*m));
        addGraphEdge(g,i,i%2?2*m:2*m+1);
    }
    addGraphNode(g,0,0,1,false);
    addGraphNode(g,0,0,-1,false);
    swirl(g,0.25);
    d=(1-cos(M_PI/m))/(1+cos(M_PI/m));
    for(int i=0;i<2*m;i++){
        displayGraph->v[i].x=cos(M_PI*i/m);
        displayGraph->v[i].y=sin(M_PI*i/m);
        displayGraph->v[i].z=i%2?d:-d;
    }
    displayGraph->v[2*m].z=1;
    displayGraph->v[2*m+1].z=-1;
    graphNormalizeScale(displayGraph);
}

void generateBox(graph *g,double a1,double a2,double a3){
    addGraphNode(g,+a1,+a2,+a3,false);
    addGraphNode(g,+a1,+a2,-a3,false);
    addGraphNode(g,+a1,-a2,+a3,false);
    addGraphNode(g,+a1,-a2,-a3,false);
    addGraphNode(g,-a1,+a2,+a3,false);
    addGraphNode(g,-a1,+a2,-a3,false);
    addGraphNode(g,-a1,-a2,+a3,false);
    addGraphNode(g,-a1,-a2,-a3,false);
}

void generateCube(graph *g,double a){
    generateBox(g,a,a,a);
}

void generate6Points(graph *g,double a1,double a2){
    addGraphNode(g,+a1, a2,  0,false);
    addGraphNode(g,-a1, a2,  0,false);
    addGraphNode(g,  0,+a1, a2,false);
    addGraphNode(g,  0,-a1, a2,false);
    addGraphNode(g, a2,  0,+a1,false);
    addGraphNode(g, a2,  0,-a1,false);
}

void generateOctahedron(graph *g,double a){
    generate6Points(g,a,0);
}

void generateIcosahedron(graph *g,double a1,double a2){
    generate6Points(g,a1,a2);
    generate6Points(g,a1,-a2);
}

void generate3Boxes(graph *g,double a1,double a2,double a3){
    generateBox(g,a1,a2,a3);
    generateBox(g,a3,a1,a2);
    generateBox(g,a2,a3,a1);
}

void buildRhombicDodecahedron(){ // dual of the cuboctahedron (convex hull of a cube plus its dual octahedron)
    graph *g=newGraph();
    generateCube(g,1);
    generateOctahedron(g,2);
    addGraphEdgesDist(g,1.7,1.75);
    swirl(g,0.2);
}

void buildDeltoidalIcositetrahedron(){ // dual of the rhombicuboctahedron (cube plus dual octahedron plus centers of edges)
    double r1=(1+sqrt(8))/7;
    double r2=sqrt(2)/2;
    graph *g=newGraph();
    generateCube(g,r1);
    generateOctahedron(g,1);
    generateIcosahedron(g,r2,r2); // cuboctahedron
    addGraphEdgesDist(g,0.76,0.77);
    addGraphEdgesDist(g,0.58,0.6);
    swirl(g,0.15);
}

void buildRhombicTriancontahedron(){ // dual of the icosidodecahedron (convex hull of a regular dodecahedron plus its dual icosahedron)
    double r1=(1+sqrt(5))/2;
    double r2=1/r1;
    graph *g=newGraph();
    generateIcosahedron(g,1,r1);
    generateIcosahedron(g,r1,r2); // dodecahedron
    generateCube(g,1); // dodecahedron
    addGraphEdgesDist(g,1.1,1.2);
    swirl(g,0.15);
}

void buildDeltoidalHexecontahedron(){ // dual of the rhombicosidodecahedron (dodecahedron plus dual icosahedron plus centers of edges)
    double r0=(5-sqrt(5))/4;
    double r1=(15+sqrt(5))/22;
    double r2=sqrt(5)/2;
    double r3=(5+sqrt(5))/6;
    double r4=(5+4*sqrt(5))/11;
    double r5=(5+sqrt(5))/4;
    double r6=(5+3*sqrt(5))/6;
    double r7=(25+9*sqrt(5))/22;
    double r8=sqrt(5);
    graph *g=newGraph();
    generateIcosahedron(g,r1,r7);
    generateIcosahedron(g,r6,r3);
    generateCube(g,r4);
    generateOctahedron(g,r8);
    generate3Boxes(g,r0,r2,r5);
    addGraphEdgesDist(g,0.8,0.81);
    addGraphEdgesDist(g,1.2,1.3);
    swirl(g,0.1);
}

void buildTCuboctahedron(){
    graph *g=newGraph();
    generateIcosahedron(g,1,1); // cuboctahedron
    addGraphEdgesDist(g,1.4,1.5);
    truncSwirl(g,0.25,0.05);
}

//void buildTOrthobicupola(){
//    graph *g=newGraph();
//    generateIcosahedron(g,1,1); // cuboctahedron
//// rotate a triangle by 180 degrees
//    addGraphEdgesDist(g,1.4,1.5);
//    truncSwirl(g,0.25,0.05);
//}

void buildTIcosidodecahedron(){
    double r1=(sqrt(5)+1)/2;
    double r2=r1*r1;
    graph *g=newGraph();
    generateOctahedron(g,r1);
    generate3Boxes(g,0.5,r1/2,r2/2);
    addGraphEdgesDist(g,0.9,1.1);
    truncSwirl(g,0.2,0.025);
}

void buildCustomDiagram(){
    double d=2;
    double w=10;
    double h=1.5;
    double a1=0.25;
    double a2=0.55;
    double t1=0.8;
    double t2=0.2;
    double t3=0.85;
    double t4=0.45;

    freeLines();
    newLine(newPointC(w,h,d,true),newPointC(-w,h,d,true));
    newLine(newPointC(w,h,-d,true),newPointC(-w,h,-d,true));
    newLine(newPointC(d,-h,w,true),newPointC(d,-h,-w,true));
    newLine(newPointC(-d,-h,w,true),newPointC(-d,-h,-w,true));

    newLine(newPointR(0,0),newPointR(1,0));
    newLine(newPointR(0,1),newPointR(1,1));
    newLine(newPointR(2,0),newPointR(3,0));
    newLine(newPointR(2,1),newPointR(3,1));

    newLine(newPointR(0,t1),newPointR(3,t2));
    newLine(newPointR(0,t3),newPointR(3,t4));
    newLine(newPointR(1,t1),newPointR(3,1-t2));
    newLine(newPointR(1,t3),newPointR(3,1-t4));
    newLine(newPointR(1,1-t1),newPointR(2,1-t2));
    newLine(newPointR(1,1-t3),newPointR(2,1-t4));
    newLine(newPointR(0,1-t1),newPointR(2,t2));
    newLine(newPointR(0,1-t3),newPointR(2,t4));

    extendLine(4,a1,a1);
    extendLine(5,a1,a1);
    extendLine(6,a2,a2);
    extendLine(7,a2,a2);

    connectLines(true);
}
