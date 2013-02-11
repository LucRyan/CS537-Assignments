//CS 537 Exmaple SL_3.3
//George Kamberov
//
//A simple 3D scene displaying: A large transparent green box and a 
// red wire inside the green box. 
//
// Interaction: 
//      The whole scene can be rotated around the x axis by pressing the X/x key 
//      W (or w) toggle: press the W (or w) to start/stop spinning the wire  at constant 
//      speed around the Y-axis 
//      
//      Use the up/down direction (arrow)  keys to Move the camera back/forward  
//      (along the fixed viewing axis). 
//
// We use perspective projection.
//
// Shaders used: 
//  vshader30_TwoCubes_FullPipe.glsl
//  fshader30_TwoCubes.glsl
//

#include "stdafx.h"
void m_glewInitAndVersion(void);
void reshape(int width, int height);

GLuint program; 

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint buffers[2];



const int NumVertices = 36; //  (6 faces)(2 triangles/face)(3 vertices/triangle)
int NumSegments =3; //number of line segments, can be variable

point4 points[NumVertices];
point4 curvePoints[5] ={
	point4(-0.5, 0.0, 0.0, 1.0),
	point4(0.5, 0.0, 0.0, 1.0),
	point4(0.5, 1.0, 0.0, 1.0),
	point4(0.22,0.22, 0.0, 1.0), 
	point4(1.5, 1.0, -1.0, 1.0)
};  //size should > NumSegments

GLfloat theta = 0;
GLfloat step = 20;
GLfloat theta_y = 0;
int spinToggle = 0; 
GLfloat d=0.5;
int w; int h;


// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};


// RGBA olors
color4 red_opaque = color4( 1.0, 0.0, 0.0, 1.0 );
color4 green_transparent = color4( 0.0, 1.0, 0.0, 0.1 ); 

//uniform variable locations
GLuint color_loc;
GLuint proj_loc;
GLuint model_view_loc;


//make sure you start with the default coordinate system
mat4 projmat = identity();

mat4 modelviewStackTop = Angel::mat4(1.0);
mat4 modelviewStackBottom = Angel::mat4(1.0);


//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;


void
quad( int a, int b, int c, int d )
{
points[Index] = vertices[a]; Index++;
points[Index] = vertices[b]; Index++;
points[Index] = vertices[c]; Index++;
points[Index] = vertices[a]; Index++;
points[Index] = vertices[c]; Index++;
points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
cube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	
    cube(); 


    // Create and initialize a buffer object
    glGenBuffers( 2, &buffers[0] );
	
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points),
		  points, GL_STATIC_DRAW );



   // Load shaders and use the resulting shader program
    program = InitShader( "vshader30_TwoCubes_FullPipe.glsl", "fshader30_TwoCubes.glsl" );
    glUseProgram( program );

	color_loc = glGetUniformLocation(program, "color");
	proj_loc       = glGetUniformLocation(program, "projection");
	model_view_loc = glGetUniformLocation(program, "modelview");

   
    glClearColor( 1.0, 1.0, 1.0, 0.0 ); 
	glClearDepth( 1.0 ); 
	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	//Connectiion with the shader
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );

	//Make sure you start with the Default Coordinate Systems
    projmat=Angel::mat4(1.0);
	modelviewStackTop = Angel::mat4(1.0);

	//Set up the camera optics
	projmat = projmat*Perspective(30,1.0,1.0,10.0); 
	glUniformMatrix4fv(proj_loc, 1, GL_TRUE, projmat);
	
    //Position and orient the camera
	modelviewStackTop = modelviewStackTop*Translate(0.0, 0.0, -d);

	/*****
    **** Build a dynamic scene
	*****/
	
	//Spin the whole scene by pressing the X key
	modelviewStackTop =  modelviewStackTop*RotateX( theta); 
	
	
	
	//Parse the scene graph preserving current states at branches as needed
	modelviewStackBottom= modelviewStackTop; //in lieu of glPush

	
	//Put A Red Wire (trajecory trace) in the world
    
	//Spin the wire
	modelviewStackTop = modelviewStackTop*RotateY( theta_y);
	//Scale the wire model to appropritae size
	modelviewStackTop =  modelviewStackTop*Scale(0.25, 0.25, 0.25); 
    glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, modelviewStackTop);

	//Make sure it is a red wire (we did not bother to use BO for this)
	glUniform4fv(color_loc, 1, red_opaque);

	//Start loading the wire -- The attributes in the wire vertices are in VBO buffers[1] 
	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
	//Tell the shader  to read  the vertex cords from a location  in the current buffer
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//Load  data in the buffer
    glBufferData( GL_ARRAY_BUFFER, sizeof(curvePoints), curvePoints, GL_STREAM_DRAW ); 
	//render the wire as a line strip
	glDrawArrays(GL_LINE_STRIP, 0, 4); 
	
    
	//Put A Transparent Green cube in the world. The cube contains the wire.
	
	modelviewStackTop = modelviewStackBottom; //in lieu of glPop
    glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, modelviewStackTop);

	//Enable and set up the transparency computations
	glDepthMask(GL_FALSE); //to deal with order of transparent  objects. Restore when done. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //this is default but just in case
	//Set up the collor attributes of the cube
	glUniform4fv(color_loc, 1, green_transparent);
	//Now bind the VBO that contains the cubes vertex coordinates
	glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
	//Tell the shader  to read  the vertex cords from a location  in the current buffer
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	//Render the cube as a  bunch of triangles.
	glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	
	glDepthMask(GL_TRUE);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 'x': {	theta += step; 
		if (theta > 360.0) {
		theta -= 360.0;
			}
			  }
	break;
	case 'X':{	theta -= step; 
		if (theta < - 360.0) {
		theta += 360.0;
			}
			 }
	break;



	case 'W': case 'w': 
		{
			if (spinToggle == 1) {spinToggle = 0;}
			else {spinToggle = 1;}
		}
	break;

	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
    }
}

void specKey(int key, int x, int y)
{ 
	switch( key ) {
	case GLUT_KEY_UP :{	d -= 0.5;} 
	break;
	case GLUT_KEY_DOWN :{	d += 0.5;} 
	break;
	}
}
void idle(void){

//Spin the wire or pause the spinning
theta_y += step*spinToggle; 
if (theta_y > 360.0) {
		theta_y -= 360.0;
		}
glutPostRedisplay();
}
//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	glutCreateWindow( "VBO: A red wire inside a cube" );
	m_glewInitAndVersion();
	

    init();

	glutReshapeFunc(reshape);
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutSpecialFunc( specKey );
	glutIdleFunc(idle);
	

    glutMainLoop();
    return 0;
}

void m_glewInitAndVersion(void)
{
   fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
   fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
   fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
   }
   fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void reshape( int width, int height )
//the same objects are shown (possibly scaled) w/o shape distortion 
//original viewport is a square
{

	glViewport( 0, 0, (GLsizei) width, (GLsizei) height );

}
