#pragma once

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

/*const char *shaderVertex =
    "#version 330 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    ""
    "void main(){\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    ourColor = aColor;\n"
    "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\n";
*/

const char *shaderVertex =
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "//#extension GL_ARB_separate_shader_objects : enable\n"
    "out vec2 TexCoords;\n"
    "out vec4 ParticleColor;\n"
    "uniform mat4 projection;\n"
    "uniform vec2 offset;\n"
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "    float scale = 10.0;\n"
    "    TexCoords = vertex.zw;\n"
    "    ParticleColor = color;\n"
    "    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);\n"
    "}\n";
/*
const char* shaderFragment =
    "#version 330 core\n"
    "out vec4 FragColor\n;"
    "in vec3 ourColor\n;"
    "in vec2 TexCoord\n;"
    "uniform sampler2D texture1\n;"
    "void main()  {\n"
    "    FragColor = texture(texture1, TexCoord)\n;"
    "}\n;";
*/

const char *shaderFragment =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"in vec4 ParticleColor;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D sprite;\n"
"\n"
"void main()\n"
"{\n"
"    color = (texture(sprite, TexCoords) * ParticleColor);\n"
"}\n";


const char *shaderGeometry =
    "\n";


enum TypeShader
{
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER
};

class Shader
{
public:

    Shader() : m_vertexShader(0), m_fragmentShader(0),
        m_geometryShader(0), m_isVertexShader(false),
        m_isFragmentShader(false), m_isGeometryShader(false), m_id(0)
    { }

    ~Shader() {}

    void loadShader(const GLchar *shader, TypeShader type)
    {
        if(type == TypeShader::VERTEX_SHADER)
        {

            m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
            //const GLchar *c_str = shader.c_str();
            glShaderSource(m_vertexShader, 1, &shaderVertex, NULL);
            glCompileShader(m_vertexShader);
            shaderCompileStatus(m_vertexShader, __FILE__ , __LINE__);
            m_isVertexShader = true;
        }

        if(type == TypeShader::FRAGMENT_SHADER)
        {
            m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            //const char *c_str = shader.c_str();
            glShaderSource(m_fragmentShader, 1, &shaderFragment, NULL);
            glCompileShader(m_fragmentShader);
            shaderCompileStatus(m_fragmentShader, __FILE__ , __LINE__);
            m_isFragmentShader = true;
        }

        if(type == TypeShader::GEOMETRY_SHADER)
        {
            m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryShader, 1, &shaderGeometry, NULL);
            glCompileShader(m_geometryShader);
            m_isGeometryShader = true;
        }
    }

    void useShaderProgram()
    {
        glUseProgram(m_id);
    }

    GLuint getShaderProgram()
    {
        return m_id;
    }

    void createShaderProgram()
    {
        m_id = glCreateProgram();

        if(m_isVertexShader)
        {
            glAttachShader(m_id, m_vertexShader);
        }
        if(m_isFragmentShader)
        {
            glAttachShader(m_id, m_fragmentShader);
        }
        if(m_isGeometryShader)
        {
            glAttachShader(m_id, m_geometryShader);
        }


        glLinkProgram(m_id);
        programCompileStatus(m_id, __FILE__ , __LINE__);
    }

    /*const char *getShaderReader(const std::string &shader)
    {
        std::string line;
        std::string source;
        std::ifstream file(shader);
        if(file.is_open())
        {
            while(std::getline(file, line))
            {
                std::cerr << line << std::endl;
                source = source + line; + "\n";
            }

            file.close();
        }
        else
        {
            std::cerr << "Cannot open file! " << shader << std::endl;
        }

        std::cerr << source << std::endl;

        return source.c_str();
     }*/


    void setUniformMatrix4x(const std::string &type, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, type.c_str()), 1, GL_FALSE, &matrix[0][0]);
    }

    void setUniformInt(const std::string &type, const GLint value)
    {
        glUniform1i(glGetUniformLocation(m_id, type.c_str()), value);
    }

    void setUniformVec2(const std::string &type, const glm::vec2 &value) {
        glUniform2f(glGetUniformLocation(m_id, type.c_str()), value.x, value.y);
    }

    void setUniformVec4(const std::string &type, const glm::vec4 &value) {
        glUniform4f(glGetUniformLocation(m_id, type.c_str()), value.x, value.y, value.z, value.w);
    }

protected:

    void shaderCompileStatus(GLuint shader, std::string file, int line)
    {
        GLint isCompiled;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if(!isCompiled)
        {
            int logLenght;
            GLchar log[1024];
            glGetShaderInfoLog(shader, 1024, &logLenght, log);
            std::cerr << "[WARN] Shader compilation error : "  << log <<
                " - Log lenght: " << logLenght <<
                " - File: " << file <<
                " - Line: " << line <<
                "\n";
        }
        else
        {
            int logLenght;    // optional: de-allocate all resources once they've outlived their purpose:
            // ------------------------------------------------------------------------
            GLchar log[1024];
            glGetShaderInfoLog(shader, 1024, &logLenght, log);
            std::cerr << "[INFO] Shader compilation success ! " << log <<
                " - Log lenght: " << logLenght <<
                " - File: " << file <<
                " - Line: " << line <<
                "\n";
        }
    }

    void programCompileStatus(GLuint program, std::string file, int line)
    {
        GLint isCompiled;

        glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);
        if(!isCompiled)
        {
            int logLenght;
            GLchar log[1024];
            glGetProgramInfoLog(program, 1024, &logLenght, log);
            std::cerr << "[WARN] Program linker error : "  << log <<
                " - Log lenght: " << logLenght <<
                " - File: " << file <<
                " - Line: " << line <<
                "\n";
        }
        else
        {
            int logLenght;
            GLchar log[1024];
            glGetProgramInfoLog(program, 1024, &logLenght, log);
            std::cerr << "[INFO] Program linker success ! " << log <<
                " - Log lenght: " << logLenght <<
                " - File: " << file <<
                " - Line: " << line <<
                "\n";
        }
    }

private:

    // Shader program idvertices
    GLuint m_id;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_geometryShader;

    bool m_isVertexShader;
    bool m_isFragmentShader;
    bool m_isGeometryShader;
};


