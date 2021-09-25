#include <string>
#include "Functions.h"

int main(void)
{
    GLFWwindow* window;
    int height = 1920;
    int width = 1080;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(height, width, "Synthwave Wallpaper App", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
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

    
    float time = .0f, increment = 0.01f;
    float r = .0f;


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        //Setting uniforms
        GLCall(glUniform1f(locationLongerTime, r));
        GLCall(glUniform1f(locationTime, time));
        GLCall(glUniform2f(locationResolution, height, width));
        GLCall(glUniform1f(locationAspectRatio, height/width));

        //Drawing objects
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (time > .1f) time = .0f;
        time += increment;

        //Changing r
        if (r > 100.f) r = .0f;
        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}