//
//  Mouse.cpp
//  testingOpenGL
//
//  Created by Michael on 3/22/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#include "Mouse.h"

int Mouse::lastX = 0;
int Mouse::lastY = 0;
int Mouse::currentX = 0;
int Mouse::currentY = 0;
int Mouse::deltaX = 0;
int Mouse::deltaY = 0;

bool Mouse::leftButton = false;
bool Mouse::middleButton = false;
bool Mouse::rightButton = false;

void Mouse::move(int x, int y) {
    currentX = x;
    currentY = y;
}

void Mouse::update() {
    if (Viewport::grabbed) {
        deltaX = currentX - lastX;
        deltaY = currentY - lastY;
        
        lastX = currentX;
        lastY = currentY;
        
        if (currentX <= 0 || currentX >= Viewport::width) {
            lastX -= currentX - Viewport::width/2;
            glutWarpPointer(Viewport::width/2, currentY);
        }
        if (currentY <= 0 || currentY >= Viewport::height) {
            lastY -= currentY - Viewport::height/2;
            glutWarpPointer(currentY, Viewport::height/2);
        }
    }

}

void Mouse::click(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            leftButton = true;
            if (x >= 0 && x < Viewport::width && y >= 0 && y < Viewport::height) {
                Viewport::setGrabbed();
            }
        }
        if (state == GLUT_UP) {
            leftButton = false;
        }
    }
}