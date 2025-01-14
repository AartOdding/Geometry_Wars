#include <iostream>
#include <utility>
#include <queue>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>


#include "graphics/Shape.h"
#include "graphics/Renderer.h"
#include "graphics/ShaderManager.h"
#include "graphics/Background.h"
#include "graphics/glm_type_registration.h"

#include "opengl/Shader.h"
#include "opengl/ShaderState.h"
#include "opengl/Attribute.h"
#include "opengl/Uniform.h"

#include "components/RenderComponent.h"

#include "utility/random.h"
#include "utility/io.h"




Renderer::Renderer()
{
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    

    auto [path, success] = find_folder("Geometry_Wars");
    auto shaders_folder = path / "data";

    if (fs::exists(shaders_folder) && fs::is_directory(shaders_folder))
    {
        std::cout << "\nShaders folder found at:\n";
        std::cout << shaders_folder << '\n';
    }
    else
    {
        std::cout << "\nUnable to locate \'shaders\' folder.";
    }


    msaa_resolver = std::make_unique<ShaderState>(*ShaderManager::get("resolveMSAA"));
    msaa_resolver->attribute["position"] = std::vector<glm::vec2>{ {-1, 1 }, { 1, 1 }, { -1, -1 }, { 1, 1 }, { 1, -1 }, { -1, -1 } };
    msaa_resolver->static_uniform["amount"] = 8;


    combine_shader = std::make_unique<ShaderState>(*ShaderManager::get("combine"));
    combine_shader->attribute["position"] = std::vector<glm::vec2>{ {-1, 1 }, { 1, 1 }, { -1, -1 }, { 1, 1 }, { 1, -1 }, { -1, -1 } };


    final_shader = std::make_unique<ShaderState>(*ShaderManager::get("final"));
    final_shader->attribute["position"] = std::vector<glm::vec2>{ {-1, 1 }, { 1, 1 }, { -1, -1 }, { 1, 1 }, { 1, -1 }, { -1, -1 } };
    final_shader->uniform["inverted"] = 0;
    final_shader->uniform["r_transform"] = r_transform;
    final_shader->uniform["g_transform"] = g_transform;
    final_shader->uniform["b_transform"] = b_transform;


    //final_shader->add_uniform("r_transform", Type::MAT_3x3);
    //final_shader->add_uniform("g_transform", Type::MAT_3x3);
    //final_shader->add_uniform("b_transform", Type::MAT_3x3);

    render_texture = std::make_unique<ShaderState>(*ShaderManager::get("renderFBO"));
    render_texture->attribute["position"] = std::vector<glm::vec2>{ {-1, 1 }, { 1, 1 }, { -1, -1 }, { 1, 1 }, { 1, -1 }, { -1, -1 } };
    render_texture->static_uniform["viewport"] = glm::ivec2(640, 480);
    

    std::vector<glm::vec2> s{ {-10, -10}, { -10, 10 }, { 10, 10}, { 10, -10} };
    
    for (int i = 0; i < 230; ++i) random(100); // discard some because the pattern starts ugly

    for (auto& shape : test_shapes)
    {
        shape.set_shape(s);
        shape.translate(random(0, 500), random(0, 500));
        shape.rotate(random(30.0f));
        shape.scale(random(1.0f, 5.0f), random(1.0f, 5.0f));
        shape.set_color(random(255), 150, 200, 255);
        //shape.set_line_width(random(3, 20));
        shape.set_line_width(4);
    }


    frame_buffer_1.add_texture(Texture::Type::NORMALIZED_NO_MIPMAP, GL_RGBA8, GL_COLOR_ATTACHMENT0);
    frame_buffer_1.get_texture(0).set_wrap_x(GL_CLAMP_TO_EDGE);
    frame_buffer_1.get_texture(0).set_wrap_y(GL_CLAMP_TO_EDGE);

    frame_buffer_2.add_texture(Texture::Type::NORMALIZED_NO_MIPMAP, GL_RGBA8, GL_COLOR_ATTACHMENT0);
    frame_buffer_2.get_texture(0).set_wrap_x(GL_CLAMP_TO_EDGE);
    frame_buffer_2.get_texture(0).set_wrap_y(GL_CLAMP_TO_EDGE);

    frame_buffer_3.add_texture(Texture::Type::NORMALIZED_NO_MIPMAP, GL_RGBA8, GL_COLOR_ATTACHMENT0);
    frame_buffer_3.get_texture(0).set_wrap_x(GL_CLAMP_TO_EDGE);
    frame_buffer_3.get_texture(0).set_wrap_y(GL_CLAMP_TO_EDGE);

    background = std::make_unique<Background>(Background::Theme::DARK_THEME);
}




Renderer::~Renderer()
{
    ShaderManager::shutdown();
}




void Renderer::shake(float amount, float decrease)
{
    shake_amount = amount;
    shake_decrease = decrease;
}


void Renderer::render_frame(Game::State game_state)
{
    frame_buffer_1.start_rendering();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (game_state == Game::State::PLAYING)
    {
        for (auto renderable : renderables)
        {
            renderable->render();
        }
    }

	renderables.clear();

    if (game_state == Game::State::WELCOME)
    {
        for (auto& shape : test_shapes)
        {
            shape.rotate(0.01);
            shape.render();
        }
    }

    frame_buffer_1.stop_rendering();

    // ***************************************************************

    auto near_blur = blur_near.apply(frame_buffer_1.get_texture(0));

    // ***************************************************************

    frame_buffer_2.start_rendering();

    auto original_texture = &(frame_buffer_1.get_texture(0));
    auto blurred_texture = &near_blur->get_texture(0);

    combine_shader->activate();

    glUniform1i(glGetUniformLocation(combine_shader->get_shader().get_handle(), "tex1"), 0);
    glUniform1i(glGetUniformLocation(combine_shader->get_shader().get_handle(), "tex2"), 1);

    glActiveTexture(GL_TEXTURE0);
    original_texture->bind();

    glActiveTexture(GL_TEXTURE1);
    blurred_texture->bind();

    combine_shader->uniform["weights"] = glm::vec2(0.35, 0.75);


    glDrawArrays(GL_TRIANGLES, 0, 6);


    frame_buffer_2.stop_rendering();
    
    // ***************************************************************

    frame_buffer_3.start_rendering();

    background->render();

    frame_buffer_3.stop_rendering();


    // **************************************************************

    static int counter = 0;
    counter++;

    final_shader->activate();
    
    if (counter % 2 == 0)
    {
        r_transform = glm::mat3(1);
        r_transform = glm::translate(r_transform, glm::vec2(random(-shake_amount, shake_amount), random(-shake_amount, shake_amount)));

        g_transform = glm::mat3(1);
        g_transform = glm::translate(r_transform, glm::vec2(random(-shake_amount, shake_amount), random(-shake_amount, shake_amount)));

        b_transform = glm::mat3(1);
        b_transform = glm::translate(r_transform, glm::vec2(random(-shake_amount, shake_amount), random(-shake_amount, shake_amount)));
    }

    shake_amount -= shake_decrease;

    if (shake_amount < 0.0f)
    {
        shake_amount = 0.0f;
    }


    final_shader->uniform["r_transform"] = r_transform;
    final_shader->uniform["g_transform"] = g_transform;
    final_shader->uniform["b_transform"] = b_transform;


    glUniform1i(glGetUniformLocation(final_shader->get_shader().get_handle(), "tex1"), 0);
    glUniform1i(glGetUniformLocation(final_shader->get_shader().get_handle(), "tex2"), 1);

    glActiveTexture(GL_TEXTURE0);
    frame_buffer_3.get_texture(0).bind();

    glActiveTexture(GL_TEXTURE1);
    frame_buffer_2.get_texture(0).bind();

    final_shader->uniform["weights"] = glm::vec2(1, 1);
    final_shader->uniform["inverted"] = 0;

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (game_state == Game::State::WELCOME)
    {
        background->render_welcome_screen();
    }
}



void Renderer::resized(int w, int h)
{
    for (auto& shape : test_shapes)
    {
        shape.reset_transformation();
        shape.translate(random(0, w), random(0, h));
        shape.rotate(random(30.0f));
        shape.scale(random(1.5f, 8.0f), random(1.5f, 8.0f));
    }

    width = w;
    height = h;

    background->window_resized(w, h);

    blur_near.window_resized(w, h);

    frame_buffer_1.stop_rendering();
    frame_buffer_2.stop_rendering();
    frame_buffer_3.stop_rendering();

    glViewport(0, 0, w, h);

    frame_buffer_1.set_size(w, h);
    frame_buffer_2.set_size(w, h);
    frame_buffer_3.set_size(w, h);

    ShaderManager::get("default")->static_uniform["viewport"] = glm::vec2(w, h);
    ShaderManager::get("renderFBO")->static_uniform["viewport"] = glm::vec2(w, h);
    ShaderManager::get("combine")->static_uniform["viewport"] = glm::vec2(w, h);
    ShaderManager::get("final")->static_uniform["viewport"] = glm::vec2(w, h);
}

void Renderer::queueToRender(RenderComponent * component)
{
	renderables.emplace_back(component);
}
