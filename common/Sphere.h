#ifndef SPHERE_H
#define SPHERE_H

#include "RigidBody.h"

class Drawable;

class Sphere : public RigidBody {
public:
    Drawable* sphere;
    float r;
    float grav_pull = 0;
    int index = 0;
    glm::mat4 modelMatrix;

    Sphere(std::string path, glm::vec3 pos, glm::vec3 vel, float radius, float mass);
	Sphere(glm::vec3 pos, float radius);
    Sphere(std::string path, glm::vec3 pos, float radius, float mass, float grav_pull_const);
    ~Sphere();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
    void update_static();
};

#endif