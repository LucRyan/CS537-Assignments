#include "glm.h"

typedef Angel::vec4  point4;

typedef struct treenode  //data structure of objects in scene
{
	void ( *f )();  //transform and render function
	struct treenode *sibling;
	struct treenode *child;
} treenode, *t_ptr; 

//point to objects
t_ptr ptr_begin, ptr_car, ptr_camera, ptr_sky, ptr_ground;
t_ptr ptr_house0, ptr_dude, ptr_tree0, ptr_tree1, ptr_tree2, ptr_tree3, ptr_road, ptr_room0, ptr_room1;
t_ptr ptr_vase, ptr_venus, ptr_ball, ptr_cubeMap;
GLMmodel *skyDome, *carMod, *house0Mod, *dudeMod, *tree0Mod, *vaseMod, *venusMod;

const int NumHillMesh = 3;
point4 hillMesh[NumHillMesh*2] = {
	//vertex	
	point4(5.0, 0.0, 0.0, 1.0),
	point4(0.0, 10.0, 0.0, 1.0),
	point4(-5.0, 0.0, 0.0, 1.0),

	//normal
	point4(0.0, 0.0, 1.0, 0.0),
	point4(0.0, 0.0, 1.0, 0.0),
	point4(0.0, 0.0, 1.0, 0.0)
};

const int NumHouseMesh = 36;
point4 houseMesh[NumHouseMesh*2];
point4 house[8]= {
	point4( -10, -10,  10, 1.0 ),  //0
	point4( -10,  10,  10, 1.0 ),  //	1	
	point4(  10,  10,  10, 1.0 ),  //2
	point4(  10, -10,  10, 1.0 ),	//3
	point4( -10, -10, -10, 1.0 ),	//4
	point4( -10,  10, -10, 1.0 ),	//5
	point4(  10,  10, -10, 1.0 ),	//6
	point4(  10, -10, -10, 1.0 )	//7
};

const int NumCubeMesh = 36;
point4 cubeMesh[NumCubeMesh*3];
point4 cube[8]= {
	point4( -10, -10,  10, 1.0 ),  //0
	point4( -10,  10,  10, 1.0 ),  //	1	
	point4(  10,  10,  10, 1.0 ),  //2
	point4(  10, -10,  10, 1.0 ),	//3
	point4( -10, -10, -10, 1.0 ),	//4
	point4( -10,  10, -10, 1.0 ),	//5
	point4(  10,  10, -10, 1.0 ),	//6
	point4(  10, -10, -10, 1.0 )	//7
};
