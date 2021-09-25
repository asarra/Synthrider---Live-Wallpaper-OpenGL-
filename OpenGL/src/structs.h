#include <iostream>

struct quad {
    float vertices[8];
    unsigned int indices[6];
};

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};