#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <map>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Sphere.h"

using namespace std;
using namespace glm;


static std::vector<unsigned int> VEC_UINT_DEFAUTL_VALUE{};
static std::vector<glm::vec3> VEC_VEC3_DEFAUTL_VALUE{};
static std::vector<glm::vec2> VEC_VEC2_DEFAUTL_VALUE{};
static std::map<std::string, GLuint> MAP_STRING_GLUINT_DEFAULT_VALUE{};
/**
* A very simple .obj loader. Use only for teaching purposes. Use loadOBJWithTiny()
* instead.
*/
void loadOBJ(
    const std::string& path,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* A .vtp loader.
*/
void loadVTP(
    const std::string& path,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* An .obj loader that uses tinyobjloader library. Any mesh (quad) is triangulated.
*
* https://github.com/syoyo/tinyobjloader
*/
void loadOBJWithTiny(
    const std::string& path,
    std::vector<glm::vec3>& verticies,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* Create VBO indexing.
* http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
*/
void indexVBO(
    const std::vector<glm::vec3>& in_vertices,
    const std::vector<glm::vec2>& in_uvs,
    const std::vector<glm::vec3>& in_normals,
    std::vector<unsigned int> & out_indices,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
);

class Drawable {
public:
    const int instances;
    Drawable(std::string path);

    Drawable(std::string path, vec3 centre, const int instances, vector <Sphere*>& all_spheres, vector <vec3>& z_proj);
    Drawable(std::string path, GLboolean reverse);

    Drawable(
        const std::vector<glm::vec3>& vertices,
        const std::vector<glm::vec2>& uvs = VEC_VEC2_DEFAUTL_VALUE,
        const std::vector<glm::vec3>& normals = VEC_VEC3_DEFAUTL_VALUE);

    ~Drawable();

    void bind();
    void bind_instanced();


    /* Bind VAO before calling draw */
    void draw(int mode = GL_TRIANGLES);
    void draw_instanced(int mode = GL_TRIANGLES);


public:
    std::vector<glm::vec3> vertices, normals, indexedVertices, indexedNormals;
    std::vector<glm::vec2> uvs, indexedUVS;
    std::vector<unsigned int> indices;
    
    GLuint VAO, verticesVBO, uvsVBO, normalsVBO, elementVBO, model_buffer, scales_buffer;

private:
    vector <glm::mat4> model_matrices;
    vector <glm::mat4> translations;
    vector <glm::mat4> rotations;
    vector <float> scales;
    
    void createContext();
    void createContext(vector <Sphere*>& all_spheres, vector <vec3>& z_proj, vec3 centre);
    void createContext(GLboolean reverse);

    void asteroid_transforms(vector <mat4>& translations, vector <mat4>& rotations, vector <mat4>& model_matrices,
        vector <float>& scales, vector <Sphere*>& all_spheres,
        vector <vec3>& z_proj, const unsigned int instanced_amount, vec3 centre);

    float randf()
    {
        return -1.0f + (rand() / (RAND_MAX / 2.0f));
    }


};

/*****************************************************************************/

namespace ogl {
    struct Material {
        glm::vec4 Ka;
        glm::vec4 Kd;
        glm::vec4 Ks;
        float Ns;
        GLuint texKa;
        GLuint texKd;
        GLuint texKs;
        GLuint texNs;
    };

    class Mesh {
    public:
        Mesh(const std::vector<glm::vec3>& vertices,
             const std::vector<glm::vec2>& uvs,
             const std::vector<glm::vec3>& normals,
             const Material& mtl);
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&& other);
        ~Mesh();
        void bind();
        void draw(int mode = GL_TRIANGLES);
    public:
        std::vector<glm::vec3> vertices, normals, indexedVertices, indexedNormals;
        std::vector<glm::vec2> uvs, indexedUVS;
        std::vector<unsigned int> indices;
        Material mtl;
        GLuint VAO, verticesVBO, uvsVBO, normalsVBO, elementVBO;
    private:
        void createContext();
    };

    class Model {
    public:
        using MTLUploadFunction = void(const Material&);
        Model(std::string path, MTLUploadFunction* uploader = nullptr);
        ~Model();
        void draw();
    private:
        std::vector<Mesh> meshes;
        std::map<std::string, GLuint> textures;
        MTLUploadFunction* uploadFunction;
    private:
        void loadOBJWithTiny(const std::string& filename);
        void loadTexture(const std::string& filename);
    };
}

#endif
