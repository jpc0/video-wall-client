#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader(const std::string &filepath) : _FilePath{filepath}, _RendererID{0}
{
    ShaderProgramSource source = ParseShaderFromFile(filepath);
    _RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::Shader(const std::string &raw_string, const bool &raw) : _FilePath{""}, _RendererID{0}
{
    ShaderProgramSource source = ParseShaderFromString(raw_string);
    _RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(_RendererID);
}

uint32_t Shader::CompileShader(uint32_t type, const std::string &source)
{
    uint32_t id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
uint32_t Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    uint32_t program = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
ShaderProgramSource Shader::ParseShaderFromString(const std::string &raw_string)
{
    std::stringstream stream(raw_string);
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    // std::cout << ss[0].str() << " " << ss[1].str() << std::endl;
    return {ss[0].str(), ss[1].str()};
}

ShaderProgramSource Shader::ParseShaderFromFile(const std::string &filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    // std::cout << ss[0].str() << " " << ss[1].str() << std::endl;
    return {ss[0].str(), ss[1].str()};
}

void Shader::Bind() const
{
    glUseProgram(_RendererID);
}
void Shader::Unbind() const
{
    glUseProgram(0);
}

// Set uniforms
void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniform1f(const std::string &name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform1i(const std::string &name, int32_t value)
{
    glUniform1i(GetUniformLocation(name), value);
}

int32_t Shader::GetUniformLocation(const std::string &name)
{
    if (_UniformLocationCache.find(name) != _UniformLocationCache.end())
        return _UniformLocationCache[name];

    int32_t location = glGetUniformLocation(_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' does't exist" << std::endl;

    _UniformLocationCache[name] = location;
    return location;
}