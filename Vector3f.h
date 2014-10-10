//
//  Vector3f.h
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

#ifndef __testingOpenGL__Vector3f__
#define __testingOpenGL__Vector3f__

#include <iostream>

class Vector3f {
public:
    double x = 0;
    double y = 0;
    double z = 0;
};

#endif /* defined(__testingOpenGL__Vector3f__) */
