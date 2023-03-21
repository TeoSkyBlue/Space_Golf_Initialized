#include <glm/glm.hpp>

class Light {
public:

    GLFWwindow* window;
    // Light parameters
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 lightPosition_worldspace;

    glm::vec4 La;
    glm::vec4 Ld;
    glm::vec4 Ls;
    float power;

    float nearPlane;
    float farPlane;

    bool orthoProj;

    float lightSpeed;
    float mouseSpeed;
    float horizontalAngle;
    float verticalAngle;
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;


    // Where the light will look at
    glm::vec3 targetPosition;

    // Constructor
    Light(GLFWwindow* window,
        glm::vec4 init_La,
        glm::vec4 init_Ld,
        glm::vec4 init_Ls,
        glm::vec3 init_position,
        float init_power);
  
    void update();
    void update_mouse();


    glm::mat4 lightVP();
};
