//
//  Camera.h
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

#ifndef __testingOpenGL__Camera__
#define __testingOpenGL__Camera__

#include <iostream>
#include <unistd.h>
#include "Vector3f.h"

class Camera {
public:
    static Vector3f position;
    static Vector3f rotation;
};

#endif /* defined(__testingOpenGL__Camera__) */
