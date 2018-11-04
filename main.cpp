#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
//
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#endif

#include "ShaderProgram.h"
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if
#include <time.h>  
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#include "FlareMap.h"

#include "Script.h";
#include "GameObject.h";
#include "Sprite.h";
#include "Animation.h";
#include "Vector3.h";
#include "GroundSpikeScript.h";
#include "App.h";
#include <iostream>
#include <memory>




std::shared_ptr<App> app(new App);

class GameState {
public:

	std::shared_ptr<App> app;

	GameState(){

	}


	void set_app(std::shared_ptr<App> app_){
		app = app_;
	}


	GameObject player;

	std::vector<GameObject> enemies;
	std::vector<GameObject> bullets;

	int score;


	void render(){

	}

	void update(){

	}

	void process_input(){

	}
};


class MainMenu : public GameState {
public:

	MainMenu(std::shared_ptr<App> app_){
		app = app_;
	}

	void render(){

		app->draw_text("Platformer", -1.5f, 1.0f, app->font_texture, 0.4, 0.165f);
		app->draw_text("Press Spacebar to play", -1.5f, 0.0f, app->font_texture, 0.4, 0.165f);
		app->draw_text("Press ESC to quit", -1.5f, -0.5f, app->font_texture, 0.4, 0.165f);

		app->draw_text("Controls: Press K to shoot. Spacebar to jump", -1.5f, -1.0f, app->font_texture, 0.4, 0.165f);
	}

	void update(){

	}

	void process_input(){

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				app->done = true;
			}


			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_SPACE){
					app->mode = app->STATE_GAME_LEVEL;
				}

				if (event.key.keysym.sym == SDLK_ESCAPE){
					app->mode = app->STATE_GAME_OVER;
					SDL_Quit();
					exit(1);
				}
			}
		}
	}

};



bool shouldRemoveObject(GameObject* obj) {
	if (obj->destroyed){
		return true;
	}

	if (obj->life <= 0){
		return true;
	}

	return false;

}

bool shouldRemoveBullet(GameObject bullet) {
	if (bullet.timeAlive() > 8) {
		return true;
	}

	return shouldRemoveObject(&bullet);
}


bool shouldRemoveEnemy(GameObject enemy) {

	if (enemy.life == 0){
		return true;
	}

	return shouldRemoveObject(&enemy);
}



class Level {
public:
	std::string name;
	GLuint tile_texture;
	float tile_sheet_width = 0;
	float tile_sheet_height = 0;
	std::shared_ptr<App> app;


	Level(const std::string& name_){
		name = name_;
	}
};


class GameLevel : public GameState {
public:
	GLuint sprite_sheet_texture;



	//std::string current_level = "map_2";

	int current_level_index = 0;
	std::unordered_map<std::string, GLuint> tile_textures;


	std::vector<Level*> levels;

	float sheet_width = 0;
	float sheet_height = 0;
	//float tile_sheet_width = 0;
	//float tile_sheet_height = 0;

	std::vector<GameObject*> objects;
	std::vector<GameObject*> enemies;
	std::vector<GameObject> spells;
	std::unordered_map<std::string, GameObject> gui_objects;

	int score = 0;
	int enemies_per_row = 11;


	std::vector<float> verts;
	std::vector<float> tex_coords;


	GameObject box;
	GameObject box2;

	Animation load_animation_from_sheet(const std::string& file_name, int count, float pixel_width, float pixel_height, float world_size){
		float tex_width = 0;
		float tex_height = 0;
		GLuint sheet_tex = app->LoadTexture(file_name.c_str(), &tex_width, &tex_height);
		Animation new_anim;
		float sheet_y = 0;
		for (int x = 0; x < count; x++){
			Sprite sprite_1(sheet_tex, (pixel_width * x) / tex_width, sheet_y / tex_height, pixel_width / tex_width, pixel_height / tex_height, world_size);
			new_anim.add_sprite(sprite_1);
		}

		return new_anim;
	}


	GameObject* create_ground_spike(float x, float y, float speed, float direction){
		GameObject* enemy_ground_spike = new GameObject("ground_spike");
		enemy_ground_spike->set_pos(x, y, 0);
		enemy_ground_spike->set_draw_mode("texture");
		enemy_ground_spike->set_velocity(speed * direction, 0);
		enemy_ground_spike->apply_velocity = true;
		enemy_ground_spike->set_size(0.36, 0.36);
		enemy_ground_spike->set_verts(app->quad_verts(enemy_ground_spike->size.x, enemy_ground_spike->size.y));
		enemy_ground_spike->set_direction(1, 0.0f);
		enemy_ground_spike->apply_gravity = false;

		enemy_ground_spike->acceleration.x = 0;
		enemy_ground_spike->set_app(app);
		Animation ground_spike_animation = load_animation_from_sheet("resources/ground_spike.png", 7, 24, 24, enemy_ground_spike->size.x);
		ground_spike_animation.interval = 0.05;
		ground_spike_animation.set_app(app);
		enemy_ground_spike->add_animation("idle", ground_spike_animation);
		enemy_ground_spike->set_animation("idle");


		GroundSpikeScript* new_ground_spike_script = new GroundSpikeScript(enemy_ground_spike);
		enemy_ground_spike->add_script("ground_spike", new_ground_spike_script);
		return enemy_ground_spike;
	}



	float health_bar_height = 0.8f;

	GameLevel(std::shared_ptr<App> app_){
		app = app_;
		sprite_sheet_texture = app->LoadTexture("resources/sheet.png", &sheet_width, &sheet_height);




		Level* level1 = new Level("map_1");
		level1->app = app;
		level1->tile_texture = app->LoadTexture("resources/map_1.png", &level1->tile_sheet_width, &level1->tile_sheet_height);

		Level* level2 = new Level("map_2");
		level2->app = app;
		level2->tile_texture = app->LoadTexture("resources/map_2.png", &level2->tile_sheet_width, &level2->tile_sheet_height);

		Level* level3 = new Level("map_3");
		level3->app = app;
		level3->tile_texture = app->LoadTexture("resources/map_3.png", &level3->tile_sheet_width, &level3->tile_sheet_height);

		levels.push_back(level1);
		levels.push_back(level2);
		levels.push_back(level3);


		float player_width = 0.55;
		float player_height = 0.85f;

		player.set_app(app);
		player.set_name("zero");
		player.set_pos(0.0f, -1.8f, 0, true);
		player.strings["shooter_name"] = "hero";
		player.set_draw_mode("texture");
		player.set_velocity(0, 0);
		player.apply_velocity = true;
		player.set_size(player_width, player_height);
		player.set_verts(app->quad_verts(player_width, player_height));
		player.set_direction(1, 0.0f);
		player.constant_x_velocity = true;

		player.max_life = 90;
		player.life = player.max_life;


		Animation mega_run_anim = load_animation_from_sheet("resources/mega_run.png", 13, 48, 48, player_height);
		player.add_animation("run", mega_run_anim);


		Animation mega_run_shoot_anim = load_animation_from_sheet("resources/mega_run_shoot.png", 13, 64, 48, player_height);
		mega_run_shoot_anim.interval = 0.05;
		player.add_animation("run_shoot", mega_run_shoot_anim);


		Animation mega_idle_anim = load_animation_from_sheet("resources/mega_idle.png", 1, 48, 48, player_height);
		player.add_animation("idle", mega_idle_anim);

		Animation mega_idle_shoot = load_animation_from_sheet("resources/mega_idle_shoot.png", 1, 48, 48, player_height);
		mega_idle_shoot.loop = false;
		player.add_animation("idle_shoot", mega_idle_shoot);

		player.set_animation("idle");



		float padel_width = 0.08f;
		float padel_height = 0.08f;

		box.set_app(app);
		box.set_name("box");
		box.set_pos(player.x(), player.y(), player.z());
		box.set_draw_mode("shape");
		box.set_color(1.0f, 0.3f, 0.3f, 1);
		box.set_size(padel_width, padel_height);
		box.set_verts(app->quad_verts(padel_width, padel_height));



		box2.set_name("box2");
		box2.set_pos(player.x(), player.y(), player.z());
		box2.set_draw_mode("shape");
		box2.set_color(0.89f, 0.3f, 0.67f, 1);
		box2.set_size(player.width(), player.height());
		box2.set_verts(app->quad_verts(player.width(), player.height()));



		float health_bar_x = 0.5f;
		float health_bar_y = 0.5f;

		GameObject red_bar;
		red_bar.set_app(app);
		red_bar.set_name("box");
		red_bar.set_pos(health_bar_x, health_bar_y, 0, true);
		red_bar.set_draw_mode("shape");
		red_bar.set_color(1.0f, 0.0f, 0.0f, 1);
		red_bar.check_collisions = false;
		red_bar.apply_gravity = false;
		red_bar.isStatic = true;
		red_bar.apply_velocity = false;
		red_bar.set_size(0.25, health_bar_height);
		red_bar.set_verts(app->quad_verts(red_bar.size.x, red_bar.size.y));
		gui_objects["red_bar"] = red_bar;


		GameObject health_bar;
		health_bar.set_app(app);
		health_bar.set_name("box");
		health_bar.set_pos(health_bar_x, health_bar_y, 0, true);
		health_bar.set_draw_mode("shape");
		health_bar.set_color(0.0f, 1.0f, 0.0f, 1);
		health_bar.set_size(0.25, health_bar_height);
		health_bar.set_verts(app->quad_verts(health_bar.size.x, health_bar.size.y));
		health_bar.check_collisions = false;
		health_bar.apply_gravity = false;
		health_bar.isStatic = true;
		health_bar.apply_velocity = false;
		gui_objects["health_bar"] = health_bar;

		load_current_level();

	}


	Level* current_level(){
		if (current_level_index < levels.size()){
			return levels[current_level_index];
		}


		return NULL;
	}


	void load_current_level(){

		for (int x = 0; x < enemies.size(); x++){
			delete enemies[x];
		}

		enemies.clear();

		if (current_level_index == 0){

			enemies.push_back(create_ground_spike(7.0f, -3.2f, 5, -1));
			enemies.push_back(create_ground_spike(3.2f, -3.2f, 2, 1));
			enemies.push_back(create_ground_spike(14.0f, -3.2f, 2.5f, 1));
			enemies.push_back(create_ground_spike(15.0f, -3.2f, 3.0f, -1));



			GameObject* enemy = new GameObject("greymon");
			enemy->set_app(app);
			enemy->set_pos(11.4f, -1.6f, 0);
			enemy->set_draw_mode("texture");
			enemy->set_velocity(0, 0);
			enemy->apply_velocity = false;
			enemy->set_size(0.5, 0.7);
			enemy->set_verts(app->quad_verts(enemy->size.x, enemy->size.y));
			enemy->set_direction(-1, 0.0f);
			enemy->add_animation("idle", 1);
			enemy->set_animation("idle");
			enemy->constant_x_velocity = false;
			enemy->acceleration.x = 0.0f;

			enemies.push_back(enemy);





			GameObject* enemy2 = new GameObject("greymon");
			enemy2->set_app(app);
			enemy2->set_pos(16.4f, -2.0f, 0);
			enemy2->set_draw_mode("texture");
			enemy2->set_velocity(0, 0);
			enemy2->apply_velocity = false;
			enemy2->set_size(0.5, 0.7);
			enemy2->set_verts(app->quad_verts(enemy->size.x, enemy->size.y));
			enemy2->set_direction(-1, 0.0f);
			enemy2->add_animation("idle", 1);
			enemy2->set_animation("idle");
			enemy2->constant_x_velocity = false;
			enemy2->acceleration.x = 0.0f;

			enemies.push_back(enemy2);
		}
		else if (current_level_index == 1){
			enemies.push_back(create_ground_spike(2.0f, -3.0f, 5, -1));
			enemies.push_back(create_ground_spike(2.4f, -3.0f, 2, 1));

			enemies.push_back(create_ground_spike(5.0f, -3.65f, 8, -1));
			enemies.push_back(create_ground_spike(5.4f, -3.65f, 3, 1));


			enemies.push_back(create_ground_spike(14.4f, -3.7f, 2.5f, 1));
			enemies.push_back(create_ground_spike(15.7f, -3.7f, 3.0f, -1));




			GameObject* enemy2 = new GameObject("greymon");
			enemy2->set_app(app);
			enemy2->set_pos(16.4f, -2.0f, 0);
			enemy2->set_draw_mode("texture");
			enemy2->set_velocity(0, 0);
			enemy2->apply_velocity = false;
			enemy2->set_size(0.5, 0.7);
			enemy2->set_verts(app->quad_verts(enemy2->size.x, enemy2->size.y));
			enemy2->set_direction(-1, 0.0f);
			enemy2->add_animation("idle", 1);
			enemy2->set_animation("idle");
			enemy2->constant_x_velocity = false;
			enemy2->acceleration.x = 0.0f;

			enemies.push_back(enemy2);
			
		}
		else if (current_level_index == 2){
			enemies.push_back(create_ground_spike(2.0f, -3.0f, 3, -1));
			enemies.push_back(create_ground_spike(2.4f, -3.0f, 2, 1));
			enemies.push_back(create_ground_spike(2.2f, -3.0f, 1, 1));


			enemies.push_back(create_ground_spike(5.0f, -3.65f, 8, -1));
			enemies.push_back(create_ground_spike(5.4f, -3.65f, 3, 1));

			enemies.push_back(create_ground_spike(8.0f, -3.65f, 8, -1));
			enemies.push_back(create_ground_spike(8.4f, -3.65f, 3, 1));
			enemies.push_back(create_ground_spike(8.2f, -3.65f, 5, 1));

			enemies.push_back(create_ground_spike(14.4f, -3.7f, 2.5f, 1));
			enemies.push_back(create_ground_spike(15.7f, -3.7f, 3.0f, -1));

		}



		verts.clear();
		tex_coords.clear();



		if (app->map != NULL){
			delete app->map;
			app->map = NULL;
		}

		app->map = new FlareMap();
		app->map->Load("resources/" + current_level()->name + ".txt");
		for (int i = 0; i < app->map->entities.size(); i++) {
			PlaceEntity(app->map->entities[i].type, app->map->entities[i].x * app->TILE_SIZE, app->map->entities[i].y * -app->TILE_SIZE);
		}



		for (int z = 0; z < app->map->layers.size(); z++){
			//for (int z = 0; z < 1; z++){
			for (int y = 0; y < app->map->mapHeight; y++) {
				for (int x = 0; x < app->map->mapWidth; x++) {
					// do something with map.mapData[y][x] 
					//std::cout << map.mapData[y][x] << std::endl;
					std::vector<float> this_verts;
					std::vector<float> this_tex_coords;

					load_tile(app->map->layers[z][y][x], x, y, this_verts, this_tex_coords);

					verts.insert(verts.end(), this_verts.begin(), this_verts.end());
					tex_coords.insert(tex_coords.end(), this_tex_coords.begin(), this_tex_coords.end());
				}
			}
		}
	}


	~GameLevel(){
		for (int x = 0; x < objects.size(); x++){
			delete objects[x];
		}

		for (int x = 0; x < enemies.size(); x++){
			delete enemies[x];
		}

		

	}

	float enemy_movement_direction = -1;
	float last_movement_direction_change = 0;
	float movement_change_interval = 1;
	int movement_change_count = 0;

	float last_movement = 0;
	float last_attack = 0;
	float attack_interval = 1.0f;

	int row_index = 0;
	int row_change_count = 0;


	void PlaceEntity(std::string type, float x, float y) {
		// place your entity at x, y based on type string
	}



	void worldToTileCoord(float worldX, float worldY, int *gridX, int *gridY){
		*gridX = (int)(worldX / app->TILE_SIZE);
		*gridY = (int)(-worldY / app->TILE_SIZE);
	}



	void enemy_shoot(GameObject* enemy){
		Animation bullet_animation;
		Sprite bullet_sprite_1_1("resources/blast_1_1.png");
		Sprite bullet_sprite_1_2("resources/blast_1_2.png");
		bullet_animation.add_sprite(bullet_sprite_1_1);
		bullet_animation.add_sprite(bullet_sprite_1_2);

		bullets.push_back(enemy->shoot(bullet_animation));
	}

	void update(){
		player.update();


		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), shouldRemoveBullet), bullets.end());

		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), shouldRemoveObject), enemies.end());
		//objects.erase(std::remove_if(objects.begin(), objects.end(), shouldRemoveObject), objects.end());

		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].update();
		}


		for (int i = 0; i < enemies.size(); i++) {
			

			if (enemies[i]->name == "greymon"){
				enemies[i]->update();
				if (app->get_runtime() - last_attack > 3.0f){
					//enemies[i]->direction[0] = player.velocity.x;
					enemies[i]->velocity.x = player.velocity.x * -1;
					last_attack = app->get_runtime();
					enemy_shoot(enemies[i]);
				}
			}
			else{
				enemies[i]->update();
			}
		}


		for (int i = 0; i < objects.size(); i++) {
			objects[i]->update();
		}


		for (int i = 0; i < spells.size(); i++) {
			spells[i].update();
		}


		for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it){
			it->second.update();
		}

		


		handle_collisions();

		float life_percent = (player.life / player.max_life);

		
		gui_objects["health_bar"].pos.y = gui_objects["health_bar"].start_pos.y - ((health_bar_height - (health_bar_height * life_percent)) / 2);
		gui_objects["health_bar"].size.y = health_bar_height * (player.life / player.max_life);
		gui_objects["health_bar"].set_verts(app->quad_verts(gui_objects["health_bar"].size.x, gui_objects["health_bar"].size.y));




		if (player.pos.x  > (app->map->mapWidth * app->tile_world_size) - 0.6f){
			player.pos.x = player.start_pos.x;
			player.pos.y = player.start_pos.y;
			current_level_index += 1;


			if (current_level_index >= levels.size()){
				current_level_index = levels.size() - 1;
				game_won();
				return;
			}
			load_current_level();
		}

	}



	void render(){



		//Move the camera to follow player
		app->viewMatrix.Identity();

		float clamped_screen_x = screen_x_clamp(player.x(), 0.0f, (app->map->mapWidth * app->tile_world_size) - app->tile_world_size);
		float clamped_screen_y = screen_y_clamp(player.y() - 1.0f, 0.0f, (app->map->mapHeight * app->tile_world_size));
		app->viewMatrix.Translate(-clamped_screen_x, -clamped_screen_y, 0);

		//Draw tilemap
		app->batch_draw(current_level()->tile_texture, verts, tex_coords);

		box.set_pos(player.x(), player.y());
		box2.set_pos(player.x(), player.y());

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->draw();
		}

		player.draw();

		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].draw();
		}

		for (int i = 0; i < objects.size(); i++) {
			objects[i]->draw();
		}


		for (int i = 0; i < spells.size(); i++) {
			spells[i].draw();
		}



		//float new_x = clamped_screen_x;


		for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it){

			//it->second.pos.x = new_x;
			
			it->second.pos.x = clamped_screen_x - 3.3f;
			it->second.pos.y = clamped_screen_y + 1.5f;
			it->second.draw();
		}


		//app->draw_text("points: " + std::to_string(score), 0.5f, -0.5f, app->font_texture, 0.4, 0.165f);
		//app->draw_text("lives: " + std::to_string(player.lives), 0.5f, -0.8f, app->font_texture, 0.4, 0.165f);

	}

	float screen_y_clamp(float val, float top, float bottom){
		if (val - (app->screen_height / 2) < top){
			return top - (app->screen_height / 2);
		}
		else if (val + (app->screen_height / 2) > bottom){
			return bottom + (app->screen_height / 2);
		}

		return val;
	}


	float screen_x_clamp(float val, float left, float right){
		if (val - (app->screen_width / 2) < left){
			return left + (app->screen_width / 2);
		}
		else if (val + (app->screen_width / 2) > right){
			return right - (app->screen_width / 2);
		}

		return val;
	}



	void load_tile(int tile_id, int pos_x, int pos_y, std::vector<float>& verts, std::vector<float>& tex_coords){
		if (tile_id == 0){
			return;
		}

		Level* level = current_level();


		//Convert tile_id to x,y
		//Starts from top left at 0 -> sheet_width
		//then goes to next row starting from left
		int tile_sheet_x_width = level->tile_sheet_width / app->TILE_SIZE; //x grid size
		int tile_sheet_y_height = level->tile_sheet_height / app->TILE_SIZE; //x grid size
		int tile_x = (tile_id % tile_sheet_x_width) - 1;
		int tile_y = tile_id / tile_sheet_x_width;


		//float tile_world_x = (pos_x * tile_size) - (screen_width / 2.0f);
		//float tile_world_y = -1.0f*(pos_y * tile_size) + (screen_height / 2.0f);
		float tile_world_x = (pos_x * app->tile_world_size);
		float tile_world_y = -1.0f*(pos_y * app->tile_world_size);

		


		Sprite tile_sprite(level->tile_texture, (tile_x * app->TILE_SIZE) / level->tile_sheet_width, (tile_y * app->TILE_SIZE) / level->tile_sheet_height, app->TILE_SIZE / level->tile_sheet_width, app->TILE_SIZE / level->tile_sheet_height, app->tile_world_size);

		tile_sprite.set_pos(tile_world_x, tile_world_y, 0);
		tile_sprite.set_vert_pos(tile_world_x, tile_world_y, 0);
		verts = tile_sprite.get_verts();
		tex_coords = tile_sprite.get_tex_coords();
	}



	void game_won(){
		std::cout << "YOU WON" << std::endl;
		app->mode = app->STATE_GAME_WON;
	}


	void game_over(){
		std::cout << "GAME OVER" << std::endl;
		app->mode = app->STATE_GAME_OVER;
	}


	void player_got_hit(){

		player.take_hit(10);

		if (player.life <= 0){
			game_over();
		}

		//player.set_pos(0, -1.68f);
	}


	GameObject create_spell_hit(Vector3 pos){
		Animation spell_hit_animation;
		Sprite blast_hit_1("resources/blast_hit_1.png");
		Sprite blast_hit_2("resources/blast_hit_2.png");
		Sprite blast_hit_3("resources/blast_hit_3.png");
		Sprite blast_hit_4("resources/blast_hit_4.png");
		spell_hit_animation.add_sprite(blast_hit_1);
		spell_hit_animation.add_sprite(blast_hit_2);
		spell_hit_animation.add_sprite(blast_hit_3);
		spell_hit_animation.add_sprite(blast_hit_4);

		spell_hit_animation.loop = false;


		//GameObject* new_spell_hit = new GameObject();
		//new_spell_hit->set_pos(pos.x, pos.y);
		//new_spell_hit->set_velocity(0, 0);
		//new_spell_hit->set_direction(0, 0);
		//new_spell_hit->set_draw_mode("texture");
		//new_spell_hit->set_size(0.1, 0.1);
		//new_spell_hit->set_verts(quad_verts(0.1, 0.1));
		//new_spell_hit->apply_gravity = false;
		//new_spell_hit->check_collisions = false;


		//new_spell_hit->add_animation("idle", spell_hit_animation);
		//new_spell_hit->set_animation("idle");


		////
		GameObject new_spell_hit;
		new_spell_hit.set_app(app);
		new_spell_hit.set_pos(pos.x, pos.y);
		new_spell_hit.set_velocity(0, 0);
		new_spell_hit.set_direction(1, 0);
		new_spell_hit.set_draw_mode("texture");
		new_spell_hit.set_size(1, 1);
		new_spell_hit.set_verts(app->quad_verts(new_spell_hit.size.x, new_spell_hit.size.y));
		new_spell_hit.apply_gravity = false;
		new_spell_hit.check_collisions = false;


		spell_hit_animation.set_app(app);
		new_spell_hit.add_animation("idle", spell_hit_animation);
		new_spell_hit.set_animation("idle");

		return new_spell_hit;

		//bullets.push_back(new_spell_hit);
	}


	void handle_collisions(){

		for (int y = 0; y < enemies.size(); y++){
			if (app->check_box_collision(player, *enemies[y])){
				player_got_hit();
				break;
			}
		}


		for (int x = 0; x < bullets.size(); x++){
			bool continue_to_next_loop = false;

			if (bullets[x].strings["shooter_name"] == "hero"){
				for (int y = 0; y < enemies.size(); y++){
					if (enemies[y]->name == "greymon" && app->check_box_collision(bullets[x], *enemies[y])){

						spells.push_back(create_spell_hit(bullets[x].pos));
						bullets[x].destroy();
						enemies[y]->take_hit(20);
						if (enemies[y]->life <= 0){
							enemies[y]->destroy();
						}
						score += 10;
						continue_to_next_loop = true;
						break;
					}
				}
			}
			else{
				if (app->check_box_collision(bullets[x], player)){
					player_got_hit();

					spells.push_back(create_spell_hit(bullets[x].pos));
					//create_spell_hit(bullets[x].pos);
					bullets[x].destroy();
					continue_to_next_loop = true;

					break;
				}
			}


			if (continue_to_next_loop){
				continue;
			}

		}
	}


	std::string last_key = "";

	void process_input(){
		Uint8* keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));

		if (keysArray[SDL_SCANCODE_RETURN]){
			printf("MESSAGE: <RETURN> is pressed...\n");
		}

		if (keysArray[SDL_SCANCODE_W]){
			//player.move_up();
		}

		if (keysArray[SDL_SCANCODE_S]){
			//player.move_down();
		}


		bool moving = false;
		if (keysArray[SDL_SCANCODE_D]){
			last_key = "D";
			player.set_animation("run");
			player.move_right();
			moving = true;
		}

		if (keysArray[SDL_SCANCODE_A]){
			last_key = "A";
			player.set_animation("run");
			player.move_left();
			moving = true;
		}


		if (!moving){

			if (player.current_animation_name != "idle_shoot"){
				player.set_animation("idle");
			}
			
			player.stop_moving();
		}




		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				app->done = true;
			}


			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_SPACE){
					player.jump();
				}


				if (event.key.keysym.sym == SDLK_k){

					player.set_animation("idle_shoot");
					Animation bullet_animation;
					Sprite bullet_sprite_1_1("resources/blast_1_1.png");
					Sprite bullet_sprite_1_2("resources/blast_1_2.png");
					bullet_animation.add_sprite(bullet_sprite_1_1);
					bullet_animation.add_sprite(bullet_sprite_1_2);
					GameObject bullet_ = player.shoot(bullet_animation);
					bullet_.strings["shooter_name"] = "hero";

					bullets.push_back(bullet_);
				}
			}


		}


	}



};


MainMenu* mainMenu;
GameLevel* gameLevel;


void render_game() {

	if (app->mode == app->STATE_MAIN_MENU){
		mainMenu->render();
	}
	else if (app->mode == app->STATE_GAME_LEVEL){
		gameLevel->render();
	}
	else if (app->mode == app->STATE_GAME_OVER){
		glClear(GL_COLOR_BUFFER_BIT);
		app->viewMatrix.SetPosition(0, 0, 0);
		app->draw_text("GAME OVER", -1.25f, 0, app->font_texture, 0.5, 0.3);
	}
	else if (app->mode == app->STATE_GAME_WON){
		app->viewMatrix.SetPosition(0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		app->draw_text("YOU WON", -1.25f, 0, app->font_texture, 0.5, 0.3);
	}
}

void update_game() {
	if (app->mode == app->STATE_MAIN_MENU){
		mainMenu->update();
	}
	else if (app->mode == app->STATE_GAME_LEVEL){
		gameLevel->update();
	}
}

void process_input() {
	if (app->mode == app->STATE_MAIN_MENU){
		mainMenu->process_input();
	}
	else if (app->mode == app->STATE_GAME_LEVEL){
		gameLevel->process_input();
	}
	else{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				app->done = true;
			}
		}
	}
}





int left_score = 0;
int right_score = 0;

int main(int argc, char *argv[]) {

	app->init();

	mainMenu = new MainMenu(app);

	gameLevel = new GameLevel(app);

	float lastFrameTicks = 0.0f;
	float FIXED_TIMESTEP = 0.0166666f;
	float MAX_TIMESTEPS = 6;
	float accumulator = 0.0f;
	float elapsed2 = 0.0f;

	//FIXED TIMESTEP:
	while (!app->done) {
		float ticks = app->get_runtime();
		elapsed2 = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		elapsed2 += accumulator;
		if (elapsed2 < FIXED_TIMESTEP){
			accumulator = elapsed2;
			continue;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		process_input();
		while (elapsed2 >= FIXED_TIMESTEP){
			app->elapsed = FIXED_TIMESTEP;
			update_game();
			elapsed2 -= FIXED_TIMESTEP;
		}
		accumulator = elapsed2;
		render_game();

		SDL_GL_SwapWindow(app->displayWindow);
	}

	//VARIABLE TIMESTEP:
	//while (!done) {
	//	float ticks = get_runtime();
	//	elapsed = ticks - lastFrameTicks;
	//	lastFrameTicks = ticks;

	//	glClear(GL_COLOR_BUFFER_BIT);

	//	process_input();
	//	update_game();
	//	render_game();

	//	SDL_GL_SwapWindow(displayWindow);
	//}

	//Cleanup
	delete mainMenu;
	delete gameLevel;
	delete app->tex_program;
	delete app->shape_program;



	SDL_Quit();
	return 0;
}
