//
//  main.cpp
//  testingOpenGL
//
//  Created by Michael on 3/21/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <OpenGL/gl.h>

#include <math.h>
#include "MaffStuff.h"
#include "Vector3f.h"

#include "Viewport.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Camera.h"

#define WINDOW_LENGTH 800
#define WINDOW_WIDTH 800
#define WINDOW_X 275
#define WINDOW_Y 100
#define WINDOW_TITLE "COMP-5/6400 Assignment 5"

using namespace std;

struct colorStruct {
    float redValue;
    float greenValue;
    float blueValue;
};

struct floatVert {
    double x;
    double y;
    double z;
};

colorStruct DimGrey = {0.329412f, 0.329412f, 0.329412f};
colorStruct Grey = {0.752941f, 0.752941f, 0.752941f};
colorStruct LightGrey = {0.658824f, 0.658824f, 0.658824f};
colorStruct VLightGrey = {0.80f, 0.80f, 0.80f};
colorStruct DarkOliveGreen = {0.309804f, 0.309804f, 0.184314f};
colorStruct Orange = {1.0f, 0.5f, 0.0f};
colorStruct DarkBrown = {0.36f, 0.25f, 0.20f};
colorStruct Tan = {0.858824f, 0.576471f, 0.439216f};
colorStruct LightBlue = {0.74902f, 0.847059f, 0.847059f};
colorStruct Wheat = {0.847059f, 0.847059f, 0.74902f};
colorStruct DarkSlateBlue = {0.419608f, 0.137255f, 0.556863f};
colorStruct NavyBlue = {0.137255f, 0.137255f, 0.556863f};

Vector3f billyPosition;
Vector3f billyRotation;

int step = 0;

GLfloat zfront = 0;
GLfloat zback = -0.1;
GLfloat zshoe = 0.025 + zfront;
GLfloat fNormalX;
GLfloat fNormalY;
GLfloat fNormalZ;
floatVert fVert1;
floatVert fVert2;
floatVert fVert3;
GLfloat lightPos[] = { 0.0f, 0.0f, 50.0f, 1.0f };
GLfloat specularWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f}; // white
GLfloat specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat spotDir[] = { 0.0f, 0.0f, -1.0f };
GLfloat theta = 60.0;

GLfloat lightPos2[] = { 0.0f, 0.0f, 50.0f, 1.0f };
GLfloat spotDir2[] = { 0.0f, -1.0f, 0.0f };
GLfloat specularBlue[] = { 0.0, 0.0, 1.0, 1.0}; // blue
GLfloat ambient[] = {1, 1, 1, 1.0};
GLfloat diffuse[] {1.0, 1.0, 1.0, 1.0};
GLfloat theta2 = 60.0;

const float MOUSE_SENSITIVITY = 0.5;
const float WALKING_SPEED = 100.0;
const float MAX_TILT = 89;

float LAST_TIME;
float CURRENT_TIME;
float DELTA_TIME;

void toggleLight(int x);
void initLight();
void initGL();
void preProcessEvents();
void reshape(int width, int height);
void display();

void tour();
void drawBilly();
void drawSides();
void drawBuildingRoof();
void drawBuildingFloor();
void drawSquare();
void drawTriangle();
void CalculateVectorNormal(floatVert fVt1[], floatVert fVt2[], floatVert fVt3[],
                           GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ);

int main(int argc, char ** argv) {

    // Initialize GLUT and OpenGL
    glutInit(&argc, argv);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_LENGTH);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    
    // Create Window
    glutCreateWindow(WINDOW_TITLE);
    
    // Setup GLUT Callback Functions
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
//    glutIdleFunc(tour);
    
    // Keyboard and Mouse
    glutKeyboardFunc(Keyboard::keyDown);
    glutKeyboardUpFunc(Keyboard::keyUp);
    glutMotionFunc(Mouse::move);
    glutPassiveMotionFunc(Mouse::move);
    glutMouseFunc(Mouse::click);
    
    glClearColor(DarkOliveGreen.redValue, DarkOliveGreen.greenValue, DarkOliveGreen.blueValue, 1);

    // Enable features
    initGL();
    
    // Enter Main Loop
    glutMainLoop();
    
    return 0;
}

/* Initialize OpenGL Graphics */
void initGL() {
    glClearDepth(1.0f); // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);    // Enable depth testing for z-culling
    
    initLight();
    
    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);
    // All materials hereafter have full specular reflectivity with a high shine
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, 500);
    
    glEnable(GL_NORMALIZE); // Normalize all normal vectors
    glDepthFunc(GL_LEQUAL); // Set the type of depth-test
    glShadeModel(GL_SMOOTH);    // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void preProcessEvents() {
    
    // Camera Transformations
    glRotatef(Camera::rotation.x, 1, 0, 0);
    glRotatef(Camera::rotation.y, 0, 1, 0);
    glRotatef(Camera::rotation.z, 0, 0, 1);
    glTranslatef(-Camera::position.x, -Camera::position.y, Camera::position.z);
    
    // Update Globals
    CURRENT_TIME = ((float)glutGet(GLUT_ELAPSED_TIME)) / 1000;
    DELTA_TIME = CURRENT_TIME - LAST_TIME;
    LAST_TIME = CURRENT_TIME;
    
    Mouse::update();
    
    // Process Mouse
    Camera::rotation.y += (float)Mouse::deltaX*MOUSE_SENSITIVITY;
    Camera::rotation.x += (float)Mouse::deltaY*MOUSE_SENSITIVITY;
    
    if (Camera::rotation.x > MAX_TILT) {
        Camera::rotation.x = MAX_TILT;
    }
    if (Camera::rotation.x < -MAX_TILT) {
        Camera::rotation.x = -MAX_TILT;
    }
    
    // Camera movement
    if (Keyboard::key['w']) { // Forward
        if (Camera::position.z < 490 && Camera::position.z > -334
            && Camera::position.x < 445 && Camera::position.x > -466) {
            Camera::position.x += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(Camera::rotation.y);
            Camera::position.z += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(Camera::rotation.y);
        }
        else {
            Camera::position.z += copysign(1,-Camera::position.z)*0.1;
            Camera::position.x += copysign(1,-Camera::position.z)*0.1;
        }
    }
    if (Keyboard::key['s']) { // Backward
        if (Camera::position.z < 490 && Camera::position.z > -334
            && Camera::position.x < 445 && Camera::position.x > -466) {
            Camera::position.x += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(Camera::rotation.y + 180);
            Camera::position.z += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(Camera::rotation.y + 180);
        }
        else {
            Camera::position.z += copysign(1,-Camera::position.z)*0.1;
            Camera::position.x += copysign(1,-Camera::position.z)*0.1;
        }
    }
    if (Keyboard::key['a']) { // Strafe left
        if (Camera::position.z < 490 && Camera::position.z > -334
            && Camera::position.x < 445 && Camera::position.x > -466) {
            Camera::rotation.y -= 1;
//            Camera::position.x += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(Camera::rotation.y + 270);
//            Camera::position.z += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(Camera::rotation.y + 270);
        }
        else {
            Camera::position.z += copysign(1,-Camera::position.z)*0.1;
            Camera::position.x += copysign(1,-Camera::position.z)*0.1;
        }
    }
    if (Keyboard::key['d']) { // Strafe right
        if (Camera::position.z < 490 && Camera::position.z > -334
            && Camera::position.x < 445 && Camera::position.x > -466) {
            Camera::rotation.y += 1;
//            Camera::position.x += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(Camera::rotation.y + 90);
//            Camera::position.z += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(Camera::rotation.y + 90);
        }
        else {
            Camera::position.z += copysign(1,-Camera::position.z)*0.1;
            Camera::position.x += copysign(1,-Camera::position.z)*0.1;
        }
    }
    if (Keyboard::key['t']) {
        tour();
    }
    
    
    if (Keyboard::key[27]) { // Hit esc to release the mouse
        Viewport::setUnGrabbed();
        Camera::position.x = 0;
        Camera::position.z = 0;
        Camera::rotation.y = 0;
        billyPosition.x = 0;
        billyPosition.z = 0;
        billyRotation.x = 0;
    }
    if (Keyboard::key['q']) { // Elevate
        if (Camera::position.y < 90) {
            Camera::position.y += 10;
        }
    }
    if (Keyboard::key['e']) { // De-elevate
        if (Camera::position.y > 0) {
            Camera::position.y -= 10;
        }
    }
    // Student movement - did this cause I couldn't figure out how to draw model directly
    // in front of camera repeatedly
    if (Keyboard::key['i']) { // Forward
        if (billyPosition.z < 490 && billyPosition.z > -334
            && billyPosition.x < 445 && billyPosition.x > -466) {
            billyPosition.x += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(billyRotation.y + 180);
            billyPosition.z -= (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(billyRotation.y + 180);
        }
        else {
            billyPosition.z += copysign(1,-billyPosition.z)*0.1;
            billyPosition.x += copysign(1,-billyPosition.z)*0.1;
        }
    }
    if (Keyboard::key['k']) { // Backward
        if (billyPosition.z < 490 && billyPosition.z > -334
            && billyPosition.x < 445 && billyPosition.x > -466) {
            billyPosition.x -= (WALKING_SPEED*DELTA_TIME)*MaffStuff::dsin(billyRotation.y + 180);
            billyPosition.z += (WALKING_SPEED*DELTA_TIME)*MaffStuff::dcos(billyRotation.y + 180);
        }
        else {
            billyPosition.z += copysign(1,-billyPosition.z)*0.1;
            billyPosition.x += copysign(1,-billyPosition.z)*0.1;
        }
    }
    if (Keyboard::key['j']) { // Strafe left
        billyRotation.y += 1;
    }
    if (Keyboard::key['l']) { // Strafe right
        billyRotation.y -= 1;
    }
    
    // Update spotlight position to follow the camera's POV
    lightPos[0] = Camera::position.x;
    lightPos[1] = Camera::position.y;
    lightPos[2] = Camera::position.z;
    spotDir[0] = Camera::position.x;
    spotDir[1] = Camera::position.y;
    spotDir[2] = Camera::position.z;
    // Update spotlight to follow directly above student
    lightPos2[0] = billyPosition.x;
    lightPos2[1] = billyPosition.y + 50;
    lightPos2[2] = billyPosition.z;

    
    if (Keyboard::key['1']) {
        toggleLight(1);
    }
    if (Keyboard::key['2']) {
        toggleLight(2);
    }
    if (Keyboard::key['3']) {
        toggleLight(3);
    }
    if (Keyboard::key['4']) {
        toggleLight(4);
    }
    if (Keyboard::key['-']) {
        if (glIsEnabled(GL_LIGHT0)) {
            theta -= 1;
            glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,theta);
        }
        if (glIsEnabled(GL_LIGHT1)) {
            theta2 -= 1;
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, theta2);
        }
    }
    if (Keyboard::key['=']) {
        if (glIsEnabled(GL_LIGHT0)) {
            theta += 1;
            glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,theta);
        }
        if (glIsEnabled(GL_LIGHT1)) {
            theta2 += 1;
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, theta2);
        }
    }
    glutPostRedisplay();
}

void initLight() {
    glEnable(GL_LIGHTING);  // Enable lighting
    // SPOTLIGHT
    glLightfv(GL_LIGHT0,GL_SPECULAR,specularWhite);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
    // Specific spot effects
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,theta);     // Cut off angle is 60 degrees
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,100.0f);        // Fairly shiny spot
    glEnable(GL_LIGHT0);     // Enable this light in particular
    
    // AMBIENT
    glLightfv(GL_LIGHT1, GL_SPECULAR,specularBlue);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir2);
//    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    // Specific spot effects
    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,theta2);     // Cut off angle is 60 degrees
    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,100.0f);        // Fairly shiny spot
    glEnable(GL_LIGHT1);     // Enable this light in particular
}

void toggleLight(int x) {
    switch (x) {
        case 1:
            theta = 60;
            glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,theta);
            glEnable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            break;
        case 2:
            glDisable(GL_LIGHT0);
            theta2 = 60;
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,theta2);
            glEnable(GL_LIGHT1);
            break;
        case 3:
            theta = 60;
            glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,theta);
            theta2 = 60;
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,theta2);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            break;
        case 4:
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            break;
        default:
            break;
    }
}

void reshape(int width, int height) {
    Viewport::width = width;
    Viewport::height = height;
    
    
    if(Viewport::height == 0) Viewport::height = 1;
    
    float ratio = (float) Viewport::width/Viewport::height;
    
    glMatrixMode(GL_PROJECTION);
    
    glViewport(0, 0, Viewport::width, Viewport::height);
    
    gluPerspective(45, ratio, 0.01, 1000);
    
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    preProcessEvents();
    
    drawBilly();
    drawBuildingRoof();
    drawBuildingFloor();
    drawSides();
    cout << "z pos: "<< Camera::position.z << endl;
    glutSwapBuffers();
}

void tour() {
    Keyboard::keyDown('w', 0, 0);
    Keyboard::keyDown('a', 0, 0);
}

void drawBilly() {
    glPushMatrix();
    // Camera Transformations
    glTranslatef(-billyPosition.x, 0, billyPosition.z);
    glRotatef(billyRotation.y, 0, 1, 0);
    glScalef(10, 10, 10);
    
    /* Front portion of student */
    
    // Hair
    glColor3f(DarkBrown.redValue, DarkBrown.greenValue, DarkBrown.blueValue);
    glBegin(GL_QUADS);
    glVertex3f(0.15, 0.90, zfront);
    glVertex3f(-0.15, 0.90, zfront);
    glVertex3f(-0.15, 0.80, zfront);
    glVertex3f(0.15, 0.80, zfront);
    fVert1 = {-0.15, 0.90, zfront};
    fVert2 = {-0.15, 0.80, zfront};
    fVert3 = {0.15, 0.80, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Skin
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.15, 0.80, zfront);
    glVertex3f(-0.15, 0.80, zfront);
    glVertex3f(-0.15, 0.60, zfront);
    glVertex3f(0.15, 0.60, zfront);
    fVert1 = {-0.15, 0.90, zfront};
    fVert2 = {-0.15, 0.80, zfront};
    fVert3 = {0.15, 0.80, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Eyes
    glColor3f(1, 1, 1);
    glVertex3f(-0.055, 0.75, zfront+0.001);
    glVertex3f(-0.075, 0.75, zfront+0.001);
    glVertex3f(-0.075, 0.7, zfront+0.001);
    glVertex3f(-0.055, 0.7, zfront+0.001);
    fVert1 = {-0.055, 0.75, zfront+0.001};
    fVert2 = {-0.075, 0.75, zfront+0.001};
    fVert3 = {-0.075, 0.7, zfront+0.001};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.055, 0.75, zfront+0.001);
    glVertex3f(0.075, 0.75, zfront+0.001);
    glVertex3f(0.075, 0.70, zfront+0.001);
    glVertex3f(0.055, 0.70, zfront+0.001);
    fVert1 = {0.055, 0.75, zfront+0.001};
    fVert2 = {0.075, 0.75, zfront+0.001};
    fVert3 = {0.075, 0.70, zfront+0.001};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glColor3f(0, 0, 1);
    glVertex3f(-0.035, 0.75, zfront+0.001);
    glVertex3f(-0.055, 0.75, zfront+0.001);
    glVertex3f(-0.055, 0.7, zfront+0.001);
    glVertex3f(-0.035, 0.7, zfront+0.001);
    fVert1 = {-0.035, 0.75, zfront+0.001};
    fVert2 = {-0.055, 0.75, zfront+0.001};
    fVert3 = {-0.055, 0.7, zfront+0.001};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.035, 0.75, zfront+0.001);
    glVertex3f(0.055, 0.75, zfront+0.001);
    glVertex3f(0.055, 0.70, zfront+0.001);
    glVertex3f(0.035, 0.70, zfront+0.001);
    fVert1 = {0.035, 0.75, zfront+0.001};
    fVert2 = {0.055, 0.75, zfront+0.001};
    fVert3 = {0.055, 0.70, zfront+0.001};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Mouth
    glColor3f(0, 0, 0);
    glVertex3f(0.04, 0.65, zfront+0.001);
    glVertex3f(-0.04, 0.65, zfront+0.001);
    glVertex3f(-0.04, 0.625, zfront+0.001);
    glVertex3f(0.04, 0.625, zfront+0.001);
    fVert1 = {0.04, 0.65, zfront+0.001};
    fVert2 = {-0.04, 0.65, zfront+0.001};
    fVert3 = {-0.04, 0.625, zfront+0.001};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Neck
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.055, 0.60, zfront);
    glVertex3f(-0.055, 0.60, zfront);
    glVertex3f(-0.055, 0.575, zfront);
    glVertex3f(0.055, 0.575, zfront);
    fVert1 = {0.055, 0.60, zfront};
    fVert2 = {-0.055, 0.60, zfront};
    fVert3 = {-0.055, 0.575, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Tshirt
    glColor3f(1, 0.5, 0);
    glVertex3f(0.075, 0.575, zfront);
    glVertex3f(-0.075, 0.575, zfront);
    glVertex3f(-0.075, 0.35, zfront);
    glVertex3f(0.075, 0.35, zfront);
    fVert1 = {0.075, 0.575, zfront};
    fVert2 = {-0.075, 0.575, zfront};
    fVert3 = {-0.075, 0.35, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.075, 0.575, zfront);
    glVertex3f(-0.099, 0.575, zfront);
    glVertex3f(-0.099, 0.50, zfront);
    glVertex3f(-0.075, 0.5, zfront);
    fVert1 = {-0.075, 0.575, zfront};
    fVert2 = {-0.099, 0.575, zfront};
    fVert3 = {-0.099, 0.50, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.075, 0.575, zfront);
    glVertex3f(0.099, 0.575, zfront);
    glVertex3f(0.099, 0.50, zfront);
    glVertex3f(0.075, 0.50, zfront);
    fVert1 = {0.075, 0.575, zfront};
    fVert2 = {0.099, 0.575, zfront};
    fVert3 = {0.099, 0.50, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.15, 0.575, zfront);
    glVertex3f(0.099, 0.575, zfront);
    glVertex3f(0.099, 0.45, zfront);
    glVertex3f(0.15, 0.45, zfront);
    fVert1 = {0.15, 0.575, zfront};
    fVert2 = {0.099, 0.575, zfront};
    fVert3 = {0.099, 0.45, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.15, 0.575, zfront);
    glVertex3f(-0.099, 0.575, zfront);
    glVertex3f(-0.099, 0.45, zfront);
    glVertex3f(-0.15, 0.45, zfront);
    fVert1 = {-0.15, 0.575, zfront};
    fVert2 = {-0.099, 0.575, zfront};
    fVert3 = {-0.099, 0.45, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Wrists
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.15, 0.45, zfront);
    glVertex3f(0.099, 0.45, zfront);
    glVertex3f(0.099, 0.35, zfront);
    glVertex3f(0.15, 0.35, zfront);
    fVert1 = {0.15, 0.45, zfront};
    fVert2 = {0.099, 0.45, zfront};
    fVert3 = {0.099, 0.35, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.15, 0.45, zfront);
    glVertex3f(-0.099, 0.45, zfront);
    glVertex3f(-0.099, 0.35, zfront);
    glVertex3f(-0.15, 0.35, zfront);
    fVert1 = {-0.15, 0.45, zfront};
    fVert2 = {-0.099, 0.45, zfront};
    fVert3 = {-0.099, 0.35, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Pants
    glColor3f(NavyBlue.redValue, NavyBlue.greenValue, NavyBlue.blueValue);
    glVertex3f(0.075, 0.35, zfront);
    glVertex3f(-0.075, 0.35, zfront);
    glVertex3f(-0.075, 0.275, zfront);
    glVertex3f(0.075, 0.275, zfront);
    fVert1 = {0.075, 0.35, zfront};
    fVert2 = {-0.075, 0.35, zfront};
    fVert3 = {-0.075, 0.275, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.010, 0.275, zfront);
    glVertex3f(-0.075, 0.275, zfront);
    glVertex3f(-0.075, 0.100, zfront);
    glVertex3f(-0.01, 0.1, zfront);
    fVert1 = {-0.010, 0.275, zfront};
    fVert2 = {-0.075, 0.275, zfront};
    fVert3 = {-0.075, 0.100, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.010, 0.275, zfront);
    glVertex3f(0.075, 0.275, zfront);
    glVertex3f(0.075, 0.100, zfront);
    glVertex3f(0.010, 0.100, zfront);
    fVert1 = {0.010, 0.275, zfront};
    fVert2 = {0.075, 0.275, zfront};
    fVert3 = {0.075, 0.100, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Shoes
    glColor3f(0, 0, 0);
    glVertex3f(-0.010, 0.1, zshoe);
    glVertex3f(-0.075, 0.1, zshoe);
    glVertex3f(-0.075, .05, zshoe);
    glVertex3f(-0.010, .05, zshoe);
    fVert1 = {-0.010, 0.1, zshoe};
    fVert2 = {-0.075, 0.1, zshoe};
    fVert3 = {-0.075, .05, zshoe};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.010, 0.1, zshoe);
    glVertex3f(0.075, 0.1, zshoe);
    glVertex3f(0.075, .05, zshoe);
    glVertex3f(0.010, .05, zshoe);
    fVert1 = {0.010, 0.1, zshoe};
    fVert2 = {0.075, 0.1, zshoe};
    fVert3 = {0.075, .05, zshoe};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    /* Back portion of student */
    
    // Hair
    glColor3f(DarkBrown.redValue, DarkBrown.greenValue, DarkBrown.blueValue);
    glBegin(GL_QUADS);
    glVertex3f(0.15, 0.90, zback);
    glVertex3f(-0.15, 0.90, zback);
    glVertex3f(-0.15, 0.80, zback);
    glVertex3f(0.15, 0.80, zback);
    fVert1 = {0.15, 0.90, zback};
    fVert2 = {-0.15, 0.90, zback};
    fVert3 = {-0.15, 0.80, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Skin
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.15, 0.80, zback);
    glVertex3f(-0.15, 0.80, zback);
    glVertex3f(-0.15, 0.60, zback);
    glVertex3f(0.15, 0.60, zback);
    fVert1 = {0.15, 0.80, zback};
    fVert2 = {-0.15, 0.80, zback};
    fVert3 = {-0.15, 0.60, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Neck
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.055, 0.600, zback);
    glVertex3f(-0.055, 0.60, zback);
    glVertex3f(-0.055, .575, zback);
    glVertex3f(0.055, 0.575, zback);
    fVert1 = {0.055, 0.600, zback};
    fVert2 = {-0.055, 0.60, zback};
    fVert3 = {-0.055, .575, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Tshirt
    glColor3f(1, 0.5, 0);
    glVertex3f(0.075, 0.575, zback);
    glVertex3f(-0.075, .575, zback);
    glVertex3f(-0.075, 0.35, zback);
    glVertex3f(0.075, 0.35, zback);
    fVert1 = {0.075, 0.575, zback};
    fVert2 = {-0.075, .575, zback};
    fVert3 = {-0.075, 0.35, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.075, 0.575, zback);
    glVertex3f(-0.099, 0.575, zback);
    glVertex3f(-0.099, 0.500, zback);
    glVertex3f(-0.075, 0.5, zback);
    fVert1 = {-0.075, 0.575, zback};
    fVert2 = {-0.099, 0.575, zback};
    fVert3 = {-0.099, 0.500, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.075, 0.575, zback);
    glVertex3f(0.099, 0.575, zback);
    glVertex3f(0.099, 0.500, zback);
    glVertex3f(0.075, 0.5, zback);
    fVert1 = {0.075, 0.575, zback};
    fVert2 = {0.099, 0.575, zback};
    fVert3 = {0.099, 0.500, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.150, 0.575, zback);
    glVertex3f(0.099, 0.575, zback);
    glVertex3f(0.099, 0.450, zback);
    glVertex3f(0.15, 0.45, zback);
    fVert1 = {0.150, 0.575, zback};
    fVert2 = {0.099, 0.575, zback};
    fVert3 = {0.099, 0.450, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.150, 0.575, zback);
    glVertex3f(-0.099, 0.575, zback);
    glVertex3f(-0.099, 0.450, zback);
    glVertex3f(-0.15, 0.45, zback);
    fVert1 = {-0.150, 0.575, zback};
    fVert2 = {-0.099, 0.575, zback};
    fVert3 = {-0.099, 0.450, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Wrists
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.150, 0.45, zback);
    glVertex3f(0.099, 0.45, zback);
    glVertex3f(0.099, 0.35, zback);
    glVertex3f(0.15, 0.35, zback);
    fVert1 = {0.150, 0.45, zback};
    fVert2 = {0.099, 0.45, zback};
    fVert3 = {0.099, 0.35, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.150, 0.45, zback);
    glVertex3f(-0.099, 0.45, zback);
    glVertex3f(-0.099, 0.35, zback);
    glVertex3f(-0.15, 0.35, zback);
    fVert1 = {-0.150, 0.45, zback};
    fVert2 = {-0.099, 0.45, zback};
    fVert3 = {-0.099, 0.35, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Pants
    glColor3f(NavyBlue.redValue, NavyBlue.greenValue, NavyBlue.blueValue);
    glVertex3f(0.075, 0.350, zback);
    glVertex3f(-0.075, 0.35, zback);
    glVertex3f(-0.075, .275, zback);
    glVertex3f(0.075, 0.275, zback);
    fVert1 = {0.075, 0.350, zback};
    fVert2 = {-0.075, 0.35, zback};
    fVert3 = {-0.075, .275, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.01, 0.275, zback);
    glVertex3f(-0.075, .275, zback);
    glVertex3f(-0.075, 0.10, zback);
    glVertex3f(-0.01, 0.1, zback);
    fVert1 = {-0.01, 0.275, zback};
    fVert2 = {-0.075, .275, zback};
    fVert3 = {-0.075, 0.10, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.010, 0.275, zback);
    glVertex3f(0.075, 0.275, zback);
    glVertex3f(0.075, 0.100, zback);
    glVertex3f(0.01, 0.1, zback);
    fVert1 = {0.010, 0.275, zback};
    fVert2 = {0.075, 0.275, zback};
    fVert3 = {0.075, 0.100, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Shoes
    glColor3f(0, 0, 0);
    glVertex3f(-0.010, 0.1, zback);
    glVertex3f(-0.075, 0.1, zback);
    glVertex3f(-0.075, .05, zback);
    glVertex3f(-0.01, 0.05, zback);
    fVert1 = {-0.010, 0.1, zback};
    fVert2 = {-0.075, 0.1, zback};
    fVert3 = {-0.075, .05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.01, 0.10, zback);
    glVertex3f(0.075, 0.1, zback);
    glVertex3f(0.075, .05, zback);
    glVertex3f(0.01, 0.05, zback);
    fVert1 = {0.01, 0.10, zback};
    fVert2 = {0.075, 0.1, zback};
    fVert3 = {0.075, .05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    /* Left Side */
    
    // Hair
    glColor3f(DarkBrown.redValue, DarkBrown.greenValue, DarkBrown.blueValue);
    glBegin(GL_QUADS);
    glVertex3f(-0.15, 0.90, zfront);
    glVertex3f(-0.15, 0.90, zback);
    glVertex3f(-0.15, 0.80, zback);
    glVertex3f(-0.15, 0.80, zfront);
    fVert1 = {-0.15, 0.90, zfront};
    fVert2 = {-0.15, 0.90, zback};
    fVert3 = {-0.15, 0.80, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Skin
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(-0.15, 0.80, zfront);
    glVertex3f(-0.15, 0.80, zback);
    glVertex3f(-0.15, 0.60, zback);
    glVertex3f(-0.15, 0.60, zfront);
    fVert1 = {-0.15, 0.80, zfront};
    fVert2 = {-0.15, 0.80, zback};
    fVert3 = {-0.15, 0.60, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Neck
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(-0.055, 0.6, zfront);
    glVertex3f(-0.055, 0.6, zback);
    glVertex3f(-0.055, 0.575, zback);
    glVertex3f(-0.055, 0.575, zfront);
    fVert1 = {-0.055, 0.6, zfront};
    fVert2 = {-0.055, 0.6, zback};
    fVert3 = {-0.055, 0.575, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Tshirt
    glColor3f(1, 0.5, 0);
    glVertex3f(-0.075, 0.575, zfront);
    glVertex3f(-0.075, 0.575, zback);
    glVertex3f(-0.075, 0.350, zback);
    glVertex3f(-0.075, 0.35, zfront);
    fVert1 = {-0.075, 0.575, zfront};
    fVert2 = {-0.075, 0.575, zback};
    fVert3 = {-0.075, 0.350, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.099, 0.575, zfront);
    glVertex3f(0.099, 0.575, zback);
    glVertex3f(0.099, 0.450, zback);
    glVertex3f(0.099, 0.45, zfront);
    fVert1 = {0.099, 0.575, zfront};
    fVert2 = {0.099, 0.575, zback};
    fVert3 = {0.099, 0.450, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Wrists
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.099, 0.45, zfront);
    glVertex3f(0.099, 0.45, zback);
    glVertex3f(0.099, 0.35, zback);
    glVertex3f(0.099, 0.35, zfront);
    fVert1 = {0.099, 0.45, zfront};
    fVert2 = {0.099, 0.45, zback};
    fVert3 = {0.099, 0.35, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.15, 0.45, zfront);
    glVertex3f(0.15, 0.45, zback);
    glVertex3f(0.15, 0.35, zback);
    glVertex3f(0.15, 0.35, zfront);
    fVert1 = {0.15, 0.45, zfront};
    fVert2 = {0.15, 0.45, zback};
    fVert3 = {0.15, 0.35, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Pants
    glColor3f(NavyBlue.redValue, NavyBlue.greenValue, NavyBlue.blueValue);
    glVertex3f(-0.075, 0.35, zfront);
    glVertex3f(-0.075, 0.350, zback);
    glVertex3f(-0.075, 0.100, zback);
    glVertex3f(-0.075, 0.1, zfront);
    fVert1 = {-0.075, 0.35, zfront};
    fVert2 = {-0.075, 0.350, zback};
    fVert3 = {-0.075, 0.100, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Shoes
    glColor3f(0, 0, 0);
    glVertex3f(-0.075, 0.10, zshoe);
    glVertex3f(-0.075, 0.10, zback);
    glVertex3f(-0.075, 0.05, zback);
    glVertex3f(-0.075, 0.05, zshoe);
    fVert1 = {-0.075, 0.10, zshoe};
    fVert2 = {-0.075, 0.10, zback};
    fVert3 = {-0.075, 0.05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.01, 0.10, zshoe);
    glVertex3f(-0.01, 0.10, zback);
    glVertex3f(-0.01, 0.05, zback);
    glVertex3f(-0.01, 0.05, zshoe);
    fVert1 = {-0.01, 0.10, zshoe};
    fVert2 = {-0.01, 0.10, zback};
    fVert3 = {-0.01, 0.05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    /* Right Side */
    
    // Hair
    glColor3f(DarkBrown.redValue, DarkBrown.greenValue, DarkBrown.blueValue);
    glBegin(GL_QUADS);
    glVertex3f(0.15, 0.90, zfront);
    glVertex3f(0.15, 0.90, zback);
    glVertex3f(0.15, 0.80, zback);
    glVertex3f(0.15, 0.80, zfront);
    fVert1 = {0.15, 0.90, zfront};
    fVert2 = {0.15, 0.90, zback};
    fVert3 = {0.15, 0.80, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Skin
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.15, 0.8, zfront);
    glVertex3f(0.15, 0.80, zback);
    glVertex3f(0.15, 0.60, zback);
    glVertex3f(0.15, 0.60, zfront);
    fVert1 = {0.15, 0.8, zfront};
    fVert2 = {0.15, 0.80, zback};
    fVert3 = {0.15, 0.60, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Neck
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(0.055, 0.60, zfront);
    glVertex3f(0.055, 0.600, zback);
    glVertex3f(0.055, 0.575, zback);
    glVertex3f(0.055, 0.575, zfront);
    fVert1 = {0.055, 0.60, zfront};
    fVert2 = {0.055, 0.600, zback};
    fVert3 = {0.055, 0.575, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Tshirt
    glColor3f(1, 0.5, 0);
    glVertex3f(0.075, 0.575, zfront);
    glVertex3f(0.075, 0.5750, zback);
    glVertex3f(0.075, 0.3500, zback);
    glVertex3f(0.075, 0.35, zfront);
    fVert1 = {0.075, 0.575, zfront};
    fVert2 = {0.075, 0.5750, zback};
    fVert3 = {0.075, 0.3500, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.099, .575, zfront);
    glVertex3f(-0.099, 0.575, zback);
    glVertex3f(-0.099, 0.450, zback);
    glVertex3f(-0.099, 0.45, zfront);
    fVert1 = {-0.099, .575, zfront};
    fVert2 = {-0.099, 0.575, zback};
    fVert3 = {-0.099, 0.450, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Wrists
    glColor3f(Tan.redValue, Tan.greenValue, Tan.blueValue);
    glVertex3f(-0.099, 0.45, zfront);
    glVertex3f(-0.099, 0.450, zback);
    glVertex3f(-0.099, 0.350, zback);
    glVertex3f(-0.099, 0.35, zfront);
    fVert1 = {-0.099, 0.45, zfront};
    fVert2 = {-0.099, 0.450, zback};
    fVert3 = {-0.099, 0.350, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(-0.15, 0.45, zfront);
    glVertex3f(-0.15, 0.450, zback);
    glVertex3f(-0.15, 0.350, zback);
    glVertex3f(-0.15, 0.35, zfront);
    fVert1 = {-0.15, 0.45, zfront};
    fVert2 = {-0.15, 0.450, zback};
    fVert3 = {-0.15, 0.350, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Pants
    glColor3f(NavyBlue.redValue, NavyBlue.greenValue, NavyBlue.blueValue);
    glVertex3f(0.075, 0.35, zfront);
    glVertex3f(0.075, 0.350, zback);
    glVertex3f(0.075, 0.100, zback);
    glVertex3f(0.075, 0.1, zfront);
    fVert1 = {0.075, 0.35, zfront};
    fVert2 = {0.075, 0.350, zback};
    fVert3 = {0.075, 0.100, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Shoes
    glColor3f(0, 0, 0);
    glVertex3f(0.075, 0.10, zshoe);
    glVertex3f(0.075, 0.10, zback);
    glVertex3f(0.075, 0.05, zback);
    glVertex3f(0.075, 0.05, zshoe);
    fVert1 = {0.075, 0.10, zshoe};
    fVert2 = {0.075, 0.10, zback};
    fVert3 = {0.075, 0.05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glVertex3f(0.01, 0.10, zshoe);
    glVertex3f(0.01, 0.10, zback);
    glVertex3f(0.01, 0.05, zback);
    glVertex3f(0.01, 0.05, zshoe);
    fVert1 = {0.075, 0.10, zshoe};
    fVert2 = {0.075, 0.10, zback};
    fVert3 = {0.075, 0.05, zback};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    /* Cell Phone */
    // Front
    glColor3f(0, 0, 0);
    glVertex3f(0.10, 0.35, zfront);
    glVertex3f(0.15, 0.35, zfront);
    glVertex3f(0.15, 0.20, zfront);
    glVertex3f(0.1, 0.20, zfront);
    fVert1 = {0.10, 0.35, zfront};
    fVert2 = {0.15, 0.35, zfront};
    fVert3 = {0.15, 0.20, zfront};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Back
    glVertex3f(0.10, 0.35, zback+.09);
    glVertex3f(0.15, 0.35, zback+.09);
    glVertex3f(0.15, 0.20, zback+.09);
    glVertex3f(0.1, 0.20, zback+.09);
    fVert1 = {0.10, 0.35, zback+.09};
    fVert2 = {0.15, 0.35, zback+.09};
    fVert3 = {0.15, 0.20, zback+.09};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Right Side
    glVertex3f(0.1, 0.350000, zfront);
    glVertex3f(0.1, 0.35, zback+0.09);
    glVertex3f(0.1, 0.20, zback+0.09);
    glVertex3f(0.1, 0.20, zfront);
    fVert1 = {0.1, 0.350000, zfront};
    fVert2 = {0.1, 0.35, zback+0.09};
    fVert3 = {0.1, 0.20, zback+0.09};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Right Side
    glVertex3f(0.150, 0.35000, zfront);
    glVertex3f(0.15, 0.35, zback+0.09);
    glVertex3f(0.15, 0.20, zback+0.09);
    glVertex3f(0.15, 0.20, zfront);
    fVert1 = {0.150, 0.35000, zfront};
    fVert2 = {0.15, 0.35, zback+0.09};
    fVert3 = {0.15, 0.20, zback+0.09};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    // Panel
    glColor3f(1, 0, 0);
    glVertex3f(.1125, .34, zfront+0.01);
    glVertex3f(0.14, 0.34, zfront+0.01);
    glVertex3f(0.14, 0.21, zfront+0.01);
    glVertex3f(0.1125, 0.21, zfront+0.01);
    fVert1 = {.1125, .34, zfront+0.01};
    fVert2 = {0.14, 0.34, zfront+0.01};
    fVert3 = {0.14, 0.21, zfront+0.01};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    
    glEnd();
    glPopMatrix();
}

// Sides are red
void drawSides() {
    glPushMatrix();
    glScalef(500, 1, 500);
    glColor3f(1, 0, 0);
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 100, 0.68);
    glVertex3f(0.90, 100, 0.68);
    glVertex3f(0.90, -1., 0.68);
    glVertex3f(-0.125, -1, 0.68);
    fVert1 = {-0.125, 100, 0.68};
    fVert2 = {0.90, 100, 0.68};
    fVert3 = {0.90, -1., 0.68};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 100, 0.68);
    glVertex3f(-0.125, 100, 0.51);
    glVertex3f(-0.125, -1, 0.51);
    glVertex3f(-0.125, -1, 0.68);
    fVert1 = {-0.125, 100, 0.68};
    fVert2 = {-0.125, 100, 0.51};
    fVert3 = {-0.125, -1, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 100, 0.51);
    glVertex3f(-0.125, -1, 0.51);
    glVertex3f(-0.785, -1, 0.51);
    glVertex3f(-0.785, 100, 0.51);
    fVert1 = {-0.125, 100, 0.51};
    fVert2 = {-0.125, -1, 0.51};
    fVert3 = {-0.785, -1, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 100, 0.51);
    glVertex3f(-0.785, 100, -0.17);
    glVertex3f(-0.785, -1, -0.17);
    glVertex3f(-0.785, -1, 0.51);
    fVert1 = {-0.785, 100, 0.51};
    fVert2 = {-0.785, 100, -0.17};
    fVert3 = {-0.785, -1, -0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.375, 100, -0.17);
    glVertex3f(-0.785, 100, -0.17);
    glVertex3f(-0.785, -1, -0.17);
    glVertex3f(-0.375, -1, -0.17);
    fVert1 = {-0.375, 100, -0.17};
    fVert2 = {-0.785, 100, -0.17};
    fVert3 = {-0.785, -1, -0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.375, 100, -0.17);
    glVertex3f(-0.375, 100, -0.33);
    glVertex3f(-0.375, -1, -0.33);
    glVertex3f(-0.375, -1, -0.17);
    fVert1 = {-0.375, 100, -0.17};
    fVert2 = {-0.375, 100, -0.33};
    fVert3 = {-0.375, -1, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 100, -0.33);
    glVertex3f(-0.375, 100, -0.33);
    glVertex3f(-0.375, -1, -0.33);
    glVertex3f(-0.785, -1, -0.33);
    fVert1 = {-0.785, 100, -0.33};
    fVert2 = {-0.375, 100, -0.33};
    fVert3 = {-0.375, -1, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 100, -0.33);
    glVertex3f(-0.785, 100, -0.38);
    glVertex3f(-0.785, -1, -0.38);
    glVertex3f(-0.785, -1, -0.33);
    fVert1 = {-0.785, 100, -0.33};
    fVert2 = {-0.785, 100, -0.38};
    fVert3 = {-0.785, -1, -0.38};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.95, 100, -0.38);
    glVertex3f(-0.785, 100, -0.38);
    glVertex3f(-0.785, -1, -0.38);
    glVertex3f(-0.95, -1, -0.38);
    fVert1 = {-0.95, 100, -0.38};
    fVert2 = {-0.785, 100, -0.38};
    fVert3 = {-0.785, -1, -0.38};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.95, 100, -0.38);
    glVertex3f(-0.95, 100, -0.66);
    glVertex3f(-0.95, -1, -0.66);
    glVertex3f(-0.95, -1, -0.38);
    fVert1 = {-0.95, 100, -0.38};
    fVert2 = {-0.95, 100, -0.66};
    fVert3 = {-0.95, -1, -0.66};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.95, 100, -0.66);
    glVertex3f(-0.785, 100, -0.66);
    glVertex3f(-0.785, -1, -0.66);
    glVertex3f(-0.95, -1, -0.66);
    fVert1 = {-0.95, 100, -0.66};
    fVert2 = {-0.785, 100, -0.66};
    fVert3 = {-0.785, -1, -0.66};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 100, -0.66);
    glVertex3f(-0.785, 100, -1.00);
    glVertex3f(-0.785, -1.0, -1.0);
    glVertex3f(-0.785, -1, -0.66);
    fVert1 = {-0.785, 100, -0.66};
    fVert2 = {-0.785, 100, -1.00};
    fVert3 = {-0.785, -1.0, -1.0};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 100, -1);
    glVertex3f(0.535, 100, -1.);
    glVertex3f(0.535, -1.0, -1);
    glVertex3f(-0.785, -1, -1);
    fVert1 = {-0.785, 100, -1};
    fVert2 = {0.535, 100, -1.};
    fVert3 = {0.535, -1.0, -1};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.535, 100, -0.84);
    glVertex3f(0.535, 100, -1.00);
    glVertex3f(0.535, -1.0, -1.0);
    glVertex3f(0.535, -1, -0.84);
    fVert1 = {0.535, 100, -0.84};
    fVert2 = {0.535, 100, -1.00};
    fVert3 = {0.535, -1.0, -1.0};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 100, -0.84);
    glVertex3f(0.535, 100, -0.84);
    glVertex3f(0.535, -1., -0.84);
    glVertex3f(0.625, -1, -0.84);
    fVert1 = {0.625, 100, -0.84};
    fVert2 = {0.535, 100, -0.84};
    fVert3 = {0.535, -1., -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 100, -0.33);
    glVertex3f(0.625, 100, -0.84);
    glVertex3f(0.625, -1., -0.84);
    glVertex3f(0.625, -1, -0.33);
    fVert1 = {0.625, 100, -0.33};
    fVert2 = {0.625, 100, -0.84};
    fVert3 = {0.625, -1., -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 100, -0.33);
    glVertex3f(0.625, 100, -0.33);
    glVertex3f(0.625, -1., -0.33);
    glVertex3f(0.375, -1, -0.33);
    fVert1 = {0.375, 100, -0.33};
    fVert2 = {0.625, 100, -0.33};
    fVert3 = {0.625, -1., -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 100.0, 0.0);
    glVertex3f(0.375, 100, -0.33);
    glVertex3f(0.375, -1., -0.33);
    glVertex3f(0.375, -1, 0);
    fVert1 = {0.375, 100.0, 0.0};
    fVert2 = {0.375, 100, -0.33};
    fVert3 = {0.375, -1., -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 100, 0);
    glVertex3f(0.845, 100, 0);
    glVertex3f(0.845, -1, 0);
    glVertex3f(0.375, -1, 0);
    fVert1 = {0.375, 100, 0};
    fVert2 = {0.845, 100, 0};
    fVert3 = {0.845, -1, 0};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.845, 100, 0);
    glVertex3f(0.9, 100.0, 0);
    glVertex3f(0.9, -1.0, 0.);
    glVertex3f(0.845, -1, 0);
    fVert1 = {0.845, 100, 0};
    fVert2 = {0.9, 100.0, 0};
    fVert3 = {0.9, -1.0, 0.};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(.9, 100, 0.34);
    glVertex3f(0.9, 100, 0.0);
    glVertex3f(0.9, -1.0, 0.);
    glVertex3f(0.9, -1, 0.34);
    fVert1 = {.9, 100, 0.34};
    fVert2 = {0.9, 100, 0.0};
    fVert3 = {0.9, -1.0, 0.};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.9, 100, 0.68);
    glVertex3f(0.9, 100, 0.34);
    glVertex3f(0.9, -1, 0.34);
    glVertex3f(0.9, -1, 0.68);
    fVert1 = {0.9, 100, 0.68};
    fVert2 = {0.9, 100, 0.34};
    fVert3 = {0.9, -1, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glPopMatrix();
}

// Roof is shades of grey
void drawBuildingRoof() {
    glPushMatrix();
    glTranslatef(0, 100, 0);
    glScalef(500, 1, 500);
    
    glColor3f(Grey.redValue, Grey.greenValue, Grey.blueValue);
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.50); //
    glVertex3f(-0.785, 0, -0.33); //
    glVertex3f(-0.785, 0, -0.84);
    glVertex3f(-0.535, 0, -0.67);
    fVert1 = {-0.535, 0, -0.50};
    fVert2 = {-0.785, 0, -0.33};
    fVert3 = {-0.785, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Upper roof - very light grey
    glColor3f(VLightGrey.redValue, VLightGrey.greenValue, VLightGrey.blueValue);
    glBegin(GL_QUAD_STRIP);
    glVertex3f(-0.125, 0,  0.00);
    glVertex3f(-0.125, 0,  0.17);
    glVertex3f(0.125, 0.,  0.00);
    glVertex3f(0.125, 0,  0.17);
    fVert1 = {-0.125, 0,  0.00};
    fVert2 = {-0.125, 0,  0.17};
    fVert3 = {0.125, 0.,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.125, 0,  0.17);
    glVertex3f(0.125, 0,  0.34);
    glVertex3f(-.535, 0,  0.17);
    glVertex3f(-0.535, 0,  0.34);
    fVert1 = {0.125, 0,  0.17};
    fVert2 = {0.125, 0,  0.34};
    fVert3 = {-.535, 0,  0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.125, 0,  0.17);
    glVertex3f(0.125, 0,  0.51);
    glVertex3f(0.625, 0,  0.17);
    glVertex3f(0.625, 0,  0.51);
    fVert1 = {0.125, 0,  0.17};
    fVert2 = {0.125, 0,  0.51};
    fVert3 = {0.625, 0,  0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //
    glBegin(GL_QUAD_STRIP);
    glVertex3f(-0.375, 0,  0.00);
    glVertex3f(-0.375, 0, -0.17); //
    glVertex3f(-0.785, 0,  0.00);
    glVertex3f(-0.785, 0, -0.17); //
    fVert1 = {-0.375, 0,  0.00};
    fVert2 = {-0.375, 0, -0.17};
    fVert3 = {-0.785, 0,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.845, 0,  0.00); //
    glVertex3f(0.845, 0,  0.34);
    glVertex3f(0.9, 0,  0.00); //
    glVertex3f(0.9, 0,  0.34); //
    fVert1 = {0.845, 0,  0.00};
    fVert2 = {0.845, 0,  0.34};
    fVert3 = {0.9, 0,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0,  0.00);
    glVertex3f(-0.125, 0,  -0.5);
    glVertex3f(0.125, 0,  -0.5);
    glVertex3f(0.125, 0,  0.00);
    fVert1 = {-0.125, 0,  0.00};
    fVert2 = {-0.125, 0,  -0.5};
    fVert3 = {0.125, 0,  -0.5};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.50);
    glVertex3f(-0.535, 0, -0.67);
    glVertex3f(0.375, 0, -0.67);
    glVertex3f(0.375, 0, -0.50);
    fVert1 = {-0.535, 0, -0.50};
    fVert2 = {-0.535, 0, -0.67};
    fVert3 = {0.375, 0, -0.67};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.84); //
    glVertex3f(-0.785, 0, -1.00); //
    glVertex3f(0.535, 0, -1.00); //
    glVertex3f(0.535, 0, -0.84); //
    fVert1 = {-0.785, 0, -0.84};
    fVert2 = {-0.785, 0, -1.00};
    fVert3 = {0.535, 0, -1.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - light grey
    glColor3f(LightGrey.redValue, LightGrey.greenValue, LightGrey.blueValue);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.125, 0, 0.17);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.125, 0, 0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.17);
    glVertex3f(-0.375, 0, 0.00);
    glVertex3f(-0.785, 0, 0.00);
    glVertex3f(-0.535, 0, 0.17);
    fVert1 = {-0.125, 0, 0.17};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.785, 0, 0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON); // eh?
    glVertex3f(0.125, 0, 0.34);
    glVertex3f(0.0, 0.0, 0.51);
    glVertex3f(-0.125, 0, 0.51);
    glVertex3f(0.125, 0, 0.34);
    fVert1 = {0.125, 0, 0.34};
    fVert2 = {0.0, 0.0, 0.51};
    fVert3 = {-0.125, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 0, 0.51);
    glVertex3f(0.68, 0, 0.51);
    glVertex3f(0.9, 0, 0.34);
    glVertex3f(0.845, 0, 0.34);
    fVert1 = {0.625, 0, 0.51};
    fVert2 = {0.68, 0, 0.51};
    fVert3 = {0.9, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.124, 0, 0.17);
    glVertex3f(0.375, 0, 0.00); //
    glVertex3f(0.845, 0, 0.00); //
    glVertex3f(0.625, 0, 0.17);
    fVert1 = {0.124, 0, 0.17};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.845, 0, 0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.00);
    glVertex3f(-0.375, 0, 0.00); //
    glVertex3f(-0.375, 0, -0.33);//
    glVertex3f(-0.125, 0, -0.5);
    fVert1 = {-0.125, 0, 0.00};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON); // eh?
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00); //
    glVertex3f(0.375, 0, -0.33); //
    glVertex3f(0.125, 0, -0.5);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.375, 0, -0.33);
    glVertex3f(0.125, 0, -0.50);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.67);
    glVertex3f(-0.785, 0, -0.84);
    glVertex3f(0.625, 0, -0.84); //
    glVertex3f(0.535, 0, -0.67);
    fVert1 = {-0.535, 0, -0.67};
    fVert2 = {-0.785, 0, -0.84};
    fVert3 = {0.625, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.66); //
    glVertex3f(-0.62, 0, -0.52);
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.95, 0, -0.66); //
    fVert1 = {-0.785, 0, -0.66};
    fVert2 = {-0.62, 0, -0.52};
    fVert3 = {-0.785, 0, -0.52};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - grey
    glColor3f(Grey.redValue, Grey.greenValue, Grey.blueValue);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.125, 0, 0.00);
    glVertex3f(-0.375, 0, 0.00);
    glVertex3f(-0.125, 0, 0.17);
    fVert1 = {-0.125, 0, 0.00};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.125, 0, 0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0.0, 0.); //
    glVertex3f(-0.785, 0, 0.51); //
    glVertex3f(-0.535, 0, 0.34);
    glVertex3f(-0.535, 0, 0.17);
    fVert1 = {-0.785, 0.0, 0.};
    fVert2 = {-0.785, 0, 0.51};
    fVert3 = {-0.535, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.68); //
    glVertex3f(-0.125, 0, 0.51); //
    glVertex3f(0.125, 0., 0.34);
    glVertex3f(0.125, 0, 0.51);
    fVert1 = {-0.125, 0, 0.68};
    fVert2 = {-0.125, 0, 0.51};
    fVert3 = {0.125, 0., 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(0.9, 0, 0.34); //
    glVertex3f(0.9, 0, 0.68); //
    glVertex3f(0.68, 0, 0.51);
    fVert1 = {0.9, 0, 0.34};
    fVert2 = {0.9, 0, 0.68};
    fVert3 = {0.68, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 0, 0.17);
    glVertex3f(0.625, 0, 0.51);
    glVertex3f(0.845, 0, 0.34);
    glVertex3f(0.845, 0, 0.00);
    fVert1 = {0.625, 0, 0.17};
    fVert2 = {0.625, 0, 0.51};
    fVert3 = {0.845, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 0, -0.50);
    glVertex3f(0.625, 0, -0.33); //
    glVertex3f(0.625, 0, -0.84); //
    glVertex3f(0.375, 0, -0.67);
    fVert1 = {0.375, 0, -0.50};
    fVert2 = {0.625, 0, -0.33};
    fVert3 = {0.625, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.95, 0., -0.38); //
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.95, 0., -0.66); //
    fVert1 = {-0.95, 0., -0.38};
    fVert2 = {-0.785, 0, -0.52};
    fVert3 = {-0.95, 0., -0.66};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - dark grey
    glColor3f(DimGrey.redValue, DimGrey.greenValue, DimGrey.blueValue);
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, 0.34);
    glVertex3f(-0.785, 0, 0.51); //
    glVertex3f(-0.125, 0, 0.51); //
    glVertex3f(0.125, 0, 0.34);
    fVert1 = {-0.535, 0, 0.34};
    fVert2 = {-0.785, 0, 0.51};
    fVert3 = {-0.125, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0., 0.51);
    glVertex3f(-0.125, 0, 0.68); //
    glVertex3f(0.90, 0.0, 0.68); //
    glVertex3f(0.68, 0, 0.51);
    fVert1 = {0.125, 0., 0.51};
    fVert2 = {-0.125, 0, 0.68};
    fVert3 = {0.90, 0.0, 0.68};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 0, -0.33); //
    glVertex3f(0.125, 0, -0.50);
    glVertex3f(0.375, 0, -0.50);
    glVertex3f(0.625, 0, -0.33); //
    fVert1 = {0.375, 0, -0.33};
    fVert2 = {0.125, 0, -0.50};
    fVert3 = {0.375, 0, -0.50};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.375, 0, -0.33); //
    glVertex3f(-0.785, 0, -0.33); //
    glVertex3f(-0.535, 0, -0.50);
    glVertex3f(-0.125, 0, -0.50);
    fVert1 = {-0.375, 0, -0.33};
    fVert2 = {-0.785, 0, -0.33};
    fVert3 = {-0.535, 0, -0.50};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.38); //
    glVertex3f(-0.95, 0., -0.38); //
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.62, 0, -0.52);
    fVert1 = {-0.785, 0, -0.38};
    fVert2 = {-0.95, 0., -0.38};
    fVert3 = {-0.785, 0, -0.52};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glPopMatrix();
}

// Floor is blue
void drawBuildingFloor() {
    glPushMatrix();
    glTranslatef(0, -1, 0);
    glScalef(500, 1, 500);
    
    glColor3f(0, 0 , 1);
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.50);
    glVertex3f(-0.785, 0, -0.33);
    glVertex3f(-0.785, 0, -0.84);
    glVertex3f(-0.535, 0, -0.67);
    fVert1 = {-0.535, 0, -0.50};
    fVert2 = {-0.785, 0, -0.33};
    fVert3 = {-0.785, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Upper roof - very light grey
    glBegin(GL_QUAD_STRIP);
    glVertex3f(-0.125, 0,  0.00);
    glVertex3f(-0.125, 0,  0.17);
    glVertex3f(0.125, 0,  0.00);
    glVertex3f(0.125, 0,  0.17);
    fVert1 = {-0.125, 0,  0.00};
    fVert2 = {-0.125, 0,  0.17};
    fVert3 = {0.125, 0,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(-0.375, 0,  0.00);
    glVertex3f(-0.375, 0, -0.17);
    glVertex3f(-0.785, 0,  0.00);
    glVertex3f(-0.785, 0, -0.17);
    fVert1 = {-0.375, 0,  0.00};
    fVert2 = {-0.375, 0, -0.17};
    fVert3 = {-0.785, 0,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.125, 0,  0.17);
    glVertex3f(0.125, 0,  0.34);
    glVertex3f(-0.535, 0,  0.17);
    glVertex3f(-0.535, 0,  0.34);
    fVert1 = {0.125, 0,  0.17};
    fVert2 = {0.125, 0,  0.34};
    fVert3 = {-0.535, 0,  0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.125, 0,  0.17);
    glVertex3f(0.125, 0,  0.51);
    glVertex3f(0.625, 0,  0.17);
    glVertex3f(0.625, 0,  0.51);
    fVert1 = {0.125, 0,  0.17};
    fVert2 = {0.125, 0,  0.51};
    fVert3 = {0.625, 0,  0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.845, 0,  0.00);
    glVertex3f(0.845, 0,  0.34);
    glVertex3f(0.9, 0,  0.00);
    glVertex3f(0.9, 0,  0.34);
    fVert1 = {0.845, 0,  0.00};
    fVert2 = {0.845, 0,  0.34};
    fVert3 = {0.9, 0,  0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0,  0.00);
    glVertex3f(-0.125, 0,  -0.5);
    glVertex3f(0.125, 0,  -0.5);
    glVertex3f(0.125, 0,  0.00);
    fVert1 = {-0.125, 0,  0.00};
    fVert2 = {-0.125, 0,  -0.5};
    fVert3 = {0.125, 0,  -0.5};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.50);
    glVertex3f(-0.535, 0, -0.67);
    glVertex3f(0.375, 0, -0.67);
    glVertex3f(0.375, 0, -0.50);
    fVert1 = {-0.535, 0, -0.50};
    fVert2 = {-0.535, 0, -0.67};
    fVert3 = {0.375, 0, -0.67};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.84);
    glVertex3f(-0.785, 0, -1.00);
    glVertex3f(0.535, 0, -1.00);
    glVertex3f(0.535, 0, -0.84);
    fVert1 = {-0.785, 0, -0.84};
    fVert2 = {-0.785, 0, -1.00};
    fVert3 = {0.535, 0, -1.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - light grey
    glBegin(GL_TRIANGLES);
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.125, 0, 0.17);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.125, 0, 0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.17);
    glVertex3f(-0.375, 0, 0.00);
    glVertex3f(-0.785, 0, 0.00);
    glVertex3f(-0.535, 0, 0.17);
    fVert1 = {-0.125, 0, 0.17};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.785, 0, 0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0., 0.34);
    glVertex3f(-0.0, 0.0, 0.51);
    glVertex3f(-0.125, 0, 0.51);
    glVertex3f(0.125, 0, 0.34);
    fVert1 = {0.125, 0., 0.34};
    fVert2 = {-0.0, 0.0, 0.51};
    fVert3 = {-0.125, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 0, 0.51);
    glVertex3f(0.68, 0, 0.51);
    glVertex3f(0.9, 0.0, 0.34);
    glVertex3f(0.845, 0, 0.34);
    fVert1 = {0.625, 0, 0.51};
    fVert2 = {0.68, 0, 0.51};
    fVert3 = {0.9, 0.0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.124, 0, 0.17);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.845, 0, 0.00);
    glVertex3f(0.625, 0, 0.17);
    fVert1 = {0.124, 0, 0.17};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.845, 0, 0.00};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.00);
    glVertex3f(-0.375, 0, 0.00);
    glVertex3f(-0.375, 0, -0.33);
    glVertex3f(-0.125, 0, -0.5);
    fVert1 = {-0.125, 0, 0.00};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.375, 0, -0.33);
    glVertex3f(0.125, 0, -0.5);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0, 0.00);
    glVertex3f(0.375, 0, 0.00);
    glVertex3f(0.375, 0, -0.33);
    glVertex3f(0.125, 0, -0.50);
    fVert1 = {0.125, 0, 0.00};
    fVert2 = {0.375, 0, 0.00};
    fVert3 = {0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, -0.67);
    glVertex3f(-0.785, 0, -0.84);
    glVertex3f(0.625, 0, -0.84);
    glVertex3f(0.535, 0, -0.67);
    fVert1 = {-0.535, 0, -0.67};
    fVert2 = {-0.785, 0, -0.84};
    fVert3 = {0.625, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.66);
    glVertex3f(-0.62, 0, -0.52);
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.95, 0, -0.66);
    fVert1 = {-0.785, 0, -0.66};
    fVert2 = {-0.62, 0, -0.52};
    fVert3 = {-0.785, 0, -0.52};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - grey
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.125, 0, 0.00);
    glVertex3f(-0.375, 0, 0.00);
    glVertex3f(-0.125, 0, 0.17);
    fVert1 = {-0.125, 0, 0.00};
    fVert2 = {-0.375, 0, 0.00};
    fVert3 = {-0.125, 0, 0.17};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0.0, 0);
    glVertex3f(-0.785, 0, 0.51);
    glVertex3f(-0.535, 0, 0.34);
    glVertex3f(-0.535, 0, 0.17);
    fVert1 = {-0.785, 0.0, 0};
    fVert2 = {-0.785, 0, 0.51};
    fVert3 = {-0.535, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.125, 0, 0.68);
    glVertex3f(-0.125, 0, 0.51);
    glVertex3f(0.125, 0, 0.34);
    glVertex3f(0.125, 0, 0.51);
    fVert1 = {-0.125, 0, 0.68};
    fVert2 = {-0.125, 0, 0.51};
    fVert3 = {0.125, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(0.9, 0, 0.34);
    glVertex3f(0.9, 0, 0.68);
    glVertex3f(0.68, 0, 0.51);
    fVert1 = {0.9, 0, 0.34};
    fVert2 = {0.9, 0, 0.68};
    fVert3 = {0.68, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.625, 0, 0.17);
    glVertex3f(0.625, 0, 0.51);
    glVertex3f(0.845, 0, 0.34);
    glVertex3f(0.845, 0, 0.00);
    fVert1 = {0.625, 0, 0.17};
    fVert2 = {0.625, 0, 0.51};
    fVert3 = {0.845, 0, 0.34};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 0, -0.5);
    glVertex3f(0.535, 0, -0.33);
    glVertex3f(0.375, 0, -0.33);
    glVertex3f(0.125, 0, -0.5);
    fVert1 = {0.375, 0, -0.5};
    fVert2 = {0.535, 0, -0.33};
    fVert3 = {0.375, 0, -0.33};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 0, -0.50);
    glVertex3f(0.625, 0, -0.33);
    glVertex3f(0.625, 0, -0.84);
    glVertex3f(0.375, 0, -0.67);
    fVert1 = {0.375, 0, -0.50};
    fVert2 = {0.625, 0, -0.33};
    fVert3 = {0.625, 0, -0.84};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.95, 0, -0.38);
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.95, 0, -0.66);
    fVert1 = {-0.95, 0, -0.38};
    fVert2 = {-0.785, 0, -0.52};
    fVert3 = {-0.95, 0, -0.66};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    //Slanted roof - dark grey
    glBegin(GL_POLYGON);
    glVertex3f(-0.535, 0, 0.34);
    glVertex3f(-0.785, 0, 0.51);
    glVertex3f(-0.125, 0, 0.51);
    glVertex3f(0.125, 0, 0.34);
    fVert1 = {-0.535, 0, 0.34};
    fVert2 = {-0.785, 0, 0.51};
    fVert3 = {-0.125, 0, 0.51};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.125, 0, 0.51);
    glVertex3f(-0.125, 0, 0.68);
    glVertex3f(0.90, 0, 0.68);
    glVertex3f(0.68, 0, 0.51);
    fVert1 = {0.125, 0, 0.51};
    fVert2 = {-0.125, 0, 0.68};
    fVert3 = {0.90, 0, 0.68};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0.375, 0, -0.33);
    glVertex3f(0.125, 0, -0.50);
    glVertex3f(0.375, 0, -0.50);
    glVertex3f(0.625, 0, -0.33);
    fVert1 = {0.375, 0, -0.33};
    fVert2 = {0.125, 0, -0.50};
    fVert3 = {0.375, 0, -0.50};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.375, 0, -0.33);
    glVertex3f(-0.785, 0, -0.33);
    glVertex3f(-0.535, 0, -0.50);
    glVertex3f(-0.125, 0, -0.50);
    fVert1 = {-0.375, 0, -0.33};
    fVert2 = {-0.785, 0, -0.33};
    fVert3 = {-0.535, 0, -0.50};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(-0.785, 0, -0.38);
    glVertex3f(-0.95, 0, -0.38);
    glVertex3f(-0.785, 0, -0.52);
    glVertex3f(-0.62, 0, -0.52);
    fVert1 = {-0.785, 0, -0.38};
    fVert2 = {-0.95, 0, -0.38};
    fVert3 = {-0.785, 0, -0.52};
    CalculateVectorNormal(&fVert1, &fVert2, &fVert3, &fNormalX, &fNormalY, &fNormalZ);
    glNormal3f(fNormalX, fNormalY, fNormalZ);
    glEnd();
    
    glPopMatrix();
}

void CalculateVectorNormal(floatVert fVt1[], floatVert fVt2[], floatVert fVt3[], GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ) {
    GLfloat Qx, Qy, Qz, Px, Py, Pz;
    
    Qx = fVt2->x-fVt1->x;
    Qy = fVt2->y-fVt1->y;
    Qz = fVt2->z-fVt1->z;
    Px = fVt3->x-fVt1->x;
    Py = fVt3->y-fVt1->y;
    Pz = fVt3->z-fVt1->z;
    
    *fNormalX = Py*Qz - Pz*Qy;
    *fNormalY = Pz*Qx - Px*Qz;
    *fNormalZ = Px*Qy - Py*Qx;
}