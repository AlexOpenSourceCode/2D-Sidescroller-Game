#include "Script.h";
#include "GameObject.h";

Script::Script(){

}

Script::Script(GameObject* go_){
	set_game_object(go_);
}

void Script::update(){

}

void Script::on_event(const std::string& event_name){

}

void Script::set_game_object(GameObject* go){
	obj = go;
}


void Script::set_app(std::shared_ptr<App> app_){
	app = app_;
}
