#include "Sphere.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>

using namespace glm;

Sphere::Sphere(std::string path, vec3 pos, vec3 vel, float radius, float mass)
    : RigidBody() {
    sphere = new Drawable(path);
    grav_pull = 0;
    r = radius;
    m = mass;
    x = pos;
    v = vel;
    P = m * v;

    if (radius == 0) throw std::logic_error("Sphere: radius != 0");
    mat3 I = mat3(
        2.0f / 5 * mass*radius*radius, 0, 0,
        0, 2.0f / 5 * mass*radius*radius, 0,
        0, 0, 2.0f / 5 * mass*radius*radius);
    I_inv = inverse(I);
}



Sphere::Sphere(glm::vec3 pos, float radius) : RigidBody() {
	r = radius;
	x = pos;

}

Sphere::Sphere(std::string path, glm::vec3 pos, float radius, float mass, float grav_pull_const) : RigidBody() {
    sphere = new Drawable(path);
    r = radius;
    m = mass;
    x = pos;
    grav_pull = grav_pull_const;
}

Sphere::~Sphere() {
    delete sphere;
}

void Sphere::draw(unsigned int drawable) {
    sphere->bind();
    sphere->draw();
}

void Sphere::update(float t, float dt) {
    //integration
    advanceState(t, dt);

    // compute model matrix
    mat4 scale = glm::scale(mat4(), vec3(r, r, r));
    mat4 translation = translate(mat4(), vec3(x.x, x.y, x.z));
#ifdef USE_QUATERNIONS
    //Did you ever wonder why is everything upside down?
    //With the low price of 3.99$ you too can have your answer!
    mat4 rotation = mat4_cast(q) * transpose(mat4_cast(q));
    //q times vec3(1, 1, 1) times transpose(q)
#else
    mat4 rotation = mat4(R);
#endif
    modelMatrix = translation * rotation * scale;
}


void Sphere::update_static() {
    //integration
    //advanceState(t, dt);

    // compute model matrix
    mat4 scale = glm::scale(mat4(), vec3(r, r, r));
    mat4 translation = translate(mat4(), vec3(x.x, x.y, x.z));
#ifdef USE_QUATERNIONS
    mat4 rotation = mat4_cast(q * conjugate(q));
    //q 1 -q
#else
    mat4 rotation = mat4(R);
#endif
    modelMatrix = translation * rotation * scale;
}