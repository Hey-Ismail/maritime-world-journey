#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1416f


// Scene select: 1, 2, 3, 4
int currentScene = 1;

// Shared ship
float shipX      = -220.0f;  // Scene 1/2/3
float shipX4     = -1.2f;    // Scene 4
float shipSpeed  = 5.0f;
float shipSpeed4 = 0.007f;
int   shipDir    = 1;        // +1 left->right, -1 right->left
bool  shipMoving = false;
int ship4TripCount = 0;   // 0 = not yet started; odd = left→right; even = right→left


// Circle for Scene 1 & Scene 4
void circle(float radius, float xc, float yc, float r, float g, float b)
{
    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    for (int i = 0; i < 200; i++) {
        float angle = (i * 2 * PI) / 200;
        glVertex2f(radius * cos(angle) + xc, radius * sin(angle) + yc);
    }
    glEnd();
}

void resetShipForScene(int scene)
{
    shipMoving = false;
    if (scene == 1) {
        shipDir = 1;
        shipX   = -220.0f;
        shipX4  = -1.2f;
    } else if (scene == 2) {
        shipDir = -1;
        shipX   = 1600.0f;
        shipX4  =  1.2f;
    } else if (scene == 3) {
        shipDir = 1;
        shipX   = -220.0f;
        shipX4  = -1.2f;
    } else {          // scene 4 — reset without changing trip count
        // direction & position are set at click time, not here
    }
}

void drawShipShape()
{
    // ===== HULL — symmetric, both ends taper equally =====
    glBegin(GL_POLYGON);
    glColor3f(0.78f, 0.80f, 0.82f);
    glVertex2f(-280.0f,  0.0f);   // left tip
    glVertex2f(-250.0f, 20.0f);   // left upper shoulder
    glVertex2f( 250.0f, 20.0f);   // right upper shoulder
    glVertex2f( 280.0f,  0.0f);   // right tip
    glVertex2f( 250.0f,-20.0f);   // right lower shoulder
    glVertex2f(-250.0f,-20.0f);   // left lower shoulder
    glEnd();

    // ===== RED WATERLINE STRIPE =====
    glBegin(GL_POLYGON);
    glColor3f(0.75f, 0.18f, 0.18f);
    glVertex2f(-278.0f,  -5.0f);
    glVertex2f(-250.0f, -12.0f);
    glVertex2f( 250.0f, -12.0f);
    glVertex2f( 278.0f,  -5.0f);
    glVertex2f( 250.0f,   2.0f);
    glVertex2f(-250.0f,   2.0f);
    glEnd();

    // ===== DECK 1 (main body) =====
    glBegin(GL_QUADS);
    glColor3f(0.90f, 0.91f, 0.93f);
    glVertex2f(-220.0f, 20.0f);
    glVertex2f( 220.0f, 20.0f);
    glVertex2f( 220.0f, 55.0f);
    glVertex2f(-220.0f, 55.0f);
    glEnd();

    // ===== DECK 2 =====
    glBegin(GL_QUADS);
    glColor3f(0.84f, 0.87f, 0.90f);
    glVertex2f(-155.0f, 55.0f);
    glVertex2f( 155.0f, 55.0f);
    glVertex2f( 155.0f, 83.0f);
    glVertex2f(-155.0f, 83.0f);
    glEnd();

    // ===== BRIDGE — centered, flat =====
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.83f, 0.87f);
    glVertex2f(-75.0f, 83.0f);
    glVertex2f( 75.0f, 83.0f);
    glVertex2f( 75.0f,108.0f);
    glVertex2f(-75.0f,108.0f);
    glEnd();

    // ===== BRIDGE PORTHOLES — symmetric pairs =====
    glColor3f(0.35f, 0.62f, 0.78f);
    float bpx[] = {-52.0f, -24.0f, 24.0f, 52.0f};
    for (int i = 0; i < 4; i++) {
        glBegin(GL_POLYGON);
        for (int j = 0; j < 24; j++) {
            float a = j * 2.0f * PI / 24;
            glVertex2f(bpx[i] + 7.0f*cos(a), 95.0f + 7.0f*sin(a));
        }
        glEnd();
    }

    // ===== PORTHOLE WINDOWS — DECK 1, symmetric =====
    glColor3f(0.35f, 0.62f, 0.78f);
    float phx[] = {-190.0f, -155.0f, -120.0f, 120.0f, 155.0f, 190.0f};
    for (int i = 0; i < 6; i++) {
        glBegin(GL_POLYGON);
        for (int j = 0; j < 24; j++) {
            float a = j * 2.0f * PI / 24;
            glVertex2f(phx[i] + 8.0f*cos(a), 37.0f + 8.0f*sin(a));
        }
        glEnd();
    }

    // ===== DECK 2 WINDOWS — symmetric pairs =====
    glColor3f(0.35f, 0.62f, 0.78f);
    float dwx[] = {-140.0f, -112.0f, 112.0f, 134.0f};
    for (int i = 0; i < 4; i++) {
        glBegin(GL_QUADS);
        glVertex2f(dwx[i],       62.0f);
        glVertex2f(dwx[i]+18.0f, 62.0f);
        glVertex2f(dwx[i]+18.0f, 74.0f);
        glVertex2f(dwx[i],       74.0f);
        glEnd();
    }

    // ===== FUNNEL — centered =====
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.18f, 0.18f);
    glVertex2f(-20.0f, 55.0f);
    glVertex2f( 20.0f, 55.0f);
    glVertex2f( 20.0f, 95.0f);
    glVertex2f(-20.0f, 95.0f);
    glEnd();
    // Funnel black band
    glBegin(GL_QUADS);
    glColor3f(0.10f, 0.10f, 0.10f);
    glVertex2f(-20.0f, 63.0f);
    glVertex2f( 20.0f, 63.0f);
    glVertex2f( 20.0f, 71.0f);
    glVertex2f(-20.0f, 71.0f);
    glEnd();
    // Funnel top cap
    glBegin(GL_QUADS);
    glColor3f(0.60f, 0.12f, 0.12f);
    glVertex2f(-23.0f, 92.0f);
    glVertex2f( 23.0f, 92.0f);
    glVertex2f( 23.0f, 98.0f);
    glVertex2f(-23.0f, 98.0f);
    glEnd();

    // ===== MAST — centered =====
    glBegin(GL_QUADS);
    glColor3f(0.50f, 0.50f, 0.52f);
    glVertex2f( -2.5f,  95.0f);
    glVertex2f(  2.5f,  95.0f);
    glVertex2f(  2.5f, 135.0f);
    glVertex2f( -2.5f, 135.0f);
    glEnd();
    // Radar crossbar
    glBegin(GL_LINES);
    glColor3f(0.50f, 0.50f, 0.52f);
    glVertex2f(-28.0f, 128.0f);
    glVertex2f( 28.0f, 128.0f);
    glEnd();
}

void drawShip(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    drawShipShape();
    glPopMatrix();
}

void drawShip4(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.0012f, 0.0012f, 1.0f);
    drawShipShape();
    glPopMatrix();
}

// Scene 3 circle helper
void circle3(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * PI * i / segments;
        glVertex2f(cx + radius * cos(theta), cy + radius * sin(theta));
    }
    glEnd();
}

// Scene 1 variables
bool night1 = false;
int  boxX1 = 0, boxY1 = 0;
int  carYellow1 = 0;
int  carBlue1 = 0;

// Scene 3 variables
int   night3 = 0;
float carBlue3X = 1000.0f;
float bus3X = -700.0f;
int   car3On = 0;
float cloud3X1 = 0.0f;
float cloud3X2 = -300.0f;
float cloud3X3 = -600.0f;
const float scene3W = 1500.0f;
float blade3Angle = 0.0f;
float jet3Offset = 0.0f;
int   jets3On = 0;

// Scene 4 variables
float bird4X = -1.0f;
float bird4Speed = 0.01f;
int   wing4 = 0;
int   day4 = 1;

float cloud4X1 = -1.2f;
float cloud4X2 = -0.5f;
float cloud4X3 =  1.2f;
float cloud4Speed = 0.0025f;

float s4_boat1move    = 0.0f;
float s4_boat1speed   = 0.012f;
int   s4_boat1reverse = 0;

float s4_boat2move  = -1.2f;
float s4_boat2speed = 0.008f;

// old variable aliases kept so existing draw blocks stay unchanged
#define s1_night night1
#define s1_blx boxX1
#define s1_bly boxY1
#define s1_car carYellow1
#define s1_car1 carBlue1

#define s2_carPos car2GrayX
#define s2_bx car2RedX
#define s2_trainPos train2X
#define s2_trainMoving train2On
#define s2_cloud1X cloud2X1
#define s2_cloud2X cloud2X2
#define s2_isNight night2
#define s2_rainday rain2
#define s2_cloudSpeed cloud2Speed

#define s3_Night night3
#define s3_carPositionX carBlue3X
#define s3_car2 bus3X
#define s3_carRunning car3On
#define s3_cloud1X cloud3X1
#define s3_cloud2X cloud3X2
#define s3_cloud3X cloud3X3
#define s3_sceneW scene3W
#define s3_bladeAngle blade3Angle
#define s3_jetOffset jet3Offset
#define s3_jetsMoving jets3On

#define s4_birdMove bird4X
#define s4_birdSpeed bird4Speed
#define s4_wingState wing4
#define s4_isday day4
#define s4_cloud1x cloud4X1
#define s4_cloud2x cloud4X2
#define s4_cloud3x cloud4X3
#define s4_cloudSpd cloud4Speed

// Scene 4 cloud helper
void drawCloud4(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    circle(0.05f,  0.0f,  0.0f,  0.95f, 0.95f, 0.95f);
    circle(0.04f,  0.05f, 0.02f, 0.95f, 0.95f, 0.95f);
    circle(0.04f, -0.05f, 0.02f, 0.95f, 0.95f, 0.95f);
    circle(0.035f, 0.0f,  0.04f, 0.95f, 0.95f, 0.95f);
    glPopMatrix();
}

// Scene 3 blade helper
void drawBlade3(float cx, float cy, float length, float angle)
{
    float rad = angle * PI / 180.0f;
    float x1 = cx + length * cos(rad);
    float y1 = cy + length * sin(rad);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx, cy);
    glVertex2f(x1 - 5, y1);
    glVertex2f(x1 + 5, y1);
    glEnd();
}

// Scene 3 cloud helper
void drawCloud3(float x, float y)
{
    if (night3) glColor3ub(220, 220, 210);
    else          glColor3ub(255, 255, 255);
    circle3(x,       y,      40.0f, 50);
    circle3(x - 30,  y,      30.0f, 50);
    circle3(x + 30,  y,      30.0f, 50);
    circle3(x,       y + 20, 25.0f, 50);
}

// Scene 3 blue car
void drawCar3(void)
{
    glPushMatrix();
    glTranslatef(carBlue3X, 258.0f, 0.0f);
    glScalef(0.72f, 0.44f, 1.0f);

    glColor3ub(44, 110, 158);
    glBegin(GL_POLYGON);
    glVertex2f(200, 350); glVertex2f(420, 350);
    glVertex2f(420, 392); glVertex2f(380, 430);
    glVertex2f(250, 430); glVertex2f(210, 392);
    glEnd();

    // Windows — yellow at night, dark in day
    if (night3) glColor3ub(255, 255, 0);
    else          glColor3ub(14,  24,  34);

    glBegin(GL_QUADS);
    glVertex2f(255, 392); glVertex2f(315, 392);
    glVertex2f(305, 424); glVertex2f(262, 424);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(322, 392); glVertex2f(372, 392);
    glVertex2f(372, 420); glVertex2f(322, 424);
    glEnd();

    glColor3ub(0, 0, 0);
    circle3(250, 340, 20.0f, 100); // front wheel
    circle3(350, 340, 20.0f, 100); // rear wheel
    glPopMatrix();
}

// Scene 3 bus
void drawBus3(void)
{
    glPushMatrix();
    glTranslatef(bus3X, 180.0f, 0.0f);
    glScalef(0.8f, 0.45f, 1.0f);

    glColor3ub(186, 122, 72);
    glBegin(GL_POLYGON);
    glVertex2f(1000, 370); glVertex2f(1200, 370);
    glVertex2f(1200, 420); glVertex2f(1170, 440);
    glVertex2f(1020, 440); glVertex2f(1000, 420);
    glEnd();

    // Windows — yellow at night, black in day
    if (night3) glColor3ub(255, 255, 0);
    else          glColor3ub(0,   0,   0);

    // Five window groups
    glBegin(GL_QUADS);
    glVertex2f(1020,420); glVertex2f(1040,420); glVertex2f(1040,435); glVertex2f(1020,425);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(1050,420); glVertex2f(1080,420); glVertex2f(1080,435); glVertex2f(1050,435);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(1090,420); glVertex2f(1120,420); glVertex2f(1120,435); glVertex2f(1090,435);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(1130,420); glVertex2f(1155,420); glVertex2f(1155,435); glVertex2f(1130,435);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(1165,420); glVertex2f(1185,420); glVertex2f(1185,425); glVertex2f(1165,435);
    glEnd();

    // Side window on engine block
    glBegin(GL_QUADS);
    glVertex2f(1160,370); glVertex2f(1190,370); glVertex2f(1190,410); glVertex2f(1160,410);
    glEnd();

    glColor3ub(1, 1, 1);
    circle3(1050, 360, 20.0f, 100); // front wheel
    circle3(1150, 360, 20.0f, 100); // rear wheel
    glPopMatrix();
}

void reset3(void)
{
    carBlue3X = 1000.0f;
    bus3X     = -700.0f;
    blade3Angle = 0.0f;
    cloud3X1 = 0.0f;
    cloud3X2 = -300.0f;
    cloud3X3 = -600.0f;
    jet3Offset = 0.0f;
    car3On = 0;
    jets3On = 0;
}

// Scene 2 variables
float car2GrayX  = 0.0f;
float car2RedX   = 0.0f;
float train2X    = 0.0f;
bool  train2On   = false;
float cloud2X1   = 0.0f;
float cloud2X2   = -150.0f;
bool  night2     = false;
bool  rain2      = false;
const float cloud2Speed = 1.0f;

// Scene 2 ellipse helper
void oval2(float rx, float ry, float cx, float cy)
{
    glBegin(GL_POLYGON);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(rx * cos(angle) + cx, ry * sin(angle) + cy);
    }
    glEnd();
}

void cloudDot2(float cx, float cy, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}

void drawCloud2(float x, float y)
{
    if (night2) return; // no clouds at night in Scene 2

    if (rain2) glColor3f(0.2f, 0.2f, 0.2f);
    else            glColor3f(1.0f, 1.0f, 1.0f);

    cloudDot2(x,       y,       30);
    cloudDot2(x + 35,  y,       30);
    cloudDot2(x - 35,  y,       30);
    cloudDot2(x + 15,  y + 25,  25);
    cloudDot2(x - 15,  y + 25,  25);
}

void drawLamp2(float x, float y)
{
    // Pillar
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(x,      y);       glVertex2f(x + 10, y);
    glVertex2f(x + 10, y + 100); glVertex2f(x,      y + 100);
    glEnd();

    // Lamp bulb — yellow at night/rain, white otherwise
    if (night2 || rain2) glColor3f(1.0f, 1.0f, 0.0f);
    else                          glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + 5 + 15 * cos(angle), y + 100 + 15 * sin(angle));
    }
    glEnd();
}

void drawTrain2()
{
    glPushMatrix();
    glTranslatef(train2X, 0.0f, 0.0f);

    // ---- Engine car ----
    glColor3f(0.192f, 0.576f, 0.705f);
    glBegin(GL_QUADS);
    glVertex2i(100,360); glVertex2i(170,360);
    glVertex2i(170,410); glVertex2i(100,410);
    glEnd();

  // Engine windows
glColor3f(1.0f, 1.0f, 1.0f);

glBegin(GL_QUADS);
glVertex2i(110,385);
glVertex2i(120,385);
glVertex2i(120,400);
glVertex2i(110,400);
glEnd();

glBegin(GL_QUADS);
glVertex2i(130,385);
glVertex2i(140,385);
glVertex2i(140,400);
glVertex2i(130,400);
glEnd();

glBegin(GL_QUADS);
glVertex2i(150,385);
glVertex2i(160,385);
glVertex2i(160,400);
glVertex2i(150,400);
glEnd();

// Engine red stripe
glColor3f(1.0f, 0.0f, 0.0f);

glBegin(GL_QUADS);
glVertex2i(100,370);
glVertex2i(170,370);
glVertex2i(170,375);
glVertex2i(100,375);
glEnd();

// Engine coupler
glColor3f(1.0f, 1.0f, 1.0f);

glBegin(GL_QUADS);
glVertex2i(170,360);
glVertex2i(176,370);
glVertex2i(176,420);
glVertex2i(170,410);
glEnd();

glBegin(GL_QUADS);
glVertex2i(101,410);
glVertex2i(170,410);
glVertex2i(176,420);
glVertex2i(105,420);
glEnd();

// Engine wheels
glPushMatrix();
glTranslatef(115,352,0);
glColor3ub(109,109,115);
oval2(10,10,0,0);
glPopMatrix();

glPushMatrix();
glTranslatef(155,352,0);
glColor3ub(109,109,115);
oval2(10,10,0,0);
glPopMatrix();

    // ---- Three additional passenger cars (offset loop) ----
    for (int offset = -75; offset <= 150; offset += 75)
    {
        glPushMatrix();

glTranslatef(offset, 0, 0);

glColor3f(0.192f, 0.576f, 0.705f);

glBegin(GL_QUADS);
glVertex2i(100,360);
glVertex2i(170,360);
glVertex2i(170,410);
glVertex2i(100,410);
glEnd();

glColor3f(1.0f,1.0f,1.0f);

glBegin(GL_QUADS);
glVertex2i(110,385);
glVertex2i(120,385);
glVertex2i(120,400);
glVertex2i(110,400);
glEnd();

glBegin(GL_QUADS);
glVertex2i(130,385);
glVertex2i(140,385);
glVertex2i(140,400);
glVertex2i(130,400);
glEnd();

glBegin(GL_QUADS);
glVertex2i(150,385);
glVertex2i(160,385);
glVertex2i(160,400);
glVertex2i(150,400);
glEnd();

glColor3f(1.0f,0.0f,0.0f);

glBegin(GL_QUADS);
glVertex2i(100,370);
glVertex2i(170,370);
glVertex2i(170,375);
glVertex2i(100,375);
glEnd();

glColor3f(1.0f,1.0f,1.0f);

glBegin(GL_QUADS);
glVertex2i(170,360);
glVertex2i(176,370);
glVertex2i(176,420);
glVertex2i(170,410);
glEnd();

glBegin(GL_QUADS);
glVertex2i(101,410);
glVertex2i(170,410);
glVertex2i(176,420);
glVertex2i(105,420);
glEnd();

glPushMatrix();

glTranslatef(115,352,0);

glColor3ub(109,109,115);

oval2(10,10,0,0);

glPopMatrix();

glPushMatrix();

glTranslatef(155,352,0);

glColor3ub(109,109,115);

oval2(10,10,0,0);

glPopMatrix();

glPopMatrix();
    }

  // ---- Chimney & smoke ----

glColor3f(1.0f, 0.0f, 0.0f);

glBegin(GL_QUADS);
glVertex2i(322,405);
glVertex2i(330,405);
glVertex2i(330,392);
glVertex2i(322,392);
glEnd();

glColor3f(0.325f, 0.101f, 0.619f);

glBegin(GL_QUADS);
glVertex2i(310,413);
glVertex2i(300,413);
glVertex2i(300,418);
glVertex2i(310,418);
glEnd();

// Smoke puffs

glColor3f(0.709f, 0.701f, 0.717f);

glPushMatrix();
glTranslatef(303,427,0);
oval2(3,3,0,0);
glPopMatrix();

glPushMatrix();
glTranslatef(293,430,0);
oval2(4,4,0,0);
glPopMatrix();

glPushMatrix();
glTranslatef(283,432,0);
oval2(5,5,0,0);
glPopMatrix();

glPopMatrix(); // end train group
}

// ============================================================
//   SCENE 2 — DRAW
// ============================================================
void Scene2()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1500, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ---------- SKY ----------
    glBegin(GL_QUADS);
    if      (s2_isNight && s2_rainday) glColor3f(0.1f, 0.1f, 0.2f);
    else if (s2_isNight)               glColor3f(0.0f, 0.0f, 0.2f);
    else if (s2_rainday)               glColor3f(0.3f, 0.3f, 0.5f);
    else                               glColor3f(0.529f, 0.808f, 0.922f);
    glVertex2i(0,550);    glVertex2i(1500,550);
    glVertex2i(1500,1000);glVertex2i(0,1000);
    glEnd();

    // ---------- SUN / MOON ----------
    // Sun: yellow filled circle at top-right
    // Moon: white filled circle at same position
    if (s2_isNight && !s2_rainday) {
        // Moon
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float t = i * 3.1416f / 180.0f;
            glVertex2f(1250 + 50*cos(t), 900 + 50*sin(t));
        }
        glEnd();
    } else if (!s2_isNight) {
        // Sun
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float t = i * 3.1416f / 180.0f;
            glVertex2f(1250 + 50*cos(t), 900 + 50*sin(t));
        }
        glEnd();
    }

    // ---------- MOVING CLOUDS (day only, animated) ----------
    glPushMatrix();
    glTranslatef(s2_cloud1X, 0.0f, 0.0f);
    drawCloud2(10.0f, 950.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(s2_cloud2X, 0.0f, 0.0f);
    drawCloud2(15.0f, 850.0f);
    glPopMatrix();

    // Extra rain clouds (static, day only)
    if (s2_rainday) {
        drawCloud2(600,  900);
        drawCloud2(800,  850);
        drawCloud2(1000, 920);
        drawCloud2(1250, 900);
    }

    // ---------- HILLS — BACK LAYER ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.1f,  0.3f,  0.1f);
    else if (s2_isNight)               glColor3f(0.0f,  0.2f,  0.05f);
    else if (s2_rainday)               glColor3f(0.0f,  0.5f,  0.1f);
    else                               glColor3f(0.0f,  0.4f,  0.1f);
    glBegin(GL_POLYGON);
    glVertex2i(-50,686);  glVertex2i(250,850);  glVertex2i(500,800);
    glVertex2i(750,850);  glVertex2i(1000,750); glVertex2i(1250,840);
    glVertex2i(1550,686);
    glEnd();

    // ---------- HILLS — FRONT LAYER ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.15f, 0.35f, 0.15f);
    else if (s2_isNight)               glColor3f(0.1f,  0.25f, 0.1f);
    else if (s2_rainday)               glColor3f(0.2f,  0.6f,  0.2f);
    else                               glColor3f(0.2f,  0.7f,  0.2f);
    glBegin(GL_POLYGON);
    glVertex2i(100,686);  glVertex2i(250,730);  glVertex2i(500,710);
    glVertex2i(750,750);  glVertex2i(1050,720); glVertex2i(1300,740);
    glVertex2i(1450,710); glVertex2i(1500,686);
    glEnd();

    // ---------- ROAD ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.1f,  0.1f,  0.1f);
    else if (s2_isNight)               glColor3f(0.2f,  0.2f,  0.2f);
    else if (s2_rainday)               glColor3f(0.25f, 0.25f, 0.25f);
    else                               glColor3f(0.3f,  0.3f,  0.3f);
    glBegin(GL_QUADS);
    glVertex2i(0,600); glVertex2i(1500,600);
    glVertex2i(1500,686); glVertex2i(0,686);
    glEnd();

    // Road centre line
    if (s2_isNight) glColor3f(0.8f, 0.8f, 0.8f);
    else            glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2i(0,641); glVertex2i(1500,641);
    glVertex2i(1500,646); glVertex2i(0,646);
    glEnd();

    // ---------- LAMP POSTS (7 along road) ----------
    drawLamp2(100,  685);
    drawLamp2(300,  685);
    drawLamp2(500,  685);
    drawLamp2(700,  685);
    drawLamp2(900,  685);
    drawLamp2(1100, 685);
    drawLamp2(1300, 685);

    // ---------- GREY CAR (moves left, animated) ----------
    glPushMatrix();
    glTranslatef(s2_carPos, 0.0f, 0.0f);
    glColor3f(0.33f, 0.33f, 0.33f);
    glBegin(GL_QUADS);
    glVertex2i(200,650); glVertex2i(300,650);
    glVertex2i(300,700); glVertex2i(200,700);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2i(220,700); glVertex2i(280,700);
    glVertex2i(260,730); glVertex2i(240,730);
    glEnd();
    // Wheels
    glPushMatrix(); glTranslatef(220,640,0); glColor3f(0.0f,0.0f,0.0f); oval2(12,12,0,0); glPopMatrix();
    glPushMatrix(); glTranslatef(280,640,0); glColor3f(0.0f,0.0f,0.0f); oval2(12,12,0,0); glPopMatrix();
    glPopMatrix();

    // ---------- RED CAR (moves right, animated) ----------
    glPushMatrix();
    glTranslatef(s2_bx, 0.0f, 0.0f);
    glColor3ub(255, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(1410,610); glVertex2d(1490,610);
    glVertex2d(1485,640); glVertex2d(1410,640);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2d(1420,640); glVertex2d(1475,640);
    glVertex2d(1465,670); glVertex2d(1430,670);
    glEnd();
    // Windows
    glColor3ub(220, 220, 220);
    glBegin(GL_POLYGON);
    glVertex2d(1425,640); glVertex2d(1445,640);
    glVertex2d(1445,660); glVertex2d(1430,660);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2d(1450,640); glVertex2d(1470,640);
    glVertex2d(1465,660); glVertex2d(1450,660);
    glEnd();
    // Wheels
    glColor3ub(0,0,0);
    oval2(10,14,1435,610);
    oval2(10,14,1465,610);
    glColor3ub(245,245,245);
    oval2(6,10,1435,610);
    oval2(6,10,1465,610);
    glPopMatrix();

    // ---------- GRASS ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.1f, 0.3f, 0.1f);
    else if (s2_isNight)               glColor3f(0.2f, 0.4f, 0.2f);
    else if (s2_rainday)               glColor3f(0.4f, 0.6f, 0.2f);
    else                               glColor3f(0.6f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2i(0,340); glVertex2i(1500,340);
    glVertex2i(1500,600); glVertex2i(0,600);
    glEnd();

    // ---------- TREES — BOTTOM ROW ----------
    for (int i = 50; i < 1500; i += 150) {
        // Trunk
        glColor3f(0.4f, 0.2f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2i(i,340); glVertex2i(i+20,340);
        glVertex2i(i+20,400); glVertex2i(i,400);
        glEnd();
        // Foliage layers
        if      (s2_isNight && s2_rainday) glColor3f(0.0f,0.2f,0.0f);
        else if (s2_isNight)               glColor3f(0.0f,0.3f,0.0f);
        else if (s2_rainday)               glColor3f(0.0f,0.4f,0.0f);
        else                               glColor3f(0.0f,0.6f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2i(i-30,400); glVertex2i(i+50,400); glVertex2i(i+10,460);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2i(i-25,430); glVertex2i(i+45,430); glVertex2i(i+10,490);
        glEnd();
    }

    // ---------- TREES — MIDDLE ROW ----------
    for (int i = 100; i < 1500; i += 150) {
        glColor3f(0.4f, 0.2f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2i(i,470); glVertex2i(i+20,470);
        glVertex2i(i+20,530); glVertex2i(i,530);
        glEnd();
        if      (s2_isNight && s2_rainday) glColor3f(0.0f,0.2f,0.0f);
        else if (s2_isNight)               glColor3f(0.0f,0.3f,0.0f);
        else if (s2_rainday)               glColor3f(0.0f,0.4f,0.0f);
        else                               glColor3f(0.0f,0.6f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2i(i-30,530); glVertex2i(i+50,530); glVertex2i(i+10,590);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2i(i-25,560); glVertex2i(i+45,560); glVertex2i(i+10,620);
        glEnd();
    }

    // ---------- RAILWAY LINE ----------
    if (s2_isNight) glColor3ub(100,100,100); else glColor3ub(150,150,144);
    glBegin(GL_QUADS);
    glVertex2i(0,340); glVertex2i(0,365);
    glVertex2i(1500,365); glVertex2i(1500,340);
    glEnd();

    // Rail edges
    if (s2_isNight) glColor3ub(50,50,50); else glColor3ub(0,0,0);
    glBegin(GL_QUADS);
    glVertex2i(0,362); glVertex2i(1500,362); glVertex2i(1500,365); glVertex2i(0,365);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2i(0,340); glVertex2i(1500,340); glVertex2i(1500,344); glVertex2i(0,344);
    glEnd();

    // Rail sleepers
    if (s2_isNight) glColor3ub(50,50,50); else glColor3ub(0,0,0);
    glBegin(GL_LINES);
    for (float j = 0; j <= 1500; j += 20) {
        glVertex2i(10+(int)j, 340);
        glVertex2i(15+(int)j, 365);
    }
    glEnd();

    // ---------- TRAIN (mouse click toggles start/stop) ----------
    drawTrain2();

    // ---------- WALL / DAM ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.3f, 0.2f, 0.1f);
    else if (s2_isNight)               glColor3f(0.4f, 0.2f, 0.1f);
    else if (s2_rainday)               glColor3f(0.5f, 0.25f,0.1f);
    else                               glColor3f(0.6f, 0.3f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f,310.0f);    glVertex2f(1500.0f,310.0f);
    glVertex2f(1500.0f,340.0f); glVertex2f(0.0f,340.0f);
    glEnd();

    // Dam horizontal lines
    if (s2_isNight) glColor3f(0.3f,0.3f,0.3f); else glColor3f(0.5f,0.5f,0.5f);
    for (float y = 310.0f; y <= 340.0f; y += 5.0f) {
        glBegin(GL_LINES); glVertex2f(0.0f,y); glVertex2f(1500.0f,y); glEnd();
    }
    // Dam vertical lines
    if (s2_isNight) glColor3f(0.3f,0.15f,0.1f); else glColor3f(0.4f,0.2f,0.1f);
    for (float x = 50.0f; x <= 1450.0f; x += 100.0f) {
        glBegin(GL_LINES); glVertex2f(x,310.0f); glVertex2f(x,340.0f); glEnd();
    }

    // ---------- BEACH ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.4f,  0.3f,  0.2f);
    else if (s2_isNight)               glColor3f(0.6f,  0.5f,  0.3f);
    else if (s2_rainday)               glColor3f(0.8f,  0.7f,  0.5f);
    else                               glColor3f(0.94f, 0.85f, 0.56f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f,250.0f);    glVertex2f(1500.0f,250.0f);
    glVertex2f(1500.0f,310.0f); glVertex2f(0.0f,310.0f);
    glEnd();

    // ---------- WATER ----------
    if      (s2_isNight && s2_rainday) glColor3f(0.0f, 0.2f, 0.3f);
    else if (s2_isNight)               glColor3f(0.0f, 0.3f, 0.5f);
    else if (s2_rainday)               glColor3f(0.0f, 0.3f, 0.5f);
    else                               glColor3f(0.0f, 0.5f, 0.7f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f,0.0f);      glVertex2f(1500.0f,0.0f);
    glVertex2f(1500.0f,250.0f); glVertex2f(0.0f,250.0f);
    glEnd();

    // ---------- SHARED CLICK-START SHIP ----------
    drawShip(shipX, 150.0f);

    // ---------- RAIN EFFECT (random streaks when rainday=true) ----------
    if (s2_rainday) {
        glColor3f(0.8f, 0.8f, 1.0f);
        for (int i = 0; i < 1000; i++) {
            int x = rand() % 1500;
            int y = rand() % 1000;
            glBegin(GL_LINES);
            glVertex2i(x, y);
            glVertex2i(x, y - 10);
            glEnd();
        }
    }

    glutSwapBuffers();
}

// ============================================================
//   SCENE 1 — DRAW
// ============================================================
void Scene1()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1500, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ---------- SKY ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.05f, 0.05f, 0.2f);
    else          glColor3f(0.678f, 0.847f, 0.902f);
    glVertex2f(0,0); glVertex2f(1500,0);
    glVertex2f(1500,1000); glVertex2f(0,1000);
    glEnd();

    // ---------- SUN / MOON ----------
    if (!s1_night) {
        circle(40, 850, 900, 1.0f, 1.0f, 0.6f);
    } else {
        circle(40, 850, 900, 1.0f, 1.0f, 0.8f);
        circle(35, 865, 900, 0.05f, 0.05f, 0.2f); // crescent overlay
    }

    // ---------- CLOUDS ----------
    float cr = s1_night ? 0.4f : 0.9f;
    float cg = s1_night ? 0.4f : 0.9f;
    float cb = s1_night ? 0.5f : 0.9f;
    int cloudXs1[] = {320,345,345,370,370,395, 1060,1085,1085,1110,1110,1135};
    int cloudYs1[] = {885,870,900,870,900,885,  875, 860, 890, 860, 890, 875};
    for (int i = 0; i < 12; i++)
        circle(20, cloudXs1[i], cloudYs1[i], cr, cg, cb);

    // ---------- MOUNTAINS ----------
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.329f, 0.471f);
    glVertex2f(0,550); glVertex2f(0,700); glVertex2f(100,760); glVertex2f(120,730);
    glVertex2f(180,800); glVertex2f(240,700); glVertex2f(260,730);
    glVertex2f(300,700); glVertex2f(300,550);
    glEnd();

    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.1f,0.3f,0.25f); else glColor3f(0.2f,0.6f,0.5f);
    glVertex2f(150,550); glVertex2f(200,650); glVertex2f(320,770); glVertex2f(380,850);
    glVertex2f(500,650); glVertex2f(520,730); glVertex2f(600,650); glVertex2f(700,550);
    glEnd();

    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.15f,0.4f,0.3f); else glColor3f(0.3f,0.75f,0.6f);
    glVertex2f(220,550); glVertex2f(260,600); glVertex2f(320,700); glVertex2f(400,770);
    glVertex2f(480,650); glVertex2f(530,620); glVertex2f(590,550);
    glEnd();

    // ---------- LAND / GRASS STRIPS ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.2f,0.2f); else glColor3f(0.94f,0.87f,0.73f);
    glVertex2f(0,450); glVertex2f(1500,450); glVertex2f(1500,380); glVertex2f(0,380);
    glEnd();

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.1f,0.1f); else glColor3f(0.85f,0.45f,0.25f);
    glVertex2f(0,450); glVertex2f(1500,450); glVertex2f(1500,470); glVertex2f(0,470);
    glEnd();

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.0f,0.2f,0.2f); else glColor3f(0.0f,0.573f,0.561f);
    glVertex2f(0,470); glVertex2f(1500,470); glVertex2f(1500,550); glVertex2f(0,550);
    glEnd();

    // Grass tufts
    int bush = 0;
    for (int i = 0; i < 30; i++) {
        glBegin(GL_TRIANGLES);
        if (s1_night) glColor3f(0.1f,0.3f,0.1f); else glColor3f(0.380f,0.788f,0.333f);
        glVertex2f(bush+0,470);
         glVertex2f(bush+50,470);
          glVertex2f(bush+25,490);
        glVertex2f(bush+0,475);
         glVertex2f(bush+50,475);
          glVertex2f(bush+25,495);
        glVertex2f(bush+0,480);
         glVertex2f(bush+50,480);
          glVertex2f(bush+25,500);
        glVertex2f(bush+0,485);
         glVertex2f(bush+50,485);
          glVertex2f(bush+25,500);
        glEnd();
        bush += 50;
    }

    // ---------- BRIDGE ROAD ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.3f,0.3f,0.3f); else glColor3f(0.75f,0.75f,0.75f);
    glVertex2f(0,520); glVertex2f(1500,520); glVertex2f(1500,550); glVertex2f(0,550);
    glEnd();

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.1f,0.1f,0.1f); else glColor3f(0.3f,0.3f,0.3f);
    glVertex2f(0,520); glVertex2f(1500,520); glVertex2f(1500,510); glVertex2f(0,510);
    glEnd();

    // Road pillars
    int road = 0;
    for (int i = 0; i < 5; i++) {
        glBegin(GL_QUADS);
        if (s1_night) glColor3f(0.1f,0.1f,0.1f); else glColor3f(0.3f,0.3f,0.3f);
        glVertex2f(road+100,470); glVertex2f(road+130,470);
        glVertex2f(road+130,510); glVertex2f(road+100,510);
        glEnd();
        road += 300;
    }

    // ---------- LIGHTHOUSE ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.2f,0.2f); else glColor3f(0.6f,0.6f,0.6f);
    glVertex2f(1260,650); glVertex2f(1340,650); glVertex2f(1340,670); glVertex2f(1260,670);
    glEnd();

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.4f,0.4f,0.5f); else glColor3f(0.9f,0.9f,0.9f);
    glVertex2f(1270,670); glVertex2f(1330,670); glVertex2f(1330,750); glVertex2f(1270,750);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(1270,750); glVertex2f(1275,750); glVertex2f(1275,780); glVertex2f(1270,780);
    glVertex2f(1325,750); glVertex2f(1330,750); glVertex2f(1330,780); glVertex2f(1325,780);
    glEnd();

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.25f,0.13f,0.05f); else glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(1290,750); glVertex2f(1310,750); glVertex2f(1310,755); glVertex2f(1290,755);
    glEnd();

    circle(10, 1300, 762, 1.0f, 0.9f, 0.7f); // light bulb

    glBegin(GL_TRIANGLES);
    glColor3f(0.25f, 0.25f, 0.25f);
    glVertex2f(1260,780); glVertex2f(1300,820); glVertex2f(1340,780);
    glEnd();

    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.2f,0.1f,0.1f); else glColor3f(0.85f,0.45f,0.25f);
    glVertex2f(1080,550); glVertex2f(1080,570); glVertex2f(1250,650);
    glVertex2f(1350,650); glVertex2f(1500,570); glVertex2f(1500,550);
    glEnd();

    // ---------- MIDDLE BUILDING ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.4f,0.4f,0.5f); else glColor3f(0.9f,0.9f,0.9f);
    glVertex2f(620,550); glVertex2f(1080,550); glVertex2f(1080,680); glVertex2f(620,680);
    glEnd();

    // Left glass section
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.3f,0.4f); else glColor3f(0.4f,0.6f,0.8f);
    glVertex2f(625,550); glVertex2f(825,550); glVertex2f(825,670); glVertex2f(625,670);
    glEnd();

    // Left windows grid
    s1_bly = 0;
    glBegin(GL_QUADS);
    for (int j = 0; j < 3; j++) {
        s1_blx = 0;
        for (int i = 0; i < 5; i++) {
            if (s1_night) glColor3f(1.0f,0.9f,0.6f); else glColor3f(0.678f,0.847f,0.902f);
            glVertex2f(s1_blx+630,s1_bly+555); glVertex2f(s1_blx+655,s1_bly+555);
            glVertex2f(s1_blx+655,s1_bly+570); glVertex2f(s1_blx+630,s1_bly+570);
            s1_blx += 40;
        }
        s1_bly += 35;
    }
    glEnd();

    // Right glass section
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.3f,0.4f); else glColor3f(0.4f,0.6f,0.8f);
    glVertex2f(875,550); glVertex2f(1075,550); glVertex2f(1075,670); glVertex2f(875,670);
    glEnd();

    // Right windows grid
    s1_bly = 0;
    glBegin(GL_QUADS);
    for (int j = 0; j < 3; j++) {
        s1_blx = 0;
        for (int i = 0; i < 5; i++) {
            if (s1_night) glColor3f(1.0f,0.9f,0.6f); else glColor3f(0.678f,0.847f,0.902f);
            glVertex2f(s1_blx+880,s1_bly+555); glVertex2f(s1_blx+905,s1_bly+555);
            glVertex2f(s1_blx+905,s1_bly+570); glVertex2f(s1_blx+880,s1_bly+570);
            s1_blx += 40;
        }
        s1_bly += 35;
    }
    glEnd();

    // Centre connector
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.15f,0.15f,0.15f); else glColor3f(0.3f,0.3f,0.3f);
    glVertex2f(830,550); glVertex2f(870,550); glVertex2f(870,610); glVertex2f(830,610);
    glEnd();

    // ---------- LAMP POSTS ----------
    int k = 0;
    for (int i = 0; i < 10; i++) {
        glBegin(GL_QUADS);
        if (s1_night) glColor3f(0.2f,0.125f,0.05f); else glColor3f(0.4f,0.25f,0.1f);
        glVertex2f(k+100,550); glVertex2f(k+110,550);
        glVertex2f(k+110,600); glVertex2f(k+100,600);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(k+97,600); glVertex2f(k+113,600);
        glVertex2f(k+113,620); glVertex2f(k+97,620);
        glEnd();
        circle(5, k+105, 610, 1.0f, 1.0f, 0.6f);
        k += 215;
    }

    // ---------- YELLOW CAR (moving left) ----------
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.3f,0.3f,0.0f); else glColor3f(1.0f,0.8f,0.0f);
    glVertex2f(s1_car+0,535); glVertex2f(s1_car+150,535);
    glVertex2f(s1_car+150,575); glVertex2f(s1_car+145,570); glVertex2f(s1_car+0,565);
    glEnd();
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.7f,1.0f,0.9f); else glColor3f(0.4f,0.5f,0.8f);
    glVertex2f(s1_car+0,565); glVertex2f(s1_car+145,565); glVertex2f(s1_car+150,570);
    glVertex2f(s1_car+150,585); glVertex2f(s1_car+145,580);
    glVertex2f(s1_car+140,585); glVertex2f(s1_car+0,585);
    glEnd();
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.3f,0.3f,0.0f); else glColor3f(1.0f,0.8f,0.0f);
    glVertex2f(s1_car+0,585); glVertex2f(s1_car+140,585);
    glVertex2f(s1_car+145,595); glVertex2f(s1_car+10,595);
    glEnd();
    circle(10, s1_car+125, 535, 0.0f, 0.0f, 0.0f);
    circle(10, s1_car+25,  535, 0.0f, 0.0f, 0.0f);

    // ---------- BLUE CAR (moving left, opposite lane) ----------
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.1f,0.2f,0.3f); else glColor3f(0.2f,0.4f,0.5f);
    glVertex2f(s1_car1+1495,530); glVertex2f(s1_car1+1485,545);
    glVertex2f(s1_car1+1400,545); glVertex2f(s1_car1+1390,530);
    glEnd();
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.1f,0.2f,0.3f); else glColor3f(0.2f,0.4f,0.5f);
    glVertex2f(s1_car1+1420,545); glVertex2f(s1_car1+1430,555);
    glVertex2f(s1_car1+1465,555); glVertex2f(s1_car1+1475,545);
    glEnd();
    glBegin(GL_POLYGON);
    if (s1_night) glColor3f(0.7f,1.0f,0.9f); else glColor3f(0.4f,0.5f,0.8f);
    glVertex2f(s1_car1+1422,545); glVertex2f(s1_car1+1432,553);
    glVertex2f(s1_car1+1463,553); glVertex2f(s1_car1+1473,545);
    glEnd();
    circle(8, s1_car1+1420, 530, 0.0f, 0.0f, 0.0f);
    circle(8, s1_car1+1475, 530, 0.0f, 0.0f, 0.0f);

    // ---------- WATER ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.0f,0.25f,0.35f); else glColor3f(0.0f,0.5f,0.7f);
    glVertex2f(0,380); glVertex2f(1500,420);
    glVertex2f(1500,0); glVertex2f(0,0);
    glEnd();

    // ---------- SMALL SAILBOAT (static) ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.05f,0.1f,0.15f); else glColor3f(0.1f,0.2f,0.3f);
    glVertex2f(1250,380); glVertex2f(1400,380);
    glVertex2f(1380,360); glVertex2f(1270,360);
    glEnd();
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.27f,0.14f,0.04f); else glColor3f(0.54f,0.27f,0.07f);
    glVertex2f(1325,380); glVertex2f(1330,380);
    glVertex2f(1330,470); glVertex2f(1325,470);
    glEnd();
    glBegin(GL_TRIANGLES);
    if (s1_night) glColor3f(0.4f,0.4f,0.5f); else glColor3f(0.9f,0.9f,0.9f);
    glVertex2f(1330,470); glVertex2f(1330,400); glVertex2f(1400,400);
    glEnd();
    glBegin(GL_TRIANGLES);
    if (s1_night) glColor3f(0.4f,0.4f,0.5f); else glColor3f(0.9f,0.9f,0.9f);
    glVertex2f(1325,470); glVertex2f(1325,400); glVertex2f(1280,400);
    glEnd();

    // ---------- SPEEDBOAT (static) ----------
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.2f,0.2f); else glColor3f(0.3f,0.3f,0.3f);
    glVertex2f(950,300); glVertex2f(900,350);
    glVertex2f(1200,350); glVertex2f(1100,300);
    glEnd();
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.45f,0.45f,0.4f); else glColor3f(0.9f,0.9f,0.8f);
    glVertex2f(950,350); glVertex2f(1050,350);
    glVertex2f(1050,370); glVertex2f(950,370);
    glEnd();
    glBegin(GL_QUADS);
    if (s1_night) glColor3f(1.0f,0.9f,0.6f); else glColor3f(0.4f,0.6f,0.8f);
    glVertex2f(960,350); glVertex2f(1040,350);
    glVertex2f(1040,365); glVertex2f(960,365);
    glEnd();

    s1_blx = 0;
    for (int i = 0; i < 6; i++) {
        glBegin(GL_QUADS);
        if (s1_night) glColor3f(1.0f,0.9f,0.6f); else glColor3f(0.75f,0.75f,0.75f);
        glVertex2f(s1_blx+950,320); glVertex2f(s1_blx+970,320);
        glVertex2f(s1_blx+970,340); glVertex2f(s1_blx+950,340);
        glEnd();
        s1_blx += 30;
    }

    glBegin(GL_QUADS);
    if (s1_night) glColor3f(0.2f,0.25f,0.4f); else glColor3f(0.4f,0.5f,0.8f);
    glVertex2f(1050,350); glVertex2f(1055,350);
    glVertex2f(1055,420); glVertex2f(1050,420);
    glEnd();

    glBegin(GL_TRIANGLES);
    if (s1_night) glColor3f(0.3f,0.3f,0.3f); else glColor3f(0.75f,0.75f,0.75f);
    glVertex2f(1055,420); glVertex2f(1120,350); glVertex2f(1190,350);
    glEnd();

    // ---------- SHARED CLICK-START SHIP ----------
    drawShip(shipX, 230.0f);

    glutSwapBuffers();
}

// ============================================================
//   SCENE 3 — DRAW
// ============================================================
void Scene3()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1500, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ---------- SKY ----------
    if (s3_Night) glColor3ub(64, 64, 80);
    else          glColor3ub(135, 206, 235);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 450.0f); glVertex2f(1500.0f, 450.0f);
    glVertex2f(1500.0f,1000.0f); glVertex2f(0.0f,1000.0f);
    glEnd();

    // ---------- FIGHTER JETS (pair, animated) ----------
    float jet1X = 800.0f + s3_jetOffset;
    float jet2X = 920.0f + s3_jetOffset;

    // Jet 1
    glColor3ub(46,104,145);
    glBegin(GL_QUADS);
    glVertex2f(jet1X,850); glVertex2f(jet1X-50,850);
    glVertex2f(jet1X-50,870); glVertex2f(jet1X,870);
    glEnd();
    glColor3ub(226,235,242);
    glBegin(GL_TRIANGLES);
    glVertex2f(jet1X-50,850); glVertex2f(jet1X-80,850); glVertex2f(jet1X-50,870);
    glEnd();
    glColor3ub(18,47,72);
    glBegin(GL_LINES);
    glVertex2f(jet1X-81,851); glVertex2f(jet1X-51,871);
    glVertex2f(jet1X-51,871); glVertex2f(jet1X-51,851);
    glEnd();
    glColor3ub(226,235,242);
    glBegin(GL_TRIANGLES);
    glVertex2f(jet1X,870); glVertex2f(jet1X-25,870); glVertex2f(jet1X,890);
    glEnd();
    glColor3ub(18,47,72);
    glBegin(GL_LINES);
    glVertex2f(jet1X-26,871); glVertex2f(jet1X-1,891);
    glVertex2f(jet1X-1,891);  glVertex2f(jet1X-1,871);
    glEnd();
    // Jet 1 cockpit windows
    if (s3_Night) glColor3ub(255,255,0); else glColor3ub(247,252,255);
    glBegin(GL_QUADS);
    glVertex2f(jet1X-5,855);  glVertex2f(jet1X-20,855);
    glVertex2f(jet1X-20,865); glVertex2f(jet1X-5,865);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(jet1X-25,855); glVertex2f(jet1X-40,855);
    glVertex2f(jet1X-40,865); glVertex2f(jet1X-25,865);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(jet1X-51,850); glVertex2f(jet1X-59,850);
    glVertex2f(jet1X-59,860); glVertex2f(jet1X-51,860);
    glEnd();

    // Jet 2
    glColor3ub(46,104,145);
    glBegin(GL_QUADS);
    glVertex2f(jet2X,800); glVertex2f(jet2X-50,800);
    glVertex2f(jet2X-50,820); glVertex2f(jet2X,820);
    glEnd();
    glColor3ub(226,235,242);
    glBegin(GL_TRIANGLES);
    glVertex2f(jet2X-50,800); glVertex2f(jet2X-80,800); glVertex2f(jet2X-50,820);
    glEnd();
    glColor3ub(18,47,72);
    glBegin(GL_LINES);
    glVertex2f(jet2X-81,801); glVertex2f(jet2X-50,821);
    glVertex2f(jet2X-50,821); glVertex2f(jet2X-51,801);
    glEnd();
    glColor3ub(226,235,242);
    glBegin(GL_TRIANGLES);
    glVertex2f(jet2X,820); glVertex2f(jet2X-25,820); glVertex2f(jet2X,840);
    glEnd();
    glColor3ub(18,47,72);
    glBegin(GL_LINES);
    glVertex2f(jet2X-26,821); glVertex2f(jet2X-1,841);
    glVertex2f(jet2X-1,841);  glVertex2f(jet2X-1,821);
    glEnd();
    // Jet 2 cockpit windows
    if (s3_Night) glColor3ub(255,255,0); else glColor3ub(247,252,255);
    glBegin(GL_QUADS);
    glVertex2f(jet2X-5,805);  glVertex2f(jet2X-20,805);
    glVertex2f(jet2X-20,815); glVertex2f(jet2X-5,815);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(jet2X-25,805); glVertex2f(jet2X-40,805);
    glVertex2f(jet2X-40,815); glVertex2f(jet2X-25,815);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(jet2X-51,800); glVertex2f(jet2X-59,800);
    glVertex2f(jet2X-59,810); glVertex2f(jet2X-51,810);
    glEnd();

    // ---------- HOUSE 1 (small, right side) ----------
    if (s3_Night) glColor3ub(120,108,95); else glColor3ub(201,184,156);
    glBegin(GL_POLYGON);
    glVertex2f(150,450); glVertex2f(300,450);
    glVertex2f(300,550); glVertex2f(150,550);
    glEnd();
    if (s3_Night) glColor3ub(79,63,53); else glColor3ub(131,88,56);
    glBegin(GL_TRIANGLES);
    glVertex2f(140,550); glVertex2f(310,550); glVertex2f(225,600);
    glEnd();
    // Window — lit at night
    if (s3_Night) glColor3ub(247,223,136); else glColor3ub(242,236,220);
    glBegin(GL_POLYGON);
    glVertex2f(160,520); glVertex2f(190,520);
    glVertex2f(190,540); glVertex2f(160,540);
    glEnd();

    // ---------- WIND TURBINE — LEFT (tall) ----------
    glColor3ub(198,205,210);
    glBegin(GL_POLYGON);
    glVertex2f(60,450); glVertex2f(80,450);
    glVertex2f(80,800); glVertex2f(60,800);
    glEnd();
    glColor3ub(88,96,108);
    circle3(70, 810, 18, 60);
    glColor3ub(255,255,255);
    drawBlade3(70, 810, 140, blade3Angle);
    drawBlade3(70, 810, 140, blade3Angle + 120);
    drawBlade3(70, 810, 140, blade3Angle + 240);

    // ---------- HOUSE 2 (large, left side) ----------
    if (s3_Night) glColor3ub(120,108,95); else glColor3ub(201,184,156);
    glBegin(GL_POLYGON);
    glVertex2f(10,450); glVertex2f(140,450);
    glVertex2f(140,600); glVertex2f(10,600);
    glEnd();
    if (s3_Night) glColor3ub(79,63,53); else glColor3ub(131,88,56);
    glBegin(GL_TRIANGLES);
    glVertex2f(-20,600); glVertex2f(170,600); glVertex2f(67,700);
    glEnd();
    // Door window
    if (s3_Night) glColor3ub(247,223,136); else glColor3ub(242,236,220);
    glBegin(GL_POLYGON);
    glVertex2f(60,450); glVertex2f(100,450);
    glVertex2f(100,520); glVertex2f(60,520);
    glEnd();
    // Side windows
    if (s3_Night) glColor3ub(247,223,136); else glColor3ub(242,236,220);
    glBegin(GL_QUADS);
    glVertex2f(20,540); glVertex2f(50,540); glVertex2f(50,570); glVertex2f(20,570);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(100,540); glVertex2f(130,540); glVertex2f(130,570); glVertex2f(100,570);
    glEnd();

    // ---------- WIND TURBINE — RIGHT (shorter) ----------
    glColor3ub(198,205,210);
    glBegin(GL_POLYGON);
    glVertex2f(250,450); glVertex2f(270,450);
    glVertex2f(270,700); glVertex2f(250,700);
    glEnd();
    glColor3ub(88,96,108);
    circle3(260, 710, 15, 50);
    glColor3ub(255,255,255);
    drawBlade3(260, 710, 120, blade3Angle);
    drawBlade3(260, 710, 120, blade3Angle + 120);
    drawBlade3(260, 710, 120, blade3Angle + 240);

    // ---------- SUN / MOON ----------
    if (s3_Night) glColor3ub(232,236,240); else glColor3ub(255,234,160);
    {
        float cx=1350.0f, cy=850.0f, r=100.0f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx,cy);
        for (int i=0;i<=100;i++){
            float t=2.0f*PI*i/100;
            glVertex2f(cx+r*cos(t), cy+r*sin(t));
        }
        glEnd();
    }

    // ---------- HILLS (5 layered triangles) ----------
    int hillL[][2] = {{100,500},{440,800},{680,1100},{880,1280},{1080,1510}};
    int hillP[]    = {400,700,850,1090,1250};
    int hillH[]    = {650,600,700,670,770};
    for (int i=0;i<5;i++){
        if (s3_Night) glColor3ub(93,102,117); else glColor3ub(168,153,132);
        glBegin(GL_TRIANGLES);
        glVertex2f(hillL[i][0],450); glVertex2f(hillL[i][1],450); glVertex2f(hillP[i],hillH[i]);
        glEnd();
        glColor3ub(89,83,76);
        glBegin(GL_LINES);
        glVertex2f(hillL[i][1]+1,451); glVertex2f(hillP[i]+1,hillH[i]+1);
        glVertex2f(hillP[i]+1,hillH[i]+1); glVertex2f(hillL[i][0]+1,451);
        glEnd();
    }
    // 6th hill (wide, background)
    if (s3_Night) glColor3ub(93,102,117); else glColor3ub(168,153,132);
    glBegin(GL_TRIANGLES);
    glVertex2f(1070,450); glVertex2f(1700,450); glVertex2f(1470,970);
    glEnd();
    glColor3ub(89,83,76);
    glBegin(GL_LINES);
    glVertex2f(1701,451); glVertex2f(1471,971);
    glVertex2f(1471,971); glVertex2f(1071,451);
    glEnd();

    // ---------- MOVING CLOUDS ----------
    glPushMatrix();
    glTranslatef(s3_cloud1X, 0.0f, 0.0f);
    drawCloud3(10.0f, 900.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(s3_cloud2X, 0.0f, 0.0f);
    drawCloud3(15.0f, 850.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(s3_cloud3X, 0.0f, 0.0f);
    drawCloud3(20.0f, 820.0f);
    glPopMatrix();

    // ---------- GRASS / SMALL TREE TUFTS ----------
    if (s3_Night) glColor3ub(38,84,44); else glColor3ub(62,132,66);
    for (int i=100; i<1500; i+=20){
        glBegin(GL_TRIANGLES);
        glVertex2f(i,450); glVertex2f(i+10,450); glVertex2f(i+5, 470+(i%30));
        glEnd();
    }

    // ---------- ROAD ----------
    if (s3_Night) glColor3ub(56,59,65); else glColor3ub(87,89,92);
    glBegin(GL_QUADS);
    glVertex2f(0,330); glVertex2f(1500,330);
    glVertex2f(1500,450); glVertex2f(0,450);
    glEnd();
    // Centre line
    if (s3_Night) glColor3ub(206,194,158); else glColor3ub(238,227,184);
    glBegin(GL_QUADS);
    glVertex2f(0,385); glVertex2f(1500,385);
    glVertex2f(1500,395); glVertex2f(0,395);
    glEnd();

    // ---------- CARS ----------
    drawCar3(); // blue car (left-moving)
    drawBus3(); // bus (right-moving)

    // ---------- FIELD (left of water) ----------
    if (s3_Night) glColor3ub(36,82,46); else glColor3ub(58,126,64);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f,190.0f); glVertex2f(700.0f,190.0f);
    glVertex2f(1000.0f,330.0f); glVertex2f(0.0f,330.0f);
    glEnd();

    // ---------- WATER (upper band) ----------
    if (s3_Night) glColor3ub(33,54,76); else glColor3ub(45,118,156);
    glBegin(GL_POLYGON);
    glVertex2f(700.0f,190.0f); glVertex2f(1500.0f,190.0f);
    glVertex2f(1500.0f,330.0f); glVertex2f(1000.0f,330.0f);
    glEnd();

    // ---------- WATER (lower/deep band) ----------
    if (s3_Night) glColor3ub(19,40,62); else glColor3ub(26,101,139);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f,0.0f); glVertex2f(1500.0f,0.0f);
    glVertex2f(1500.0f,190.0f); glVertex2f(0.0f,190.0f);
    glEnd();

    // ---------- STATIC SMALL BOAT (right, on water) ----------
    if (s3_Night) glColor3ub(72,45,31); else glColor3ub(149,93,48);
    glBegin(GL_POLYGON);
    glVertex2f(1170.0f,220.0f); glVertex2f(1400.0f,220.0f);
    glVertex2f(1448.0f,252.0f); glVertex2f(1128.0f,252.0f);
    glEnd();
    if (s3_Night) glColor3ub(96,61,40); else glColor3ub(179,117,66);
    glBegin(GL_POLYGON);
    glVertex2f(1194.0f,220.0f); glVertex2f(1384.0f,220.0f);
    glVertex2f(1420.0f,242.0f); glVertex2f(1162.0f,242.0f);
    glEnd();
    if (s3_Night) glColor3ub(194,182,154); else glColor3ub(232,220,191);
    glBegin(GL_QUADS);
    glVertex2f(1178.0f,252.0f); glVertex2f(1434.0f,252.0f);
    glVertex2f(1428.0f,258.0f); glVertex2f(1184.0f,258.0f);
    glEnd();
    if (s3_Night) glColor3ub(48,58,72); else glColor3ub(67,83,102);
    glBegin(GL_QUADS);
    glVertex2f(1270.0f,220.0f); glVertex2f(1300.0f,220.0f);
    glVertex2f(1292.0f,206.0f); glVertex2f(1278.0f,206.0f);
    glEnd();
    // Mast
    if (s3_Night) glColor3ub(116,121,130); else glColor3ub(150,155,164);
    glBegin(GL_QUADS);
    glVertex2f(1282.0f,252.0f); glVertex2f(1291.0f,252.0f);
    glVertex2f(1291.0f,358.0f); glVertex2f(1282.0f,358.0f);
    glEnd();
    // Boom
    if (s3_Night) glColor3ub(100,106,116); else glColor3ub(130,136,146);
    glBegin(GL_QUADS);
    glVertex2f(1291.0f,296.0f); glVertex2f(1364.0f,296.0f);
    glVertex2f(1362.0f,302.0f); glVertex2f(1291.0f,302.0f);
    glEnd();
    // Main sail
    if (s3_Night) glColor3ub(231,227,214); else glColor3ub(250,248,241);
    glBegin(GL_TRIANGLES);
    glVertex2f(1291.0f,350.0f); glVertex2f(1291.0f,258.0f); glVertex2f(1386.0f,302.0f);
    glEnd();
    // Foresail
    glBegin(GL_TRIANGLES);
    glVertex2f(1282.0f,334.0f); glVertex2f(1236.0f,293.0f); glVertex2f(1282.0f,293.0f);
    glEnd();
    // Porthole lights
    if (s3_Night) glColor3ub(242,214,120); else glColor3ub(222,236,248);
    circle3(1230.0f,235.0f,4.0f,20);
    circle3(1255.0f,235.0f,4.0f,20);
    circle3(1280.0f,235.0f,4.0f,20);
    circle3(1305.0f,235.0f,4.0f,20);
    circle3(1330.0f,235.0f,4.0f,20);

    // ---------- SHARED CLICK-START SHIP ----------
    drawShip(shipX, 145.0f);

    glutSwapBuffers();
}

// ============================================================
//   SCENE 4 — DRAW
// ============================================================
void Scene4()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // ---------- SKY ----------
    glBegin(GL_QUADS);
    if (s4_isday == 1) {
        glColor3f(0.3f, 0.6f, 0.95f);
        glVertex2f(-1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glColor3f(0.7f, 0.9f, 1.0f);
        glVertex2f(1.0f, 0.2f); glVertex2f(-1.0f, 0.2f);
    } else {
        glColor3f(0.02f, 0.02f, 0.15f);
        glVertex2f(-1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glColor3f(0.05f, 0.08f, 0.3f);
        glVertex2f(1.0f, 0.2f); glVertex2f(-1.0f, 0.2f);
    }
    glEnd();

    // ---------- SUN / MOON ----------
    if (s4_isday == 0) {
        circle(0.08f, -0.65f, 0.75f, 0.9f, 0.9f, 1.0f);
        circle(0.08f, -0.60f, 0.75f, 0.05f, 0.05f, 0.2f); // crescent mask
    } else {
        circle(0.08f, -0.65f, 0.75f, 1.0f, 0.8f, 0.0f);
    }

    // ---------- SUN RAYS (day only) ----------
    if (s4_isday == 1) {
        glBegin(GL_LINES);
        glColor3f(1.0f, 0.8f, 0.0f);
        glVertex2f(-0.65f,0.85f); glVertex2f(-0.65f,0.90f);
        glVertex2f(-0.65f,0.65f); glVertex2f(-0.65f,0.60f);
        glVertex2f(-0.55f,0.75f); glVertex2f(-0.50f,0.75f);
        glVertex2f(-0.75f,0.75f); glVertex2f(-0.80f,0.75f);
        glVertex2f(-0.58f,0.82f); glVertex2f(-0.54f,0.86f);
        glVertex2f(-0.72f,0.82f); glVertex2f(-0.76f,0.86f);
        glVertex2f(-0.58f,0.68f); glVertex2f(-0.54f,0.64f);
        glVertex2f(-0.72f,0.68f); glVertex2f(-0.76f,0.64f);
        glEnd();
    }

    // ---------- CLOUDS (day only, animated) ----------
    if (s4_isday == 1) {
        drawCloud4(cloud4X1, 0.85f);
        drawCloud4(cloud4X2, 0.75f);
        drawCloud4(cloud4X3, 0.90f);
    }

    // ---------- MOUNTAINS ----------
    glBegin(GL_POLYGON);
    if (s4_isday==1) glColor3f(0.05f,0.25f,0.1f); else glColor3f(0.2f,0.3f,0.2f);
    glVertex2f(-1.0f,0.2f); glVertex2f(-0.8f,0.55f); glVertex2f(-0.6f,0.35f);
    glVertex2f(-0.4f,0.7f); glVertex2f(-0.2f,0.4f);  glVertex2f(0.0f,0.75f);
    glVertex2f(0.2f,0.4f);  glVertex2f(0.4f,0.65f);  glVertex2f(0.6f,0.35f);
    glVertex2f(0.8f,0.5f);  glVertex2f(1.0f,0.3f);   glVertex2f(1.0f,0.2f);
    glEnd();

    // ---------- FOREST BASE ----------
    glBegin(GL_POLYGON);
    if (s4_isday==1) glColor3f(0.1f,0.4f,0.1f); else glColor3f(0.05f,0.15f,0.08f);
    glVertex2f(-1.0f,0.2f); glVertex2f(-0.8f,0.25f); glVertex2f(-0.6f,0.2f);
    glVertex2f(-0.4f,0.3f); glVertex2f(-0.2f,0.2f);  glVertex2f(0.0f,0.28f);
    glVertex2f(0.2f,0.2f);  glVertex2f(0.4f,0.27f);  glVertex2f(0.6f,0.2f);
    glVertex2f(0.8f,0.25f); glVertex2f(1.0f,0.2f);
    glEnd();

    // ---------- WATER ----------
    glBegin(GL_POLYGON);
    if (s4_isday==1) glColor3f(0.0f,0.45f,0.75f); else glColor3f(0.0f,0.25f,0.45f);
    glVertex2f(-1.0f,0.2f); glVertex2f(1.0f,0.2f);
    glVertex2f(1.0f,-1.0f); glVertex2f(-1.0f,-1.0f);
    glEnd();

    // ---------- BEACH ----------
    glBegin(GL_POLYGON);
    if (s4_isday==1) glColor3f(0.95f,0.85f,0.55f); else glColor3f(0.55f,0.50f,0.35f);
    glVertex2f(0.2f,0.2f);   glVertex2f(1.0f,0.2f);   glVertex2f(1.0f,-1.0f);
    glVertex2f(0.6f,-1.0f);  glVertex2f(0.45f,-0.8f); glVertex2f(0.35f,-0.5f);
    glVertex2f(0.25f,-0.2f); glVertex2f(0.2f,0.2f);
    glEnd();

    // ---------- BEACH FOOTBALL ----------
    circle(0.0395f, 0.55f, -0.75f, 0.0f,  0.0f,  0.0f);
    circle(0.038f,  0.55f, -0.75f, 0.95f, 0.95f, 0.95f);
    circle(0.010f, 0.550f,-0.745f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.535f,-0.728f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.565f,-0.728f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.522f,-0.748f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.578f,-0.748f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.535f,-0.765f, 0.0f,  0.0f,  0.0f);
    circle(0.009f, 0.565f,-0.765f, 0.0f,  0.0f,  0.0f);

    // ---------- BEACH CHAIR 1 ----------
    glBegin(GL_POLYGON); glColor3f(0.85f,0.55f,0.15f);
    glVertex2f(0.35f,0.15f); glVertex2f(0.44f,0.15f);
    glVertex2f(0.44f,0.13f); glVertex2f(0.35f,0.13f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.85f,0.55f,0.15f);
    glVertex2f(0.42f,0.15f); glVertex2f(0.44f,0.15f);
    glVertex2f(0.45f,0.21f); glVertex2f(0.43f,0.21f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(0.355f,0.13f); glVertex2f(0.365f,0.13f);
    glVertex2f(0.365f,0.09f); glVertex2f(0.355f,0.09f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(0.428f,0.13f); glVertex2f(0.438f,0.13f);
    glVertex2f(0.438f,0.09f); glVertex2f(0.428f,0.09f);
    glEnd();

    // ---------- BEACH CHAIR 2 ----------
    glBegin(GL_POLYGON); glColor3f(0.85f,0.55f,0.15f);
    glVertex2f(0.37f,0.07f); glVertex2f(0.46f,0.07f);
    glVertex2f(0.46f,0.05f); glVertex2f(0.37f,0.05f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.85f,0.55f,0.15f);
    glVertex2f(0.44f,0.07f); glVertex2f(0.46f,0.07f);
    glVertex2f(0.47f,0.13f); glVertex2f(0.45f,0.13f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(0.375f,0.05f); glVertex2f(0.385f,0.05f);
    glVertex2f(0.385f,0.01f); glVertex2f(0.375f,0.01f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(0.448f,0.05f); glVertex2f(0.458f,0.05f);
    glVertex2f(0.458f,0.01f); glVertex2f(0.448f,0.01f);
    glEnd();

// ---------- BEACH UMBRELLA ----------

circle(0.07f, 0.545f, -0.29f, 0.75f, 0.65f, 0.40f);

glBegin(GL_POLYGON);
glColor3f(0.55f,0.27f,0.07f);

glVertex2f(0.540f,-0.30f);
glVertex2f(0.550f,-0.30f);
glVertex2f(0.550f,-0.10f);
glVertex2f(0.540f,-0.10f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.85f,0.15f,0.15f);

glVertex2f(0.545f,-0.10f);
glVertex2f(0.430f,-0.13f);
glVertex2f(0.470f,-0.04f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.95f,0.95f,0.95f);

glVertex2f(0.545f,-0.10f);
glVertex2f(0.470f,-0.04f);
glVertex2f(0.545f,-0.01f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.85f,0.15f,0.15f);

glVertex2f(0.545f,-0.10f);
glVertex2f(0.545f,-0.01f);
glVertex2f(0.620f,-0.04f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.95f,0.95f,0.95f);

glVertex2f(0.545f,-0.10f);
glVertex2f(0.620f,-0.04f);
glVertex2f(0.660f,-0.13f);

glEnd();

glLineWidth(1.5f);

glBegin(GL_LINE_STRIP);
glColor3f(0.4f,0.2f,0.05f);

glVertex2f(0.430f,-0.13f);
glVertex2f(0.470f,-0.04f);
glVertex2f(0.545f,-0.01f);
glVertex2f(0.620f,-0.04f);
glVertex2f(0.660f,-0.13f);

glEnd();

    // ---------- BOAT 1 (animated, bounces) ----------
    glPushMatrix();
    glTranslatef(s4_boat1move, 0.00f, 0.0f);
    glBegin(GL_POLYGON); glColor3f(0.48f,0.24f,0.07f);
    glVertex2f(-0.80f,0.05f); glVertex2f(-0.68f,0.05f);
    glVertex2f(-0.70f,0.0f);  glVertex2f(-0.78f,0.0f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.78f,0.66f,0.50f);
    glVertex2f(-0.76f,0.05f); glVertex2f(-0.76f,0.09f);
    glVertex2f(-0.72f,0.09f); glVertex2f(-0.72f,0.05f);
    glEnd();
    glPopMatrix();

    // ---------- BOAT 2 (animated, docks at right) ----------
    glPushMatrix();
    glTranslatef(s4_boat2move, 0.15f, 0.0f);
    glBegin(GL_POLYGON); glColor3f(0.1f,0.1f,0.4f);
    glVertex2f(-0.12f,0.02f); glVertex2f(0.10f,0.02f);
    glVertex2f(0.06f,-0.05f); glVertex2f(-0.10f,-0.05f);
    glEnd();
    glBegin(GL_POLYGON); glColor3f(0.6f,0.6f,0.7f);
    glVertex2f(-0.05f,0.02f); glVertex2f(-0.05f,0.07f);
    glVertex2f(0.03f,0.07f);  glVertex2f(0.03f,0.02f);
    glEnd();
    glBegin(GL_LINES); glColor3f(0.3f,0.2f,0.1f);
    glVertex2f(-0.01f,0.07f); glVertex2f(-0.01f,0.18f);
    glEnd();
    glBegin(GL_TRIANGLES); glColor3f(0.9f,0.9f,0.9f);
    glVertex2f(0.01f,0.18f); glVertex2f(0.01f,0.07f); glVertex2f(-0.08f,0.13f);
    glEnd();
    glPopMatrix();

    // ---------- SHARED CLICK-START SHIP ----------
    drawShip4(shipX4, -0.08f);

// ---------- TREE 1 ----------

glBegin(GL_POLYGON);
glColor3f(0.70f,0.55f,0.30f);

glVertex2f(0.86f,-0.01f);
glVertex2f(0.98f,-0.01f);
glVertex2f(0.95f,-0.04f);
glVertex2f(0.82f,-0.04f);

glEnd();

glBegin(GL_POLYGON);
glColor3f(0.55f,0.27f,0.07f);

glVertex2f(0.91f,-0.02f);
glVertex2f(0.93f,-0.02f);
glVertex2f(0.93f,0.06f);
glVertex2f(0.91f,0.06f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.0f,0.5f,0.0f);

glVertex2f(0.87f,0.06f);
glVertex2f(0.97f,0.06f);
glVertex2f(0.92f,0.13f);

glVertex2f(0.88f,0.10f);
glVertex2f(0.96f,0.10f);
glVertex2f(0.92f,0.18f);

glVertex2f(0.89f,0.14f);
glVertex2f(0.95f,0.14f);
glVertex2f(0.92f,0.23f);

glEnd();

// ---------- TREE 2 ----------

glBegin(GL_POLYGON);
glColor3f(0.70f,0.55f,0.30f);

glVertex2f(0.76f,-0.05f);
glVertex2f(0.88f,-0.05f);
glVertex2f(0.85f,-0.08f);
glVertex2f(0.72f,-0.08f);

glEnd();

glBegin(GL_POLYGON);
glColor3f(0.55f,0.27f,0.07f);

glVertex2f(0.81f,-0.07f);
glVertex2f(0.83f,-0.07f);
glVertex2f(0.83f,0.01f);
glVertex2f(0.81f,0.01f);

glEnd();

glBegin(GL_TRIANGLES);
glColor3f(0.0f,0.5f,0.0f);

glVertex2f(0.77f,0.01f);
glVertex2f(0.87f,0.01f);
glVertex2f(0.82f,0.08f);

glVertex2f(0.78f,0.05f);
glVertex2f(0.86f,0.05f);
glVertex2f(0.82f,0.13f);

glVertex2f(0.79f,0.09f);
glVertex2f(0.85f,0.09f);
glVertex2f(0.82f,0.18f);

glEnd();

    // ---------- STARS (night only) ----------
    if (s4_isday == 0) {
   glPointSize(2.5f);

glBegin(GL_POINTS);

glColor3f(1.0f,1.0f,1.0f);

glVertex2f(-0.9f,0.9f);
glVertex2f(-0.7f,0.8f);
glVertex2f(-0.5f,0.95f);

glVertex2f(-0.3f,0.85f);
glVertex2f(-0.1f,0.9f);
glVertex2f(0.1f,0.88f);

glVertex2f(0.3f,0.92f);
glVertex2f(0.5f,0.8f);
glVertex2f(0.7f,0.9f);

glVertex2f(0.9f,0.85f);
glVertex2f(-0.8f,0.6f);
glVertex2f(-0.4f,0.7f);

glVertex2f(0.0f,0.75f);
glVertex2f(0.4f,0.65f);
glVertex2f(0.8f,0.7f);

glEnd();
    }

    // ---------- BIRDS (animated, flapping wings) ----------
    glPushMatrix();
    glTranslatef(s4_birdMove, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    if (s4_wingState == 0) {
        glVertex2f(-0.2f,0.85f);
         glVertex2f(-0.18f,0.83f);
        glVertex2f(-0.18f,0.83f);
         glVertex2f(-0.16f,0.85f);
        glVertex2f(0.1f,0.75f);
         glVertex2f(0.12f,0.73f);
        glVertex2f(0.12f,0.73f);
        glVertex2f(0.14f,0.75f);
        glVertex2f(0.4f,0.88f);
         glVertex2f(0.42f,0.86f);
        glVertex2f(0.42f,0.86f);
        glVertex2f(0.44f,0.88f);
    } else {
       glVertex2f(-0.2f,0.83f);
glVertex2f(-0.18f,0.85f);

glVertex2f(-0.18f,0.85f);
glVertex2f(-0.16f,0.83f);

glVertex2f(0.1f,0.73f);
glVertex2f(0.12f,0.75f);

glVertex2f(0.12f,0.75f);
glVertex2f(0.14f,0.73f);

glVertex2f(0.4f,0.86f);
glVertex2f(0.42f,0.88f);

glVertex2f(0.42f,0.88f);
glVertex2f(0.44f,0.86f);
    }
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

// Display router
void display()
{
    if      (currentScene == 1) Scene1();
    else if (currentScene == 2) Scene2();
    else if (currentScene == 3) Scene3();
    else                        Scene4();
}

// Scene 1 timer
void timer1(int value)
{
    if (currentScene == 1) {
        carYellow1 -= 5;
        carBlue1 -= 6;
        if (carYellow1 < -200)  carYellow1 = 1500;
        if (carBlue1 < -2100) carBlue1 = 600;
        glutPostRedisplay();
    }
    glutTimerFunc(10, timer1, 0);
}

// Scene 2 timer
void timer2(int value)
{
    if (currentScene == 2) {
        // Grey car moves left, wraps around
        car2GrayX -= 2.0f;
        if (car2GrayX < -200.0f) car2GrayX = 1500.0f;

        // Red car moves right, wraps around
        car2RedX += 2.8f;
        if (car2RedX > 0.0f) car2RedX = -1500.0f;

        // Clouds drift right (only visible during day)
        cloud2X1 += cloud2Speed;
        cloud2X2 += cloud2Speed;
        if (cloud2X1 > 1500.0f) cloud2X1 = -100.0f;
        if (cloud2X2 > 1500.0f) cloud2X2 = -100.0f;

        // Train moves right when toggled on by mouse click
        if (train2On) {
            train2X += 6.0f;
            if (train2X > 1500.0f) train2X = -200.0f;
        }

        glutPostRedisplay();
    }
    glutTimerFunc(16, timer2, 0);
}

// Scene 3 timer
void timer3(int value)
{
    if (currentScene == 3) {
        // Spin turbine blades
        blade3Angle += 4.0f;
        if (blade3Angle >= 360.0f) blade3Angle -= 360.0f;

        // Drift clouds rightward
        cloud3X1 += 3.0f;
        cloud3X2 += 3.0f;
        cloud3X3 += 3.0f;
        if (cloud3X1 > 1500.0f) cloud3X1 = -100.0f;
        if (cloud3X2 > 1500.0f) cloud3X2 = -100.0f;
        if (cloud3X3 > 1500.0f) cloud3X3 = -100.0f;

        // Car 1 moves left (triggered by mouse click in Scene 3)
        if (car3On) {
            carBlue3X -= 2.5f;
            if (carBlue3X < -360.0f) carBlue3X = scene3W;
        }

        // Bus (car2) moves right always
        bus3X += 2.8f;
        if (bus3X > 750.0f) bus3X = -1000.0f;

        // Fighter jets move left (started by any key press in Scene 3)
        if (jets3On) {
            jet3Offset -= 2.5f;
            // Wrap jets back to right when fully off-screen left
            if (920.0f + jet3Offset < 0.0f)
                jet3Offset = scene3W + 120.0f - 800.0f;
        }

        glutPostRedisplay();
    }
    glutTimerFunc(16, timer3, 0);
}

// Shared ship timer
void shipTimer(int value)
{
    if (shipMoving) {
        if (currentScene == 4) shipX4 += (float)shipDir * shipSpeed4;
        else                   shipX  += (float)shipDir * shipSpeed;

        if (currentScene == 1 && shipX > 1600.0f) {
            currentScene = 2;
            resetShipForScene(2);
        }
        else if (currentScene == 2 && shipX < -220.0f) {
            currentScene = 3;
            resetShipForScene(3);
        }
        else if (currentScene == 3 && shipX > 1600.0f) {
            currentScene = 4;
            resetShipForScene(4);
        }
        else if (currentScene == 4) {
            // Left→right: stop near the beach edge (~0.18)
            if (shipDir == 1 && shipX4 > -0.07f) {
                shipMoving = false;   // park at beach, wait for next click
            }
            // Right→left: stop when fully off left edge
            if (shipDir == -1 && shipX4 < -1.25f) {
                shipMoving = false;
                ship4TripCount = 0;   // fully reset so next click starts fresh
                shipX4 = -1.2f;
                shipDir = 1;
            }
        }

        glutPostRedisplay();
    }
    glutTimerFunc(16, shipTimer, 0);
}

// Scene 4 bird timer
void birdTimer(int value)
{
    if (currentScene == 4) {
        bird4X += bird4Speed;
        if (bird4X > 1.2f) bird4X = -1.2f;
        wing4 = (wing4 == 0) ? 1 : 0;
        glutPostRedisplay();
    }
    glutTimerFunc(200, birdTimer, 0);
}

// Scene 4 cloud timer
void cloudTimer4(int value)
{
    if (currentScene == 4) {
        cloud4X1 += cloud4Speed;
        cloud4X2 += cloud4Speed;
        cloud4X3 -= cloud4Speed;
        if (cloud4X1 > 1.2f)  cloud4X1 = -1.2f;
        if (cloud4X2 > 1.2f)  cloud4X2 = -1.2f;
        if (cloud4X3 < -1.2f) cloud4X3 =  1.2f;
        glutPostRedisplay();
    }
    glutTimerFunc(50, cloudTimer4, 0);

}

void updateBoat1(int value)
{
    if (currentScene == 4) {
        if (s4_boat1reverse == 0) s4_boat1move -= s4_boat1speed;
        else                      s4_boat1move += s4_boat1speed;
        if (s4_boat1move < -0.90f) s4_boat1reverse = 1;
        if (s4_boat1move >  0.92f) s4_boat1speed   = 0.0f;
        glutPostRedisplay();
    }
    glutTimerFunc(50, updateBoat1, 0);
}

void updateBoat2(int value)
{
    if (currentScene == 4) {
        s4_boat2move += s4_boat2speed;
        if (s4_boat2move > 0.15f) s4_boat2speed = 0.0f;
        glutPostRedisplay();
    }
    glutTimerFunc(50, updateBoat2, 0);
}

// Keyboard
void keyboard(unsigned char key, int, int)
{
    // ----- Scene switching -----
    if (key == '1') { currentScene = 1; resetShipForScene(1); glutPostRedisplay(); return; }
    if (key == '2') { currentScene = 2; resetShipForScene(2); glutPostRedisplay(); return; }
    if (key == '3') { currentScene = 3; resetShipForScene(3); glutPostRedisplay(); return; }
    if (key == '4') { currentScene = 4; resetShipForScene(4); glutPostRedisplay(); return; }

    // ----- Day / Night (shared across all scenes) -----
    if (key == 'n') { night1 = true;  night2 = true;  night3 = 1; day4 = 0; glutPostRedisplay(); }
    if (key == 'd') { night1 = false; night2 = false; night3 = 0; day4 = 1; glutPostRedisplay(); }

    // ----- Scene 2 specific: toggle rain -----
    if (key == 'c' && currentScene == 2) { rain2 = !rain2; glutPostRedisplay(); }

    // ----- Scene 3 specific: reset + jets -----
    if (key == 'r' && currentScene == 3) reset3();
    if (currentScene == 3 && key != 27)  jets3On = 1;

    // ----- ESC to quit -----
    if (key == 27) exit(0);

    if(key=='p' && currentScene==4) shipMoving ==false;
}

// Mouse
void mouse(int button, int state, int, int)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        if (currentScene == 4) {
            if (!shipMoving) {
                ship4TripCount++;
                if (ship4TripCount % 2 == 1) {
                    // Odd click → left to right
                    shipDir  =  1;
                    shipX4   = -1.2f;
                } else {
                    // Even click → right to left
                    shipDir  = -1;
                    shipX4   =  0.18f;   // start from where it stopped
                }
                shipMoving = true;
            }
            return;   // don't fall through to other scene logic
        }

        // All other scenes
        shipMoving = true;

        // Scene 2 — toggle train
        if (currentScene == 2) {
            train2On = !train2On;
        }
        // Scene 3 — start blue car (one-way, no stop)
        if (currentScene == 3 && !car3On) {
            car3On = 1;
        }
    }
}

// ============================================================
//   MAIN
// ============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1500, 1000);
    glutInitWindowPosition(200, 0);
    glutCreateWindow("Maritime World Journey  |  1=Scene1  2=Scene2  3=Scene3  4=Scene4  |  d=Day  n=Night  c=Rain(S2) mouse1=Ship");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    resetShipForScene(currentScene);

    // Start all timers — each only acts when its matching scene is active
    glutTimerFunc(10,  timer1, 0); // Scene 1: car animation
    glutTimerFunc(16,  timer2, 0); // Scene 2: cars, clouds, train
    glutTimerFunc(16,  timer3, 0); // Scene 3: turbine, clouds, cars, jets
    glutTimerFunc(16,  shipTimer, 0); // Shared ship + scene transitions
    glutTimerFunc(200, birdTimer,  0); // Scene 4: bird wing flap
    glutTimerFunc(50,  cloudTimer4,0); // Scene 4: cloud drift
    glutTimerFunc(50, updateBoat1, 0);//scene 4:boats
    glutTimerFunc(50, updateBoat2, 0);

    glutMainLoop();
    return 0;
}
