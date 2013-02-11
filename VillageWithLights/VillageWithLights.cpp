// VillageWithLights  Lights, Texture, Models
//------------------------------------------------------------------------
// Control:
// ------------------------------------------------------------------------
// Car: 	
// ------------------------------------------------------------------------
// 	F   	- get off(automatically get off by reaching the area)
//    	V  	- switch camera
//    	Up	- move forward
// 	Down 	- move backward
// 	Right	- turn right
// 	Left	- turn left
// 	Q	- quit
// ------------------------------------------------------------------------
// Fly:
// ------------------------------------------------------------------------
// 	Z	- roll
// 	X	- pitch
// 	C	- yaw
// 	Up	- move forward
// 	Down 	- move backward
// 	Right	- move right
// 	Left	- move left
// 	F2	- turn off light in room
// 	Q	- quit
// ------------------------------------------------------------------------
// K
//#include <S>

#include "stdafx.h"
#include "Camera.h"
#include "World.h"

#define SCREENWIDTH 512
#define SCREENHEIGHT 512
#define PI 3.141592653

GLuint glutPro, arrayPro, skyPro, carPro, cubeMapPro; 
GLuint buffer;
GLuint vao;
GLuint vPosition, vNormal, vTexCoord;
Angel::mat4 projmat;  //projection matrix
stack<Angel::mat4> modelViewStack;  //model view matrix stack
CCamera camera	( Angel::vec4(0.0, 0.0, 1.0, 0.0), Angel::vec4(0.0, 1.0, 0.0, 0.0), 50 );
int switchCam = 2;  //0:fly 1:inCar 2:followCar
int index = 0;
bool switcher = true;
// Texture objects and storage for texture image
GLuint textures[2];
GLuint cubemap;
//GLubyte image[TextureSize][TextureSize][3];
GLubyte *grassTex, *skyTex, *roadTex, *castleTex, *wallTex; 
GLubyte *frontTex, *backTex, *leftTex, *rightTex, *topTex, *bottomTex;
float skyRotate = 0.0;
Angel::vec4 carPosition(0.0,-1.2,50.0,1.0);  //the terrain's high is -2.0
Angel::vec4 carForward(0.0,0.0,-1.0,0.0) ;
float carRotation = 0.0;
float carTurn = 3.0;

void reshape( int, int );
void init();
void display();
void idle();
void keyBoard( unsigned char, int, int );
void funcKey( int, int, int );
void m_glewInitAndVersion();
void traverse( t_ptr );  //traverse





int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( SCREENWIDTH, SCREENHEIGHT );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "Village" );
	glutFullScreen();
	m_glewInitAndVersion();
	init();
	
	glutReshapeFunc( reshape );
	glutDisplayFunc( display );
	glutIdleFunc(idle);
	glutKeyboardFunc( keyBoard );
	glutSpecialFunc( funcKey );

	glutMainLoop();
	return 0;
}

void init()
{	
	makeCube();	index = 0;

	glutPro = InitShader( "glutPro.v", "glutPro.f" );
	arrayPro = InitShader( "arrayPro.v", "arrayPro.f" );
	skyPro = InitShader( "skyPro.v", "skyPro.f" );
	carPro = InitShader( "carPro.v", "carPro.f" );
	cubeMapPro = InitShader( "cubeMap.v", "cubeMap.f" );

	glGenTextures( 2, textures );
	glGenTextures(1, &cubemap);

	Angel::mat4 modelView;  //tmp
	modelViewStack.push( modelView );  //stack

	glClearColor(0.5, 0.8, 0.9, 1.0);
	glClearDepth( 1.0f );
	glShadeModel( GL_FLAT );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	buildTree();
	loadTexture();
	loadModel();
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDepthMask( GL_TRUE );

	projmat = Angel::Perspective( 60.0, 1, 1, 200 ); 
	/*modelViewStack.top() = camera.getLookAt();*/

	glUseProgram( glutPro );
	glUniformMatrix4fv(glGetUniformLocation( glutPro, "Projection" ), 1, GL_TRUE, projmat);  //pass to shader
	glUniformMatrix4fv(glGetUniformLocation( glutPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());

	glUseProgram( arrayPro );
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "Projection" ), 1, GL_TRUE, projmat);  //pass to shader
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());

	glUseProgram( skyPro );
	glUniformMatrix4fv(glGetUniformLocation( skyPro, "Projection" ), 1, GL_TRUE, projmat);  //pass to shader
	glUniformMatrix4fv(glGetUniformLocation( skyPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	
	glUseProgram( carPro );
	glUniformMatrix4fv(glGetUniformLocation( carPro, "Projection" ), 1, GL_TRUE, projmat);  //pass to shader
	glUniformMatrix4fv(glGetUniformLocation( carPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());

	glUseProgram( cubeMapPro );
	glUniformMatrix4fv(glGetUniformLocation( cubeMapPro, "Projection" ), 1, GL_TRUE, projmat);  //pass to shader
	glUniformMatrix4fv(glGetUniformLocation( cubeMapPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());

	glPolygonMode(GL_FRONT, GL_FILL);
	if ( carPosition.z <= 0.0 )
		switchCam = 0;
	traverse( ptr_begin );  //begin traverse the tree
	
	glutSwapBuffers();
}

void reshape( int width, int height ) {
	glViewport( 0, 0, (GLsizei) width, (GLsizei) height );
}

void idle() {
	skyRotate += 0.05;
	glutPostRedisplay();
}
void keyBoard( unsigned char key, int x, int y ) {
	if ( switchCam == 0 ) {  //fly Cam
		switch(key) {
			case 'x': // pitch up
				camera.pitchUp();
				break;
			case 'X': // pitch down
				camera.pitchDown();
				break;
			case 'c': // yaw counter-clockwise in the un plane
				camera.yawRight();
				break;
			case 'C': // yaw clockwise in the un plane
				camera.yawLeft();
				break;
			case 'z': // roll clockwise in the uv plane
				camera.rollRight();
				break;
			case 'Z': // roll counterclockwise in the uv plane
				camera.rollLeft();
				break;
			case 033:  // Escape key
			case 'q': case 'Q':
				exit( EXIT_SUCCESS );
		}
	} else {
		switch(key) {
			case 'v': // switch cam
			case 'V':  // inCar/followCar
				if ( switchCam == 1)
					switchCam = 2;
				else
					switchCam = 1;
				break;
			case 'f':
			case 'F':
				switchCam = 0;
				break;
			case 033:  // Escape key
			case 'q': case 'Q':
				exit( EXIT_SUCCESS );
		}
	}
	glutPostRedisplay();
}

void funcKey( int key, int x, int y ) {
	if( switchCam == 0 ) {
		switch( key ) {
			case GLUT_KEY_UP: // MOVE Forward
				camera.moveForward();
				break;
			case GLUT_KEY_DOWN: // MOVE Backward
				camera.moveBackward();
				break;
			case GLUT_KEY_LEFT: // MOVE Left
				camera.moveLeft();
				break;
			case GLUT_KEY_RIGHT: // MOVE Right
				camera.moveRight();
				break;
			case GLUT_KEY_F2: // switch
				switcher = !switcher;
				break;
			default:
				break;
		}
	} else {
		switch( key ) {
			case GLUT_KEY_UP: // move forward		
				carPosition += carForward;
				break;
			case GLUT_KEY_DOWN: // move backward
				carPosition -= carForward;
				break;
			case GLUT_KEY_LEFT:
				carRotation += carTurn;
				camera.yawLeft( carTurn );
				carForward = Angel::vec4(-sin(carRotation/180*PI), 0, -cos(carRotation/180*PI), 0);
				break;
			case GLUT_KEY_RIGHT:
				carRotation -= carTurn;
				camera.yawRight( carTurn );
				carForward = Angel::vec4(-sin(carRotation/180*PI), 0, -cos(carRotation/180*PI), 0);
				break;
		}
	}
	glutPostRedisplay();
}

void m_glewInitAndVersion(void) {
	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void traverse(t_ptr root) {
	if(root==NULL) return;
	modelViewStack.push( modelViewStack.top() );  //pushMatrix();
	root->f();
	if(root->child!=NULL) traverse(root->child);
	modelViewStack.pop();  //popMatrix();
	if(root->sibling!=NULL) traverse(root->sibling);
}



