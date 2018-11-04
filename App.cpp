#include "App.h";
#include "GameObject.h";
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

void App::init(){
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk* laser1 = Mix_LoadWAV("resources/pain.wav");
	sounds["pain"] = laser1;

	//Play music
	music1 = Mix_LoadMUS("resources/ffx.mp3");
	Mix_PlayMusic(music1, -1);



	map = new FlareMap();

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer - AFL294@NYU.EDU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif



	font_texture = LoadTexture("resources/font.png", &font_sheet_width, &font_sheet_height);


	glViewport(0, 0, 1000, 600);

	tex_program = new ShaderProgram();

	tex_program->Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");


	shape_program = new ShaderProgram();

	shape_program->Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");


	projectionMatrix.SetOrthoProjection(screen_left, screen_right, screen_bottom, screen_top, -1.0f, 1.0f);

	mode = STATE_MAIN_MENU;
	//mode = STATE_GAME_LEVEL;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}



float App::get_runtime(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	return ticks;
}


GLuint App::LoadTexture(const char* filePath, float* width, float* height){
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL){
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	*width = w;
	*height = h;

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
	return retTexture;
}

std::vector<float> App::quad_verts(float width, float height){
	return{
		-width / 2, height / 2, //top left
		-width / 2, -height / 2, // bottom left
		width / 2, -height / 2, //bottom right
		width / 2, height / 2 //top right
	};
}



void App::draw_text(std::string text, float x, float y, int fontTexture, float size, float spacing){



	float texture_size = 1.0 / 16.0f;

	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int i = 0; i < text.size(); i++){
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;

		vertexData.insert(vertexData.end(), {
			((spacing * i) + (-0.5f * size)), 0.5f * size,
			((spacing * i) + (-0.5f * size)), -0.5f * size,
			((spacing * i) + (0.5f * size)), 0.5f * size,
			((spacing * i) + (0.5f * size)), -0.5f * size,
			((spacing * i) + (0.5f * size)), 0.5f * size,
			((spacing * i) + (-0.5f * size)), -0.5f * size,
		});


		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size
		});
	}



	modelMatrix.Identity();
	modelMatrix.Translate(x, y, 0);
	tex_program->SetModelMatrix(modelMatrix);
	tex_program->SetProjectionMatrix(projectionMatrix);
	tex_program->SetViewMatrix(viewMatrix);
	glUseProgram(tex_program->programID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//Draws sprites pixel perfect with no blur
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glVertexAttribPointer(tex_program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(tex_program->positionAttribute);

	glVertexAttribPointer(tex_program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(tex_program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(tex_program->positionAttribute);
	glDisableVertexAttribArray(tex_program->texCoordAttribute);

}




void App::batch_draw(int texture_id, std::vector<float>& verts, std::vector<float>& texCoords){
	tex_program->SetModelMatrix(modelMatrix);
	tex_program->SetProjectionMatrix(projectionMatrix);
	tex_program->SetViewMatrix(viewMatrix);

	modelMatrix.Identity();
	modelMatrix.Translate(0, 0, 0);
	tex_program->SetModelMatrix(modelMatrix);
	tex_program->SetViewMatrix(viewMatrix);

	glUseProgram(tex_program->programID);

	glUseProgram(tex_program->programID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//Draws sprites pixel perfect with no blur
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glVertexAttribPointer(tex_program->positionAttribute, 2, GL_FLOAT, false, 0, verts.data());
	glEnableVertexAttribArray(tex_program->positionAttribute);

	glVertexAttribPointer(tex_program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(tex_program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glDrawArrays(GL_TRIANGLES, 0, verts.size() / 2);

	glDisableVertexAttribArray(tex_program->positionAttribute);
	glDisableVertexAttribArray(tex_program->texCoordAttribute);

}

float App::radians_to_degrees(float radians) {
	return radians * (180.0 / M_PI);
}

float App::degrees_to_radians(float degrees){
	return ((degrees)* M_PI / 180.0);
}

bool App::check_box_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){

	if (
		y1 + h1 < y2 ||
		y1 > y2 + h2 ||
		x1 + w1 < x2 ||
		x1 > x2 + w2) {

		return false;
	}
	else{
		return true;
	}
}

bool App::check_box_collision(GameObject& obj1, GameObject& obj2){
	if (obj1.destroyed){
		return false;
	}

	if (obj2.destroyed){
		return false;
	}

	return check_box_collision(obj1.top_left_x(), obj1.top_left_y(), obj1.width(), obj1.height(), obj2.top_left_x(), obj2.top_left_y(), obj2.width(), obj2.height());
}





void App::play_sound(std::string sound_name){
	if (sounds.find(sound_name) == sounds.end()){
		//key not found
		return;
	}

	Mix_PlayChannel(-1, sounds[sound_name], 0); //if channel is -1, pick the first free unreserved channel.
}




