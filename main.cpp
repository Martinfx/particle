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

/*
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
*/

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
        // update other events like input handling
        glfwPollEvents();
    }

    void checkSwapBuffer()
    {
        // put the stuff we've been drawing onto the display
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

    static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
    {

    }

    //glm::vec3 campos;
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            //    campos =+ m_camera.getSpeed() *  m_camera.getCameraFront();
            //
            //    m_camera.updateCameraPosition(
            //                campos);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {

        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {

        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {

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

    //void setCamera(Camera camera)
    //{
    //    m_camera = camera;
    //}


private:

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

        glfwSetMouseButtonCallback(m_window, mouseCallback);
        glfwSetKeyCallback(m_window, keyboardCallback);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    shader.loadShader("shader.vs", TypeShader::VERTEX_SHADER);
    shader.loadShader("shader.fs", TypeShader::FRAGMENT_SHADER);
    shader.createShaderProgram();


    ParticleSystem pSys(shader,1000);
    pSys.Initialize();

    /*GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shaderVertex, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shaderFragment, NULL);
    glCompileShader(fs);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);*/

    /*  GLint mvp_location, vpos_location, vcol_location, tex_location;
    //mvp_location  = glGetUniformLocation(shader.getShaderProgram(), "MVP");
    vpos_location = glGetAttribLocation(shader.getShaderProgram(), "vPos");
    vcol_locatitinyxml2on = glGetAttribLocation(shader.getShaderProgram(), "vCol");
    tex_location  = glGetAttribLocation(shader.getShaderProgram(), "aTexCoord");

    std::cout << vpos_location << std::endl;
    std::cout << vcol_location << std::endl;
    std::cout << tex_location << std::endl;

    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glCheckError();

     glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vpos_location);
#extension GL_ARB_separate_shader_objects : enable
    glVertexAttribPointer(vcol_locatioprocessInputn, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(vcol_location);

    // texture coord attribute
    glVertexAttribPointer(tex_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(tex_location);

tinyxml2

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    unsigned char *data = stbi_load("box.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    shader.useShaderProgram();

    // Set texture in shader
    shader.setUniformInt("texture1", 0);


    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    // Loop until the user closes the window
    while (!window.checkCloseWindow())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //camera.updateSpeedCamera(deltaTime);

        window.updateFpsCounter();
        window.updateframeBufferSize();
tinyxml2
        // wipe the drawing surface clear
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.5f, 0.7f, 0.6f);
        glCheckError();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glCheckError();
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
        float radius = 6.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        projection = glm::perspective(glm::radians(45.0f),
                                      static_cast<float>(window.getWidthWindow()) /
                                          static_cast<float>(window.getHeightWindow()),
                                      0.1f,
                                      100.0f);
        glCheckError();
         //glm::mat4 transform;
        //transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
        //transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        shader.useShaderProgram();
        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(transform));

        shader.setUniformMatrix4xtinyxml24("projecti#extension GL_ARB_separate_shader_objects : enableon", projection);
        shader.setUniformMatrix4x4("view", view);
        //shader.setUniformMatrix4x4("model", model);

        glBindVertexArray(vao);
        //for(unsigned int i = 0; i < 10; i++)
        //{
            glm::mat4 model2;
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
            //float angle = 20.0f * i;
            model2 = glm::rotate(model2, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniformMatrix4x4("model", model2);

            //glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //}

        // draw points 0-3 from the currently bound vao with current in-use shader
        //glBindVertexArray(vao);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        window.checkPoolEvents();
        window.checkSwapBuffer();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    return 0;
}*/

  /*  unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    */

    // load and create a textureGLWindow
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
        // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    unsigned char *data = stbi_load("box.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader.useShaderProgram(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "sprite"), 0);

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;


    // render loop
    // -----------
    while (!window.checkCloseWindow())
    {
        window.updateFpsCounter();
        window.updateframeBufferSize();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        pSys.Update(deltaTime, 200);
        pSys.Render();

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // render container
        shader.useShaderProgram();
      //  glBindVertexArray(VAO);
      //  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        window.checkSwapBuffer();
        window.checkPoolEvents();

    }

    /*glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    */
    return 0;
}
