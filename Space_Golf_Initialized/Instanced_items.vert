#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in mat4 model_matrix;
layout(location = 7) in float scale;


uniform mat4 P;
uniform mat4 V;

uniform mat4 lightVP;

vec4 instanced_item_worldspace;
out vec3 vertex_position_worldspace;
out vec3 vertex_position_cameraspace;
out vec3 vertex_normal_cameraspace;
out vec2 vertex_UV;
out vec4 vertex_position_lightspace;

void main() {
    
    
    instanced_item_worldspace = model_matrix * vec4(scale * vertexPosition_modelspace, 1);

    gl_Position =   P * V * instanced_item_worldspace;

    
    // FS
    vertex_position_worldspace = instanced_item_worldspace.xyz;
    vertex_position_cameraspace = (V * instanced_item_worldspace).xyz;
    vertex_normal_cameraspace = (V * model_matrix * vec4(vertexNormal_modelspace, 0)).xyz; 
    vertex_UV = vertexUV;

    // Task 4.2
    vertex_position_lightspace = lightVP * instanced_item_worldspace;
}
