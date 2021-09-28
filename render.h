#define SVG_SCALE (0.25)
#define SVG_STROKE1 (0.67) // (0.25)
#define SVG_STROKE2 (1.0) // (0.5)
#define SVG_DOT (7.5)

extern bool savingSVG;

#define grain (wins/8)
#define DOT_SIZE (5.0)

extern bool doubleDisplay;
extern bool drawSphere;
extern bool drawHidden;
extern bool drawVertices;
extern bool drawPoles;
extern bool drawMeridian;
extern bool drawClosest;
extern bool drawAuxiliary;
extern bool drawMercator;
extern int drawTypes;

extern point eye,up,right;

void setWindowSize();
void toggleDoubleDisplay();
bool sphere2screen(point a,double *x,double *y);
bool screen2sphere(int x,int y,point *a,bool front);
void resetRotation();
void rotateView(double x,double y);
void drawColor(int r,int g,int b);
void drawCircle(double x,double y,double r,bool full);
void drawLine(double x1,double y1,double x2,double y2,bool thick,bool poly);
void endPolyLine(bool thick);
void Render();
void saveSVG();
