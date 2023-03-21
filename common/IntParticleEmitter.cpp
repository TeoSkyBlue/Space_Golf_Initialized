#include "IntParticleEmitter.h"
//#include "Sphere.h"
#include <iostream>
#include <algorithm>


#ifdef USE_PARALLEL_TRANSFORM
    #include <execution>
#endif 



IntParticleEmitter::IntParticleEmitter(Drawable* _model, int number) {
    model = _model;
    number_of_particles = number;
    emitter_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    p_attributes.resize(number_of_particles, particleAttributes());

    model_matrices.resize(number_of_particles, glm::mat4());
    translations.resize(number_of_particles, glm::mat4(0.0f));
    rotations.resize(number_of_particles, glm::mat4(1.0f));
    scales.resize(number_of_particles, 1.0f);

    configureVAO();
}

void IntParticleEmitter::renderParticles(int time) {
    if (number_of_particles == 0) return;
    bindAndUpdateBuffers();
    glDrawElementsInstanced(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0, number_of_particles);
}

glm::vec4 IntParticleEmitter::calculateBillboardRotationMatrix(glm::vec3 particle_pos, glm::vec3 camera_pos)
{
    //Caclulate the rotation and angle of the rotation required
    //the function returns a vec4. The first three values are the
    //axis of rotation and the fourth is angle.
    return glm::vec4(0, 0, 0, 0);
}

void IntParticleEmitter::bindAndUpdateBuffers()
{
    if (use_sorting) {
        std::sort(p_attributes.begin(), p_attributes.end());
        //std::reverse(p_attributes.begin(), p_attributes.end());
    }

#ifdef USE_PARALLEL_TRANSFORM
    //Calculate the model matrix in parallel to save performance
    std::transform(std::execution::par_unseq, p_attributes.begin(), p_attributes.end(), translations.begin(),
        [](particleAttributes p)->glm::mat4 {
            if (p.life == 0) return glm::mat4(0.0f);
            return glm::translate(glm::mat4(), p.position) * glm::rotate(glm::mat4(), glm::radians(p.rot_angle), p.rot_axis);
        });

    
    if(use_rotations)
         std::transform(std::execution::par_unseq, p_attributes.begin(), p_attributes.end(), rotations.begin(),
            [](particleAttributes p)->glm::mat4 {
                if (p.life == 0) return glm::mat4(0.0f);
                return glm::rotate(glm::mat4(), glm::radians(p.rot_angle), p.rot_axis);
            });
    else {
        std::fill(rotations.begin(), rotations.end(), glm::mat4(1.0f));
    }
    
    std::transform(std::execution::par_unseq, p_attributes.begin(), p_attributes.end(), scales.begin(),
        [](particleAttributes p)->float {
            return p.mass/50;
        });
        
#else
    for (int i = 0; i < p_attributes.size(); i++) {
        auto p = p_attributes[i];
        translations[i] = glm::translate(glm::mat4(), p.position);
    }

    if(use_rotations)
        for (int i = 0; i < p_attributes.size(); i++) {
            auto p = p_attributes[i];
            rotations[i] = glm::rotate(glm::mat4(), glm::radians(p.rot_angle), p.rot_axis);
        }
    else {
        std::fill(rotations.begin(), rotations.end(), glm::mat4(1.0f));
    }

    for (int i = 0; i < p_attributes.size(); i++) {
        auto p = p_attributes[i];
        scales[i] = p.mass;
    }

    



#endif // USE_PARALLEL_TRANSFORM

    /*
    for (int i = 0; i < number_of_particles; i++) {
        model_matrices[i] = (translations[i] * rotations[i]);
    }
    */

    //Bind the VAO
    glBindVertexArray(emitterVAO);

    /*
    //Send transformation data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_particles * sizeof(glm::mat4), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_particles * sizeof(glm::mat4), &translations[0]); //Sending data

    glBindBuffer(GL_ARRAY_BUFFER, rotations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_particles * sizeof(glm::mat4), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_particles * sizeof(glm::mat4), &rotations[0]); //Sending data
    */ //this is useless overhead. We will be GPU bound anyway, might as well have the matrices be calculated on CPU instead.
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_particles * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_particles * sizeof(glm::mat4), &translations[0]);



    glBindBuffer(GL_ARRAY_BUFFER, scales_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_particles * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning and reallocating to avoid synchronization, see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_particles * sizeof(float), &scales[0]); //Sending data
}

void IntParticleEmitter::changeParticleNumber(int new_number) {
    if(new_number == number_of_particles) return;

    number_of_particles = new_number;
    p_attributes.resize(number_of_particles, particleAttributes());
    translations.resize(number_of_particles, glm::mat4(0.0f));
    rotations.resize(number_of_particles, glm::mat4(1.0f));
    scales.resize(number_of_particles, 1.0f);

}

void IntParticleEmitter::configureVAO()
{
    glGenVertexArrays(1, &emitterVAO);
    glBindVertexArray(emitterVAO);


    //We are using the model's buffer but since they are already in the GPU from the Drawable's constructor we just need to configure 
    //our own VAO by using glVertexAttribPointer and glEnableVertexAttribArray but without sending any data with glBufferData.
    glBindBuffer(GL_ARRAY_BUFFER, model->verticesVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (model->indexedNormals.size() != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, model->normalsVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }


    if (model->indexedUVS.size() != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, model->uvsVBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->elementVBO);

    //GLSL treats mat4 data as 4 vec4. So we need to enable attributes 3,4,5 and 6, one for each vec4
    glGenBuffers(1, &transformations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    //This tells opengl how each particle should get data its slice of data from the mat4
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glGenBuffers(1, &rotations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rotations_buffer);

    /*
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);
    */
    glGenBuffers(1, &scales_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, scales_buffer);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
}

