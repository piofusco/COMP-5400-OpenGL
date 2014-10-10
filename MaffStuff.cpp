//
//  MaffStuff.cpp
//  testingOpenGL
//
//  Created by Michael on 3/22/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#include "MaffStuff.h"
#include <cmath>

double MaffStuff::degreesToRadians(double degrees) {
    return degrees*M_PI/180;
}

double MaffStuff::dsin(double theta) {
    return sin(degreesToRadians(theta));
}

double MaffStuff::dcos(double theta) {
    return cos(degreesToRadians(theta));
}