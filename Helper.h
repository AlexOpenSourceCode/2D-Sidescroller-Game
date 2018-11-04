#ifndef HELPER_H
#define HELPER_H


#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>


#include "ShaderProgram.h"
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if

static inline float radians_to_degrees(float radians) {
	return radians * (180.0 / M_PI);
}

static inline float degrees_to_radians(float degrees){
	return ((degrees)* M_PI / 180.0);
}

class Helper{
public:





};

#endif



