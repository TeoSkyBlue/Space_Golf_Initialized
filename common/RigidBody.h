#ifndef RIGID_BODY_H
#define RIGID_BODY_H
#include <iostream>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * Task 1b: use rotation matrix to encode the orientation of a rigid body
 * There is a better way to represent the orientation of a rigid body than
 * using a 3 x 3 rotation matrix. For a number of reasons, unit quaternions,
 * a type of four element vector normalized to unit length, are a better choice
 * than for rotation matrices. For rigid body simulation, the most important
 * reason to avoid using rotation matrices is because of numerical drift.
 */
#define USE_QUATERNIONS

class RigidBody {
public:
#ifdef USE_QUATERNIONS
    static const int STATES = 13;
#else
    static const int STATES = 18;
#endif
    /** m: mass */
    float m;
    /** x: position, v: velocity, w: angular velocity */
    glm::vec3 x, v, w;
    /** I_inv: inverse inertia matrix */
    glm::mat3 I_inv;
    /** the orientation of a rigid body can be encoded by a 3D rotation matrix
    or by a quaternion */
#ifdef USE_QUATERNIONS
    glm::quat q;
#else
    glm::mat3 R;
#endif
    /** P: momentum, L: angular momentum */
    glm::vec3 P, L;
    /**
     * The forcing function accepts t, y and returns a vector of 6 floats
     * [fx, fy, fz, taux, tauy, tauz] corresponding to the values of the
     * applied forces. By default zero forces are applied, otherwise the user
     * must specify the forcing function.
     */
    std::function<std::vector<float>(float t, const std::vector<float>& y)> forcing =
        [](float t, const std::vector<float>& y)->std::vector<float> {
        std::vector<float> f(6, 0.0f);
        return f;
    };

    RigidBody();
    ~RigidBody();
    /** Get state vector y */
    std::vector<float> getY();
    /** Get state vector y */
    void setY(const std::vector<float>& y);
    /** Get state derivative vector dy / dt */
    std::vector<float> dydt(float t, const std::vector<float>& y);
    /** Calculate the kinetic energy of the rigid body KE = 1/2 m u^T u + 1/2 w^T I w */
    float calcKinecticEnergy();
    /** Euler method for advancing the state y(t + h) = y(t) + h dy(t) / dt */
    std::vector<float> euler(float t, float h, const std::vector<float>& y0);
    /** Runge-Kutta 4th order for advancing the state (error/step ~ O(h^5) */
    std::vector<float> rungeKuta4th(float t, float h, const std::vector<float>& y0);
    /** Advances the state from t to t + h using Euler or RunkeKutta */
    void advanceState(float t, float h);
};

#endif
