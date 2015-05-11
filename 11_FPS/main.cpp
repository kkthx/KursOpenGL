#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
//******************************************************************************
int window_width;
int window_height;
GLuint shaders;
GLFWwindow* window;
//******************************************************************************
GLint LoadShaders(std::string vertex_shader, std::string fragment_shader)
{
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_data;
    std::ifstream vertex_shader_file(vertex_shader.c_str(), std::ios::in);
    if (vertex_shader_file.is_open())
    {
        std::string line;
        while (std::getline(vertex_shader_file, line))
            vertex_shader_data += "\n" + line;

        vertex_shader_file.close();
    }

    std::string fragment_shader_data;
    std::ifstream fragment_shader_file(fragment_shader.c_str(), std::ios::in);
    if (fragment_shader_file.is_open())
    {
        std::string line;
        while (std::getline(fragment_shader_file, line))
            fragment_shader_data += "\n" + line;

        fragment_shader_file.close();
   }

   const char* vertex_ptr = vertex_shader_data.c_str();
   const char* fragment_ptr = fragment_shader_data.c_str();
   glShaderSource(vertex_shader_id, 1, &vertex_ptr, NULL);
   glShaderSource(fragment_shader_id, 1, &fragment_ptr, NULL);

   glCompileShader(vertex_shader_id);
   glCompileShader(fragment_shader_id);

   GLuint shader_programme = glCreateProgram();
   glAttachShader(shader_programme, vertex_shader_id);
   glAttachShader(shader_programme, fragment_shader_id);
   glLinkProgram(shader_programme);

   glDeleteShader(vertex_shader_id);
   glDeleteShader(fragment_shader_id);

   return shader_programme;
}
//******************************************************************************
static void ErrorCallback(int /*error*/, const char* description)
{
    std::cout << "GLFW Error: " << description << std::endl;
}
//******************************************************************************
static void KeyCallback(GLFWwindow* window, int key, int /*scancode*/,
                        int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
//******************************************************************************
static void WindowSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    window_width = width;
    window_height = height;
}
//******************************************************************************
int CreateWindow(int width, int height, std::string name, int samples,
                 bool fullscreen)
{
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, samples);

    if (fullscreen)
    {
        const GLFWvidmode* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window_width = video_mode->width;
        window_height = video_mode->height;
        window = glfwCreateWindow(window_width, window_height, name.c_str(),
                                  glfwGetPrimaryMonitor(), NULL);
    }
    else
    {
        window_width = width;
        window_height = height;
        window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    }

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        return -1;

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    return 0;
}
//******************************************************************************
void EnableDepthTesting()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}
//******************************************************************************
void ProcessWindowEvents()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}
//******************************************************************************
void Terminate()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
//******************************************************************************
bool RenderingEnabled()
{
    if (glfwWindowShouldClose(window))
        return false;

    return true;
}
//******************************************************************************
void ClearColor(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);
}
//******************************************************************************
void FPSCounter(double& fps)
{
    static double prev_time = glfwGetTime();
    double actual_time = glfwGetTime();

    static int frames_counter = 0;

    double elapsed_time = actual_time - prev_time;
    if (elapsed_time >= 1.0)
    {
        prev_time = actual_time;

        fps = static_cast<double>(frames_counter) / elapsed_time;

        frames_counter = 0;
    }

    frames_counter++;
}
//******************************************************************************
int main()
{
    int result = CreateWindow(640, 480, "GL Window", 4, false);
    if (result)
        return -1;

    GLfloat points[] = {
            -0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
        };

    GLfloat colours[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };

    GLuint position_vbo = 0;
    glGenBuffers(1, &position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint colour_vbo = 0;
    glGenBuffers(1, &colour_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colour_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colour_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    EnableDepthTesting();

    glm::mat4 translate_matrix;
    glm::mat4 rotate_matrix;

    const float radius = 0.5;
    float x_trans = 0.0;
    float y_trans = 0.0;
    float angle = 0;

    GLuint shaders = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");

    GLint trans_uniform = glGetUniformLocation(shaders, "trans_matrix");
    if(trans_uniform == -1)
        std::cout << "Variable 'trans_matrix' not found." << std::endl;

    glUseProgram(shaders);

    while (RenderingEnabled())
    {
        static double fps = 0;
        FPSCounter(fps);
        std::string title = "Game @ FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window, title.c_str());

        ClearColor(0.5, 0.5, 0.5);

        static double prev_time = glfwGetTime();
        double actual_time = glfwGetTime();
        double elapsed_time = actual_time - prev_time;

        if (elapsed_time > 0.01)
        {
           prev_time = actual_time;
           angle += 1;

           x_trans = radius * cos(angle * M_PI / 180.0);
           y_trans = radius * sin(angle * M_PI / 180.0);
        }

        translate_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x_trans, y_trans, 0.0));
        rotate_matrix = glm::rotate(translate_matrix, angle, glm::vec3(0, 1, 0));

        glUniformMatrix4fv(trans_uniform, 1, GL_FALSE, glm::value_ptr(rotate_matrix));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ProcessWindowEvents();
    }

    Terminate();

    return 0;
}
