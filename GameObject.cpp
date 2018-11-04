#include "GameObject.h";



void GameObject::init(){
	created_at = get_runtime();
	set_pos(0, 0);
	set_animation("idle");
	direction[0] = 1;
}


GameObject::GameObject(){
	init();
}

GameObject::GameObject(const std::string& name_){
	init();

	name = name_;
}


float GameObject::get_runtime(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	return ticks;
}



void GameObject::set_name(const std::string& str_){
	name = str_;
}


void GameObject::set_pos(float x, float y){
	pos.x = x;
	pos.y = y;
}

void GameObject::destroy(){
	destroyed = true;
}

void GameObject::set_pos(float x, float y, float z, bool initial){
	set_pos(x, y);
	pos.z = z;

	if (initial){
		start_pos.x = x;
		start_pos.y = y;
		start_pos.z = z;
	}
}

float GameObject::timeAlive(){
	return get_runtime() - created_at;
}

void GameObject::add_animation(const std::string& animation_name, int animation_count){
	Animation run_animation(name + "_" + animation_name, animation_count);
	run_animation.set_app(app);
	animations[animation_name] = run_animation;
	
}


void GameObject::add_animation(const std::string& animation_name, Animation animation){
	animation.set_app(app);
	animations[animation_name] = animation;
}

void GameObject::set_animation(const std::string& animation_name){
	current_animation_name = animation_name;
	animations[current_animation_name].reset();
}

void GameObject::move_y(float delta_y){
	pos.y += delta_y;
}
void GameObject::move_x(float delta_x){
	pos.x += delta_x;
}


void GameObject::set_app(std::shared_ptr<App> app_){
	app = app_;
}



void GameObject::move_up(){
	move_y(3 * app->elapsed);
}

void GameObject::move_down(){
	move_y(-3 * app->elapsed);
}

void GameObject::move_left(){
	direction[0] = -1;
	//move_x(-velocity.x * elapsed);
	velocity.x = -3.2f;
}


void GameObject::move_right(){
	direction[0] = 1;
	//move_x(velocity.x * elapsed);
	velocity.x = 3.2f;
}

void GameObject::stop_moving(){
	acceleration.x = 0;
	velocity.x = 0;
}



void GameObject::jump(){
	//if (is_colliding_bottom()){
		velocity.y = 4.8f;
		move_y(0.1f);
		jumping = true;
	//}
}

void GameObject::flip_x_direction(){
	direction[0] *= -1.0f;
	velocity.x = fabs(velocity.x) * direction[0];
}

void GameObject::set_x(float x_){
	pos.x = x_;
}


void GameObject::set_y(float y_){
	pos.y = y_;
}


void GameObject::worldToTileCoord(float worldX, float worldY, int* gridX, int* gridY){
	*gridX = (int)(worldX / app->TILE_SIZE);
	*gridY = (int)(-worldY / app->TILE_SIZE);
}


int GameObject::colliding_bottom(){
	//Check for collision on feet with ground
	float test_x = x();
	float test_y = y() - height() / 2;

	int grid_x = (int)(test_x / app->tile_world_size);
	int grid_y = (int)(-(test_y) / app->tile_world_size);


	//layer index 3 = platform
	//worldToTileCoord(x(), y() + (0.1f), &grid_x, &grid_y);
	try{
		int data = app->map->layers[3][grid_y][grid_x];
		if (data > 0){
			last_grid_x = grid_x;
			last_grid_y = grid_y;
			return data;
		}
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		return 0;
	}

	/////////
	grid_x = (int)(test_x / app->tile_world_size) - 1;
	try{
		int data = app->map->layers[3][grid_y][grid_x];
		if (data > 0){
			last_grid_x = grid_x;
			last_grid_y = grid_y;
			return data;
		}
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		return 0;
	}
	//////////////////////
	grid_x = (int)(test_x / app->tile_world_size) + 1;
	try{
		int data = app->map->layers[3][grid_y][grid_x];
		if (data > 0){
			last_grid_x = grid_x;
			last_grid_y = grid_y;
			return data;
		}
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		return 0;
	}



	return 0;
}

bool GameObject::is_colliding_bottom(){
	int col_data = colliding_bottom();
	if (col_data > 0){
		return true;
	}
	else{
		return false;
	}
}



bool GameObject::colliding_directly_left(){
	//Prevent object from going off left side of screen
	if (x() - (width() / 2) < 0.1){
		return true;
	}


	bool col_left = false;

	int grid_x_3 = (int)((x() - (width() / 2)) / 0.18f);
	int grid_y_3 = (int)(-(y()) / 0.18f);
	int data3 = app->map->layers[3][grid_y_3][grid_x_3];


	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		//pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		col_left = true;
	}
	

	return col_left;
}



bool GameObject::colliding_directly_right(){
	//Prevent object from going off right side of screen
	if (x() + (width() / 2) > (app->map->mapWidth * app->tile_world_size) - 0.2f){
		//pos.x = (app->map->mapWidth * app->tile_world_size) - (width() / 2);
		return true;
	}

	bool col_right = false;

	//Check for collision on right:
	int grid_x_4 = (int)((x() + (width() / 2)) / 0.18f);
	int grid_y_4 = (int)(-(y()) / 0.18f);
	int data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		//pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		col_right = true;
	}


	return col_right;
}

bool GameObject::colliding_left(){
	//Prevent object from going off left side of screen
	if (x() - (width() / 2) < 0){
		pos.x = 0 + width() / 2;
		return true;
	}


	int grid_x_3 = (int)((x() - (width() / 2)) / 0.18f);
	int grid_y_3 = (int)(-(y()) / 0.18f);
	int data3 = app->map->layers[3][grid_y_3][grid_x_3];
	collidedLeft = false;

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
	}


	//Collision on left upper
	grid_y_3 = (int)(-(y()) / 0.18f) + 1;
	data3 = app->map->layers[3][grid_y_3][grid_x_3];

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
	}


	//Collision on left lower
	grid_y_3 = (int)(-(y()) / 0.18f) - 1;
	data3 = app->map->layers[3][grid_y_3][grid_x_3];

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
	}

	return collidedLeft;
}


bool GameObject::colliding_right(){
	//Prevent object from going off right side of screen
	if (x() + (width() / 2) > app->map->mapWidth * app->tile_world_size){
		pos.x = (app->map->mapWidth * app->tile_world_size) - (width() / 2);
		return true;
	}


	collidedRight = false;
	//Check for collision on right:
	int grid_x_4 = (int)((x() + (width() / 2)) / 0.18f);
	int grid_y_4 = (int)(-(y()) / 0.18f);
	int data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
	}


	//Check for collision on upper right:
	grid_y_4 = (int)(-(y()) / 0.18f) + 1;
	data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
	}

	//Check for collision on lower right:
	grid_y_4 = (int)(-(y()) / 0.18f) - 1;
	data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
	}


	return collidedRight;
}



double GameObject::distance(float x1, float y1, float x2, float y2) {
	const double x_diff = x1 - x2;
	const double y_diff = y1 - y2;
	return std::sqrt(x_diff * x_diff + y_diff * y_diff);
}



void GameObject::broadcast_event(const std::string& event_name){
	for (auto it = scripts.begin(); it != scripts.end(); ++it){
		it->second->on_event(event_name);
	}
}

void GameObject::check_for_collisions(){
	//Prevent object from going off left side of screen
	if (x() - (width() / 2) < 0){
		pos.x = 0 + width() / 2;
		broadcast_event("left_collide");
	}

	//Prevent object from going off right side of screen
	if (x() + (width() / 2) > app->map->mapWidth * app->tile_world_size){
		pos.x = (app->map->mapWidth * app->tile_world_size) - (width() / 2);
		broadcast_event("right_collide");
	}

	//Prevent object from going off top
	if (y() + (height() / 2) > 0){
		pos.y = 0 - height() / 2;
	}

	//Prevent object from going off bottom
	if (y() - (height() / 2) < -app->map->mapHeight * app->tile_world_size){
		pos.y += 2.8f;
	}


	if (is_colliding_bottom()){
		jumping = false;
		velocity.y = 0;
		pos.y += fabs((last_grid_y * app->tile_world_size) + (y() - height() / 2.0f));
		collidedBottom = true;
	}
	else{
		collidedBottom = false;
	}



	//check for head collision
	int grid_x_2 = (int)(x() / 0.18f);
	int grid_y_2 = (int)fabs(-(y() + height() / 2.6f) / 0.18f);
	int data2 = app->map->layers[3][grid_y_2][grid_x_2];
	if (data2 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		velocity.y = 0.0f;
		pos.y -= (y() + height() / 2.6f) - ((-app->tile_world_size * grid_y_2) - app->tile_world_size);
		collidedTop = true;
	}
	else{
		collidedTop = false;
	}
	//Head right side
	grid_x_2 = (int)(x() / 0.18f) + 1;
	data2 = app->map->layers[3][grid_y_2][grid_x_2];
	if (data2 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		velocity.y = 0.0f;
		pos.y -= (y() + height() / 2.6f) - ((-app->tile_world_size * grid_y_2) - app->tile_world_size);
		collidedTop = true;
	}
	else{
		collidedTop = false;
	}
	//Head left side
	grid_x_2 = (int)(x() / 0.18f) - 1;
	data2 = app->map->layers[3][grid_y_2][grid_x_2];
	if (data2 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		velocity.y = 0.0f;
		pos.y -= (y() + height() / 2.6f) - ((-app->tile_world_size * grid_y_2) - app->tile_world_size);
		collidedTop = true;
	}
	else{
		collidedTop = false;
	}



	//Check for collision on left:
	int grid_x_3 = (int)((x() - (width() / 2.0f)) / 0.18f);
	int grid_y_3 = (int)(-(y()) / 0.18f);
	int data3 = app->map->layers[3][grid_y_3][grid_x_3];
	collidedLeft = false;

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2.0f) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
		broadcast_event("left_collide");
	}


	//Collision on left upper
	grid_y_3 = (int)(-(y()) / 0.18f) + 1;
	data3 = app->map->layers[3][grid_y_3][grid_x_3];

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
	}


	//Collision on left lower
	grid_y_3 = (int)(-(y()) / 0.18f) - 1;
	data3 = app->map->layers[3][grid_y_3][grid_x_3];

	if (data3 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		pos.x -= (x() - width() / 2) - ((app->tile_world_size * grid_x_3) + app->tile_world_size);
		acceleration.x = 0;
		//velocity.x = 0;
		collidedLeft = true;
	}



	//Check for collision on right:
	collidedRight = false;
	int grid_x_4 = (int)((x() + (width() / 2)) / 0.18f);
	int grid_y_4 = (int)(-(y()) / 0.18f);
	int data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
		broadcast_event("right_collide");
	}
	//else{
	//	collidedRight = false;
	//}

	//Check for collision on upper right:
	grid_y_4 = (int)(-(y()) / 0.18f) + 1;
	data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
	}
	//else{
	//	collidedRight = false;
	//}

	//Check for collision on lower right:
	grid_y_4 = (int)(-(y()) / 0.18f) - 1;
	data4 = app->map->layers[3][grid_y_4][grid_x_4];

	if (data4 > 0){
		//std::cout << "Tile at heroes head: " << data2 << "   grid_x, grid_y" << grid_x_2 << "," << grid_y_2 << std::endl;
		acceleration.x = 0;
		//velocity.x = 0;
		pos.x -= fabs((x() + width() / 2) - ((app->tile_world_size * grid_x_4)));
		collidedRight = true;
	}
	//else{
	//	collidedRight = false;
	//}
}

float GameObject::left(){
	float left_side = pos.x - (size.x / 2);
	return left_side;
}

float GameObject::right(){
	float right_side = pos.x + (size.x / 2);
	return right_side;
}

float GameObject::top(){
	float top_side = pos.y + (size.y / 2);
	return top_side;
}

float GameObject::bottom(){
	float bottom_side = pos.y - (size.y / 2);
	return bottom_side;
}

void GameObject::update_scripts(){
	for (auto it = scripts.begin(); it != scripts.end(); ++it){
		it->second->set_app(app);
		it->second->update();
	}
}

void GameObject::update(){
	//pos.x += std::cosf(movement_angle) * elapsed * 1.0f;
	//pos.y += std::sinf(movement_angle) * elapsed * 1.0f;


	update_scripts();


	if (apply_velocity){
		float gravity = -11.0f;


		if (constant_x_velocity){
			velocity.x = velocity.x;
		}
		else{
			velocity.x += acceleration.x * app->elapsed;
		}

		if (apply_gravity){
			velocity.y += gravity * app->elapsed;
		}


		pos.y += app->elapsed * velocity.y;
		if (check_collisions){
			check_for_collisions();
		}

		pos.x += app->elapsed * velocity.x; //velocity contains direction
	
		if (check_collisions){
			check_for_collisions();
		}

	}

	Animation* current_animation = get_current_animation();

	//std::cout << current_animation_name << std::endl;
	if (current_animation != NULL){
		if (strings["shooter_name"] == "hero"){
			if (current_animation_name == "idle_shoot" && get_runtime() - last_shoot > 0.2){
				//std::cout << "SETTING TO IDLE FROM IDLESHOOT" << std::endl;
				current_animation_name = "idle";
				set_animation("idle");
				return;
			}
		}

		current_animation->update();
		current_animation->update_size(size.x, size.y);


		//if (!current_animation->loop && current_animation->done_time_elapsed(current_animation->interval)){
		//	destroy();
		//}
	}


	

}

Animation* GameObject::get_current_animation(){
	if (animations.count(current_animation_name)){
		return &animations[current_animation_name];
	}
	else{
		return NULL;
	}
}


void GameObject::set_direction_x(float x_){
	direction[0] = x_;
}

void GameObject::set_direction_y(float y_){
	direction[1] = y_;
}

void GameObject::set_direction(float x_, float y_){
	direction[0] = x_;
	direction[1] = y_;
}

float GameObject::x(){
	return pos.x;
}

float GameObject::y(){
	return pos.y;
}

float GameObject::z(){
	return pos.z;
}


float GameObject::top_left_x(){
	return pos.x - (size.x / 2);
}

float GameObject::top_left_y(){
	return pos.y - (size.y / 2);
}

void GameObject::set_verts(std::vector<float> arr){
	verts = arr;
}


void GameObject::set_size(float width_, float height_){
	size.x = width_;
	size.y = height_;



	set_verts(app->quad_verts(size.x, size.y));
}

void GameObject::set_draw_mode(std::string mode_){
	draw_mode = mode_;
}

void GameObject::set_velocity(float x_, float y_, float z_){
	velocity.x = x_;
	velocity.y = y_;
	velocity.z = z_;
}


void GameObject::set_color(float r, float g, float b, float a){
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}



void GameObject::draw(){
	if (destroyed){
		return;
	}

	if (draw_mode == "texture"){

		app->tex_program->SetModelMatrix(app->modelMatrix);
		app->tex_program->SetProjectionMatrix(app->projectionMatrix);
		app->tex_program->SetViewMatrix(app->viewMatrix);


		if (animations.count(current_animation_name)){
			app->modelMatrix.Identity();
			app->modelMatrix.Translate(x(), y(), z());
			app->modelMatrix.SetScale(direction[0], 1, 1);
			app->tex_program->SetModelMatrix(app->modelMatrix);
			app->tex_program->SetViewMatrix(app->viewMatrix);
			app->tex_program->SetColor(1, 1, 0, 1);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glColor3f(0,1,0);

			glUseProgram(app->tex_program->programID);

			animations[current_animation_name].draw();
		}
	}
	else if (draw_mode == "shape"){
		app->shape_program->SetModelMatrix(app->modelMatrix);
		app->shape_program->SetProjectionMatrix(app->projectionMatrix);
		app->shape_program->SetViewMatrix(app->viewMatrix);
		glUseProgram(app->shape_program->programID);


		app->modelMatrix.Identity();
		app->modelMatrix.Translate(x(), y(), z());
		app->shape_program->SetModelMatrix(app->modelMatrix);
		app->shape_program->SetColor(color[0], color[1], color[2], color[3]);

		glVertexAttribPointer(app->shape_program->positionAttribute, 2, GL_FLOAT, false, 0, verts.data());
		glEnableVertexAttribArray(app->shape_program->positionAttribute);
		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(app->shape_program->positionAttribute);
	}


}



float GameObject::width(){
	return size.x;
}


float GameObject::height(){
	return size.y;
}



GameObject GameObject::shoot(Animation animation){
	last_shoot = get_runtime();
	GameObject newBullet;
	newBullet.set_pos(x(), y() + 0.1f);
	newBullet.set_velocity(direction[0] * 3.0f, 0);
	newBullet.set_direction(direction[0], direction[1]);
	newBullet.set_draw_mode("texture");
	newBullet.set_size(0.1, 0.1);
	newBullet.set_verts(app->quad_verts(0.1, 0.1));
	newBullet.strings["shooter_name"] = name;
	newBullet.apply_gravity = false;
	newBullet.check_collisions = false;

	newBullet.set_app(app);
	animation.set_app(app);

	newBullet.add_animation("idle", animation);
	newBullet.set_animation("idle");

	return newBullet;
}


void GameObject::add_script(const std::string& script_name, Script* script){
	scripts[script_name] = script;
}


void GameObject::take_hit(float dmg){
	if (app->get_runtime() - last_hit < invincibility_duration){
		//Dont take a hit, currently invincible
		return;
	}


	app->play_sound("pain");

	last_hit = app->get_runtime();
	life -= dmg;

}