#include "Display.h"
AllScreenArray::AllScreenArray(const Configuration::ConfigData &configuration)
{
    _total_width = (configuration.width + configuration.l_bezel + configuration.r_bezel) * configuration.h_screens;
    _total_height = (configuration.height + configuration.t_bezel + configuration.b_bezel) * configuration.v_screens;
}

float AllScreenArray::GetAspectRatio()
{
    return (_total_width / _total_height);
}

Display::Display(const Configuration::ConfigData &configuration) : _renderer{configuration}, _screen{configuration}
{
}

Display::~Display()
{
    SDL_DestroyWindow(_renderer.window);
    SDL_Quit();
}

void Display::DisplaySingleImage(const Configuration::ConfigData &configuration)
{
    Texture texture(configuration.image_location);
    // Calculate the scale factor of the image, this will blow the image up to the size of the full array of screens
    // and will correct for aspect ratio
    float scale_factor{1};
    float total_pixels_ratio = _screen.GetAspectRatio();
    float texture_pixels_ratio{((float)texture.GetWidth() / (float)texture.GetHeight())};

    if (total_pixels_ratio > texture_pixels_ratio)
        scale_factor = _screen._total_height / (float)texture.GetHeight();
    else if (total_pixels_ratio == texture_pixels_ratio)
        scale_factor = _screen._total_height / (float)texture.GetHeight();
    else if (total_pixels_ratio < texture_pixels_ratio)
        scale_factor = _screen._total_width / (float)texture.GetWidth();

    float x_origin = ((_screen._total_width - (float)texture.GetWidth() * (float)scale_factor) / 2);
    float y_origin = ((_screen._total_height - (float)texture.GetHeight() * (float)scale_factor) / 2);
    float x_end = ((float)texture.GetWidth() * (float)scale_factor) + x_origin;
    float y_end = ((float)texture.GetHeight() * (float)scale_factor) + y_origin;
    float x_texture_origin{0.0f};
    float y_texture_origin{0.0f};
    float x_texture_end{1.0f};
    float y_texture_end{1.0f};

    // Set the 4 vertices of the image, each vertex is x, y, (texture_x), (texture_y)
    // We are creating a square that has the image on it, the image co-ords are from 0.0 to 1.0
    // the square's co-ords are from 0.0 to the total_width of the array of screens
    float positions[] = {
        x_origin, y_origin, x_texture_origin, y_texture_origin, // 1
        x_end, y_origin, x_texture_end, y_texture_origin,       // 2
        x_end, y_end, x_texture_end, y_texture_end,             // 3
        x_origin, y_end, x_texture_origin, y_texture_end};      // 4
    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0};

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    const std::string vs_source =
#include "../res/shaders/Basic.shader"
        ;
    Shader shader(vs_source, true);
    shader.Bind();
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);

    glm::mat4 mvp{1.0f};
    // proj is the size of the local screen
    glm::mat4 proj = glm::ortho(0.0f, (float)configuration.width, 0.0f, (float)configuration.height, -1.0f, 1.0f);
    // We move the camera to where it should be in relation to the full array of screens
    glm::mat4 view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f - (float)((configuration.width + (configuration.l_bezel + configuration.r_bezel)) * configuration.h_index), 0.0f - (float)((configuration.height + (configuration.t_bezel + configuration.b_bezel)) * configuration.v_index), 0.0f});

    shader.SetUniformMat4f("u_MVP", mvp);
    shader.SetUniformMat4f("u_Proj", proj);
    shader.SetUniformMat4f("u_View", view);

    _renderer.Clear();
    _renderer.Draw(va, ib, shader);
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();
    SDL_GL_SwapWindow(_renderer.window);
}

bool Display::ShouldExit()
{
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event) != 0)
    {
        if (sdl_event.type == SDL_QUIT)
            return true;
    }
    return false;
}