#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "shaders.hpp"
#include "glerror.hpp"

class Particle {
public:

    Particle():m_position(0.0f),
        m_velocity(0.0f),
        m_color(1.0),
        m_life(0.0f),
        m_rotate(0.0f){}

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec4 m_color;
    float m_life;
    float m_rotate;
};

class ParticleSystem
{
public:
    ParticleSystem() {}
    ParticleSystem(Shader shader, uint32_t amount);
    ~ParticleSystem() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }

    void Initialize();
    void Render();
    void Update(float dt, unsigned int newParticles, glm::vec3 offset);
    void AddParticles(short int type, glm::vec3 position, glm::vec3 velocity, float rotation, unsigned int newParticles, glm::vec3 offset);
private:

    std::vector<Particle> m_particles;
    unsigned int m_amount;
    unsigned int m_VBO, m_VAO;
    unsigned int lastUsedParticle = 0;
    Shader m_shader;

    unsigned int firstUnusedParticle();
     void respawnParticle(Particle &particle, short type, glm::vec3 position, glm::vec3 velocity, float rotation, glm::vec3 offset = glm::vec3(0.0f, 0.0f,0.0f));
};


ParticleSystem::ParticleSystem(Shader shader, uint32_t amount) :
    m_shader(shader), m_amount(amount){
}

void ParticleSystem::Initialize() {
    // set up mesh and attribute properties
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    // fill mesh buffet
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    // create default particle instances
    for (unsigned int i = 0; i < m_amount; ++i)
        m_particles.push_back(Particle());

}


void ParticleSystem::Render(){
    // use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_shader.useShaderProgram();
    for (Particle particle : m_particles)
    {
        if (particle.m_life > 0.0f)
        {
            //m_shader.setUniformVec2("offset", particle.m_position);
            m_shader.setUniformVec3("offset", particle.m_position);
            m_shader.setUniformVec4("color", particle.m_color);
            //m_shader.setUniform("rotation", particle.m_rotate);
            //this->texture.Bind();
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::Update(float dt, unsigned int newParticles, glm::vec3 offset = glm::vec3(1.0f, 2.0f,3.0f)){

/*    for (unsigned int i = 0; i < newParticles; i++)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(m_particles[unusedParticle], 0, glm::vec3(1,1,1), glm::vec3(1,1,1), 60, offset);
    }*/


    for (int i = 0; i < newParticles; ++i) {
        this->m_particles[i].m_life  -= dt;

        // if the lifetime is below 0 respawn the particle
        if ( this->m_particles[i].m_life <= 0.0f )
        {
            this->m_particles[i].m_position = glm::vec3( 0,rand() %50,0);
            this->m_particles[i].m_life = rand() % 10;
        }

        // move the particle down depending on the delta time
        this->m_particles[i].m_position -=  glm::vec3( 0.0f , dt*2.0f , 0.0f );

     //    lastUsedParticle = i;


    }

    // update all particles
    for (unsigned int i = 0; i < this->m_amount; ++i)
    {
        Particle &p = this->m_particles[i];
        p.m_life -= dt; // reduce life
        if (p.m_life > 0.0f)
        {	// particle is alive, thus update
            p.m_position -= p.m_velocity * dt * 2.f;
            p.m_color.a -= dt * 1.f;
        }
    }
}

void ParticleSystem::AddParticles(short int type, glm::vec3 position, glm::vec3 velocity, float rotation, unsigned int newParticles, glm::vec3 offset) {
    // add new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(m_particles[unusedParticle], type, position, velocity, rotation, offset);
    }
}

unsigned int ParticleSystem::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < m_amount; ++i){
        if (this->m_particles[i].m_life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->m_particles[i].m_life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleSystem::respawnParticle(Particle& particle, short int type, glm::vec3 position, glm::vec3 velocity, float rotation, glm::vec3 offset){
    float random = ((rand() % 100) - 50) / 10.0f;
    float random2 = ((rand() % 20) - 20) / 2.0f;
    float random3 = ((rand() % 10) - 10) / 1.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
   // particle.m_position = glm::vec3(random, 1, 1) * random3;
   //particle.m_color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.m_life = 1.f;
   // particle.m_rotate = rotation;*/
    particle.m_velocity = glm::vec3(0.01f,0.01f, 0.01f) ;
}

