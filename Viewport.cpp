//
//  Viewport.cpp
//  testingOpenGL
//
//  Created by Michael on 3/22/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#include "Viewport.h"

int Viewport::width = 0;
int Viewport::height = 0;

bool Viewport::grabbed = false;

void Viewport::setGrabbed() {
    grabbed = true;
    glutSetCursor(GLUT_CURSOR_NONE);
}

void Viewport::setUnGrabbed() {
    grabbed = false;
    glutSetCursor(GLUT_CURSOR_INHERIT);
}