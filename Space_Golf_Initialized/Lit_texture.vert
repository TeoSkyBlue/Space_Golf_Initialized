#version 330 core

// input vertex and UV coordinates, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;


// Output data ; will be interpolated for each fragment.
out vec2 UV;

// model view projection matrix 
uniform mat4 VP, M;

void main()
{   
    
    /*  // FS
    vertex_position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_position_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz; 
    vertex_UV = vertexUV;

    // to be implemented
    vertex_position_lightspace = lightVP * vec4(vertex_position_worldspace, 1.0f);
    
    */
    // assign vertex position
    gl_Position = M *  vec4(vertexPosition_modelspace, 1.0);
    
    UV = vertexUV;
    
}
