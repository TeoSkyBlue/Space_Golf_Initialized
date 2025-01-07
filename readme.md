Introduction
------------

This project was the beginning of my journey in the wondrous world of 
Computer Graphics and 3D rendering. It presents most of the knowledge gained
during a 7th semester introductory course in the department of Electrical and Computer
Engineering of the University of Patras.

	Techniques at work here include:
	    -Texturing
	    -Material based rendering
	    -Phong model for Diffuse and Specular direct illumination
	    -Shadowmaps
	    -User keyboard/mouse interaction
	    -Full 3D camera using Euler angles(hey, first project. mbmer?)
	    -Golf ball physics simulation using quaternions and collision handling
	    using a 'sweep and prune' approach
	    -Instanced Drawing of more than 6000 items accounting for over 6.000.000 vertices(physics is enabled here too!)
	    -Basic imGui integration with frametime limiter and options
	    -CPU-side particle generation utilizing multiple cores(quite inefficiently, but still!)
	
	Starting Scene
	
![alt text](https://github.com/TeoSkyBlue/Space_Golf_Initialized/blob/main/screenshots/default.png?raw=true)
	
	Shadowmaps Showcase!
	
![alt text](https://github.com/TeoSkyBlue/Space_Golf_Initialized/blob/main/screenshots/shadowmaps.png?raw=true)	
	
	And particles on the golf ball!

![alt text](https://github.com/TeoSkyBlue/Space_Golf_Initialized/blob/main/screenshots/particles.png?raw=true)	


All things considered, this project is end of life, has a couple known issues that I wont ever go back for
as it has more than served its purpose. It stands here as a showcase of my humble 
beginnings in this space(no pun intended).

To Build
------------

If you are so inclined, you too can build this project, provided you have a 
working visual studio 17(and upwards) installation with a C++17 compiler.
The following are the steps required for installation on Windows, since 
at the time of development I did not test on linux or mac platforms, I do not consider those supported. So:
	
	- Update your drivers!
	- Install CMake
	- Download the source code
	- Open Cmake and input the path to the 
	'Space_Golf_Initialized' directory on the
	source code box. It is the same directory
	as the readme.md file that this text is 
	generated from.
	- Create a 'build' folder on the same level
	and input its path to the 'build the binaries'
	textbox
	Your inputs should look something like:
	first textbox: C:/Users/[username]/Space_Golf_Initialized
	second textbox: C:/Users/[username]/Space_Golf_Initialized/build
	
	- Press Configure and select your installed version of Visual Studio
	as the generator for this project
	- Leave the rest of the options empty and select 'Use default
	native compilers'
	- Press Generate 
	- Press Open Project
	- Run from Visual studio
	- Have fun!
