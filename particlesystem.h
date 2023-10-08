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

    glm::vec2 m_position;
    glm::vec2 m_velocity;
    glm::vec4 m_color;
    float m_life;
    float m_rotate;
};


class ParticleSystem {
public:
    ParticleSystem(Shader shader, unsigned int amount);
    ParticleSystem();
    ~ParticleSystem() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    void Initialize();
    void Render();
    void Update(float dt, unsigned int newParticles, glm::vec2 offset);
    void AddParticles(short int type, glm::vec2 position, glm::vec2 velocity, float rotation, unsigned int newParticles, glm::vec2 offset);

private:
    std::vector<Particle> m_particles;
    unsigned int amount;
    unsigned int VBO, VAO;
    unsigned int lastUsedParticle = 0;
    Shader m_shader;

    unsigned int firstUnusedParticle();
    void respawnParticle(Particle &particle, short type, glm::vec2 position, glm::vec2 velocity, float rotation, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

ParticleSystem::ParticleSystem(Shader shader, unsigned int amount) :
    m_shader(shader), amount(amount){
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
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    // set mesh attributes
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
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
            m_shader.setUniformVec2("offset", particle.m_position);
            m_shader.setUniformVec4("color", particle.m_color);
            //m_shader.setUniform("rotation", particle.m_rotate);
            //this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::Update(float dt, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f)){

    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        respawnParticle(m_particles[unusedParticle], 0, glm::vec2(1,1), glm::vec2(1,1), 60, offset);
    }

    // update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle &p = this->m_particles[i];
        p.m_life -= dt; // reduce life
        if (p.m_life > 0.0f)
        {	// particle is alive, thus update
            p.m_position -= p.m_velocity * dt;
            p.m_color.a -= dt * 1.f;
        }
    }
}

void ParticleSystem::AddParticles(short int type, glm::vec2 position, glm::vec2 velocity, float rotation, unsigned int newParticles, glm::vec2 offset) {
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
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
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

void ParticleSystem::respawnParticle(Particle& particle, short int type, glm::vec2 position, glm::vec2 velocity, float rotation, glm::vec2 offset){
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.m_position =  random * offset;
    particle.m_color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.m_life = 1.f;
    particle.m_rotate = rotation;
    particle.m_velocity = glm::vec2(0.01f,0.01f) ;
}
