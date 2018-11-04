#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include "ShaderProgram.h"
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if
#include <time.h>  

#include "FlareMap.h"
#include "Vector3.h";

#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class App;

class Sprite{
public:
	GLuint texture_id;
	float width;
	float height;
	float aspect_ratio;
	float x_size = 0.4f;
	float y_size = 0.4f;
	float size = 0.4f;
	float u;
	float v;
	bool sheet = false;


	float x = 0;
	float y = 0;
	float z = 0;
	bool update_position = false;
	std::shared_ptr<App> app;

	Sprite(const std::string& file_path);
	void set_app(std::shared_ptr<App> app_);


	Sprite(GLuint texture_id_, float u_, float v_, float width_, float height_, float size_);


	void set_size(float x_size_, float y_size_);


	void set_size(float x_size_);

	void set_pos(float x_, float y_, float z_);


	void set_vert_pos(float x_, float y_, float z_);


	std::vector<float> get_verts();

	std::vector<float> get_tex_coords();

	void draw();

};


#endif