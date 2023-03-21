//
// Created by stagakis on 9/12/20.
// Modified by Galazios during January 2023 :3


#ifndef LIGHT_BLUEs_SPACE_GOLF
#define LIGHT_BLUEs_SPACE_GOLF
#include "IntParticleEmitter.h"
#include "Sphere.h"

class FountainEmitter : public IntParticleEmitter {
    public:
        FountainEmitter(Drawable* _model, int number);

        float elapsed_time = 0;
        float death_timer = 0;
        int death_toll = 0;
        int resupply = 0;
        bool active_death = false;
        int active_particles = 0;//number of particles that have been instantiated
        bool original = false;
        bool refreshed = false;
       
        void createNewParticle(int index) override;
        void createNew_trailingParticle(int index, Sphere & golf_ball) override;
        void update_trailing_Particles(float time, float dt, glm::vec3 camera_pos, Sphere &golf_ball) override;
        void updateParticles(float time, float dt, glm::vec3 camera_pos) override;
        void reset_trailing_Particles();
};


#endif 
