#include "main.h"

bool quit=false;
bool constructing=false;
bool constructingf=false;

bool stickyPoints=false;

int iSelected=-1;
bool fSelected;

int iClosest=-1;
double tClosest;
point pClosest;

int mousex=0,mousey=0;
point mousep;

bool replicateActions=false;
#ifdef ALTERNATE_REPLICAS
bool replicasAlternate=true;
#else
bool replicasAlternate=false;
#endif

int polyType=0;
int polyNum=0;

void FlushEvents(){
	SDL_Event event;
	while(SDL_PollEvent(&event));
}

void createLineReplicas(point p){
    point a={0,0,1};
    point q;
    point pp;
    pp.x=-p.x; pp.y=-p.y; pp.z=-p.z;
    for(int i=REPLICAS-1;i>=0;i--){
        copyPoint(pp,&q);
        rotateAroundAxis(a,M_PI*2*i/REPLICAS,&q);
        if(replicasAlternate){
            point qq;
            copyPoint(q,&qq);
            rotateAroundAxis(a,M_PI/REPLICAS,&qq);
            newLine(newPointP(qq),newPointP(qq));
        }
        else newLine(newPointP(q),newPointP(q));
        if(i==0)break;
        scale(&q,-1);
        newLine(newPointP(q),newPointP(q));
    }
    newLine(newPointP(p),newPointP(p));
}

void replicateCoords(bool b){
    point a={0,0,1};
    point *p=b?&lines[n-1].p1->p:&lines[n-1].p2->p;
    point q;
    point pp;
    pp.x=-p->x; pp.y=-p->y; pp.z=-p->z;
    for(int i=REPLICAS-1;i>=0;i--){
        copyPoint(pp,&q);
        rotateAroundAxis(a,M_PI*2*i/REPLICAS,&q);
        if(replicasAlternate){
            point qq;
            copyPoint(q,&qq);
            rotateAroundAxis(a,M_PI/REPLICAS,&qq);
            copyPoint(qq,b?&lines[n-2-2*i].p1->p:&lines[n-2-2*i].p2->p);
        }
        else copyPoint(q,b?&lines[n-2-2*i].p1->p:&lines[n-2-2*i].p2->p);
        if(i==0)break;
        scale(&q,-1);
        copyPoint(q,b?&lines[n-1-2*i].p1->p:&lines[n-1-2*i].p2->p);
    }
}

void bringForwardReplicas(int j){
    int b1=j/(2*REPLICAS)*2*REPLICAS;
    int b2=n-2*REPLICAS;
    if(j%2==0){
        for(int i=0;i<REPLICAS;i++)switchLines(b1+i*2,b1+i*2+1);
        j++;
    }
    int ofs=2*REPLICAS-1-j+b1;
    for(int i=0;i<2*REPLICAS;i++)switchLines(b1+i,b2+i);
    for(int k=0;k<ofs;k++)for(int i=0;i<2*REPLICAS-1;i++)switchLines(b2+i,n-1);
}

void deleteReplicas(int j){
    int b1=j/(2*REPLICAS)*2*REPLICAS;
    int b2=n-2*REPLICAS;
    for(int i=0;i<2*REPLICAS;i++)switchLines(b1+i,b2+i);
    for(int i=0;i<2*REPLICAS;i++)deleteLine(n-1);
}

void setPoly(bool b){
    switch(polyType){
    case 0:
        polyNum=0;
        saveLines(n);
        freeLines();
        break;
    case 1:
        if(b)polyNum=2;
        if(polyNum<2)polyNum=2;
        if(polyNum>9)polyNum=9;
        saveLines(n);
        buildPrism(polyNum);
        break;
    case 2:
        if(b)polyNum=6;
        if(polyNum<3)polyNum=3;
        if(polyNum>10)polyNum=10;
        saveLines(n);
        buildTAntiprism(polyNum);
        break;
    case 3:
        if(b)polyNum=4;
        if(polyNum<2)polyNum=2;
        if(polyNum>8)polyNum=8;
        saveLines(n);
        buildTBipyramid(polyNum);
        break;
    case 4:
        if(b)polyNum=6;
        if(polyNum<3)polyNum=3;
        if(polyNum>20)polyNum=20;
        saveLines(n);
        buildTrapezohedron(polyNum);
        break;
    case 5:
        if(b)polyNum=0;
        if(polyNum<0)polyNum=5;
        if(polyNum>5)polyNum=0;
        saveLines(n);
        switch(polyNum){
        case 0:
            buildRhombicDodecahedron();
            break;
        case 1:
            buildDeltoidalIcositetrahedron();
            break;
        case 2:
            buildRhombicTriancontahedron();
            break;
        case 3:
            buildDeltoidalHexecontahedron();
            break;
        case 4:
            buildTCuboctahedron();
            break;
        case 5:
            buildTIcosidodecahedron();
            break;
        }
        break;
    case 6:
        if(b)polyNum=4;
        if(polyNum<4)polyNum=4;
        if(polyNum>9)polyNum=9;
        saveLines(n);
        buildTwistedPrism(polyNum);
        break;
    case 7:
        polyNum=0;
        saveLines(n);
        loadDiagram("SavedDiagram5!.txt",true);
        break;
    case 8:
        polyNum=0;
        saveLines(n);
        buildCustomDiagram();
        break;
    default:
        break;
    }
}

void KeyPressed(SDL_Keycode key){
    switch(key){
    case SDLK_ESCAPE:
        if(constructing){
            constructing=false;
            restoreLines();
            iSelected=iClosest=-1;
        }
        break;
    case SDLK_BACKSPACE:
        constructing=false;
        restoreLines();
        iSelected=iClosest=-1;
        break;
    case SDLK_DELETE:
        if((!constructing)&&(iClosest>=0)){
            saveLines(n);
            if(replicateActions)deleteReplicas(iClosest);
            else deleteLine(iClosest);
            iSelected=iClosest=-1;
            connectLines(true);
        }
        break;
    case SDLK_RETURN:
        saveSVG();
        break;
    case SDLK_q:
        quit=true;
        break;
    case SDLK_d:
        if(!drawMercator)toggleDoubleDisplay();
        break;
    case SDLK_s:
        stickyPoints=!stickyPoints;
        break;
    case SDLK_b:
        drawSphere=!drawSphere;
        break;
    case SDLK_h:
        drawHidden=!drawHidden;
        break;
    case SDLK_v:
        drawVertices=!drawVertices;
        break;
    case SDLK_p:
        drawPoles=!drawPoles;
        break;
    case SDLK_m:
        drawMeridian=!drawMeridian;
        break;
    case SDLK_c:
        drawClosest=!drawClosest;
        break;
    case SDLK_t:
        drawTypes++;
        if(drawTypes>3)drawTypes=0;
        break;
    case SDLK_a:
        drawAuxiliary=!drawAuxiliary;
        break;
    case SDLK_e:
        drawMercator=!drawMercator;
        if(drawMercator!=doubleDisplay)toggleDoubleDisplay();
        break;
    case SDLK_r:
        replicateActions=!replicateActions;
        if(n%(2*REPLICAS))replicateActions=false;
        break;
    case SDLK_MINUS:
    case SDLK_KP_MINUS:
        snapDistance/=1.1;
        break;
    case SDLK_EQUALS:
    case SDLK_KP_PLUS:
        snapDistance*=1.1;
        break;
    case SDLK_DOWN:
        rotateView(0,-10);
        break;
    case SDLK_UP:
        rotateView(0,10);
        break;
    case SDLK_LEFT:
        rotateView(-10,0);
        break;
    case SDLK_RIGHT:
        rotateView(10,0);
        break;
    case SDLK_SPACE:
        break;
    case SDLK_F1:
        if(!constructing){
            saveLines(n);
            loadDiagram(filename,true);
        }
        break;
    case SDLK_F2:
        if(!constructing)saveDiagram(filename);
        break;
    case SDLK_0...SDLK_9:
        if(!constructing){
            if(polyType!=key-SDLK_0){
                polyType=key-SDLK_0;
                setPoly(true);
            }
            else setPoly(false);
        }
        break;
    default:
        break;
    }
}

void MousePressed(SDL_MouseButtonEvent *button){
    SDL_CaptureMouse(SDL_TRUE);
    if(button->button==SDL_BUTTON_RIGHT){
        if(iSelected>=0){
            saveLines(n);
            constructing=true;
            if(replicateActions)bringForwardReplicas(iSelected);
            else switchLines(iSelected,n-1);
            constructingf=fSelected;
            iSelected=iClosest=-1;
        }
        else{
            constructing=!constructing;
            if(constructing){
                saveLines(n);
                point p;
                if(!screen2sphere(button->x,button->y,&p,true))normalize(&p);
                if(replicateActions)createLineReplicas(p);
                else newLine(newPointP(p),newPointP(p));
                constructingf=false;
            }
            else connectLines(true);
        }
    }
}

void MouseMoved(SDL_MouseMotionEvent *motion){
    if(motion->state & SDL_BUTTON_LMASK){
        const Uint8 *state=SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_LCTRL])rotateLines(-motion->xrel,motion->yrel);
        else rotateView(motion->xrel,-motion->yrel);
    }
}

void StaticEvents(){
    iSelected=-1;
    SDL_GetMouseState(&mousex,&mousey);
    if(!screen2sphere(mousex,mousey,&mousep,true))normalize(&mousep);
    if(constructing){
        copyPoint(mousep,constructingf?&lines[n-1].p1->p:&lines[n-1].p2->p);
        if(replicateActions)replicateCoords(constructingf);
        if(stickyPoints)connectLines(true);
    }
    else findClosestEndpoint(mousex,mousey,&iSelected,&fSelected);
    tClosest=closestToLines(mousep,constructing?n-1:-1,&pClosest,&iClosest);
    computeLineTypes();
}

void Events(){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
        case SDL_QUIT:
            quit=true;
            break;
        case SDL_KEYDOWN:
            KeyPressed(e.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            MousePressed(&e.button);
            break;
        case SDL_MOUSEMOTION:
            MouseMoved(&e.motion);
            break;
        case SDL_MOUSEWHEEL:
            if(polyType && e.wheel.y!=0){
                if(e.wheel.y>0)polyNum++;
                else polyNum--;
                setPoly(false);
            }
            break;
        }
	}
	StaticEvents();
}
