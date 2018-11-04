#include "Animation.h"
#include "Sprite.h";
#include "App.h";

Animation::Animation(){};

Animation::Animation(const std::string& animation_name, int animation_count_){
	animation_count = animation_count_;
	for (int x = 0; x < animation_count_; x++){
		std::string file_path = RESOURCE_FOLDER"";
		file_path += "resources/" + animation_name + "_" + std::to_string(x + 1) + ".png";
		//std::cout << "Loading file: " << file_path << std::endl;
		Sprite new_sprite(file_path);


		sprites.push_back(new_sprite);
	}
}

void Animation::set_app(std::shared_ptr<App> app_){
	app = app_;

	for (int x = 0; x < sprites.size(); x++){
		sprites[x].set_app(app_);
	}
}


void Animation::add_sprite(Sprite new_sprite){
	sprites.push_back(new_sprite);
}



void Animation::advance(){
	current_index += 1;
	if (current_index >= sprites.size()){
		if (loop){
			current_index = 0;
		}
		else{
			done = true;
		}
	}
}

//seconds since program started
float Animation::get_runtime(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	return ticks;
}

void Animation::update(){
	float current_runtime = get_runtime();
	if (current_runtime - last_change > interval){
		advance();
		last_change = get_runtime();
	}
}


void Animation::update_size(float width_, float height_){
	for (int x = 0; x < sprites.size(); x++){
		//sprites[x].set_size(width_, height_);
	}
}


void Animation::reset(){
	start_time = get_runtime();
	//current_index = 0;
	//done = false;
}


void Animation::draw(){

	if (sprites.size() == 1){
		current_index = 0;
	}


	if (current_index >= 0 && current_index < sprites.size()){
		sprites[current_index].draw();
	}

}


bool Animation::done_time_elapsed(float elapsed_){
	if (!done){
		return false;
	}

	float current_runtime = get_runtime();
	if (current_runtime - last_change > elapsed_){
		return true;
	}

	return false;
}

