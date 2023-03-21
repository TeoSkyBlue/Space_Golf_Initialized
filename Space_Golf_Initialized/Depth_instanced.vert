#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 3) in mat4 model_matrix;
layout(location = 7) in float scale;
// Values that stay constant for the whole mesh.
uniform mat4 VP;


void main()
{
    gl_Position =  VP * model_matrix * vec4(vertexPosition_modelspace * scale, 1);
}