#include <string>
#include "vendor/TheCherno/Functions.h"
#include "vendor/stb_image/stb_image.h"

int main(void)
{
    GLFWwindow* window;
    int height = 1920, width = 1080;

    //Initialize the library
    if (!glfwInit()) return -1;

    //Setting it to the modern core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(height, width, "Synthrider Wallpaper App", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) std::cout << "Error" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    quad background = {
        .vertices={
            -1.0f, -1.0,    //1
             1.0f, -1.0f,   //2
             1.0f,  1.0f,   //3
            -1.0f,  1.0f,   //4
        },
        .indices={
             0,1,2,
             2,3,0
         }
    };

    //Creating vertex array
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    //Creating vertex buffer
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(background.vertices), background.vertices, GL_STATIC_DRAW));

    //Specifying layout (in) of vertex data
    char vertexBigness = 2;
    GLCall(glVertexAttribPointer(0, vertexBigness, GL_FLOAT, GL_FALSE, sizeof(background.vertices[0]) * vertexBigness, 0));
    GLCall(glEnableVertexAttribArray(0));

    //Creating index buffer
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(background.indices), background.indices, GL_STATIC_DRAW));

    //Binding Shaders
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    //Checking if uniforms exist in shader code
    GLCall(int locationLongerTime = glGetUniformLocation(shader, "u_LongerTime"));
    ASSERT(locationLongerTime != -1);
    GLCall(int locationTime = glGetUniformLocation(shader, "u_Time"));
    ASSERT(locationTime != -1);
    GLCall(int locationResolution = glGetUniformLocation(shader, "u_Resolution"));
    ASSERT(locationResolution != -1);
    GLCall(int locationAspectRatio = glGetUniformLocation(shader, "u_AspectRatio"));
    ASSERT(locationAspectRatio != -1);

    //Loading texture file
    int t_width, t_height, t_nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/textures/galaxy.jpg", &t_width, &t_height, &t_nrChannels, 0);
    if (data == nullptr) std::cout << "Error while loading image";
    
    //Passing texture data to the fragment shader
    unsigned int texture;
    GLCall(glGenTextures(1, &texture));
    GLCall(glActiveTexture(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, texture));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(data);

    float time = .0f, longerTime = time, increment = 0.01f;

    //Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        //Setting uniforms
        GLCall(glUniform1f(locationLongerTime, longerTime));
        GLCall(glUniform1f(locationTime, time));
        GLCall(glUniform2f(locationResolution, height, width));
        GLCall(glUniform1f(locationAspectRatio, height/width));

        //Binding texture and drawing objects
        GLCall(glActiveTexture(0));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        //Changing time and longerTime over time
        if (time > .1f) time = .0f;
        if (longerTime > 100.f) longerTime = .0f;
        time += increment;
        longerTime += increment;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}