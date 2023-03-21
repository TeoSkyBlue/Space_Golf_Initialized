#include "Collision.h"
//#include "Box.h"
#include "Sphere.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <execution>

using namespace glm;
using namespace std;



//maybe box sphere collision check is used for the out-of-skybox-bounds inevitability.
//void handleBoxSphereCollision(Box& box, Sphere& sphere);
//bool checkForBoxSphereCollision(glm::vec3 &pos, const float& r,
            //                    const float& size, glm::vec3& n);

bool checkForSpheretoSphereCollision(Sphere& sphere, Sphere& sphere2, glm::vec3& n1, glm::vec3& n2);
void handleSpheretoSphereCollision(Sphere& sphere, Sphere& sphere2);

//important bits.
void handleOnetoManySpheres(vector <vec3> z_axis, 
	vector <Sphere *>& all_spheres, Sphere& important_sphere);
bool checkforOnetoManySpheres(vec3 axis, Sphere& important_sphere, int j);
void handle_distilledCollision(vector <Sphere *>& all_spheres, int final_coll, Sphere& important_sphere, vec3& n); //deprecated.
void handle_distilledCollision2(vector <Sphere* >& all_spheres, vector <int> pot_colls, Sphere& important_sphere, vec3& n);

//dumb collisions in parallel for static planets.
bool handlePlanetCollision(vector <Sphere*>& massive_spheres, Sphere& important_sphere);


//collision with golf_club.
void handleGolfClubHit(Sphere& golf_club_hitbox, Sphere& important_sphere);


//helper functions
int findClosest(vector <vec3> proj, float n, float target);
float getClosest(float val1, float val2, float target);


//gravity functions
void calculateGravity(vector <Sphere*>& massive_spheres, Sphere& important_sphere, vector <forces_struct>& forces);



void handleGolfClubHit(Sphere& golf_club_hitbox, Sphere& important_sphere) {
	vec3 n;
	float dist = (important_sphere.x.x - golf_club_hitbox.x.x) * (important_sphere.x.x - golf_club_hitbox.x.x) +
		(important_sphere.x.y - golf_club_hitbox.x.y) * (important_sphere.x.y - golf_club_hitbox.x.y) +
		(important_sphere.x.z - golf_club_hitbox.x.z) * (important_sphere.x.z - golf_club_hitbox.x.z); //a bit faster than distance(x1,x2)
	float a = sqrt(dist) - (golf_club_hitbox.r + important_sphere.r);
	if (a < 0.001f) {
		n = -normalize(golf_club_hitbox.x - important_sphere.x); //reverse from other collisions, we transfer the energy.

		important_sphere.x += (abs(a) / 2 * n);
		important_sphere.v = length(golf_club_hitbox.v) * (n); 
		
		important_sphere.P = important_sphere.v * important_sphere.m;
		return;
	}


}



bool handlePlanetCollision(vector <Sphere*>& massive_spheres, Sphere& important_sphere) {
	bool success = false;
	std::for_each(std::execution::par_unseq, massive_spheres.begin(), massive_spheres.end(),
		[&](Sphere* planet)->void {
			vec3 n;
	float dist = sqrt((important_sphere.x.x - planet->x.x) * (important_sphere.x.x - planet->x.x) +
		(important_sphere.x.y - planet->x.y) * (important_sphere.x.y - planet->x.y) +
		(important_sphere.x.z - planet->x.z) * (important_sphere.x.z - planet->x.z));
	float a = dist - (planet->r + important_sphere.r);
	if (a <= 0.001f) {
		n = -normalize(planet->x - important_sphere.x);
		
		float sphere_vlength = length(important_sphere.v);
		important_sphere.x += (abs(a) / 4 * n);
		if (planet->grav_pull == 9.80665f) {
			success = true;
		}
		if (sphere_vlength < 0.4f) {
			sphere_vlength = 0.0f;
			//n = vec3(0.0);
			//cout << to_string(n) << endl;
		}
		
		if (planet->m == 1) {
			important_sphere.v = sphere_vlength * planet->grav_pull * (n);	//satellites will be mad elastic because it makes the game
			//more playable that way and I dont have to worry about what happens when I hit satellite.
			
		}	
		else {
			important_sphere.v = sphere_vlength * 0.25f * (n);
		}
		//0.8 is elasticity factor on collision. Could be unique for
		//each planet depending on the surface?
		important_sphere.P = important_sphere.v * important_sphere.m;
		return;
	}
	return;
		});
	return success;
}



void handleOnetoManySpheres(vector <vec3> z_axis,
	vector <Sphere *>& all_spheres, Sphere& important_sphere) {
	glm::vec3 n;
	vector <int> pot_colls_1;//check z axis
	pot_colls_1.reserve(z_axis.size());
	int coll_centre = -1;
	int coll_iter;
	float zdist_lowMAX, zdist_highMAX;
	float zdist_low, zdist_high;
	float z_low = important_sphere.x.z - important_sphere.r;
	float z_high = important_sphere.x.z + important_sphere.r;

	//potential thread 0 
	//find position on Z_proj array of next closest.
	coll_centre = findClosest(z_axis, z_axis.size(), important_sphere.x.z - important_sphere.r);
	//thread end;
	//cout << "collision centre" << z_axis[coll_centre][2] << endl;
	//Perform binary search on z axis to narrow down the possible collisions.
	//check for collisions.
	zdist_lowMAX = important_sphere.x.z - important_sphere.x.r - 1.0f;
	//2.0 is theoretically the maximum R you input rn with scales at instancing
	zdist_highMAX = important_sphere.x.z + important_sphere.x.r + 1.0f;

	zdist_high = -INFINITY;
	zdist_low = INFINITY;
	if (coll_centre != -1) {
		coll_iter = coll_centre;

		//potential thread 1
		while (zdist_lowMAX < zdist_low && coll_iter >= 0) {
			if ((z_axis[coll_iter][0] < z_low && z_axis[coll_iter][1] > z_low) ||  //sos
				(z_axis[coll_iter][0] < z_high && z_axis[coll_iter][1] > z_low)) {
				pot_colls_1.push_back(z_axis[coll_iter][2]);
			}
			zdist_low = (z_axis[coll_iter][1] + z_axis[coll_iter][0]) / 2;
			coll_iter--;
			

			//next element test
			//if you are on the end of the list what happens?
			//you need to also check on the higher end of the list sir.
		}

		coll_iter = coll_centre;
		//potential thread 2, if you do it in second thread, pot_colls1 will need to be a different vector.
		while (zdist_highMAX > zdist_high && coll_iter < z_axis.size()) {
			if ((z_axis[coll_iter][0] < z_low && z_axis[coll_iter][1] > z_low) ||  //sos
				(z_axis[coll_iter][0] < z_high && z_axis[coll_iter][1] > z_low)) {
				pot_colls_1.push_back(z_axis[coll_iter][2]);
			}
			zdist_high = (z_axis[coll_iter][1] + z_axis[coll_iter][0]) / 2;
			coll_iter++;
			

			//next element test
			//if you are on the end of the list what happens?
			//you need to also check on the higher end of the list sir.
		}

		if (pot_colls_1.empty()) {
			return;
		}
		else {
			handle_distilledCollision2(all_spheres, pot_colls_1, important_sphere, n);
		}

	}
}



int findClosest(vector <vec3> proj, float n, float target)
{
  // Corner cases
  //left-side case
	if (target <= proj[0][0])   //proj [i][j] i: sphere location in projection vector, j: sphere id.

		return 0;				//case target is lower than every single item on list
	//right-side case
	if (target >= proj[n - 1][0])
		return (n - 1);

	// Doing binary search
	int i = 0, j = n, mid = 0, outcome = -2;
	while (i < j) {
		mid = (i + j) / 2;

		if (proj[mid][0] == target)
			return mid;

		/* If target is less than array element,
			then search in left */
		if (target < proj[mid][0]) {

			// If target is greater than previous
			// to mid, return closest of two
			if (mid > 0 && target > proj[mid - 1][0]) {
				outcome = getClosest(proj[mid - 1][0], proj[mid][0], target);
				if (outcome == proj[mid - 1][0])
					return (mid - 1);
				else if (outcome == proj[mid][0])
					return mid;
			}
			j = mid;
		}
		/* Repeat for left half */

		  // If target is greater than mid
		else {
			if (mid < n - 1 && target < proj[mid + 1][0]) {
				outcome = getClosest(proj[mid][0], proj[mid + 1][0], target);
				if (outcome == proj[mid + 1][0]) 
					return (mid + 1);
				else if(outcome == proj[mid][0])
					return mid;
				
			}
			// update i
			i = mid + 1;
		}
	}

	// Only single element left after search
	return mid;
}

float getClosest(float val1, float val2,
	float target)
{
	if (target - val1 >= val2 - target)
		return val2;
	else
		return val1;
}



void calculateGravity(vector <Sphere*>& massive_spheres, Sphere& important_sphere, vector <forces_struct> &forces){
	std::for_each(std::execution::par_unseq, massive_spheres.begin(), massive_spheres.end(),
		[&](Sphere* planet)->void {
			//Make dist be pow from the get-go. 
				float dist = (important_sphere.x.x - planet->x.x) * (important_sphere.x.x - planet->x.x) +
				(important_sphere.x.y - planet->x.y) * (important_sphere.x.y - planet->x.y) +
				(important_sphere.x.z - planet->x.z) * (important_sphere.x.z - planet->x.z);
				vec3 n = normalize(planet->x - important_sphere.x);
				vec3 force = n * float((important_sphere.m * (1000 * planet->grav_pull) / (dist)));
				forces[planet->index].central.x = force.x;
				forces[planet->index].central.y = force.y;
				forces[planet->index].central.z = force.z;
				
	});
}


/* old monstrosity, take me out please. I was drunk.
		[&](Sphere* planet)->void {
			//const Sphere* important_sphere_val = &important_sphere;
			important_sphere.forcing = [&](float t, const vector<float>& y)->vector<float> {
			vector<float> f(6, 0.0f);
			float dist = distance(important_sphere.x, planet->x);
			vec3 n = normalize((planet->x - important_sphere.x) / dist);
			vec3 force = n * float((important_sphere.m * (10) / pow(dist, 2)));
			f[0] = force.x;
			f[1] = force.y;
			f[2] = force.z;
			return f;
		}; */



bool checkforOnetoManySpheres(vec3 axis, Sphere& important_sphere, int j) {

	float j_low = important_sphere.x[j] - important_sphere.r;
	float j_high = important_sphere.x[j] + important_sphere.r;

	if ((axis[0] < j_low && axis[1] > j_low) ||  //sos
		(axis[0] < j_high && axis[1] > j_low)) return true; //sos
	return false;
}


void handle_distilledCollision(vector <Sphere* >& all_spheres, int final_coll, Sphere& important_sphere, vec3& n) {


	//float dist = distance(all_spheres[final_coll]->x, important_sphere.x);
	float dist = sqrt((important_sphere.x.x - all_spheres[final_coll]->x.x) * (important_sphere.x.x - all_spheres[final_coll]->x.x) +
		(important_sphere.x.y - all_spheres[final_coll]->x.y) * (important_sphere.x.y - all_spheres[final_coll]->x.y) +
		(important_sphere.x.z - all_spheres[final_coll]->x.z) * (important_sphere.x.z - all_spheres[final_coll]->x.z));

	float a = dist - (all_spheres[final_coll]->r + important_sphere.r);
	n = normalize(all_spheres[final_coll]->x - important_sphere.x);

	//important_sphere.x = (a * (- n)); //still point of contention
	important_sphere.v = length(important_sphere.v) * (-n);
	important_sphere.P = important_sphere.v * important_sphere.m;

	return;

}





void handle_distilledCollision2(vector <Sphere* >& all_spheres, vector <int> pot_colls, Sphere& important_sphere, vec3& n) {
	float dist, a;
	//cout << "different check" <<"\n" << endl;    
	/*
	-This algorithm
	was totally fine, and easy to think through, and implement, and debug
	all those couts were not poor attempts at debugging code from a month ago. I would NEVER!
	It's just that sometimes I REALLY like watching my terminal going nuts and 
	printing 10 variables every frame at 60 frames per second. 
	-Wait, you mean you don't do that?
	*/
	for (int i = 0; i < pot_colls.size(); i++) {
		//cout << to_string(pot_colls[i]) << endl;
		//dist = distance(all_spheres[pot_colls[i]]->x, important_sphere.x);
		dist = sqrt((important_sphere.x.x - all_spheres[pot_colls[i]]->x.x) * (important_sphere.x.x - all_spheres[pot_colls[i]]->x.x) +
			(important_sphere.x.y - all_spheres[pot_colls[i]]->x.y) * (important_sphere.x.y - all_spheres[pot_colls[i]]->x.y) +
			(important_sphere.x.z - all_spheres[pot_colls[i]]->x.z) * (important_sphere.x.z - all_spheres[pot_colls[i]]->x.z));
		a = dist - (all_spheres[pot_colls[i]]->r + important_sphere.r);
		if (a < 0.12f) {
			
			n = -normalize((all_spheres[pot_colls[i]]->x - important_sphere.x));
			//cout << " normalized_vector " << to_string(n) << a << endl;
			important_sphere.x += (abs(a)/2 * (n));
			//cout << " sphere v length " << sphere_v_abs << endl;
			//if (sphere_v_abs > 1.0f) {
			important_sphere.v = length(important_sphere.v) * (n);
			//}
			/*		remnant of experimental ideas regarding the handling of extremely slow spheres.
			else {
				cout << "went in" << endl;
				important_sphere.v = (sphere_v_abs + 1.0f) * (n);
			}
			*/
			important_sphere.P = important_sphere.v * important_sphere.m;
			//cout << to_string(important_sphere.v) << endl;
			break;
		}
	}

	return;

}

void handleSpheretoSphereCollision(Sphere& sphere, Sphere& sphere2) {
    vec3 n1 , n2;

	float total_mass = (sphere.m + sphere2.m);
    // two normals, each for every ball
    if (checkForSpheretoSphereCollision(sphere, sphere2, n1, n2)) {
        vec3 new_speed1, new_speed2;
		
        new_speed1 = 
            (2.0f * sphere2.m * sphere2.v / total_mass) +
            ((sphere.m - sphere2.m) * sphere.v / total_mass);
            
        

        new_speed2 = 
            (2.0f * sphere.m * sphere.v / total_mass) +
            ((sphere2.m - sphere.m) * sphere2.v / total_mass);
            

        

		sphere.v = length(new_speed1) * n2;

		sphere2.v = length(new_speed2) * n1;

	
        sphere.P = sphere.m * sphere.v;
        sphere2.P = sphere2.m * sphere2.v;
        // Task 2b: define the velocity of the sphere after the collision

    }
}



bool checkForSpheretoSphereCollision(Sphere& sphere, Sphere &sphere2, glm::vec3& n1, glm::vec3& n2) {
    float a  = distance(sphere.x, sphere2.x) - (sphere.r + sphere2.r);
	


    if (a <= 0.0f) {
		n1 = normalize((sphere2.x - sphere.x) / distance(sphere.x, sphere2.x));
		n2 = normalize((sphere.x - sphere2.x) / distance(sphere2.x, sphere.x));
		
        sphere.x -=(a/2 * n1);
        
        sphere2.x -=(a/2 * n2);
		
    }
    else return false;

    return true;
}





//maybe box sphere collision check is used for the out-of-skybox-bounds inevitability.
/*

void handleBoxSphereCollision(Box& box, Sphere& sphere) {
    vec3 n;
    if (checkForBoxSphereCollision(sphere.x, sphere.r, box.size, n)) {
		sphere.v = sphere.v - n * glm::dot(sphere.v, n) * 1.9995f;//WAS 2.0
		sphere.P = sphere.m * sphere.v;
        // Task 2b: define the velocity of the sphere after the collision

    }
}

bool checkForBoxSphereCollision(vec3 &pos, const float& r,
                                const float& size, vec3& n) {
    if (pos.x - r <= 0) {
        //correction
        float dis = -(pos.x - r);
        pos = pos + vec3(dis, 0, 0);

        n = vec3(-1, 0, 0);
    } else if (pos.x + r >= size) {
        //correction
        float dis = size - (pos.x + r);
        pos = pos + vec3(dis, 0, 0);

        n = vec3(1, 0, 0);
    } else if (pos.y - r <= 0) {
        //correction
        float dis = -(pos.y - r);
        pos = pos + vec3(0, dis, 0);

        n = vec3(0, -1, 0);
    } else if (pos.y + r >= size) {
        //correction
        float dis = size - (pos.y + r);
        pos = pos + vec3(0, dis, 0);

        n = vec3(0, 1, 0);
    } else if (pos.z - r <= 0) {
        //correction
        float dis = -(pos.z - r);
        pos = pos + vec3(0, 0, dis);

        n = vec3(0, 0, -1);
    } else if (pos.z + r >= size) {
        //correction
        float dis = size - (pos.z + r);
        pos = pos + vec3(0, 0, dis);

        n = vec3(0, 0, 1);
    } else {
        return false;
    }

    return true;
}

*/