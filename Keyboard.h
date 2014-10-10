//
//  Keyboard.h
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

#ifndef __testingOpenGL__Keyboard__
#define __testingOpenGL__Keyboard__

#include <iostream>

using namespace std;

class Keyboard {
public:
    static bool key[256];
    
    static void keyDown(unsigned char k, int x, int y);
    static void keyUp(unsigned char k, int x, int y);
};

#endif /* defined(__testingOpenGL__Keyboard__) */
