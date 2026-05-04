/*
====================================================
  VILLAGE SCENE - Computer Graphics Project
  Author  : [Your Name]
  Course  : Computer Graphics Lab

  COMPILE & RUN (MSYS2 UCRT64 terminal):
  g++ village_scene.cpp -o village.exe -lfreeglut -lopengl32 -lglu32 && ./village.exe

  CRITERIA:
  [1] Primitives     : Points, Lines, Polygons, Circles
  [2] DDA Algorithm  : Beautiful perspective road
      Midpoint Circle: Sun
  [3] Transformations: Translation, Rotation, Scaling
  [4] Animation      : 4 birds flying at different heights
  [5] Theme          : Village Scene
====================================================
*/

#include <GL/glut.h>
#include <cmath>

const int W = 900, H = 600;

// ── NDC helpers ──────────────────────────────────
inline float px(float x){ return (x/W)*2.0f-1.0f; }
inline float py(float y){ return (y/H)*2.0f-1.0f; }

// ── Animation state ──────────────────────────────
// 4 birds: each has X, Y, direction, wing phase offset
float birdX[4]    = {-0.95f, -0.40f,  0.20f, -0.70f};
float birdY[4]    = { 0.78f,  0.68f,  0.82f,  0.72f};
float birdDir[4]  = {  1.0f,  -1.0f,   1.0f,  -1.0f};
float birdSpd[4]  = { 0.006f, 0.004f, 0.005f, 0.007f};
float wingPhase[4]= {  0.0f,  1.0f,   2.0f,   3.5f };

float wingAngle  =  0.0f;
float sunAngle   =  0.0f;
float cloudX     = -1.0f;

// ════════════════════════════════════════════════
// ALGORITHM 1 — DDA Line
// ════════════════════════════════════════════════
void ddaLine(float x1,float y1,float x2,float y2)
{
    float dx=x2-x1, dy=y2-y1;
    int steps=(int)(fabs(dx)>fabs(dy)?fabs(dx):fabs(dy));
    if(steps==0) return;
    float xi=dx/steps, yi=dy/steps;
    float x=x1, y=y1;
    glBegin(GL_POINTS);
    for(int i=0;i<=steps;i++){
        glVertex2f(px(x),py(y));
        x+=xi; y+=yi;
    }
    glEnd();
}

// thick DDA — draw multiple parallel shifted lines
void ddaThick(float x1,float y1,float x2,float y2,int thickness)
{
    float dx=x2-x1, dy=y2-y1;
    float len=sqrt(dx*dx+dy*dy);
    if(len==0) return;
    float nx=-dy/len, ny=dx/len;
    for(int t=-thickness/2;t<=thickness/2;t++){
        ddaLine(x1+nx*t, y1+ny*t, x2+nx*t, y2+ny*t);
    }
}

// ════════════════════════════════════════════════
// ALGORITHM 2 — Midpoint Circle
// ════════════════════════════════════════════════
void midpointCircle(int cx,int cy,int r)
{
    int x=0,y=r,d=1-r;
    auto plot8=[&](int a,int b){
        glVertex2f(px(cx+a),py(cy+b)); glVertex2f(px(cx-a),py(cy+b));
        glVertex2f(px(cx+a),py(cy-b)); glVertex2f(px(cx-a),py(cy-b));
        glVertex2f(px(cx+b),py(cy+a)); glVertex2f(px(cx-b),py(cy+a));
        glVertex2f(px(cx+b),py(cy-a)); glVertex2f(px(cx-b),py(cy-a));
    };
    glBegin(GL_POINTS);
    while(x<=y){ plot8(x,y); if(d<0)d+=2*x+3; else{d+=2*(x-y)+5;y--;} x++; }
    glEnd();
}

void filledCircle(float cx,float cy,float r,int segs=60)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=segs;i++){
        float a=i*2.0f*3.14159f/segs;
        glVertex2f(cx+r*cos(a),cy+r*sin(a));
    }
    glEnd();
}

// ════════════════════════════════════════════════
// SKY
// ════════════════════════════════════════════════
void drawSky()
{
    glBegin(GL_QUADS);
      glColor3f(0.40f,0.70f,0.95f);
      glVertex2f(-1,1); glVertex2f(1,1);
      glColor3f(0.75f,0.90f,1.0f);
      glVertex2f(1,py(200)); glVertex2f(-1,py(200));
    glEnd();
}

// ════════════════════════════════════════════════
// GROUND
// ════════════════════════════════════════════════
void drawGround()
{
    glColor3f(0.35f,0.70f,0.25f);
    glBegin(GL_QUADS);
      glVertex2f(-1,py(200)); glVertex2f(1,py(200));
      glVertex2f(1,-1);       glVertex2f(-1,-1);
    glEnd();
    glColor3f(0.28f,0.55f,0.18f);
    glBegin(GL_QUADS);
      glVertex2f(-1,py(30)); glVertex2f(1,py(30));
      glVertex2f(1,-1);      glVertex2f(-1,-1);
    glEnd();
}

// ════════════════════════════════════════════════
// RIVER — bigger with sandy banks
// ════════════════════════════════════════════════
void drawRiver()
{
    // Sandy banks
    glColor3f(0.85f,0.78f,0.50f);
    glBegin(GL_QUADS);
      glVertex2f(px(0),   py(158));
      glVertex2f(px(315), py(158));
      glVertex2f(px(345), py(0));
      glVertex2f(px(-18), py(0));
    glEnd();

    // Main water body
    glColor3f(0.18f,0.52f,0.88f);
    glBegin(GL_QUADS);
      glVertex2f(px(18),  py(148));
      glVertex2f(px(298), py(148));
      glVertex2f(px(328), py(6));
      glVertex2f(px(-5),  py(6));
    glEnd();

    // Lighter shimmer strip in middle
    glColor3f(0.42f,0.72f,1.0f);
    glBegin(GL_QUADS);
      glVertex2f(px(75),  py(132));
      glVertex2f(px(225), py(132));
      glVertex2f(px(248), py(55));
      glVertex2f(px(55),  py(55));
    glEnd();

    // Ripple lines (DDA)
    glColor3f(0.62f,0.84f,1.0f);
    glPointSize(1.8f);
    for(int i=0;i<7;i++){
        int ry=20+i*17;
        ddaLine(22+i*4, ry,   155+i*4, ry+3);
        ddaLine(165+i*3, ry+2, 290+i*2, ry+5);
    }
}

// ════════════════════════════════════════════════
// MOUNTAINS
// ════════════════════════════════════════════════
void drawMountains()
{
    glColor3f(0.45f,0.60f,0.50f);
    glBegin(GL_TRIANGLES);
      glVertex2f(px(0),  py(200)); glVertex2f(px(210),py(200)); glVertex2f(px(105),py(420));
      glVertex2f(px(140),py(200)); glVertex2f(px(380),py(200)); glVertex2f(px(260),py(400));
    glEnd();
    glColor3f(0.55f,0.68f,0.58f);
    glBegin(GL_TRIANGLES);
      glVertex2f(px(50),py(200)); glVertex2f(px(240),py(200)); glVertex2f(px(145),py(380));
    glEnd();
    glColor3f(0.96f,0.96f,1.0f);
    glBegin(GL_TRIANGLES);
      glVertex2f(px(85), py(375)); glVertex2f(px(125),py(375)); glVertex2f(px(105),py(420));
      glVertex2f(px(240),py(358)); glVertex2f(px(280),py(358)); glVertex2f(px(260),py(400));
    glEnd();
}

// ════════════════════════════════════════════════
// SUN
// ════════════════════════════════════════════════
void drawSun()
{
    int cx=760, cy=540, r=48;
    glColor3f(1.0f,0.92f,0.30f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    for(int i=0;i<16;i++){
        float a=i*3.14159f/8.0f+sunAngle;
        glVertex2f(px(cx+(r+6)*cos(a)),  py(cy+(r+6)*sin(a)));
        glVertex2f(px(cx+(r+26)*cos(a)), py(cy+(r+26)*sin(a)));
    }
    glEnd();
    glColor3f(1.0f,0.88f,0.08f);
    filledCircle(px(cx),py(cy),48.0f/W*2.0f);
    glColor3f(1.0f,0.65f,0.0f);
    glPointSize(2.5f);
    midpointCircle(cx,cy,r);
    midpointCircle(cx,cy,r-3);
}

// ════════════════════════════════════════════════
// CLOUD
// ════════════════════════════════════════════════
void drawCloud(float ox,float oy)
{
    glColor3f(1,1,1);
    filledCircle(ox,        oy,       0.075f);
    filledCircle(ox+0.09f,  oy-0.01f, 0.058f);
    filledCircle(ox-0.09f,  oy-0.01f, 0.058f);
    filledCircle(ox+0.05f,  oy+0.045f,0.052f);
    filledCircle(ox-0.04f,  oy+0.040f,0.045f);
}

// ════════════════════════════════════════════════
// ROAD — Beautiful DDA perspective road
//   Vanishing point at centre-horizon
//   Grey tarmac + white kerbs + yellow centre dash + lane markers
// ════════════════════════════════════════════════
void drawRoad()
{
    int vpx=450, vpy=202;
    int blx=285, brx=615;   // bottom left/right at y=0

    // Tarmac — fill using horizontal DDA slices
    glColor3f(0.36f,0.36f,0.38f);
    glPointSize(1.5f);
    for(int y=0;y<=200;y++){
        float t=(float)y/200.0f;
        float lx=blx + t*(vpx-blx);
        float rx=brx + t*(vpx-brx);
        ddaLine(lx, y, rx, y);
    }

    // Kerb edges — white thick DDA lines
    glColor3f(0.92f,0.92f,0.92f);
    glPointSize(2.5f);
    ddaThick(blx, 0, vpx, vpy, 4);
    ddaThick(brx, 0, vpx, vpy, 4);

    // Pavement on right side (lighter grey)
    glColor3f(0.58f,0.56f,0.54f);
    glPointSize(1.5f);
    for(int y=0;y<=200;y++){
        float t=(float)y/200.0f;
        float rx=brx+t*(vpx-brx);
        float ex=rx+18.0f*(1.0f-t);
        ddaLine(rx, y, ex, y);
    }

    // Yellow centre dashed line
    glColor3f(0.98f,0.85f,0.08f);
    glPointSize(3.0f);
    for(int seg=0;seg<12;seg++){
        float y1=seg*17.0f, y2=y1+9.0f;
        if(y2>200) y2=200;
        float t1=y1/200.0f, t2=y2/200.0f;
        float lx1=blx+t1*(vpx-blx), rx1=brx+t1*(vpx-brx);
        float lx2=blx+t2*(vpx-blx), rx2=brx+t2*(vpx-brx);
        float cx1=lx1+(rx1-lx1)*0.5f;
        float cx2=lx2+(rx2-lx2)*0.5f;
        ddaLine(cx1,y1,cx2,y2);
    }

    // White lane markers
    glColor3f(0.82f,0.82f,0.82f);
    glPointSize(2.0f);
    for(int seg=0;seg<12;seg++){
        float y1=seg*17.0f, y2=y1+8.0f;
        if(y2>200) break;
        float t1=y1/200.0f, t2=y2/200.0f;
        float lx1=blx+t1*(vpx-blx), rx1=brx+t1*(vpx-brx);
        float lx2=blx+t2*(vpx-blx), rx2=brx+t2*(vpx-brx);
        // left quarter marker
        ddaLine(lx1+(rx1-lx1)*0.25f, y1, lx2+(rx2-lx2)*0.25f, y2);
        // right quarter marker
        ddaLine(lx1+(rx1-lx1)*0.75f, y1, lx2+(rx2-lx2)*0.75f, y2);
    }
}

// ════════════════════════════════════════════════
// HOUSE — by=200 so bottom sits on ground
// ════════════════════════════════════════════════
void drawHouse(float bx,float by,float bw,float bh,
               float r,float g,float b)
{
    // Wall
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
      glVertex2f(px(bx),    py(by));
      glVertex2f(px(bx+bw), py(by));
      glVertex2f(px(bx+bw), py(by+bh));
      glVertex2f(px(bx),    py(by+bh));
    glEnd();
    // Roof
    glColor3f(r*0.72f, g*0.60f, b*0.55f);
    glBegin(GL_TRIANGLES);
      glVertex2f(px(bx-8),       py(by+bh));
      glVertex2f(px(bx+bw+8),    py(by+bh));
      glVertex2f(px(bx+bw/2.0f), py(by+bh+bw*0.52f));
    glEnd();
    // Door
    glColor3f(0.38f,0.22f,0.08f);
    float dw=bw*0.20f, dh=bh*0.38f;
    float dx2=bx+bw/2.0f-dw/2.0f;
    glBegin(GL_QUADS);
      glVertex2f(px(dx2),    py(by));
      glVertex2f(px(dx2+dw), py(by));
      glVertex2f(px(dx2+dw), py(by+dh));
      glVertex2f(px(dx2),    py(by+dh));
    glEnd();
    // Door knob
    glColor3f(0.9f,0.7f,0.1f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
      glVertex2f(px(dx2+dw*0.78f), py(by+dh*0.50f));
    glEnd();
    // Windows
    glColor3f(0.55f,0.82f,1.0f);
    float wy=by+bh*0.50f, wh=bh*0.24f, ww=bw*0.17f;
    glBegin(GL_QUADS);
      glVertex2f(px(bx+bw*0.10f),    py(wy));
      glVertex2f(px(bx+bw*0.10f+ww), py(wy));
      glVertex2f(px(bx+bw*0.10f+ww), py(wy+wh));
      glVertex2f(px(bx+bw*0.10f),    py(wy+wh));
    glEnd();
    glBegin(GL_QUADS);
      glVertex2f(px(bx+bw*0.73f),    py(wy));
      glVertex2f(px(bx+bw*0.73f+ww), py(wy));
      glVertex2f(px(bx+bw*0.73f+ww), py(wy+wh));
      glVertex2f(px(bx+bw*0.73f),    py(wy+wh));
    glEnd();
    // Window cross bars
    glColor3f(0.55f,0.70f,0.80f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
      glVertex2f(px(bx+bw*0.10f+ww/2), py(wy));
      glVertex2f(px(bx+bw*0.10f+ww/2), py(wy+wh));
      glVertex2f(px(bx+bw*0.10f),      py(wy+wh/2));
      glVertex2f(px(bx+bw*0.10f+ww),   py(wy+wh/2));
      glVertex2f(px(bx+bw*0.73f+ww/2), py(wy));
      glVertex2f(px(bx+bw*0.73f+ww/2), py(wy+wh));
      glVertex2f(px(bx+bw*0.73f),      py(wy+wh/2));
      glVertex2f(px(bx+bw*0.73f+ww),   py(wy+wh/2));
    glEnd();
}

// ════════════════════════════════════════════════
// TREE
// ════════════════════════════════════════════════
void drawTree(float tx,float ty,float scale)
{
    glColor3f(0.42f,0.26f,0.06f);
    glLineWidth(4.0f*scale);
    glBegin(GL_LINES);
      glVertex2f(px(tx),py(ty));
      glVertex2f(px(tx),py(ty+60*scale));
    glEnd();
    float greens[3][3]={{0.10f,0.55f,0.17f},{0.14f,0.63f,0.21f},{0.19f,0.70f,0.27f}};
    for(int i=0;i<3;i++){
        glColor3f(greens[i][0],greens[i][1],greens[i][2]);
        float baseY=ty+35*scale+i*24*scale;
        float hw=(52-i*13)*scale;
        glBegin(GL_TRIANGLES);
          glVertex2f(px(tx-hw),py(baseY));
          glVertex2f(px(tx+hw),py(baseY));
          glVertex2f(px(tx),   py(baseY+50*scale));
        glEnd();
    }
}

// ════════════════════════════════════════════════
// FENCE
// ════════════════════════════════════════════════
void drawFence()
{
    glColor3f(0.75f,0.55f,0.28f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
      glVertex2f(px(555),py(245)); glVertex2f(px(885),py(245));
      glVertex2f(px(555),py(222)); glVertex2f(px(885),py(222));
    glEnd();
    for(int i=0;i<=8;i++){
        int fx=555+i*42;
        glLineWidth(3.0f);
        glColor3f(0.75f,0.55f,0.28f);
        glBegin(GL_LINES);
          glVertex2f(px(fx),py(200)); glVertex2f(px(fx),py(258));
        glEnd();
        glColor3f(0.90f,0.68f,0.35f);
        glPointSize(7.0f);
        glBegin(GL_POINTS);
          glVertex2f(px(fx),py(261));
        glEnd();
    }
}

// ════════════════════════════════════════════════
// BIRD
// ════════════════════════════════════════════════
void drawBird(float bx,float by,float flap,float dir,float size)
{
    glColor3f(0.06f,0.06f,0.10f);
    glLineWidth(2.2f*size);
    float ws=0.048f*size;
    float lx=bx-ws*cos(flap), ly=by+ws*sin(flap);
    float rx=bx+ws*cos(flap), ry=by+ws*sin(flap);
    glBegin(GL_LINE_STRIP);
      glVertex2f(lx,ly); glVertex2f(bx,by); glVertex2f(rx,ry);
    glEnd();
    glBegin(GL_LINES);
      glVertex2f(bx-dir*0.010f, by);
      glVertex2f(bx-dir*0.028f, by-0.008f*size);
    glEnd();
    glPointSize(5.0f*size);
    glBegin(GL_POINTS);
      glVertex2f(bx+dir*0.016f, by+0.007f*size);
    glEnd();
}

// ════════════════════════════════════════════════
// TRANSFORMATION DEMO
// ════════════════════════════════════════════════
void drawTransformDemo()
{
    glColor3f(0.9f,0.4f,0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
      glVertex2f(px(702),py(42)); glVertex2f(px(728),py(42));
      glVertex2f(px(728),py(68)); glVertex2f(px(702),py(68));
    glEnd();
    glColor3f(1.0f,0.6f,0.1f);
    glBegin(GL_LINE_LOOP);
      glVertex2f(px(740),py(28)); glVertex2f(px(792),py(28));
      glVertex2f(px(792),py(80)); glVertex2f(px(740),py(80));
    glEnd();
    glColor3f(0.15f,0.15f,0.15f);
    glRasterPos2f(px(700),py(18));
    const char* lbl="Scale x2 + Translate";
    for(const char* c=lbl;*c;c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*c);
}

// ════════════════════════════════════════════════
// DISPLAY
// ════════════════════════════════════════════════
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawMountains();
    drawGround();
    drawRiver();
    drawRoad();
    drawSun();

    drawCloud(cloudX,       0.83f);
    drawCloud(cloudX+0.52f, 0.89f);
    drawCloud(cloudX+1.05f, 0.81f);

    // Houses — by=200 means bottom edge is exactly on ground line
    drawHouse(530, 200, 165, 115, 0.92f,0.80f,0.60f);
    drawHouse(710, 200, 135, 100, 0.85f,0.72f,0.56f);
    drawHouse(238, 200, 115,  90, 0.80f,0.66f,0.52f);

    drawFence();

    drawTree(498, 200, 1.30f);
    drawTree(632, 200, 1.10f);
    drawTree(128, 200, 1.40f);
    drawTree(172, 200, 1.05f);
    drawTree(822, 200, 1.20f);
    drawTree(858, 200, 0.95f);

    drawTransformDemo();

    // 4 birds — different sizes and flap offsets so they look natural
    drawBird(birdX[0],birdY[0],sin(wingAngle+wingPhase[0])*0.45f,birdDir[0],1.00f);
    drawBird(birdX[1],birdY[1],sin(wingAngle+wingPhase[1])*0.45f,birdDir[1],0.80f);
    drawBird(birdX[2],birdY[2],sin(wingAngle+wingPhase[2])*0.45f,birdDir[2],0.90f);
    drawBird(birdX[3],birdY[3],sin(wingAngle+wingPhase[3])*0.45f,birdDir[3],0.70f);

    // Legend
    glColor3f(0.05f,0.05f,0.35f);
    glRasterPos2f(px(8),py(592));
    const char* t="Village Scene | DDA Road | Midpoint Circle Sun | 4 Birds Animation";
    for(const char* c=t;*c;c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*c);

    glutSwapBuffers();
}

// ════════════════════════════════════════════════
// TIMER
// ════════════════════════════════════════════════
void timer(int)
{
    for(int i=0;i<4;i++){
        birdX[i]+=birdDir[i]*birdSpd[i];
        if(birdX[i]> 1.15f) birdDir[i]=-1.0f;
        if(birdX[i]<-1.15f) birdDir[i]= 1.0f;
    }
    wingAngle += 0.10f;
    sunAngle  += 0.006f;
    cloudX    += 0.0007f;
    if(cloudX>1.25f) cloudX=-1.25f;
    glutPostRedisplay();
    glutTimerFunc(16,timer,0);
}

// ════════════════════════════════════════════════
// MAIN
// ════════════════════════════════════════════════
int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(W,H);
    glutInitWindowPosition(100,80);
    glutCreateWindow("Village Scene - Computer Graphics Project");
    glClearColor(0.40f,0.70f,0.95f,1.0f);
    glutDisplayFunc(display);
    glutTimerFunc(16,timer,0);
    glutMainLoop();
    return 0;
}

