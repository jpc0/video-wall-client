#include "glad.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "Configuration.h"

int main(int argc, char *argv[])
{
    try
    {
        Configuration::ConfigData configuration{argc, argv};
        Renderer renderer{configuration};

        // Load Texture
        Texture texture(configuration.image_location);

        // Calculate the scale factor of the image, this will blow the image up to the size of the full array of screens
        // and will correct for aspect ratio
        float scale_factor;
        if (((float)configuration.total_width / (float)configuration.total_height) > ((float)texture.GetWidth() / (float)texture.GetHeight()))
            scale_factor = (float)configuration.total_height / (float)texture.GetHeight();
        else if (((float)configuration.total_width / (float)configuration.total_height) == ((float)texture.GetWidth() / (float)texture.GetHeight()))
            scale_factor = (float)configuration.total_height / (float)texture.GetHeight();
        else if (((float)configuration.total_width / (float)configuration.total_height) < ((float)texture.GetWidth() / (float)texture.GetHeight()))
            scale_factor = (float)configuration.total_width / (float)texture.GetWidth();

        // This is where the bottom left of the image will be, if the aspect ratio matches it will be 0,0
        float x_origin = ((configuration.total_width - (float)texture.GetWidth() * (float)scale_factor) / 2);
        float y_origin = ((configuration.total_height - (float)texture.GetHeight() * (float)scale_factor) / 2);

        // Set the 4 vertices of the image, each vertex is x, y, (texture_x), (texture_y)
        // We are creating a square that has the image on it, the image co-ords are from 0.0 to 1.0
        // the square's co-ords are from 0.0 to the total_width of the array of screens
        float positions[] = {
            x_origin, y_origin, 0.0f, 0.0f,                                                                                                          // 1
            ((float)texture.GetWidth() * (float)scale_factor) + x_origin, y_origin, 1.0f, 0.0f,                                                      // 2
            ((float)texture.GetWidth() * (float)scale_factor) + x_origin, ((float)texture.GetHeight() * (float)scale_factor) + y_origin, 1.0f, 1.0f, // 3
            x_origin, (float)texture.GetHeight() * (float)scale_factor + y_origin, 0.0f, 1.0f};                                                      // 4

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

        glm::mat4 mvp;
        // proj is the size of the local screen
        glm::mat4 proj = glm::ortho(0.0f, (float)configuration.width, 0.0f, (float)configuration.height, -1.0f, 1.0f);
        // We move the camera to where it should be in relation to the full array of screens
        glm::mat4 view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f - (float)((configuration.width + (configuration.l_bezel + configuration.r_bezel)) * configuration.h_index), 0.0f - (float)((configuration.height + (configuration.t_bezel + configuration.b_bezel)) * configuration.v_index), 0.0f});

        shader.SetUniformMat4f("u_MVP", mvp);
        shader.SetUniformMat4f("u_Proj", proj);
        shader.SetUniformMat4f("u_View", view);
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        bool isRunning = true;
        SDL_Event sdl_event;
        /* Loop until the user closes the window */
        while (isRunning)
        {
            /* Render here */
            renderer.Clear();
            shader.Bind();

            renderer.Draw(va, ib, shader);

            /* Swap front and back buffers */
            SDL_GL_SwapWindow(renderer.window);

            /* Poll for and process events */
            while (SDL_PollEvent(&sdl_event) != 0)
            {
                if (sdl_event.type == SDL_QUIT)
                    isRunning = false;
            }
        }
        SDL_DestroyWindow(renderer.window);
        SDL_Quit();
        return 0;
    }
    catch (Configuration::Quit &e)
    {
        return 0;
    }

    catch (INITFAIL &e)
    {
        std::cout << e.what() << std::endl;
    }
}