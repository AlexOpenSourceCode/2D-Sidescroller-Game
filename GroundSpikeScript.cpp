#include "GroundSpikeScript.h";
#include "GameObject.h";

GroundSpikeScript::GroundSpikeScript(GameObject* go_){
	set_game_object(go_);
}


void GroundSpikeScript::on_event(const std::string& event_name){
	if (event_name == "left_collide"){
		obj->velocity.x = abs(obj->velocity.x);
	}
	else if (event_name == "right_collide"){
		obj->velocity.x = abs(obj->velocity.x) * -1.0f;
	}
}

void GroundSpikeScript::update() {
	if (app->get_runtime() - last_check < 0.1){
		return;
	}


	last_check = app->get_runtime();
}

