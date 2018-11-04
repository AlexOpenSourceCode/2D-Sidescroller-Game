#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H


#include "ShaderProgram.h"
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if
#include <time.h>  

#include "FlareMap.h"
#include "Vector3.h";
#include "Script.h";

#include "Helper.h";
#include <iostream>
#include <memory>
#include "App.h";
#include "Animation.h";

class Animation;
class App;
class GameObject{
public:
	std::string name = "";
	float last_change = 0;
	float interval = .085; //milliseconds (ms)

	std::unordered_map<std::string, Animation> animations;
	std::unordered_map<std::string, std::string> strings;
	std::unordered_map<std::string, Script*> scripts;
	std::string current_animation_name = "";
	Vector3 pos;
	Vector3 start_pos;
	float color[4];
	std::vector<float> verts;
	std::string draw_mode = "texture";
	bool apply_velocity = true;
	Vector3 size;
	Vector3 velocity;
	Vector3 acceleration;

	float direction[3];
	float total_move_amount = 0;
	float movement_angle = degrees_to_radians(-180);
	float max_life = 60;
	float life = max_life;
	int lives = 5;
	bool destroyed = false;
	bool isStatic = false; //If static, no gravity, no movement, no collision checking
	bool apply_gravity = true;
	float created_at = 0;


	bool invincible = false;
	float last_hit = 0;
	float invincibility_duration = 1.2f;

	std::shared_ptr<App> app;

	void init();


	GameObject();

	GameObject(const std::string& name_);

	void set_app(std::shared_ptr<App> app_);



	void set_name(const std::string& str_);


	void set_pos(float x, float y);

	void destroy();

	void set_pos(float x, float y, float z, bool initial = false);

	float timeAlive();
	void add_animation(const std::string& animation_name, int animation_count);

	
	void add_animation(const std::string& animation_name, Animation animation);
	void set_animation(const std::string& animation_name);


	void add_script(const std::string& script_name, Script* script);

	void update_scripts();

	void move_y(float delta_y);
	void move_x(float delta_x);
	float get_runtime();

	void move_up();
	void move_down();

	void move_left();

	void move_right();

	void stop_moving();



	void jump();

	void flip_x_direction();

	void set_x(float x_);

	void set_y(float y_);

	void worldToTileCoord(float worldX, float worldY, int* gridX, int* gridY);

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;

	int last_grid_x = 0;
	int last_grid_y = 0;
	int colliding_bottom();

	bool is_colliding_bottom();

	double distance(float x1, float y1, float x2, float y2);

	void check_for_collisions();


	bool jumping = false;
	bool check_collisions = true;

	bool constant_x_velocity = false;
	virtual void update();

	Animation* get_current_animation();


	void set_direction_x(float x_);

	void set_direction_y(float y_);
	void set_direction(float x_, float y_);

	float x();

	float y();

	float z();


	float top_left_x();

	float top_left_y();

	void set_verts(std::vector<float> arr);


	void set_size(float width_, float height_);

	void set_draw_mode(std::string mode_);

	void set_velocity(float x_, float y_, float z_ = 0);


	void set_color(float r, float g, float b, float a);
	float last_shoot = 0;

	float left();
	float right();
	float top();
	float bottom();



	void take_hit(float dmg);

	void draw();
	float width();
	float height();
	bool colliding_left();
	bool colliding_right();

	void broadcast_event(const std::string& event_name);
	bool colliding_directly_right();
	bool colliding_directly_left();
	GameObject shoot(Animation animation);
};

#endif