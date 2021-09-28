typedef struct{
    point *v;
    int *e,*cw,*ccw;
    int n,m;
}graph;

graph *newGraph();
void freeGraph(graph *g);
void addGraphNode(graph *g,double x,double y,double z,bool b);
void graphNormalizeNodes(graph *g);
void graphNormalizeScale(graph *g);
void addGraphEdge(graph *g,int a,int b);
void addGraphEdgesDist(graph *g,double d1,double d2);
graph *copyGraph(graph *g);
void graphComputeCwCcw(graph *g);
int *graphBipartition(graph *g);
graph *truncateGraph(graph *g,double d,bool relative);
graph *swirlifyGraph(graph *g,double d,bool relative);
