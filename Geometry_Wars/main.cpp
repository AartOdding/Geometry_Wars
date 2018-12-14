#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "EntityManager.h"
#include "PlayerShip.h"
//Deze moeten allemaal naar 1 header denk ik
#include "HealthComponent.h"
#include "RenderComponent.h"
#include "MovementComponent.h"
#include "TransformationComponent.h"

#include <chrono>
#undef main

#include "ShaderManager.h"



void configure_context()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
}


void print_context_status()
{
    int profile, major_version, minor_version, double_buffering;
    int red_bits, green_bits, blue_bits, alpha_bits;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &double_buffering);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red_bits);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &green_bits);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &blue_bits);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &alpha_bits);

    std::cout << "version:\t" << major_version << "." << minor_version << "\n";
    std::cout << "core profile:\t" << ((profile == SDL_GL_CONTEXT_PROFILE_CORE) ? "true" : "false") << "\n";
    std::cout << "vsync:\t\t" << (double_buffering == 1 ? "true" : "false") << "\n";
    std::cout << "rgba bits:\t" << red_bits << ", " << green_bits << ", " << blue_bits << ", " << alpha_bits << "\n";
}

#include "glm_type_registration.h"

#include "Renderer.h"




int main(int argc, char* args[])
{

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "Error initializing SDL";
        std::exit(1);
    }

    SDL_Window * window = nullptr;
    SDL_GLContext opengl_context = nullptr;

    window = SDL_CreateWindow("test", 100, 100, 640, 480,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    configure_context();

    opengl_context = SDL_GL_CreateContext(window);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    SDL_GL_SetSwapInterval(1);

    print_context_status();


    // * *************************************************

    //Application

    GaussianBlur::load_shaders();

    ShaderManager::load_default_shader();
    ShaderManager::load_renderFBO_shader();
    ShaderManager::load_resolveMSAA_shader();



    Renderer renderer;

    renderer.resized(640, 480);
    // * *************************************************

	//****** Game Loop\\
	//Todo DENNIS: Maak hier nog een apparte game class voor!\\

	std::cout << "Creating Entity manager" << std::endl;
	EntityManager * eManager = new EntityManager();

	//GameObject * playerEntity = new PlayerShip(eManager->CreateEntity());

	eManager->update();

	auto test = eManager->CreateEntity();
	eManager->update();

	std::cout << "The component is being added!" << std::endl;
	test->setComponent<HealthComponent>();
	eManager->update();
	
	std::cout << "The component is got!" << std::endl;
	test->getComponent<HealthComponent>()->print();
	
	std::cout<<"The component is being removed!" << std::endl;
	test->removeComponent<HealthComponent>();
	test->setComponent<MovementComponent>();
	test->getComponent<MovementComponent>()->setConstantMovement(glm::vec2(1, 1));
	test->setComponent<RenderComponent>(renderer);

	auto blok = eManager->CreateEntity();
	blok->setComponent<MovementComponent>(glm::vec2(-50,0));
	blok->getComponent<MovementComponent>()->setConstantMovement(glm::vec2(1, 1));
	blok->setComponent<RenderComponent>(renderer);
	
	std::vector<Entity*> gameObjects;
	for (int i = 0; i < 100;i++) {
		gameObjects.emplace_back(eManager->CreateEntity());
		gameObjects.back()->setComponent<MovementComponent>(glm::vec2(i * 50, 0));
		gameObjects.back()->getComponent<MovementComponent>()->setConstantMovement(glm::vec2(1, 1));
		gameObjects.back()->setComponent<RenderComponent>(renderer);
	}
	
    auto tower = eManager->CreateEntity();
    tower->setComponent<TransformationComponent>();
    tower->setComponent<RenderComponent>(renderer);

    tower->getComponent<TransformationComponent>()->translate(100, 100);
    tower->getComponent<RenderComponent>()->shape.set_shape({ {0, 20}, {20, -20}, {-20, -20} });


	/*EntityManager::getLastComponentID<T>();
	*/

	//playerEntity->update();
	//DefaultShader default_shader;
	//default_shader.start();

	// * *************************************************



    for (bool quit = false; !quit;)
    {

		auto start = std::chrono::system_clock::now();
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
            {
                quit = true;
            }
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_LEFT: // YOUR CODE HERE
					
					break;
				case SDLK_RIGHT: // YOUR CODE HERE
					break;
				case SDLK_UP: // YOUR CODE HERE
					break;
				case SDLK_DOWN: // YOUR CODE HERE
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
            else if (event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                renderer.resized(event.window.data1, event.window.data2);
                std::cout << "Window resized to: " << event.window.data1 << ", " << event.window.data2 << "\n";
            }
			
        }

		eManager->update();
        renderer.render_frame();
        
		// * *************************************************


        // * *************************************************


        SDL_GL_SwapWindow(window);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = end - start;
		std::cout << diff.count() << std::endl;
    }


    SDL_DestroyWindow(window);


    SDL_Quit();

    return 0;
}
