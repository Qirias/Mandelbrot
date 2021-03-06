#define GL_GLEXT_PROTOTYPES
#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include "includes/glm/gtc/type_ptr.hpp"
#include "includes/shader.h"
#include <GLFW/glfw3.h>
#include <iomanip>
#include <iostream>

const unsigned int SCREEN_WIDTH  = 1024;
const unsigned int SCREEN_HEIGHT = 1024;
void processInput(GLFWwindow *window, float deltaTime);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

float cx = -0.5f, cy, cz = 1.0f, ci = 100.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool showQuad = true;

int main(void)
{
    GLFWwindow *window;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // clang-format off
    float quadVertices[] = {
        //positions   texture Coords
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    // clang-format on

    // screen quad
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // create texture for compute shader
    int tex_w = SCREEN_WIDTH, tex_h = SCREEN_HEIGHT;
    unsigned int tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[ 3 ];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[ 0 ]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[ 1 ]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[ 2 ]);
    printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_cnt[ 0 ], work_grp_cnt[ 1 ],
           work_grp_cnt[ 2 ]);

    int work_grp_size[ 3 ];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[ 0 ]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[ 1 ]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[ 2 ]);
    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n", work_grp_size[ 0 ], work_grp_size[ 1 ],
           work_grp_size[ 2 ]);

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    // Quad's Fragment and Vertex shader
    ShaderProgramSource basic = ParseShader("includes/Basic.glsl");
    unsigned int quad         = CreateShader(basic.VertexShader, basic.FragmentShader);

    // Compute Shader
    const std::string &compShader = ParseCompute("includes/computeShader.glsl");
    unsigned int mandelbrot       = CreateCompute(compShader);

    double lastTime      = 0.0;
    unsigned int counter = 0;

    // Game loop
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;
        processInput(window, deltaTime);

        useShader(mandelbrot);

        setFloat(mandelbrot, "c_x", cx);
        setFloat(mandelbrot, "c_y", cy);
        setFloat(mandelbrot, "c_z", cz);
        setFloat(mandelbrot, "c_i", ci);

        // Number of groups in dispach: X, Y, Z
        glDispatchCompute(SCREEN_WIDTH / 32, SCREEN_HEIGHT / 32, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        useShader(quad);

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_output);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ++counter;
        double currentTime = glfwGetTime();

        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS:\t" << counter << std::endl;
            ++lastTime;
            counter = 0;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, float dT)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Window close
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cx -= 0.5 * dT * cz;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cx += 0.5 * dT * cz;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cy += 0.5 * dT * cz;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cy -= 0.5 * dT * cz;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cz -= 0.4 * dT * cz;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cz += 0.8 * dT * cz;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
        ci += 10;
        std::cout << "iterations:\t" << ci <<std::endl;
    }

    if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        ci -= 10;
        std::cout << "iterations:\t" << ci <<std::endl;
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (showQuad)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        showQuad = !showQuad;
    }
}
// g++ main.cpp -lGL -lglfw && ./a.out