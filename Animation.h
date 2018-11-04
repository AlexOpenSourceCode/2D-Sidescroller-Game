#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include <iostream>
#include <memory>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


class Sprite;
class App;

#include "Sprite.h";
class Animation{
public:
	std::vector<Sprite> sprites;
	int animation_count = 0;

	float last_change = 0;
	float interval = .085; //milliseconds (ms)
	int current_index = 0;

	bool loop = true;
	bool done = false;

	float start_time = 0;

	std::shared_ptr<App> app;

	Animation();

	Animation(const std::string& animation_name, int animation_count_);

	void set_app(std::shared_ptr<App> app_);

	void add_sprite(Sprite new_sprite);

	float get_runtime();

	void advance();

	void reset();
	void update();


	void update_size(float width_, float height_);


	void draw();


	bool done_time_elapsed(float elapsed_);

};


#endif