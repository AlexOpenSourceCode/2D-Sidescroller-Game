

#ifndef APP_H
#define APP_H


#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include <vector>
#include <unordered_map>
#include <math.h>
#include <algorithm> //std::remove_if
#include <time.h>  
#include <SDL_mixer.h>
#include "FlareMap.h"
#include "Vector3.h";
#include "GroundSpikeScript.h";

class GameObject;

class App{
public:

	SDL_Window* displayWindow;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram* tex_program;
	ShaderProgram* shape_program;
	GLuint font_texture;
	float elapsed;
	bool done = false;
	float font_sheet_width = 0;
	float font_sheet_height = 0;

	float screen_left = -3.55;
	float screen_right = 3.55;
	float screen_top = 2.0f;
	float screen_bottom = -2.0f;

	float screen_width = screen_right - screen_left;
	float screen_height = screen_top - screen_bottom;

	int TILE_SIZE = 8; //pixel size
	float tile_world_size = 0.18f;

	enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER, STATE_GAME_WON };
	GameMode mode;

	FlareMap* map;




	Mix_Music* music1 = NULL;

	std::unordered_map<std::string, Mix_Chunk*> sounds;

	void play_sound(std::string sound_name);


	void init();
	//seconds since program started
	float get_runtime();


	GLuint LoadTexture(const char* filePath, float* width, float* height);

	std::vector<float> quad_verts(float width, float height);



	void draw_text(std::string text, float x, float y, int fontTexture, float size, float spacing);

	void batch_draw(int texture_id, std::vector<float>& verts, std::vector<float>& texCoords);

	float radians_to_degrees(float radians);

	float degrees_to_radians(float degrees);

	bool check_box_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

	bool check_box_collision(GameObject& obj1, GameObject& obj2);





};


#endif