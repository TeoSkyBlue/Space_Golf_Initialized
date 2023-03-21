#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;



struct forces_struct {
	vec3 central;
	vec3 spin;
};


class Box;
class Sphere;
//void handleBoxSphereCollision(Box& box, Sphere& sphere);
void handleSpheretoSphereCollision(Sphere& sphere, Sphere& sphere2);


void handleOnetoManySpheres(vector <vec3> z_axis, vector <Sphere *>& all_spheres, Sphere& important_sphere);
bool checkforOnetoManySpheres(vec3 axis, Sphere& important_sphere, int j);
void handle_distilledCollision(vector <Sphere *>& all_spheres, int final_coll, Sphere& important_sphere, vec3& n);
void handle_distilledCollision2(vector <Sphere* >& all_spheres, vector <int> pot_colls, Sphere& important_sphere, vec3& n);
void calculateGravity(vector <Sphere*>& massive_spheres, Sphere& important_sphere, vector <forces_struct>& forces);
bool handlePlanetCollision(vector <Sphere*>& massive_spheres, Sphere& important_sphere);
void handleGolfClubHit(Sphere& golf_club_hitbox, Sphere& important_sphere);


#endif
