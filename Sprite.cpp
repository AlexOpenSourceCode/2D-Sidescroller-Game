#include "Sprite.h";
#include "App.h";

Sprite::Sprite(const std::string& file_path){
	texture_id = app->LoadTexture(file_path.c_str(), &width, &height);

	aspect_ratio = (width*1.0f) / (height*1.0f);
	x_size *= aspect_ratio;

	//std::cout << "Loading sprite " << width << " " << height << std::endl;
}



Sprite::Sprite(GLuint texture_id_, float u_, float v_, float width_, float height_, float size_){
	u = u_;
	v = v_;
	width = width_;
	height = height_;
	size = size_;

	texture_id = texture_id_;

	aspect_ratio = (width*1.0f) / (height*1.0f);
	sheet = true;
}


void Sprite::set_size(float x_size_, float y_size_){
	x_size = x_size_;
	y_size = y_size_;
	size = x_size;

}



void Sprite::set_size(float x_size_){
	x_size = x_size_;
	y_size = x_size;
	x_size = x_size_ * ((height*1.0f) / (width*1.0f));
	size = x_size_;
}

void Sprite::set_pos(float x_, float y_, float z_){
	x = x_;
	y = y_;
	z = z_;
	update_position = true;
}


void Sprite::set_vert_pos(float x_, float y_, float z_){
	x = x_;
	y = y_;
	z = z_;
	update_position = true;
}


std::vector<float> Sprite::get_verts(){

	//std::vector<float> verts = {
	//	-x_size, -y_size,
	//	x_size, -y_size,
	//	x_size, y_size,
	//	-x_size, -y_size,
	//	x_size, y_size,
	//	-x_size, y_size
	//};

	std::vector<float> verts = {
		x - x_size, y - y_size,
		x + x_size, y - y_size,
		x + x_size, y + y_size,
		x - x_size, y - y_size,
		x + x_size, y + y_size,
		x - x_size, y + y_size
	};


	//if (sheet){
	//	float aspect = width / height;
	//	verts = {
	//		-0.5f * size * aspect, -0.5f * size,
	//		0.5f * size * aspect, 0.5f * size,
	//		-0.5f * size * aspect, 0.5f * size,
	//		0.5f * size * aspect, 0.5f * size,
	//		-0.5f * size * aspect, -0.5f * size,
	//		0.5f * size * aspect, -0.5f * size };
	//}


	if (sheet){
		//float aspect = width / height;
		float aspect = x_size / y_size;
		verts = {
			x - 0.5f * size * aspect, y - 0.5f * size,
			x + 0.5f * size * aspect, y + 0.5f * size,
			x - 0.5f * size * aspect, y + 0.5f * size,
			x + 0.5f * size * aspect, y + 0.5f * size,
			x - 0.5f * size * aspect, y - 0.5f * size,
			x + 0.5f * size * aspect, y - 0.5f * size };
	}



	return verts;
}

std::vector<float> Sprite::get_tex_coords(){
	std::vector<float> tex_coords = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	if (sheet){
		tex_coords = {
			u, v + height,
			u + width, v,
			u, v,
			u + width, v,
			u, v + height,
			u + width, v + height
		};
	}

	return tex_coords;
}

void Sprite::set_app(std::shared_ptr<App> app_){
	app = app_;
}

void Sprite::draw(){
	if (update_position){
		app->tex_program->SetModelMatrix(app->modelMatrix);
		app->tex_program->SetProjectionMatrix(app->projectionMatrix);
		app->tex_program->SetViewMatrix(app->viewMatrix);

		app->modelMatrix.Identity();
		app->modelMatrix.Translate(x, y, z);
		app->tex_program->SetModelMatrix(app->modelMatrix);
		app->tex_program->SetViewMatrix(app->viewMatrix);

		glUseProgram(app->tex_program->programID);
	}


	

	glUseProgram(app->tex_program->programID);


	app->tex_program->SetColor(0,1,0,0.5f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//Draws sprites pixel perfect with no blur
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::vector<float> verts = get_verts();
	std::vector<float> texCoords = get_tex_coords();

	glVertexAttribPointer(app->tex_program->positionAttribute, 2, GL_FLOAT, false, 0, verts.data());
	glEnableVertexAttribArray(app->tex_program->positionAttribute);

	glVertexAttribPointer(app->tex_program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(app->tex_program->texCoordAttribute);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBindTexture(GL_TEXTURE_2D, texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(app->tex_program->positionAttribute);
	glDisableVertexAttribArray(app->tex_program->texCoordAttribute);

}


