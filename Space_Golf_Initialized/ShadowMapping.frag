#version 330 core

in vec3 vertex_position_worldspace;
in vec3 vertex_position_cameraspace;
in vec3 vertex_normal_cameraspace;
in vec2 vertex_UV;
in vec4 vertex_position_lightspace; // shading
           

uniform sampler2D shadowMapSampler;
uniform sampler2D diffuseColorSampler;
uniform sampler2D specularColorSampler;
uniform sampler2D ambientColorSampler;
uniform sampler2D normalSampler;

uniform float time;
uniform int useTexture = 0;
uniform mat4 V;


// Phong 
// light properties
struct Light {
    vec4 La;
    vec4 Ld;
    vec4 Ls;
    vec3 lightPosition_worldspace;
    float power;
};
uniform Light light;

// materials
struct Material {
    vec4 Ka; 
    vec4 Kd;
    vec4 Ks;
    float Ns; 
};
uniform Material mtl;

out vec4 fragmentColor;

void phong(float visibility);
float ShadowCalculation(vec4 fragPositionLightspace);



void main() {   
    
    // Task 4.3
     float shadow  = ShadowCalculation(vertex_position_lightspace);


    float visibility = 1.0f - shadow;
    phong(visibility);
}



float ShadowCalculation(vec4 vertexPositionLightspace){
    // Given the position of a fragment in lightspace coordinates
    // We sample the depth buffer to see whether or not the fragment is shadowed
    
    float shadow;

    // Task 4.3
    
    // Perspective devide to bring coordinates in range[-1, 1]
    vec3 projCoords = vertexPositionLightspace.xyz / vertexPositionLightspace.w;


    // Since the depth map values are in range[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // Sampling the closest point in this position from the depth map
    // REMINDER: Since we are in lightspace coordinates,
    //           the z parameter is the depth from the camera
    float closestDepth = texture(shadowMapSampler, projCoords.xy).r;

    

    // Then we get the depth of the current vertex
    float currentDepth = projCoords.z;
	
    // If the currentDepth is larger than the closestDepth, the fragment is shadowed
    //shadow = closestDepth < currentDepth ? 1.0 : 0.0f;
    
	
    // Task 4.4
    // Correcting the quantization problem
    float bias = 0.0005;
   



    // Task 4.7 Make the shadow edges more realistic
    
    shadow = 0.0;
    vec2 depthMap_dimensions = textureSize(shadowMapSampler, 0);
    vec2 texelSize = 1.0 / depthMap_dimensions;
    for(int x = -1; x <= 1; x++ ){
        for(int y = -1; y <= 1; y++ ){
            float pcfDepth = texture(shadowMapSampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
        }
    }
    shadow /= 24.0;
   


    // Task 4.5
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
    


    return shadow;

}


void phong(float visibility) {
   
    vec4 _Ks = mtl.Ks;
    vec4 _Kd = mtl.Kd;
    vec4 _Ka = mtl.Ka;
    float _Ns = mtl.Ns;

    // use texture for materials
    //6 is for plane with its normal Sampling
    if (useTexture == 1) {
        _Ks = vec4(texture(specularColorSampler, vertex_UV).rgb * 0.4f, 1.0);
        _Kd = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 0.4f, 1.0);
        _Ka = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 0.35f, 1.0);
        _Ns = 10;
    }
    //skybox texture setup. //ambient slow turn mode
    else if (useTexture == 2) { //use Texture i should be redifined so that it is "useTexture == AmbientMode" or smmth
        _Ks = vec4(0, 0, 0, 1.0);
        _Kd = vec4(0, 0, 0, 1.0);
        _Ka = vec4((texture(ambientColorSampler, vertex_UV + (time/40.0f)).rgb) * 1.8f, 1.0);
        _Ns = 0;
    }

    else if (useTexture == 3) { //3 is for earth, or generally objects with specific ambient texture
        _Ks = vec4(texture(specularColorSampler, vertex_UV).rgb * 0.4, 1.0);
        _Kd = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 1.2, 1.0);
        _Ka = vec4(texture(ambientColorSampler, vertex_UV).rgb * 0.30, 1.0);
        _Ns = 4;
    }

    //sun like, ambient med turn mode
    else if (useTexture == 4){  
        _Ks = vec4(0, 0, 0, 1.0);
        _Kd = vec4(0, 0, 0, 1.0);
        _Ka = vec4((texture(ambientColorSampler, vertex_UV + time).rgb) * 1.8, 1.0);
        _Ns = 0;

        }
    //moonlike planet, no turn on tuxture. Ambient mode.
    else if(useTexture == 5){
        _Ks = vec4(0, 0, 0, 1.0);
        _Kd = vec4(0, 0, 0, 1.0);
        _Ka = vec4((texture(ambientColorSampler, vertex_UV).rgb) * 1.4, 1.0);
        _Ns = 6;
        }
    else if (useTexture == 6) {
        _Ks = vec4(texture(specularColorSampler, vertex_UV).rgb * 0.8f, 1.0);
        _Kd = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 1.45f, 1.0);
        _Ka = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 0.25f, 1.0);
        _Ns = 10;
    }
    if (useTexture == 7) {
        _Ks = vec4(texture(specularColorSampler, vertex_UV).rgb * 0.5f, 0.01);
        _Kd = vec4(texture(diffuseColorSampler, vertex_UV).rgb, 0.01);
        _Ka = vec4(texture(diffuseColorSampler, vertex_UV).rgb * 0.35f, 0.01);
        _Ns = 2;
    }


    // model ambient intensity (Ia)
    vec4 Ia = light.La * _Ka;

    // model diffuse intensity (Id)
    vec3 N = normalize(vertex_normal_cameraspace); 
    
    /*
    if (useTexture == 6){
        N = normalize(vec4(texture(normalSampler, vertex_UV)).rgb);
      }
    */
    vec3 L = normalize((V * vec4(light.lightPosition_worldspace, 1)).xyz - vertex_position_cameraspace);
    float cosTheta = clamp(dot(L, N), 0, 1);
    vec4 Id = light.Ld * _Kd * cosTheta; 

    // model specular intensity (Is)
    vec3 R = reflect(-L, N);
    vec3 E = normalize(- vertex_position_cameraspace);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    float specular_factor = pow(cosAlpha, _Ns);
    vec4 Is = light.Ls * _Ks * specular_factor;

    //model the light distance effect
    float distance = length(light.lightPosition_worldspace - vertex_position_worldspace);
    float distance_sq = distance * distance;

    // final fragment color
    fragmentColor = vec4(
        Ia + 
        visibility * Id * light.power / distance_sq +   // Task 4.3 Use visibility
        visibility * Is * light.power / distance_sq);
}