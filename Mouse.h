//
//  Mouse.h
//  testingOpenGL
//
//  Created by Michael on 3/22/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef __testingOpenGL__Mouse__
#define __testingOpenGL__Mouse__

#include <iostream>
#include "Viewport.h"

class Mouse {
public:
    static int lastX;
    static int lastY;
    static int currentX;
    static int currentY;
    static int deltaX;
    static int deltaY;
    
    static bool leftButton;
    static bool middleButton;
    static bool rightButton;
    
    static void move(int x, int y);
    static void update();
    static void click(int button, int state, int x, int y);
};

#endif /* defined(__testingOpenGL__Mouse__) */
