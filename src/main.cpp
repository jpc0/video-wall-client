#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "yaml.h"
#include <boost/program_options.hpp>

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

int main(int argc, char *argv[])
{

    std::string config_path;
    // clang-format off
    boost::program_options::options_description generic("Generic options");
    generic.add_options()
        ("help", "Show help message")
        ("config, c", boost::program_options::value<std::string>(&config_path)->default_value("../../config.cfg"), 
                                    "config file location");
    // clang-format on
    int _l_bezel;
    int _r_bezel;
    int _t_bezel;
    int _b_bezel;
    int _h_index;
    int _v_index;
    int _h_screens;
    int _v_screens;
    int _width;
    int _height;
    std::string _image_location;
    // clang-format off
    boost::program_options::options_description config("Configuration");
    config.add_options()
        ("l_bezel", boost::program_options::value<int>(&_l_bezel)->default_value(0), "left bezel in px")
        ("r_bezel", boost::program_options::value<int>(&_r_bezel)->default_value(0), "right bezel in px")
        ("t_bezel", boost::program_options::value<int>(&_t_bezel)->default_value(0), "top bezel in px")
        ("b_bezel", boost::program_options::value<int>(&_b_bezel)->default_value(0), "bottom bezel in px")
        ("h_index", boost::program_options::value<int>(&_h_index)->default_value(0), "horizontal index of the screen, 0 based from bottom left screen")
        ("v_index", boost::program_options::value<int>(&_v_index)->default_value(0), "verticle index of the screen, 0 based from bottom left screen")
        ("h_screens", boost::program_options::value<int>(&_h_screens)->default_value(0), "Number of screens horizontally")
        ("v_screens", boost::program_options::value<int>(&_v_screens)->default_value(0), "Number of screens vertically")
        ("width", boost::program_options::value<int>(&_width)->default_value(1280), "width of each screen in px")
        ("height", boost::program_options::value<int>(&_height)->default_value(720), "height of each screen in px")
        ("image_location", boost::program_options::value<std::string>(&_image_location)->default_value(""), "location of default image")
    ;

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(config);

    boost::program_options::options_description config_file_options;
    config_file_options.add(config);

    boost::program_options::options_description visible("Allowed options");
        visible.add(generic).add(config);

    // clang-format on

    boost::program_options::variables_map args;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).run(), args);
    boost::program_options::notify(args);

    std::ifstream ifs(config_path.c_str());
    if (!ifs)
    {
        std::cout << "Can not open config file: " << config_path << "\n";
        return 0;
    }
    else
    {
        boost::program_options::store(boost::program_options::parse_config_file(ifs, config_file_options), args);
        boost::program_options::notify(args);
    }

    if (args.count("help"))
    {
        std::cout << visible << std::endl;
        return 0;
    }
    // // TODO: Put this in a Yaml file
    // int _h_bezel = config["h_bezel"].as<int>(); // in pixels
    // int _v_bezel = config["v_bezel"].as<int>(); // in pixels
    // int _h_index = config["h_index"].as<int>(); // zero base, from bottom left
    // int _v_index = config["v_index"].as<int>(); // zero base, from bottom left
    // int _h_screens = config["h_screens"].as<int>();
    // int _v_screens = config["v_screens"].as<int>();
    // int _width = config["width"].as<int>();   // in pixels
    // int _height = config["height"].as<int>(); // in pixels
    // std::string _image_location{config["height"].as<std::string>()};

    // int _h_bezel = 0;
    // int _v_bezel = 0;
    // int _h_index = 0;
    // int _v_index = 0;
    // int _h_screens = 1;
    // int _v_screens = 1;
    // int _width = 1280;
    // int _height = 720;
    // std::string _image_location{"../../res/textures/Image_created_with_a_mobile_phone.png"};

    // std::cout
    //     << "This is what was parsed from config: " << std::endl;
    // std::cout << _h_bezel << std::endl;
    // std::cout << _v_bezel << std::endl;
    // std::cout << _h_index << std::endl;
    // std::cout << _v_index << std::endl;
    // std::cout << _h_screens << std::endl;
    // std::cout << _v_screens << std::endl;
    // std::cout << _width << std::endl;
    // std::cout << _height << std::endl;
    // std::cout << _image_location << std::endl;

    int _total_width = (_width + _l_bezel + _r_bezel) * _h_screens;
    int _total_height = (_height + _t_bezel + _b_bezel) * _v_screens;
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(_width, _height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    // Set alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    // Load Texture
    Texture texture(_image_location);

    // Calculate the scale factor of the image, this will blow the image up to the size of the full array of screens
    // and will correct for aspect ratio
    float scale_factor;
    if (((float)_total_width / (float)_total_height) > ((float)texture.GetWidth() / (float)texture.GetHeight()))
        scale_factor = (float)_total_height / (float)texture.GetHeight();
    else if (((float)_total_width / (float)_total_height) == ((float)texture.GetWidth() / (float)texture.GetHeight()))
        scale_factor = (float)_total_height / (float)texture.GetHeight();
    else if (((float)_total_width / (float)_total_height) < ((float)texture.GetWidth() / (float)texture.GetHeight()))
        scale_factor = (float)_total_width / (float)texture.GetWidth();

    // This is where the bottom left of the image will be, if the aspect ratio matches it will be 0,0
    float x_origin = ((_total_width - (float)texture.GetWidth() * (float)scale_factor) / 2);
    float y_origin = ((_total_height - (float)texture.GetHeight() * (float)scale_factor) / 2);

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
    GLCall(texture.Bind());
    shader.SetUniform1i("u_Texture", 0);

    // proj is the size of the local screen
    glm::mat4 proj = glm::ortho(0.0f, (float)_width, 0.0f, (float)_height, -1.0f, 1.0f);
    // We move the camera to where it should be in relation to the full array of screens
    glm::mat4 view = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f - (float)((_width + (_l_bezel + _r_bezel)) * _h_index), 0.0f - (float)((_height + (_t_bezel + _b_bezel)) * _v_index), 0.0f});

    glm::mat4 mvp = proj * view;

    shader.SetUniformMat4f("u_MVP", mvp);
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();

    Renderer renderer;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        shader.Bind();

        renderer.Draw(va, ib, shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}