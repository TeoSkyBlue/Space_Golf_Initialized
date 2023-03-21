// Include C++ headers
#include <iostream>
#include <string>
#include <algorithm>
#include <execution>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
//#include <common/objects.h> //my lonely deprecated solution standing in the corner
							//patiently awaiting for my return,
							//a return which will never come.

#include <common/Sphere.h>
#include <common/Collision.h>
#include <common/Light.h>
#include <common/FountainEmitter.h>

//include GUI

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"




using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

//void create_object( vector <string> object_name, vector <vec3> object_vertices, vector <vec2> object_UVs );

#define W_WIDTH 1600
#define W_HEIGHT 900
#define TITLE "Space_Golf"
#define pi 3.1415926536

// Global variables
GLuint shaderProgram, instancedProgram; //all shader Programs
GLuint MVPLocation;
GLuint mainTexture, corroded_metal_diffused, corroded_metal_specular
, galaxy_texture, secondTexture;
GLuint timeUniform;
GLuint MLocation, VPLocation;
GLuint MLocationI, VPLocationI;
vector <Sphere *> first_asteroids, all_spheres;
vector <Sphere*> massive_spheres;
vector <vec3> z_proj, x_proj;  //ARRAYS OF (CENTRE - R, CENTRE + R, ID AS FLOAT) SORTED BY AXIS_LOW
GLfloat anisotropy = 0.0f;
float aspect_ratio = W_WIDTH / W_HEIGHT;



//helper window variables
int target_framerate = 90;
//bool game_paused = false;
bool success = false;
bool strike_attempt = false;
bool reset = false;
bool show_plane = false;
bool show_particles = false;
bool golf_club_visibility = true;
bool camera_lock = false;
bool trail_mode = false;

GLfloat t = 0.0;
GLfloat strike_timing = -0.1f;
GLfloat rotation_length = 0.0;

int swing_power = 14;
int hits_attempted = 0;
int camera_speed = 13;

int succesful_runs = 0;
int best_attempt = 0;



Drawable* planet, * planet2, * skybox, *golf_club;// , * planet_earth;
Drawable* atmosphere_saturn, *atmosphere_earth;

Sphere* weighted_ball, * virtual_planet, * golf_ball ,
* planet_earth, * planet_moon, *planet_mars, *planet_starter,
*planet_fictMoon, *planet_venus, *planet_saturn, *planet_jupiter,
*golf_club_hitbox;


GLint object_count = 0, object_iter;
GLuint object_id_loc;

// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;
//GLuint lightSourceLocation;

// instanced depth program
GLuint shadowViewProjectionLocation2;
//GLuint lightSourceLocation2;

//Light properties
Light  *spotlight;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPowerLocation, lightPositionLocation;
GLuint depthMapSampler, lightVPLocation;

//Shadow map dimensions should be the first to go if gpu time gets more precious down the line.

#define SHADOW_WIDTH 8192
#define SHADOW_HEIGHT 8192

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;
#define grav 9.80665f
//all planets modeled are normalised after earth's grav_pull.
//No fancy smancy grav_forces to account for wether you are on equator or not.
//In other words, assume that planets are dots with mass on the galaxy.



//struct to store material parameters, kind of deprecated, was used for testing.

struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
};

typedef enum texture_modes{use_material, ambient_mode, diffuse_mode};


// Global Variables

GLFWwindow* window;
Camera* camera;
Light* light, *light2;
GLuint depthProgram, miniMapProgram, depthInstancedProgram;
Drawable* model1;
Drawable* model2;
Drawable* plane;
Drawable* lightball;
Drawable* asteroids, *particle;

GLuint modelDiffuseTexture, modelSpecularTexture, asteroid_tex_normals, asteroid_ambient_texture,  skybox_texture;
GLuint white_bricks_ambient, white_bricks_gloss, white_bricks_normals;
GLuint marble_diffused, marble_gloss, gray_texture, golf_texture, golf_gloss, golf_diffused;
GLuint texture_golf_club;

//planet textures
GLuint earth_day, earth_night, earth_spec, sun_texture, texture_moon;
GLuint texture_starter, texture_fictMoon, texture_venus, texture_saturn, texture_venus_atmosphere, texture_mars, texture_jupiter;
GLuint star_texture;
GLuint depthFrameBuffer, depthTexture;

Drawable* quad;

// locations for shaderProgram
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;



GLuint diffuseColorSampler;
GLuint specularColorSampler;
GLuint ambientColorSampler;
GLuint normalSampler;
GLuint useTextureLocation;


GLuint lightDirectionLocation;
GLuint lightFarPlaneLocation;
GLuint lightNearPlaneLocation;


// locations for depthProgram


// locations for instancedProgram

// locations for shaderProgram
GLuint viewMatrixLocation2;
GLuint projectionMatrixLocation2;
GLuint modelMatrixLocation2;
GLuint KaLocation2, KdLocation2, KsLocation2, NsLocation2;



GLuint diffuseColorSampler2;
GLuint specularColorSampler2;
GLuint ambientColorSampler2;
GLuint normalSampler2;
GLuint useTextureLocation2;


GLuint lightDirectionLocation2;
GLuint lightFarPlaneLocation2;
GLuint lightNearPlaneLocation2;

//light props 2
GLuint LaLocation2, LdLocation2, LsLocation2;
GLuint lightPowerLocation2, lightPositionLocation2;
GLuint depthMapSampler2, lightVPLocation2;


//Particles
FountainEmitter* f_emitter;




// Create two sample materials
const Material polishedSilver{
	vec4{0.23125, 0.23125, 0.23125, 1},
	vec4{0.2775, 0.2775, 0.2775, 1},
	vec4{0.773911, 0.773911, 0.773911, 1},
	89.6f
};

const Material turquoise{
	vec4{ 0.1, 0.18725, 0.1745, 0.8 },
	vec4{ 0.396, 0.74151, 0.69102, 0.8 },
	vec4{ 0.297254, 0.30829, 0.306678, 0.8 },
	12.8f
};

const Material polishedGold{ //it is not even close to actual polished gold at this point but whatever.
	vec4(0.74725, 0.7245, 0.7645, 1),
	vec4(0.34615, 0.3143, 0.3903, 1),
	vec4(0.897357, 0.823991, 0.508006, 1),
	10.2f

};

const Material white_rubber{
	vec4(0.05,	0.05,	0.05, 1),
	vec4(0.5,	0.5,	0.5, 1),
	vec4(0.7, 0.7, 0.7, 1),
	10.0f
};

const Material white_plastic{
	vec4(0.9, 0.9, 0.9, 1),
	vec4(0.25, 0.25, 0.25, 1),
	vec4(0.20, 0.20, 0.20, 1),
	22.0f
};



// NOTE: Since the Light and Material struct are used in the shader programs as well 
//		 it is recommended to create a function that will update all the parameters 
//       of an object.
// 
// Creating a function to upload (make uniform) the light parameters to the shader program
void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}

void uploadLight2(const Light& light) {
	glUniform4f(LaLocation2, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation2, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation2, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation2, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation2, light.power);
}


// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}






void renderHelpingWindow() {

	ImGui::Begin("Space_Golf Helper Window",NULL, ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("That naughty kid must have lost the ball again..\n"
		"Return it to Planet Earth!");               // Display some text (you can use a format strings too)

	ImGui::SliderInt("Frame-cap", &target_framerate, 30, 244);
	ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderInt("Swing Power", &swing_power, 6, 22);
	ImGui::SliderInt("Camera Speed", &camera_speed, 2, 44);
	ImGui::Text("Hits attempted: %d", hits_attempted);
	ImGui::Text("Successful Runs: %d", succesful_runs);
	if (best_attempt != int(INFINITY)) {
		ImGui::SameLine();
		ImGui::Text("Best Attempt: %d", best_attempt);
	}
	

	if (ImGui::Button("Toggle golf club"))
		golf_club_visibility = !golf_club_visibility;
	if (ImGui::Button("Show Plane"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		show_plane = !show_plane;
	ImGui::SameLine();
	if (ImGui::Button("Enable Trail Mode"))
		trail_mode = !trail_mode;
	if (ImGui::Button("Reset Game"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		reset = !reset;
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}


void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/*
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		game_paused = !game_paused;
	}
	*/ //Why would you want to pause a static game? Are you naughty? Dunno, maybe will implement later.

	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			camera->active = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			camera->active = false;
		}
	}
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (swing_power < 22) {
				swing_power += 1;
			}
		}
	//input mode is "Press once, change once, keep pressed => change repeatedly.
	//We mimic keyboards default behaviour in windows terminals.
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (swing_power > 6) {
				swing_power -= 1;
			}
		}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		strike_attempt = true;
		if(golf_club_visibility) hits_attempted++;

		//golf_club_hitbox->v = vec3(swing_power);
		if (strike_timing < 0) {
			strike_timing = t;
		}
	}
	
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		camera_lock = !camera_lock;

		}

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if(camera_speed < 44)
			camera_speed++;

	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (camera_speed > 2)
			camera_speed--;
	}
	
}


void createContext() {

	// Create and complile our GLSL program from the shader
	shaderProgram = loadShaders("ShadowMapping.vert", "ShadowMapping.frag");
	instancedProgram = loadShaders("Instanced_items.vert", "Instanced_items.frag");

	// Task 3.1 
	// Create and load the shader program for the depth buffer construction
	// You need to load and use the Depth.vertexshader, Depth.fragmentshader
	// NOTE: These files are not visible in the visual studio, as they are not a part of
	//       the main project. To add a new file to our project, we need to add it to the
	//       CMAKELists.txt and build the project again. 
	// NOTE: We alse need to create a global variable to store new shader program
	depthProgram = loadShaders("Depth.vert", "Depth.frag");
	depthInstancedProgram = loadShaders("Depth_instanced.vert", "Depth.frag");

	// Task 2.1
	// Use the SimpleTexture.vertexshader, "SimpleTexture.fragmentshader"
	miniMapProgram = loadShaders("SimpleTexture.vert", "SimpleTexture.frag");


	// NOTE: Don't forget to delete the shader programs on the free() function


	// Get pointers to uniforms
	// --- shaderProgram ---
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	// for phong lighting
	KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
	diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");
	ambientColorSampler = glGetUniformLocation(shaderProgram, "ambientColorSampler");
	normalSampler = glGetUniformLocation(shaderProgram, "normalSampler");


	// 
	timeUniform = glGetUniformLocation(shaderProgram, "time");
	useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
	

	// locations for shadow rendering
	depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
	lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");


	// --- depthProgram ---
	shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

	shadowViewProjectionLocation2 = glGetUniformLocation(depthInstancedProgram, "VP");

	// --- miniMapProgram ---
	quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");

	// --- InstancedProgram -- 
	// Get pointers to uniforms
	projectionMatrixLocation2 = glGetUniformLocation(instancedProgram, "P");
	viewMatrixLocation2 = glGetUniformLocation(instancedProgram, "V");
	modelMatrixLocation2 = glGetUniformLocation(instancedProgram, "M");
	// for phong lighting
	KaLocation2 = glGetUniformLocation(instancedProgram, "mtl.Ka");
	KdLocation2 = glGetUniformLocation(instancedProgram, "mtl.Kd");
	KsLocation2 = glGetUniformLocation(instancedProgram, "mtl.Ks");
	NsLocation2 = glGetUniformLocation(instancedProgram, "mtl.Ns");
	LaLocation2 = glGetUniformLocation(instancedProgram, "light.La");
	LdLocation2 = glGetUniformLocation(instancedProgram, "light.Ld");
	LsLocation2 = glGetUniformLocation(instancedProgram, "light.Ls");
	lightPositionLocation2 = glGetUniformLocation(instancedProgram, "light.lightPosition_worldspace");
	lightPowerLocation2 = glGetUniformLocation(instancedProgram, "light.power");
	diffuseColorSampler2 = glGetUniformLocation(instancedProgram, "diffuseColorSampler");
	specularColorSampler2 = glGetUniformLocation(instancedProgram, "specularColorSampler");
	ambientColorSampler2 = glGetUniformLocation(instancedProgram, "ambientColorSampler");
	normalSampler2 = glGetUniformLocation(instancedProgram, "normalSampler");
	useTextureLocation2 = glGetUniformLocation(instancedProgram, "useTexture");

	// locations for shadow rendering
	depthMapSampler2 = glGetUniformLocation(instancedProgram, "shadowMapSampler");
	lightVPLocation2 = glGetUniformLocation(instancedProgram, "lightVP");


	// Loading a model


	// 1. Using Drawable to load suzanne
	//model1 = new Drawable("suzanne.obj");
	// loading a diffuse and a specular texture
	modelDiffuseTexture = loadSOIL("textures//CliffJagged004_COL_VAR1_3K.jpg");
	modelSpecularTexture = loadSOIL("textures//CliffJagged004_GLOSS_3K.jpg");
	//asteroid_tex_normals = loadSOIL("textures//CliffJagged004_NRM_3K.jpg");
	asteroid_ambient_texture = loadSOIL("textures//CliffJagged004_AO_3k.jpg");

	white_bricks_ambient = loadSOIL("textures//WhiteBricks_AO_4k.jpg");
	white_bricks_gloss = loadSOIL("textures//WhiteBricks_GLOSS_4k.jpg");
	//white_bricks_normals = loadSOIL("textures//WhiteBricks_NRM_4k.jpg");


	skybox_texture = loadSOIL("textures//8k_stars_milky_way.jpg");
	//marble_gloss = loadSOIL("textures//marble_gloss_3k.jpg");
	//marble_diffused = loadSOIL("textures//marble_diffused_3k.jpg");
	golf_diffused = loadSOIL("textures//alter_golf_diffused_plus_1k.jpg");
	golf_gloss = loadSOIL("textures//alter_golf_gloss_1k.png");
	texture_golf_club = loadSOIL("textures//golf_club_diffuse.jpg");
	
	earth_day = loadSOIL("textures//8k_earth_daymap.jpg");
	earth_spec = loadSOIL("textures//2k_earth_specular_map.jpg");
	earth_night = loadSOIL("textures//8k_earth_nightmap.jpg");
	//earth_normals?

	sun_texture = loadSOIL("textures//2k_sun.jpg");
	texture_moon = loadSOIL("textures//2k_moon.jpg");
	texture_fictMoon = loadSOIL("textures//2k_ceres_fictional.jpg");
	texture_starter = loadSOIL("textures//4k_makemake_fictional.jpg");
	
	texture_venus = loadSOIL("textures//2k_venus_surface.jpg");
	//texture_venus_atmosphere = loadSOIL("textures//2k_venus_atmosphere.jpg");
	//nice visual upgrade if this is made to work, not going to lose sleep over it though.


	texture_jupiter = loadSOIL("textures//2k_jupiter.jpg");
	texture_mars = loadSOIL("textures//2k_mars.jpg");
	texture_saturn = loadSOIL("textures//8k_saturn.jpg");
	

	
	star_texture = loadSOIL("textures//star_test.jpg");


	// Task 1.2 Load earth.obj using drawable 
	skybox = new Drawable("skybox.obj");

	

	particle = new Drawable("star_3d.obj");
	
	f_emitter = new FountainEmitter(particle, 2880); //11520

	int earth_radius = 18;
	int earth_mass = 300;

	planet_earth = new Sphere("expensive_planet_quads.obj", vec3(120, 180, -300), earth_radius, earth_mass, grav);

	planet_moon = new Sphere("expensive_planet_quads.obj", vec3(30, 30, 0), 3.5f, 1, (grav * 0.1657f));

	
	/*planet_starter,
		* planet_fictMoon, * planet_venus, * planet_saturn, * planet_jupiter, */

	//fictional planet.
	planet_starter = new Sphere("expensive_planet_quads.obj", vec3(0, -earth_radius / 1.3f, 0), earth_radius/1.5f, earth_mass / 20, (grav/4.6f));

	planet_fictMoon = new Sphere("expensive_planet_quads.obj", vec3(20, 5, 10), earth_radius >> 2, 1, (0.905f * grav));

	planet_mars = new Sphere("expensive_planet_quads.obj", vec3(140, 40, -60), earth_radius >> 1, earth_mass / 10, ( 2 * grav * 0.379f));

	planet_venus = new Sphere("expensive_planet_quads.obj", vec3(228, -22, -50), earth_radius * 0.949f, earth_mass * 0.815f, (grav * 0.905f));

	planet_saturn = new Sphere("expensive_planet_quads2.obj", vec3(-40, 10, -160), earth_radius * 5, earth_mass * 95.2f, (4 * grav * 1.065f));
	//atmosphere_saturn = new Drawable("expensive_planet_quads.obj");													//(radius fix) * grav * (relative to earth fix)
	asteroids = new Drawable("space_rock2.obj", planet_saturn->x, 6000, first_asteroids, z_proj);



	planet_jupiter = new Sphere("expensive_planet_quads2.obj", vec3(-220, 180, -280), earth_radius * 4.81f, earth_mass * 317.8f, (1.6f * grav * 2.528f));



	
	//Initialize massive spheres vector, push back all massive planets and give 
	//them their index in the constructor. 
	//To be used when calculating gravitational forces
	massive_spheres.reserve(24); // expecting no more than 24 planets lol, will do by hand anyways

	massive_spheres.push_back(planet_earth);
	massive_spheres.push_back(planet_moon);
	massive_spheres.push_back(planet_starter);
	massive_spheres.push_back(planet_fictMoon);
	massive_spheres.push_back(planet_venus);
	massive_spheres.push_back(planet_mars);
	massive_spheres.push_back(planet_saturn);
	massive_spheres.push_back(planet_jupiter);

	for (int i = 0; i < massive_spheres.size(); i++) {
		massive_spheres[i]->index = i;
	}




	golf_ball = new Sphere("golf_ball_gen2_smooth.obj", vec3(0, -1.2, 0), vec3(0.0, 0.0, 0.0), 0.3, 1);

	golf_club = new Drawable("golf_club_normal.obj");
	golf_club_hitbox = new Sphere("earth.obj", vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), 0.15, 0.4);

	lightball = new Drawable("expensive_planet_quads.obj");
	//will need to change to sphere object soon. Or not, because then you have
	//possible exploit to win on golf hits.(hit sun -> move sun -> win)
	//maybe the sun kills ball but can be used to help player set-up scenarios?
	//I love this idea.


		

	


	
	// Creating a Drawable object using vertices, uvs, normals
	// In this task we will create a plane on which the shadows will be displayed

	// plane vertices
	float z = -100; // offset to move the place up/down across the y axis
	vector<vec3> floorVertices = {
		vec3(-600.0f, 500.0f, z),
		vec3(-600.0f,-500.0f, z),
		vec3(600.0f, -500.0f, z),
		vec3(600.0f, -500.0f, z),
		vec3(600.0f,  500.0f, z),
		vec3(-600.0f, 500.0f, z),

	};

	// plane normals
	vector<vec3> floorNormals = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f)
	};

	// plane uvs
	vector<vec2> floorUVs = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
	};


	// Call the Drawable constructor
	// Notice, that this way we dont have to generate VAO and VBO for the matrices
	plane = new Drawable(floorVertices, floorUVs, floorNormals);

	
	// Task 2.2: Creating a 2D quad to visualize the depthmap
	// create geometry and vao for screen-space quad

	vector<vec3> quadVertices = {
	  vec3(0.5, 0.5, 0.0),
	  vec3(1.0, 0.5, 0.0),
	  vec3(1.0, 1.0, 0.0),
	  vec3(1.0, 1.0, 0.0),
	  vec3(0.5, 1.0, 0.0),
	  vec3(0.5, 0.5, 0.0)
	};

	

	vector<vec2> quadUVs = {
	  vec2(0.0, 0.0),
	  vec2(1.0, 0.0),
	  vec2(1.0, 1.0),
	  vec2(1.0, 1.0),
	  vec2(0.0, 1.0),
	  vec2(0.0, 0.0)
	};

	quad = new Drawable(quadVertices, quadUVs);

	//*/


	// ---------------------------------------------------------------------------- //
	// -  Task 3.2 Create a depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //

	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFrameBuffer);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);


	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Telling opengl the required information about the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	//*/

	// Task 4.5 Don't shadow area out of light's viewport

	// Step 1 : (Don't forget to comment out the respective lines above
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set color to set out of border 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Next go to fragment shader and add an iff statement, so if the distance in the z-buffer is equal to 1, 
	// meaning that the fragment is out of the texture border (or further than the far clip plane) 
	// then the shadow value is 0.
	//*/

	/*/ Task 3.2 Continue
	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


	// Since the depth buffer is only for the generation of the depth texture, 
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	// Finally, we have to always check that our frame buffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//*/
	glfwSetKeyCallback(window, pollKeyboard);
}


void free() {
	// Delete Shader Programs
	glDeleteProgram(shaderProgram);
	glDeleteProgram(instancedProgram);
	glDeleteProgram(depthInstancedProgram);
	glDeleteProgram(depthProgram);
	glDeleteProgram(miniMapProgram);

	glfwTerminate();
}


void depth_pass(mat4 viewMatrix, mat4 projectionMatrix, vector <mat4> modelMats) {

	// Task 4.3



	// Setting viewport to shadow map size
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);



	// Binding the depth framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

	// Cleaning the framebuffer depth information (stored from the last render)
	glClear(GL_DEPTH_BUFFER_BIT);


	// Selecting the new shader program that will output the depth component
	glUseProgram(depthProgram);


	// sending the view and projection matrix to the shader
	mat4 view_projection = projectionMatrix * viewMatrix;
	glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);


	// ---- rendering the scene ---- //
	// creating suzanne model matrix and sending to GPU
	//mat4 modelMatrix = translate(mat4(), vec3(0.0, 0.0, -5.0));
	//glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);

	//model1->bind();
	//model1->draw();

	// same for sphere
	mat4 modelMatrix;

	if (!trail_mode) {
		// same for plane
		if (show_plane) {
			modelMatrix = modelMats[0];
			glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			plane->bind();
			plane->draw();
		}

		modelMatrix = modelMats[1];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		golf_ball->draw();

		//modelMatrix = scale(mat4(), vec3(8, 8, 8));
		modelMatrix = modelMats[2];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		//planet_earth->bind();
		planet_earth->draw();

		modelMatrix = modelMats[3];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_moon->draw();

		if (golf_club_visibility) {
			modelMatrix = modelMats[4];
			glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			golf_club->bind();
			golf_club->draw();
		}
		modelMatrix = modelMats[5];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_fictMoon->draw();

		modelMatrix = modelMats[6];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_starter->draw();

		modelMatrix = modelMats[7];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_saturn->draw();

		modelMatrix = modelMats[8];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_mars->draw();

		modelMatrix = modelMats[9];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_jupiter->draw();

		modelMatrix = modelMats[10];
		glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		planet_venus->draw();
	}

	glUseProgram(depthInstancedProgram);

	if (!trail_mode) {
		glUniformMatrix4fv(shadowViewProjectionLocation2, 1, GL_FALSE, &view_projection[0][0]);
		//instanced objects should probably have shadowModelLocation be passed through shaders and not here
		asteroids->bind_instanced();
		asteroids->draw_instanced();
	}

	if (length(golf_ball->v) != 0) {
		f_emitter->renderParticles();
	}
	

	// binding the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//*/
}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix, float time, vector <mat4>& modelMats) {

	// Step 1: Binding a frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// Step 2: Clearing color and depth info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Step 3: Selecting shader program
	glUseProgram(shaderProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	// uploading the light parameters to the shader program
	
	uploadLight(*light);

	// Task 4.1 Display shadows on the 
	//*/
	// Sending the shadow texture to the shaderProgram

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);
	mat4 light_vp = light->lightVP();
	// Sending the light View-Projection matrix to the shader program
	glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &light_vp[0][0]);



	//*/




	// ----------------------------------------------------------------- //
	// --------------------- Drawing scene objects --------------------- //	
	// ----------------------------------------------------------------- //

	// Task 1.2 - Draw the sphere on the scene
	// Use a scaling of 0.5 across all dimensions and translate it to (-3, 1, -3)
	mat4 modelMatrix2;
	if (show_plane) {
		mat4 planeModelMatrix;
		vec3 plane_dir = normalize(light->lightPosition_worldspace -
			vec3(planeModelMatrix[2][0], planeModelMatrix[2][1], planeModelMatrix[2][2]));
		vec3 plane_rot_axis = cross(vec3(0, 0, 1), plane_dir);
		float rot_angle = glm::acos(glm::dot(glm::vec3(0, 0, 1), plane_dir));
		//vec4 planeRotation = vec4(plane_rot_axis, rot_angle);
		// Task 1.3 Draw a plane under suzanne

		/* deprecated solution to make plane stick to light source at distance D.
		planeModelMatrix = translate(mat4(1.0),
			vec3(-(light->direction.x * light->lightPosition_worldspace.x),
				-(light->direction.y * light->lightPosition_worldspace.y),
				- ((light->direction.z) * 20)))

			//* rotate(mat4(), rot_angle, plane_rot_axis);
			*rotate(mat4(), light->horizontalAngle + 185.4f , vec3(0, 1, 0))
			*
			* rotate(mat4(), light->verticalAngle, vec3(1, 0, 0));
			*/
			//fun fact: At an early version of those transforms, I naively saw that the plane was placed at an offset of 180 degrees
			//behind my intended position. Being the very clever-but-not-so-much- person I am, I decided to add 180 degrees 
			// to the angle offset. Thing is, after a whole semester of working with radians, it completely slipped my mind 
			// that this was one of the dumbest things I could do. So indeed, I got shit behaviour. 
			//But as my mother likes to say, "Never take the first no for an answer". So I did not. 
			// I played around with the values, and ended with 185.4... Why so? Well, it turns out, this value
			//is a very accurate approximation of 59 times pi. So 58pi + pi. Giving a result of pi. Yeah...LOL.

		planeModelMatrix = translate(mat4(1.0),
			vec3(light->lightPosition_worldspace + (400.0f * light->direction)))

			//* rotate(mat4(), rot_angle, plane_rot_axis);
			* rotate(mat4(), light->horizontalAngle + float(pi), vec3(0, 1, 0))
			* rotate(mat4(), light->verticalAngle, vec3(1, 0, 0));

		modelMats[0] = (planeModelMatrix);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(specularColorSampler, white_bricks_gloss);
		glUniform1i(specularColorSampler, 17);

		glActiveTexture(GL_TEXTURE18);
		glBindTexture(GL_TEXTURE_2D, white_bricks_ambient);
		/*
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
		*/ //I cannot even see the difference, so not used.

		glUniform1i(diffuseColorSampler, 18);

		//glActiveTexture(GL_TEXTURE19);
		//glBindTexture(GL_TEXTURE_2D, white_bricks_normals);
		//glUniform1i(normalSampler, 19);
		glUniform1i(useTextureLocation, 6);
		plane->bind();
		plane->draw();
	}


	

	if (viewMatrix == camera->viewMatrix) {
		modelMatrix2 = translate(mat4(), camera->position) * scale(mat4(), vec3(380.0f));
	}
	else {
		modelMatrix2 = translate(mat4(), light->lightPosition_worldspace) * scale(mat4(), vec3(380.0f));
	}
	
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix2[0][0]);
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, skybox_texture);			// Assign texture to position 
	glUniform1i(ambientColorSampler, 1);						// Assign sampler to that position

	glUniform1i(useTextureLocation, 2);

	skybox->bind();
	skybox->draw();


	mat4 golfModelMat = golf_ball->modelMatrix;
	modelMats[1] = (golfModelMat);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &golfModelMat[0][0]);
	
	glActiveTexture(GL_TEXTURE2);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, golf_diffused);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 2);														// Assign sampler to that position
	glActiveTexture(GL_TEXTURE3);								//
	glBindTexture(GL_TEXTURE_2D, golf_gloss);			// Same process for specular texture
	glUniform1i(specularColorSampler, 3);
	glUniform1i(useTextureLocation, 1);
	

	//uploadMaterial(white_plastic);
	//glUniform1i(useTextureLocation, 0);
	golf_ball->draw();

	//mat4 earthModelMat = scale(mat4(), vec3(8, 8, 8));
	mat4 earthModelMat = planet_earth->modelMatrix; 
	modelMats[2] = (earthModelMat);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &earthModelMat[0][0]);
	glActiveTexture(GL_TEXTURE4);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, earth_day);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 4);														// Assign sampler to that position
	glActiveTexture(GL_TEXTURE5);	
	glBindTexture(GL_TEXTURE_2D, earth_spec);			// Same process for specular texture
	glUniform1i(specularColorSampler, 5);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, earth_night);
	glUniform1i(ambientColorSampler, 6);
	glUniform1i(useTextureLocation, 3);
	
	//planet_earth->bind();
	planet_earth->draw();


	//ALL THOSE DIFFERENT MAT4s ARE USELESS OVERHEAD, WILL HAVE TO FIX 
	//STOP BEING BORED :)
	
	float moon_rotation_radius = 35;
	float moon_rot_rad_height = 15;
	
	planet_moon->x = vec3(moon_rotation_radius * sin(t/2) + planet_earth->x.x,
		(-moon_rot_rad_height * sin(t / 2)) + planet_earth->x.y,
		moon_rotation_radius * cos(t/2) + planet_earth->x.z);
	mat4 modelMatrix = planet_moon->modelMatrix * rotate(mat4(), t/4, planet_moon->x);
	modelMats[3] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE7);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_moon);			// Assign texture to position 
	glUniform1i(specularColorSampler, 7);
	glActiveTexture(GL_TEXTURE8);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_moon);			// Assign texture to position 
	glUniform1i(diffuseColorSampler, 8);
	glUniform1i(useTextureLocation, 1);

	planet_moon->draw();


	//light ball
	mat4 lightballMat = translate(mat4(), light->lightPosition_worldspace) * scale(mat4(), vec3(4.2, 4.2, 4.2));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &lightballMat[0][0]);
	glUniform1f(timeUniform, (float)glfwGetTime()/40.0f);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, sun_texture);
	glUniform1i(ambientColorSampler, 9);
	glUniform1i(useTextureLocation, 4);
	
	lightball->bind();
	lightball->draw();
	
	mat4 golf_modelMatrix;
	vec3 golfc_cameraspace = (camera->position) + ( 1.3f * camera->direction) - (0.8f * camera->right) - (0.8f * camera->up);
	if (golf_club_visibility) {
		if (strike_attempt == true) {
			GLfloat swing_duration = t - strike_timing;
			if (rotation_length < float(2.25f * pi)) {

				switch (swing_power) {
				case 6:
					rotation_length = (swing_duration * swing_power) + (3 * pi / 2);
					break;
				case 7:
					rotation_length = (swing_duration * swing_power) + (3 * pi / 2);
					break;
				case 8:
					rotation_length = (swing_duration * swing_power) + (5 * pi / 4);
					break;
				case 9:
					rotation_length = (swing_duration * swing_power) + (pi);
					break;
				default:
					rotation_length = (swing_duration * swing_power) + (pi / 2);
					break;
				}


				//float hit_time = time;
				//nice rotation pipeline, thank god for theory.
				//Should be probable done with quaternions, maybe fix later
				//time/slow_factor should be a thing that has to do with selected speed
				//Extravagant movement is along Masashiro Sakurai's thoughts on animations in games.
				mat4 rotation_mat = translate(mat4(), vec3(0, 1, 0));
				rotation_mat = rotate(rotation_mat, rotation_length, vec3(1, 0, 0));
				rotation_mat = translate(rotation_mat, vec3(0, -1, 0));
				//mat4 rotation_mat = rotate(mat4(), rotation_length, vec3(1, 0, 0));
				golf_modelMatrix = translate(mat4(), golfc_cameraspace)
					* rotate(mat4(), camera->horizontalAngle + float(pi), vec3(0, 1, 0))
					* rotate(mat4(), camera->verticalAngle, vec3(1, 0, 0))
					* rotation_mat;
				golf_club_hitbox->v = vec3(swing_power);
			}
			else {
				rotation_length = 0.0;
				strike_timing = -0.1f;
				strike_attempt = false;
				golf_club_hitbox->v = vec3(0);
				//swing_power = 1.0f;
			}
		}//These transforms took me 2-3 days to get through and were done during exam period. Legend
		//has it that I still have nightmares about them. Though, you could guess as much just by looking at 
		//extruciating lack of elegance.

		else {

			golf_modelMatrix = translate(mat4(), golfc_cameraspace)
				* rotate(mat4(), camera->horizontalAngle + float(pi), vec3(0, 1, 0))
				* rotate(mat4(), camera->verticalAngle, vec3(1, 0, 0));

		}
		modelMats[4] = (golf_modelMatrix);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &golf_modelMatrix[0][0]);
		//glUniform1f(timeUniform, (float)glfwGetTime() / 40.0f);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, texture_golf_club);
		glUniform1i(ambientColorSampler, 10);
		glUniform1i(useTextureLocation, 5);
		golf_club->bind();
		golf_club->draw();


		//status: servicable, slightly ugly.
		golf_club_hitbox->x = vec3(golf_modelMatrix[3][0], golf_modelMatrix[3][1], golf_modelMatrix[3][2]);
		//position of our hitbox is derived from the model matrix of the golf_club itself. As it should.
		//This method has a slight delay on hitbox movement. Whatever.


		
		//(camera->position + vec3(-0.8, -1.0, -1.4));// *vec3(1, cos(90.0f + time / 20.0f), sin(90.0f + time / 20.0f));
		//golf_club_hitbox->modelMatrix = scale(modelMatrix, vec3(0.2, 0.2, 0.2)); ///Careful here because model matrix is dependent on Golf_club modelMatrix with same name.
		mat4 hitbox_modelMatrix = golf_club_hitbox->modelMatrix;// *scale(mat4(), vec3(0.5, 0.5, 0.5));
		//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &hitbox_modelMatrix[0][0]);
		//glUniform1f(timeUniform, (float)glfwGetTime() / 40.0f);

		//uploadMaterial(white_rubber);
		//glUniform1i(useTextureLocation, use_material);
		//golf_club_hitbox->draw();

	}



	// ALL OTHER PLANETS(OTHER THAN EARTH-MOON DUET




	moon_rotation_radius = 26;
	moon_rot_rad_height = 8;

	planet_fictMoon->x = vec3(moon_rotation_radius * sin(t / 2) + planet_starter->x.x,
		(-moon_rot_rad_height * sin(t / 2)) + planet_starter->x.y,
		moon_rotation_radius * cos(t / 2) + planet_starter->x.z);
	modelMatrix = planet_fictMoon->modelMatrix * rotate(mat4(), t, planet_fictMoon->x);
	modelMats[5] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE11);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_fictMoon);			// Assign texture to position 
	glUniform1i(specularColorSampler, 11);
	glUniform1i(diffuseColorSampler, 11);
	glUniform1i(useTextureLocation, 1);
	planet_fictMoon->draw();

	modelMatrix = planet_starter->modelMatrix;
	modelMats[6] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE12);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_starter);			// Assign texture to position 
	glUniform1i(specularColorSampler, 12);
	glUniform1i(diffuseColorSampler, 12);
	glUniform1i(useTextureLocation, 1);
	planet_starter->draw();

	modelMatrix = planet_saturn->modelMatrix;
	modelMats[7] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE13);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_saturn);			// Assign texture to position 
	glUniform1i(specularColorSampler, 13);
	glUniform1i(diffuseColorSampler, 13);
	glUniform1i(useTextureLocation, 1);
	planet_saturn->draw();

	/*
	modelMatrix = planet_saturn->modelMatrix * scale(mat4(), vec3(1.02f));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE15);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_venus_atmosphere);			// Assign texture to position 
	glUniform1i(specularColorSampler, 15);
	glUniform1i(diffuseColorSampler, 15);
	glUniform1i(useTextureLocation, 7);
	atmosphere_saturn->bind();
	atmosphere_saturn->draw();
	*/


	modelMatrix = planet_mars->modelMatrix;
	modelMats[8] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE14);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_mars);			// Assign texture to position 
	glUniform1i(specularColorSampler, 14);
	glUniform1i(diffuseColorSampler, 14);
	glUniform1i(useTextureLocation, 1);
	planet_mars->draw();

	modelMatrix = planet_jupiter->modelMatrix;
	modelMats[9] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE15);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_jupiter);			// Assign texture to position 
	glUniform1i(specularColorSampler, 15);
	glUniform1i(diffuseColorSampler, 15);
	glUniform1i(useTextureLocation, 1);
	planet_jupiter->draw();

	modelMatrix = planet_venus->modelMatrix;
	modelMats[10] = (modelMatrix);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE16);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, texture_venus);			// Assign texture to position 
	glUniform1i(specularColorSampler, 16);
	glUniform1i(diffuseColorSampler, 16);
	glUniform1i(useTextureLocation, 1);
	planet_venus->draw();

	
	//_______________________________________ INSTANCED PARAMETERS ______________________________________\\


	glUseProgram(instancedProgram);
	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation2, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation2, 1, GL_FALSE, &projectionMatrix[0][0]);

	// uploading the light parameters to the shader program
	uploadLight2(*light);

	// Task 4.1 Display shadows on the 
	//*/
	// Sending the shadow texture to the shaderProgram

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler2, 0);
	//mat4 light_vp = light->lightVP();
	// Sending the light View-Projection matrix to the shader program
	glUniformMatrix4fv(lightVPLocation2, 1, GL_FALSE, &light_vp[0][0]);

	// Setting up texture to display on shader program          //  --- Texture Pipeline ---
	glActiveTexture(GL_TEXTURE1);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, modelDiffuseTexture);	
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);  //Apply anisotropy to texture.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy); // Assign texture to position 
	glUniform1i(diffuseColorSampler2, 1);						// Assign sampler to that position

	glActiveTexture(GL_TEXTURE2);								//
	glBindTexture(GL_TEXTURE_2D, modelSpecularTexture);			// Same process for specular texture
	glUniform1i(specularColorSampler2, 2);
	/*
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, asteroid_tex_normals);			// Same process for specular texture
	glUniform1i(normalSampler2, 3);
	*/
	// Inside the fragment shader, there is an if statement whether to use  
	// the material of an object or sample a texture

	glUniform1i(useTextureLocation2, 1);
	asteroids->bind_instanced();
	asteroids->draw_instanced();

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, star_texture);
	glUniform1i(ambientColorSampler2, 4);
	glUniform1i(useTextureLocation2, 2);

	if (length(golf_ball->v) != 0) {
		f_emitter->renderParticles();
	}
	
}

// Task 2.3: visualize the depth_map on a sub-window at the top of the screen
void renderDepthMap() {
	// using the correct shaders to visualize the depth texture on the quad
	glUseProgram(miniMapProgram);

	//enabling the texture - follow the aforementioned pipeline
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(quadTextureSamplerLocation, 0);

	// Drawing the quad
	quad->bind();
	quad->draw();
}



void mainLoop() {


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	t = glfwGetTime();
	float frametime = 0.0;
	vector <mat4> modelMats;
	
	float particle_t = 0.0;
	light->update();
	mat4 light_proj = light->projectionMatrix;
	mat4 light_view = light->viewMatrix;

	vector <forces_struct> net_forces;
	net_forces.resize(massive_spheres.size());


	// Task 3.3
	// Create the depth buffer
	//depth_pass(light->viewMatrix, light->projectionMatrix);

	modelMats.resize(24);

	do {
		if(reset) {
			camera->position = vec3(0, 0, 5);
			light->lightPosition_worldspace = vec3(-146, 40, -24);
			golf_ball->v = vec3(0, 0, 0);
			golf_ball->x = vec3(0, -1.2, 0);
			
			f_emitter->reset_trailing_Particles();
			//reset helper window variables
			swing_power = 14;
			hits_attempted = 0;
			camera_speed = 13;
			camera_lock = false;
			trail_mode = false;
			reset = false;
		}

		camera->speed = camera_speed;
		float target_frametime = 1.0f / target_framerate;
		float currentTime = glfwGetTime();
		float dt = currentTime - t;
		//curr_s = glfwGetTime();

		frametime += dt;

		//all dts have become frametimes.
		if (frametime > target_frametime) {
			//cout << t << endl;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();



			mat4 light_proj = light->projectionMatrix;
			mat4 light_view = light->viewMatrix;
			if (camera_lock) camera->position = golf_ball->x + vec3(0, 3, 0);

			mat4 projectionMatrix = camera->projectionMatrix;
			mat4 viewMatrix = camera->viewMatrix;
			//camera->position = golf_ball->x;

			// Task 3.5
			// Create the depth buffer
			f_emitter->emitter_pos = golf_ball->x;

			depth_pass(light->viewMatrix, light->projectionMatrix, modelMats);

			// Getting camera information

			//updating planets and stuff should also happen in parallel. 
			// After all they may be only a few, but advanceState
			// is Heavy_af.Also,
			//EVERYTHING SHOULD HAPPEN IN PARALLEL!

			success = handlePlanetCollision(massive_spheres, *golf_ball);

			if (success) {
				succesful_runs++;
				if ((best_attempt > hits_attempted)|| best_attempt == 0) {
					best_attempt = hits_attempted;
				}
				reset = true;
			}

			if (length(golf_ball->v) > 0.15f) {
				
				handleOnetoManySpheres(z_proj, first_asteroids, *golf_ball);

				//this could be improved slightly by sorting massive spheres to the z_proj list, but 
				// they are below 20 and will require hand tweak to remove moving spheres, not worth it.
				// handlePlanetCollision(massive_spheres, *golf_ball);
				//all forces from all planets calculated in parallel here.
				calculateGravity(massive_spheres, *golf_ball, net_forces);


				//all forces from all planets added in series here.
				golf_ball->forcing = [&](float t, const vector<float>& y)->vector<float> {
					vector <float> f(6, 0.0f);
					for (int i = 0; i < massive_spheres.size(); i++) {
						f[0] += net_forces[i].central[0];
						f[1] += net_forces[i].central[1];
						f[2] += net_forces[i].central[2];

						//will we add spin?????? 
						/*
						f[3] += net_forces[i].spin[0];
						f[4] += net_forces[i].spin[1];
						f[5] += net_forces[i].spin[2];
						*/
					}
					return f;
				};

				golf_ball->update(t, frametime);
			}

			else {
				golf_ball->v = vec3(0.0, 0.0, 0.0);
				golf_ball->update_static();
			}


			
			handleGolfClubHit(*golf_club_hitbox, *golf_ball);

			//these updates can happen in parallel too :) 
			//though less important and probably adds more overhead than it alleviates.
			//But it would scale really well if we were to add many planets or add more interesting physics 
			//properties on them.

			std::for_each(std::execution::par_unseq, massive_spheres.begin(), massive_spheres.end(),
				[](Sphere* planet)->void {
					planet->update_static();
				});

				/*
			planet_earth->update_static();
			planet_moon->update_static();
			planet_fictMoon->update_static();
			planet_starter->update_static();
			planet_mars->update_static();
			planet_saturn->update_static();
			planet_jupiter->update_static();
			planet_venus->update_static();
			*/


			golf_club_hitbox->update_static();
			//lighting_pass(viewMatrix, projectionMatrix);

			// Task 1.5
			// Rendering the scene from light's perspective when F1 is pressed

			if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
				light->update_mouse();
				light->update();
				lighting_pass(light_view, light_proj, t, modelMats);

			}

			


			else {
				light->update();
				// Render the scene from camera's perspective
				lighting_pass(viewMatrix, projectionMatrix, t, modelMats);
			}
			//*/

			camera->update();

			f_emitter->update_trailing_Particles(currentTime, frametime, camera->position, *golf_ball);

			


			//t += dt;
			
			frametime = 0.0f;
			
			// 
			// was used for testing.
			//renderDepthMap();

			renderHelpingWindow();

			glfwSwapBuffers(window);

			glfwPollEvents();


		}
		t += dt;
	
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}


void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}


	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Gray background color
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	// Log
	logGLParameters();

	// Create camera
	camera = new Camera(window);

	// Task 1.1 Creating a light source
	// Creating a custom light 
	light = new Light(window,
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec3{ -146, 40, -24 },
		100000.0f
	);
	

}

int main(void) {
	try {
		initialize();
		createContext();
		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}

	return 0;
}