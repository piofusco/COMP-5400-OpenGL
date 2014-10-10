//
//  Viewport.h
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


#ifndef __testingOpenGL__Viewport__
#define __testingOpenGL__Viewport__

#include <iostream>

class Viewport {
public:
    static int width;
    static int height;
    
    static bool grabbed;
    
    static void setGrabbed();
    static void setUnGrabbed();
};

#endif /* defined(__testingOpenGL__Viewport__) */
