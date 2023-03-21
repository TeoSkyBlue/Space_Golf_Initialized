#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include "Sphere.h"
#include <glm/gtx/string_cast.hpp>

#define USE_PARALLEL_TRANSFORM

//Gives a random number between 0 and 1
#define RAND ((float) rand()) / (float) RAND_MAX

struct particleAttributes{
    glm::vec3 position = glm::vec3(0,0,0);
    glm::vec3 rot_axis= glm::vec3(0,1,0);
    float rot_angle = 0.0f; //degrees
    glm::vec3 accel = glm::vec3(0,0,0);
    glm::vec3 velocity = glm::vec3(0,0,0);
    float life = 0.0f;
    float mass = 0.0f;

    float dist_from_camera = 0.0f; //In case you want to do depth sorting
    bool operator < (const particleAttributes & p) const
    {
        return dist_from_camera > p.dist_from_camera;
    }
    bool operator > (const particleAttributes& p) const
    {
        return dist_from_camera > p.dist_from_camera;
    }

};


//ParticleEmitterInt is an interface class. Emitter classes must derive from this one and implement the updateParticles method
class IntParticleEmitter
{
public:
    GLuint emitterVAO;
    int number_of_particles;

    std::vector<particleAttributes> p_attributes;

    bool use_rotations = true;
    bool use_sorting = false;


    glm::vec3 emitter_pos; //the origin of the emitter

    IntParticleEmitter(Drawable* _model, int number);
	void changeParticleNumber(int new_number);

	void renderParticles(int time = 0);
	virtual void updateParticles(float time, float dt, glm::vec3 camera_pos) = 0;
    virtual void update_trailing_Particles(float time, float dt, glm::vec3 camera_pos, Sphere& trailed_ball) = 0;
	
    virtual void createNewParticle(int index) = 0;
    virtual void createNew_trailingParticle(int index, Sphere& tailed_ball) = 0;
    
    glm::vec4 calculateBillboardRotationMatrix(glm::vec3 particle_pos, glm::vec3 camera_pos);


private:

    std::vector<glm::mat4> model_matrices;
    std::vector<glm::mat4> translations;
    std::vector<glm::mat4> rotations;
    std::vector<float> scales;

    Drawable* model;
    void configureVAO();
    void bindAndUpdateBuffers();
    GLuint transformations_buffer;
    GLuint rotations_buffer;
    GLuint model_buffer;
    GLuint scales_buffer;
};

