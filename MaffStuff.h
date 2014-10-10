//
//  MaffStuff.h
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

#ifndef __testingOpenGL__MaffStuff__
#define __testingOpenGL__MaffStuff__

#include <iostream>

class MaffStuff {
public:
    static double degreesToRadians(double degrees);
    static double dsin(double theta);
    static double dcos(double theta);
};

#endif /* defined(__testingOpenGL__MaffStuff__) */
