#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "particlesystem.h"
#include "shaders.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#include "3rdparty/nuklear.h"


float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};



enum CameraDirection {
    FORWARD_DIRECTIOM = 0,
    BACKWARD_DIRECTION = 1,
    LEFT_DIRECTION = 2,
    RIGHT_DIRECTION = 3
};

class CameraSettings
{
public:

    CameraSettings() : position(0.0f, 0.0f, 10.0f), up(0.0f, 5.0f, 0.0f),
        front(0.0f, 0.0f, -1.0f), cameraSpeed(5.0), fov(45.0), yaw(-90.0),
        pitch(0.0)
    {
    }

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;

    float cameraSpeed;
    float fov;
    float yaw;
    float pitch;
};

class FpsCamera
{
public:

    FpsCamera() : m_position(0.0f, 0.0f, 10.0f), m_up(0.0f, 5.0f,  0.0f),
        m_front(0.0f, 0.0f, -1.0f), m_cameraSpeed(5.0f)
    {
    }

    FpsCamera(CameraSettings &settings) : m_settings(settings)
    {
        m_position = settings.position;
        m_up = settings.up;
        m_front = settings.front;
        m_cameraSpeed = settings.cameraSpeed;
        m_fov = settings.fov;
        m_yaw = settings.yaw;
        m_pitch = settings.pitch;
    }

    ~FpsCamera() { }

    glm::vec3 getCameraPosition()
    {
        return m_position;
    }

    glm::vec3 getCameraFront()
    {
        return m_front;
    }

    glm::vec3 getCameraUp()
    {
        return m_up;
    }

    glm::vec3 getCameraRight()
    {
        return m_right;
    }

    float getSpeed()
    {
        return m_cameraSpeed;
    }

    float getFov()
    {
        return m_fov;
    }

    void setFov(float fov)
    {
        m_fov = fov;
    }

    void setFront(glm::vec3 front)
    {
        m_front = front;
    }

    void setRight(glm::vec3 right)
    {
        m_right = right;
    }

    void setUp(glm::vec3 up)
    {
        m_up = up;
    }

    void updateCameraDirection(CameraDirection direction, float deltaTime)
    {
        float cameraSpeed = m_cameraSpeed * deltaTime;
        if(direction == CameraDirection::FORWARD_DIRECTIOM)
        {
            m_position += cameraSpeed * m_front;
        }

        if(direction == CameraDirection::BACKWARD_DIRECTION)
        {
            m_position -= cameraSpeed * m_front;
        }

        if(direction == CameraDirection::LEFT_DIRECTION)
        {
            /*m_position -= glm::normalize(
                          glm::cross(m_front,
                          m_up)) * cameraSpeed;*/
            m_position -= cameraSpeed * m_right;
        }

        if(direction == CameraDirection::RIGHT_DIRECTION)
        {
            /*m_position += glm::normalize(
                          glm::cross(m_front,
                          m_up)) * cameraSpeed;*/

            m_position += cameraSpeed * m_right;
        }
    }

    void updateMouseCameraDirection(float xoffset, float yoffset/*, GLboolean constrainPitch = true*/)
    {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        computeCamera();
    }

    void updateCameraZoom(double xoffset, double yoffset)
    {
        if (m_fov >= 1.0f && m_fov <= 45.0f)
            m_fov -= yoffset;
        if (m_fov <= 1.0f)
            m_fov = 1.0f;
        if (m_fov >= 45.0f)
            m_fov = 45.0f;
    }

    glm::mat4 getLookAtCamera()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    void computeCamera()
    {
        glm::vec3 front;
        front.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
        front.y = sinf(glm::radians(m_pitch));
        front.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
        m_front = glm::normalize(front);

        /*std::cerr << "front x : " << front.x
                  << " front y : " << front.y
                  << " front z : " << front.z << std::endl;*/

        //m_right = glm::normalize(glm::cross(m_front, m_up));
        //m_up    = glm::normalize(glm::cross(m_right, m_front));
    }

public:

    CameraSettings m_settings;

private:

    glm::vec3 m_position;
    glm::vec3 m_up;
    glm::vec3 m_front;
    glm::vec3 m_right;

    float m_cameraSpeed;
    float m_fov;
    float m_yaw;
    float m_pitch;
};

CameraSettings settings;
FpsCamera g_camera(settings);

bool firstMouse = true;
double lastX =  800.0 / 2.0;
double lastY =  600.0 / 2.0;

class GLSettings
{
public:


    GLSettings() : majorVersion(3), minorVersion(3),
        windowHeight(640), windowWidth(480), windowName("Hello") { }

    // Major version OpenGL, Minor version OpenGL
    int majorVersion, minorVersion;

    // Window width, height,
    int windowHeight, windowWidth;

    // Main window name
    std::string windowName;
};

class GLWindow
{
public:

    GLWindow(GLSettings &settings) : m_settings(settings)
    {

        if(!glfwInit())
        {
            std::cout << "Failed initilize GLFW library!" << std::endl;
            glfwTerminate();
        }

        glfwSetErrorCallback(errorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_settings.majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_settings.minorVersion);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        createWindow(m_settings.windowHeight,
                     m_settings.windowWidth,
                     m_settings.windowName);
    }

    ~GLWindow()
    {
        glfwTerminate();
    }

    int getWidthWindow()
    {
        return m_settings.windowWidth;
    }

    int getHeightWindow()
    {
        return m_settings.windowHeight;
    }

    int checkCloseWindow()
    {
        return glfwWindowShouldClose(m_window);
    }

    void checkPoolEvents()
    {
        glfwPollEvents();
    }

    void checkSwapBuffer()
    {
        glfwSwapBuffers(m_window);
    }

    void updateFpsCounter()
    {
        static double prevSecond = glfwGetTime();
        static int frameCounter = 0;

        double currentSecond = glfwGetTime();
        double elapsedSecond = currentSecond - prevSecond;

        if(elapsedSecond > 0.25)
        {
            prevSecond = currentSecond;
            double fps = static_cast<double>(frameCounter) / elapsedSecond;

            char buffer[256];
            sprintf(buffer, "fps: %.2f", fps);
            glfwSetWindowTitle(m_window, buffer);
            frameCounter = 0;
        }

        frameCounter++;
    }

    static void errorCallback(int error, const char* description)
    {
        fprintf(stderr, "Error callback: %s\n", description);
    }

    /*static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {

    }*/

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        g_camera.updateCameraZoom(xoffset, yoffset);
    }

    static void mouseCursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        if (firstMouse)
        {
            lastX = x;
            lastY = y;
            firstMouse = false;
        }

        float xoffset = x - lastX;
        float yoffset = lastY - y;

        lastX = x;
        lastY = y;

        g_camera.updateMouseCameraDirection(xoffset, yoffset);
    }

    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void getFramebufferSize(int *width, int *height)
    {
        glfwGetFramebufferSize(m_window, width, height);
    }

    void updateframeBufferSize()
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        glViewport(0, 0, width, height);
    }

    void updateCamera(float deltaTime)
    {
        if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            g_camera.updateCameraDirection(CameraDirection::FORWARD_DIRECTIOM,
                                           deltaTime);
        }

        if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            g_camera.updateCameraDirection(CameraDirection::BACKWARD_DIRECTION,
                                           deltaTime);
        }

        if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            g_camera.updateCameraDirection(CameraDirection::LEFT_DIRECTION,
                                           deltaTime);
        }

        if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            g_camera.updateCameraDirection(CameraDirection::RIGHT_DIRECTION,
                                           deltaTime);
        }
    }

protected:

    void createWindow(int width, int height, std::string name)
    {
        m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        if(!m_window)
        {
            std::cout << "Failed create GLFW window!" << std::endl;
            glfwTerminate();
        }

        // Make the window's context current
        glfwMakeContextCurrent(m_window);

        //glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetKeyCallback(m_window, keyboardCallback);
        glfwSetCursorPosCallback(m_window, mouseCursorPositionCallback);
        glfwSetScrollCallback(m_window, scrollCallback);

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // start GLEW extension handler
        glewExperimental = GL_TRUE;
        glewInit();
    }

private:

    GLSettings m_settings;
    GLFWwindow *m_window;

};

class Texture
{
public:

    Texture() {}
    ~Texture() {}

    void glEnableGlBlend() {
         glEnable(GL_BLEND);
    }

    unsigned int loadTexture(const std::string &name)
    {
        glGenTextures(1, &m_id);
        int width, height, nrChannels;
        unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
        if(data)
        {
            GLenum format = 0;
            if (nrChannels == 1)
            {
                format = GL_RED;
            }
            else if (nrChannels == 3)
            {
                format = GL_RGB;
            }
            else if (nrChannels == 4)
            {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, m_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            std::cerr << "Load texture: " << name << std::endl;
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Failed to load texture: " << name << std::endl;
            stbi_image_free(data);
        }

        return m_id;
    }

    unsigned int loadCubeTexture(std::vector<std::string> &faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if(data)
            {
                GLenum format = 0;
                if (nrChannels == 1)
                {
                    format = GL_RED;
                }
                else if (nrChannels == 3)
                {
                    format = GL_RGB;
                }
                else if (nrChannels == 4)
                {
                    format = GL_RGBA;
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }


private:

    unsigned int m_id;
};


int main(void)
{
    GLSettings settings;
    settings.windowName = "Hello OpenGL";
    settings.windowHeight = 800;
    settings.windowWidth = 600;

    GLWindow window(settings);

    Shader shader;
    shader.loadShader(shaderVertex, TypeShader::VERTEX_SHADER);
    shader.loadShader(shaderFragment, TypeShader::FRAGMENT_SHADER);
    shader.createShaderProgram();

    Texture texture;

    ParticleSystem pSys(shader,200);
    pSys.Initialize();
    //pSys.AddParticles(10,glm::vec2(0.1f,0.1f),glm::vec2(1,1),10,100,glm::vec2(0,0));

   /* ParticleSystem pSys2(shader, 2);
    pSys2.Initialize();
    pSys2.AddParticles(10,glm::vec2(0.1f,0.1f),glm::vec2(10,10),10,100,glm::vec2(0,0));
    */

    texture.loadTexture("smoke-particle-texture-399x385.png");
    texture.glEnableGlBlend();
    shader.useShaderProgram();
    glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "sprite"), 0);

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;


    // render loop
    // -----------
    while (!window.checkCloseWindow())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        window.updateFpsCounter();
        window.updateframeBufferSize();
        window.updateCamera(deltaTime);


        // render
        // ------
        glClearColor(0.2f, 0.5f, 0.7f, 0.6f);
        glClear(GL_COLOR_BUFFER_BIT);
        // bind textures on corresponding texture units


        pSys.Update(deltaTime, 100);
        //pSys2.Update(deltaTime, 10);

        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);

        // render container
        shader.useShaderProgram();

        /*glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        trans = glm::translate(trans, glm::vec3(0.0,0.0,0.0));
        unsigned int transformLoc = glGetUniformLocation(shader.getShaderProgram(), "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        */
        projection = glm::perspective(glm::radians(g_camera.getFov()),
                                      static_cast<float>(window.getWidthWindow()) /
                                          static_cast<float>(window.getHeightWindow()),
                                      0.1f,
                                      100.0f);
        shader.setUniformMatrix4x4("projection", projection);

        view = g_camera.getLookAtCamera();
        shader.setUniformMatrix4x4("view", view);

        model = glm::mat4(1.0f);
        shader.setUniformMatrix4x4("model", model);

        pSys.Render();
        //pSys2.Render();

        window.checkSwapBuffer();
        window.checkPoolEvents();
    }


    return 0;
}
