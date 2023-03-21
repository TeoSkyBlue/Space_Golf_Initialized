#version 330

in vec2 uv;
uniform sampler2D textureSampler;
out vec4 frag_colour;

// Task ?.? Perspective Shadows
float near_plane = 1.0f;
float far_plane = 30.0f;
float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // Values in [-1, 1]
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}


void main () {

  //frag_colour = vec4(1.0f);


  vec4 colour = texture(textureSampler, uv);
  
  // Task 2.3
  // render any texture
   frag_colour = vec4(colour.r, colour.r, colour.r, 1.0);

  // Task 3.4
  // render depth map
  // frag_colour = vec4(colour.r, colour.r, colour.r, 1.0);

} 