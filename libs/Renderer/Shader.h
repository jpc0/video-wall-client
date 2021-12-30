#pragma once
#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
public:
    Shader(const std::string &filepath);
    Shader(const std::string &raw_string, const bool &raw);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1i(const std::string &name, int32_t i0);
    void SetUniform1f(const std::string &name, float f0);
    void SetUniform4f(const std::string &name, float f0, float f1, float f2, float f3);

private:
    uint32_t _RendererID;
    std::string _FilePath;
    std::unordered_map<std::string, int32_t> _UniformLocationCache;
    ShaderProgramSource ParseShaderFromFile(const std::string &filepath);
    ShaderProgramSource ParseShaderFromString(const std::string &raw_string);
    int32_t GetUniformLocation(const std::string &name);
    uint32_t CompileShader(uint32_t type, const std::string &source);
    uint32_t CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
};