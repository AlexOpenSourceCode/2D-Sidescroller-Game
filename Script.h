#ifndef SCRIPT_H
#define SCRIPT_H


#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if
#include <iostream>
#include <memory>
//#include "GameObject.h";

class GameObject;
class App;

class Script {
public:
	std::shared_ptr<App> app;
	GameObject* obj = NULL;
	Script();
	Script(GameObject* go);

	float last_check = 0;

	virtual void update();
	virtual void on_event(const std::string& event_name);

	void set_game_object(GameObject* go_);
	void set_app(std::shared_ptr<App> app_);
	
};

#endif