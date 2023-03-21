#include "FountainEmitter.h"
#include "Sphere.h"
#include <iostream>
#include <algorithm>
#include <glfw3.h>


FountainEmitter::FountainEmitter(Drawable *_model, int number) : IntParticleEmitter(_model, number) {
    
}



void FountainEmitter::update_trailing_Particles(float time, float dt, glm::vec3 camera_pos, Sphere &golf_ball) {

    //This is for the fountain to slowly increase the number of its particles to the max amount
    //instead of shooting all the particles at once
    int batch = 8;
    if (active_particles < number_of_particles && ((time - elapsed_time) > 0.03f) && (length(golf_ball.v) != 0)) {
        original = true;
        
        //int limit = std::min(number_of_particles - active_particles, batch);
        for (int i = 0; i < batch; i++) {
            createNew_trailingParticle(active_particles, golf_ball);
            active_particles++;
        }
    }

    
    
    
    
    
    int golf_speed = length(golf_ball.v);
    
    bool refresh_cycle = false;
    if (int((100 * RAND))% 10 > 4)
        refresh_cycle = true;
    
    //bool refresh_cycle = true;

    for(int i = 0; i < active_particles; i++){
        particleAttributes & particle = p_attributes[i];
        /*
        if (golf_speed == 0) {
            p_attributes.pop_front();
        }
        */
        if (time - death_timer > 0.48f) {
            particle.life -= (0.2* RAND) + 0.0005;
            particle.mass -= (0.06 * RAND) + 0.0005;
            active_death = true;
            if (particle.life <= 0) {
                death_toll++;
            }
        }
        /*
        if(golf_speed!=0 &&
            (
                ((particle.life < 0) && (death_toll < (active_particles/10)))
                || (distance(particle.position, golf_ball.x) > 230)
                || ((time-elapsed_time > 0.04f) && (original == false))
            )&&
            resupply < 20
          )
          */
                                                                                                                    //my final answer to throw-up problem.
        if(((golf_speed!= 0 && resupply < 2) && (particle.life<0)) && (active_particles >= number_of_particles) && refresh_cycle)
        {
            createNew_trailingParticle(i, golf_ball);
            resupply++;
        }
        particle.rot_angle += 90*dt; 
        particle.position = particle.position + particle.velocity*dt + particle.accel*(dt*dt)*0.5f;
        particle.velocity = particle.velocity + particle.accel*dt;
        
        
    }

    if (active_death) {
        death_timer = time;
        active_death = false;
    }

    if (original) { //if you created particles in any capacity, reset timer.
        original = false;
        elapsed_time = time;
    }

    if (death_toll < 0) {
        death_toll = 0;
    }
    resupply = 0;

    
    //std::cout << time - elapsed_time << std::endl;

}


void FountainEmitter::createNew_trailingParticle(int index, Sphere &golf_ball){
    particleAttributes & particle = p_attributes[index];

    particle.position = emitter_pos + glm::vec3(-0.5 + RAND, -0.5 + RAND, -0.5 + RAND)/4.0f;
    if (golf_ball.v == vec3(0, 0, 0)) {
        particle.velocity = glm::vec3(-0.5 + RAND, -0.5 + RAND, -0.5 + RAND);
    }
    else {
        particle.velocity = glm::vec3(-golf_ball.v.x * RAND, -golf_ball.v.y * RAND, -golf_ball.v.z * RAND) / 10.0f;
    }
    particle.mass = RAND + 6.0f;
    particle.rot_axis = glm::normalize(glm::vec3(1 - 2*RAND, 1 - 2*RAND, 1 - 2*RAND));
    particle.accel = vec3(0.0, 0.0, 0.0);
    particle.rot_angle = RAND*360;
    particle.life = 1.0f; //mark it alive
}


void FountainEmitter::createNewParticle(int index) {}

void FountainEmitter::updateParticles(float time, float dt, glm::vec3 camera_pos) {}


void FountainEmitter::reset_trailing_Particles() {
    for (int i = 0; i < active_particles; i++) {
        particleAttributes& particle = p_attributes[i];
        /*
        if (golf_speed == 0) {
            p_attributes.pop_front();
        }
        */
        particle.life = 0;
        particle.mass = 0;
    }
}
