#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

using namespace glm;
float curr_s_left = 0.0f;
float curr_s_right = 0.0f;
vec3 starting_pos = vec3(0, 0, 5);

Camera::Camera(GLFWwindow* window) : window(window)
{
    aspect_ratio = 16.0f / 9.0f; 
    position = glm::vec3(0, 0, 5);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 90.0f;
    speed = 13.00f;
    mouseSpeed = 0.001f;
    fovSpeed = 2.0f;
    direction  = vec3(0, 0, 1);
    up = vec3(0, 1, 0);
    right = vec3(1, 0, 0);
}

void Camera::update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();

    float deltaTime = float(currentTime - lastTime);


    if (active) {
        // Get mouse position
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Reset mouse position for next frame
        glfwSetCursorPos(window, width / 2, height / 2);



        // Task 5.3: Compute new horizontal and vertical angles, given windows size
        //*/
        // and cursor position
        horizontalAngle += mouseSpeed * (width / 2 - xPos);
        verticalAngle += mouseSpeed * (height / 2 - yPos);
    }


    // Task 5.4: right and up vectors of the camera coordinate system
    // use spherical coordinates
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle), //r
        sin(verticalAngle),//theta
        cos(verticalAngle) * cos(horizontalAngle)  //phi
    );

    // Right vector
    right = vec3(
        -cos(horizontalAngle),
        0,
        sin(horizontalAngle)
    );



    // Up vector
    up = cross(right, direction);

    //phi unit vector

    vec3 tilt_right(up * 0.4f * cos(float(currentTime)) + right * -sin(float(currentTime)));

    vec3 tilt_left(up * -cos(float(currentTime)) + right * -sin(float(currentTime)));

    vec3 starting_position;

    // Task 5.5: update camera position using the direction/right vectors
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * speed * deltaTime;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * speed * deltaTime;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * speed * deltaTime;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * speed * deltaTime;
    }

    //Home 1.2
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += up * speed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= up * speed * deltaTime;
    }
    //Home 1.2 end

    //Home 1.3 start
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS   /* && (curr_s_left <3.14f / 6.0f) */) {
        if (curr_s_left == 0.0f) {
            starting_pos = position;
        }

        curr_s_left += float(deltaTime);

        position += 2.0f * (up * cos(3.141f / 2.0f + curr_s_left) + right * -sin(3.141f / 2.0f + curr_s_left)) * speed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        curr_s_left = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        curr_s_right = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS /* && (curr_s_right < 3.14f / 6.0f)*/) {
        curr_s_right += float(deltaTime);
        position -= 2.0f * (up * 1.2f * -cos(3.141f / 2.0f + curr_s_right) + right * -sin(3.141f / 2.0f + curr_s_right)) * speed * deltaTime;
    }
    
    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        FoV += fovSpeed * speed * deltaTime;
        if (FoV > 90.0f) {
            FoV = 90.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        FoV -= fovSpeed * speed * deltaTime;
        if (FoV < 1.0f) {
            FoV = 1.0f;
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        position = vec3(0, 0, 5);
    }


    // Task 5.7: construct projection and view matrices

    projectionMatrix = perspective(radians(FoV),
        aspect_ratio,
        0.1f, 1000.0f); 


    //projectionMatrix = ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0f, 10.5f);

    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    //*/

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}