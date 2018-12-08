#pragma once

#include <vector>

#include <glad/glad.h>


#include "texture.h"



class FrameBuffer
{

public:

    FrameBuffer(unsigned int width, unsigned int height);

    FrameBuffer(const FrameBuffer&) = delete;

    FrameBuffer(FrameBuffer&& other);

    ~FrameBuffer();


    void add_texture(const Texture2D::Settings& texture_settings, GLenum attachment_point);


    void resize(unsigned int width, unsigned int height);


    void start_rendering();


    void stop_rendering();


    void bind_to(GLenum target);


    GLuint get_handle();


private:

    unsigned int fbo_width;
    unsigned int fbo_height;

    GLuint framebuffer_handle;

    bool has_moved = false;

    std::vector<std::pair<Texture2D, GLenum>> attachments;

};
