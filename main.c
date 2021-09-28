#include "main.h"

SDL_Window *window;
SDL_Renderer *renderer;
int wins=1601;

char *filename="SavedDiagram.txt";

void Done(){
    freeLines();
    freeLinesSaved();
}

void Init(){
    Done();
    resetRotation();
    initAux();
}

void loadDiagram(const char *filename,bool loadView){
    FILE *f;
    char buf[1024];
    point p;
    endpoint e1,e2;
    double d;
    int k,a,b;
    f=fopen(filename,"rb");
    if(!f)return;
    freeLines();
    while(!feof(f)){
        fgets(buf,sizeof(buf),f);
        if(loadView&&sscanf(buf,"eye = (%lf, %lf, %lf)",&p.x,&p.y,&p.z)==3)copyPoint(p,&eye);
        else if(loadView&&sscanf(buf,"up = (%lf, %lf, %lf)",&p.x,&p.y,&p.z)==3)copyPoint(p,&up);
        else if(loadView&&sscanf(buf,"right = (%lf, %lf, %lf)",&p.x,&p.y,&p.z)==3)copyPoint(p,&right);
        else if(sscanf(buf,"line %d",&k)==1){
            while(!feof(f)){
                fgets(buf,sizeof(buf),f);
                if(sscanf(buf,"point 1 = (%lf, %lf, %lf)",&p.x,&p.y,&p.z)==3)copyPoint(p,&e1.p);
                else if(sscanf(buf,"point 2 = (%lf, %lf, %lf)",&p.x,&p.y,&p.z)==3)copyPoint(p,&e2.p);
                else if(sscanf(buf,"reference 1 = (%d, %lf)",&k,&d)==2){e1.i=k;e1.t=d;}
                else if(sscanf(buf,"reference 2 = (%d, %lf)",&k,&d)==2){e2.i=k;e2.t=d;}
                else if(sscanf(buf,"end %d",&k)==1){newLine(newPointE(&e1),newPointE(&e2));break;}
            }
        }
        else if(sscanf(buf,"graph %d",&k)==1){
            displayGraph=newGraph();
            while(!feof(f)){
                fgets(buf,sizeof(buf),f);
                if(sscanf(buf,"node %d = (%lf, %lf, %lf)",&k,&p.x,&p.y,&p.z)==4)addGraphNode(displayGraph,p.x,p.y,p.z,false);
                else if(sscanf(buf,"edge %d = (%d, %d)",&k,&a,&b)==3)addGraphEdge(displayGraph,a,b);
                else if(sscanf(buf,"end %d",&k)==1)break;
            }
        }
    }
    if(displayGraph)graphComputeCwCcw(displayGraph);
    fclose(f);
}

void saveDiagram(const char *filename){
    FILE *f;
    f=fopen(filename,"w");
    if(!f)return;
    fprintf(f,"eye = (%.17g, %.17g, %.17g)\n",eye.x,eye.y,eye.z);
    fprintf(f,"up = (%.17g, %.17g, %.17g)\n",up.x,up.y,up.z);
    fprintf(f,"right = (%.17g, %.17g, %.17g)\n",right.x,right.y,right.z);
    for(int i=0;i<n;i++){
        fprintf(f,"\nline %d\n",i);
        fprintf(f,"point 1 = (%.17g, %.17g, %.17g)\n",lines[i].p1->p.x,lines[i].p1->p.y,lines[i].p1->p.z);
        fprintf(f,"point 2 = (%.17g, %.17g, %.17g)\n",lines[i].p2->p.x,lines[i].p2->p.y,lines[i].p2->p.z);
        fprintf(f,"reference 1 = (%d, %.17g)\n",lines[i].p1->i,lines[i].p1->t);
        fprintf(f,"reference 2 = (%d, %.17g)\n",lines[i].p2->i,lines[i].p2->t);
        fprintf(f,"end %d\n",i);
    }
    if(displayGraph){
        fprintf(f,"\ngraph %d\n",displayGraph->n);
        for(int i=0;i<displayGraph->n;i++)fprintf(f,"node %d = (%.17g, %.17g, %.17g)\n",i,displayGraph->v[i].x,displayGraph->v[i].y,displayGraph->v[i].z);
        for(int i=0;i<displayGraph->m;i++)fprintf(f,"edge %d = (%d, %d)\n",i,displayGraph->e[2*i],displayGraph->e[2*i+1]);
        fprintf(f,"end %d\n",displayGraph->n);
    }
    fclose(f);
}

void enableHiDPI(){
    typedef enum{
        PROCESS_DPI_UNAWARE=0,
        PROCESS_SYSTEM_DPI_AWARE=1,
        PROCESS_PER_MONITOR_DPI_AWARE=2
    }PROCESS_DPI_AWARENESS;
    void *shcoreDLL=SDL_LoadObject("SHCORE.DLL");
    if(shcoreDLL){
        HRESULT(WINAPI *SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness);
        SetProcessDpiAwareness=(HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS))SDL_LoadFunction(shcoreDLL,"SetProcessDpiAwareness");
        if(SetProcessDpiAwareness){
            SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            return;
        }
    }
    void *userDLL=SDL_LoadObject("USER32.DLL");
    if(userDLL){
        BOOL(WINAPI *SetProcessDPIAware)(void);
        SetProcessDPIAware=(BOOL(WINAPI*)(void))SDL_LoadFunction(userDLL,"SetProcessDPIAware");
        if(SetProcessDPIAware)SetProcessDPIAware();
    }
}

int WindowEventWatcher(void *data,SDL_Event *event){
    if(event->type==SDL_WINDOWEVENT&&(event->window.event==SDL_WINDOWEVENT_RESIZED||event->window.event==SDL_WINDOWEVENT_MOVED)){
        SDL_Window *win=SDL_GetWindowFromID(event->window.windowID);
        if(win==(SDL_Window*)data){
            if(event->window.event==SDL_WINDOWEVENT_RESIZED){
                wins=event->window.data2;
                if(wins<201)wins=201;
                if(wins%2==0)wins++;
                setWindowSize();
            }
            Render();
        }
    }
    return 0;
}

int main(int argc,char *argv[]){
    if(argc>1)wins=atoi(argv[1]);
    if(wins<201)wins=201;
    if(wins>2001)wins=2001;
    if(wins%2==0)wins++;
    enableHiDPI();
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
    window=SDL_CreateWindow("Spherical Occlusion Diagrams",25,SDL_WINDOWPOS_CENTERED,wins,wins,SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    SDL_AddEventWatch(WindowEventWatcher,window);
    Init();
    Render();
    FlushEvents();
    while(!quit){
        Events();
        Render();
    }
    Done();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
