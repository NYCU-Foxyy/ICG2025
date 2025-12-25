#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    void add_shader(std::string& filepath, unsigned int type);
    void link_shader();
    void create();
    void use();
    void release();
    void set_uniform_value(const char* name, const glm::mat4& mat);
    void set_uniform_value(const char* name, const glm::mat3& mat);
    void set_uniform_value(const char* name, const glm::vec3& vec);
    void set_uniform_value(const char* name, const float value);
    void set_uniform_value(const char* name, const int value);
    unsigned int get_program_id() const;
    
private:
    unsigned int program_handle;
    std::vector<unsigned int> shader_handles;
};
