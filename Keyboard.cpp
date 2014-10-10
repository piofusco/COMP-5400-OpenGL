//
//  Keyboard.cpp
//  testingOpenGL
//
//  Created by Michael on 3/22/14.
//  Copyright (c) 2014 Gideon LTD. All rights reserved.
//

#include "Keyboard.h"

bool Keyboard::key[256];

void Keyboard::keyDown(unsigned char k, int x, int y) {
    key[k] = true;
}

void Keyboard::keyUp(unsigned char k, int x, int y) {
    key[k] = false;
}