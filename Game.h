#pragma once

#include <chrono>
#include <cstdint>

#include "engine/InputManager.h"
#include "engine/EntityManager.h"
#include "engine/CollisionManager.h"
#include "engine/ActionController.h"

#include "sound/SoundManager.h"

#include "content/GameObjectSpawner.h"

#include "graphics/ShaderManager.h"

#include "utility/random.h"
#include "content/EnemyBehaviour.h"
#include <array>

class Renderer;

class Game {

public:

    enum class State :int {
        WELCOME,
        PLAYING,
        PAUSE,
        GAMEOVER
    };

private:

	EntityManager entity_manager;
	InputManager input_manager;
	CollisionManager collision_manager;
	std::unique_ptr<Renderer> renderer;
	SoundManager sound_manager;

	GameObjectSpawner object_spawner;

	ActionController stateHandler;

	void spawn_enemy(float);


	int score;
	int lifes;
	float last_spawn = 0;
	float spawn_rate = .5;
	void reset();
	void load_shaders();
	std::array<EnemyBehaviour *, 4> behaviours = {new Chaser(), new Random(), new ChaserWhenNear(), new Shooting()};
	State current_state;
	
public:

	Game();
	~Game();

	void init();
	void update(float delta_time);

	InputManager& get_input_manager();
	Renderer& get_renderer();
	EntityManager& get_entity_manager();
	CollisionManager& get_collision_manager();
		
	void Test();
	void init_level();
	/*
	void onKeyDown(uint32_t key, bool repeated);
	void onKeyUp(uint32_t key, bool repeated);
	void onMouseDown(uint32_t button, uint8_t numOfClicks);
	void onMouseUp(uint32_t button, uint8_t numOfClicks);
	void onMouseMove(uint32_t mouseX, uint32_t mouseY, uint32_t mouseDeltaX, uint32_t mouseDeltaY);
	*/
	State get_game_state();
	void set_game_state(Game::State);

};
