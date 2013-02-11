
/***********************************************************************************************************************
*  CS 537: Loading OBJ  models in VBOs
*  Requires support for VBOs and VAOs
*  Code base: Nate Robins Tutors 
*  
* Loads a model from an obj file using GLM library and then loads all the vertices and normals into a VBO.
* Renders object using the VBO.
*
* Note: The extension only loads vertices and normals. Does not recognize groups and does not load UV or materials.
* You have to get this data yourself from the model struct.
*
* Shaders: pFshader_vert.glsl, pFshader_frag.glsl
*
*
***********************************************************************************************************************/

#include <iostream>
#include "Angel.h"
#include "glm.h"

using namespace std;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLMmodel* pmodel = NULL;
int VBO_num_vert;
int VBO_num_norm;
int num_vert;

#define SCALE_VECTOR    0.5

GLuint vPosition;
GLuint vNormal;
int winHeight = 480;
int winWidth = 640;
bool mouseDown = false;
float xrot = 0;
float yrot = 0;
float xdiff = 0;
float ydiff = 0;
GLfloat step = 5;
// Camera Coordianate System
vec4 u = vec4(1, 0, 0, 0);
vec4 v = vec4(0, 1, 0, 0);
vec4 n = vec4(0, 0, 1, 0);
vec4 eye = vec4(0,2,10,1);

// Model-view and projection matrices uniform location
GLuint  ModelViewCam, ModelViewObj, Projection, LightPosition, NormalTransformation;

// OpenGL initialization
void init()
{
  // Load the OBJ model from file
  //pmodel = glmReadOBJ("data/blender_cube_triangles.obj");
  pmodel = glmReadOBJ("data/al.obj");   /* Load the little dude model */
  //pmodel = glmReadOBJ("data/soccerball.obj");
  //pmodel = glmReadOBJ("data/girl.obj");

  if (!pmodel) exit(0);
  // Normilize vertices
  glmUnitize(pmodel);
  // Compute facet normals
  glmFacetNormals(pmodel);
  // Comput vertex normals
  glmVertexNormals(pmodel, 90.0);
  // Load the model (vertices and normals) into a vertex buffer
  glmLoadInVBO(pmodel);

  // Setup some sample materials
  color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
  color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
  color4 light_specular( 1.0, 1.0, 1.0, 1.0 );
  color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
  color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
  color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
  float  material_shininess = 100.0;
  color4 ambient_product = light_ambient * material_ambient;
  color4 diffuse_product = light_diffuse * material_diffuse;
  color4 specular_product = light_specular * material_specular;
  //LIGHT POSITION
  vec4 light_position_distant = vec4(10.0, 2.0, 0.0, 1.0);

  // Load shaders and use the resulting shader program
  GLuint program = InitShader( "pFshader_vert.glsl", "pFshader_frag.glsl" );
  glUseProgram( program );
  // set up vertex arrays
  glBindVertexArray( pmodel->vao );
  vPosition = glGetAttribLocation( program, "in_vertex" );
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
  vNormal = glGetAttribLocation( program, "in_normal" ); 
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			 BUFFER_OFFSET(sizeof(GLfloat)*4*pmodel->numPointsInVBO));

  glUniform4fv( glGetUniformLocation(program, "mat_ambient"),1, ambient_product );
  glUniform4fv( glGetUniformLocation(program, "mat_diffuse"),1, diffuse_product );
  glUniform4fv( glGetUniformLocation(program, "mat_specular"),1, specular_product );	
  glUniform4fv( glGetUniformLocation(program, "lightPosition"),1, light_position_distant );
  glUniform1f( glGetUniformLocation(program, "mat_shininess"),material_shininess );
  // Retrieve transformation uniform variable locations
  ModelViewCam = glGetUniformLocation( program, "modelView" );
  ModelViewObj = glGetUniformLocation(program, "ModelViewObj");
  Projection = glGetUniformLocation( program, "projection" );
  NormalTransformation = glGetUniformLocation( program, "normalTransformation" );

  glEnable( GL_DEPTH_TEST );
  glShadeModel(GL_FLAT);
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void display( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4 model_view = Angel::identity();
  model_view = model_view * LookAt(eye, eye-n, v);
  glUniformMatrix4fv(ModelViewCam, 1, GL_TRUE, model_view);
  model_view = Angel::identity();

  float scale_x, scale_y, scale_z;
  scale_x = 2;
  scale_y = 2;
  scale_z = 2;

  // Scale Transformation Matrix
  mat4 scaleTransformation = Scale(scale_x, scale_y, scale_z);
  // Inverse Scale Transformation Matrix 
  mat4 invScaleTranformation = Scale(1/scale_x, 1/scale_y, 1/scale_z);
  
  mat4 normalTransformation;
  normalTransformation =  RotateX( xrot ) * RotateY( yrot ) * invScaleTranformation;

  model_view = model_view * scaleTransformation * RotateX( xrot ) * RotateY( yrot );
  glUniformMatrix4fv( ModelViewObj, 1, GL_TRUE, model_view );
  glUniformMatrix4fv(NormalTransformation , 1, GL_TRUE,  normalTransformation);
  glmDrawVBO(pmodel);
  glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y )
{
  switch( key ) {
  case 033: // Escape Key
  case 'q': case 'Q':
    exit( EXIT_SUCCESS );
  break;
  }
  glutPostRedisplay();
}

//----------------------------------------------------------------------------

void reshape( int width, int height )
{
  GLfloat aspect = GLfloat(width)/height;
  mat4  projection = Perspective( 45.0, aspect, 0.0001, 300.0 );
  glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

void specialKeys(int key, int x, int y)
{
  switch( key ) {
  case GLUT_KEY_UP: // MOVE FORWARD
    eye[0] -= SCALE_VECTOR * n[0];
    eye[1] -= SCALE_VECTOR * n[1];
    eye[2] -= SCALE_VECTOR * n[2];
    break;
  case GLUT_KEY_DOWN: // MOVE Backward
    eye[0] += SCALE_VECTOR * n[0];
    eye[1] += SCALE_VECTOR * n[1];
    eye[2] += SCALE_VECTOR * n[2];
    break;
  case GLUT_KEY_RIGHT: // MOVE right
    eye[0] += SCALE_VECTOR * u[0];
    eye[1] += SCALE_VECTOR * u[1];
    eye[2] += SCALE_VECTOR * u[2];
    break;
  case GLUT_KEY_LEFT: // MOVE left
    eye[0] -= SCALE_VECTOR * u[0];
    eye[1] -= SCALE_VECTOR * u[1];
    eye[2] -= SCALE_VECTOR * u[2];
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void mouseMotionCB(int x, int y)
{
  if (mouseDown)
    {
      yrot = x - xdiff;
      xrot = y + ydiff;
      glutPostRedisplay();
    }
}
//----------------------------------------------------------------------------
void mouseCB(int button, int state, int x, int y)
{
  switch(button){
  case GLUT_LEFT_BUTTON:
    switch(state)
      {
      case GLUT_DOWN:
	mouseDown = true;
	xdiff = x - yrot;
	ydiff = -y + xrot;
	break;
      case GLUT_UP:
	mouseDown = false;
	break;
      default:
	break;
      }
    break;    
  case GLUT_MIDDLE_BUTTON:
    break;
  default:
    break;
  }
}

// Get OpenGL version information
void getGLinfo()
{
  cout << "GL Vendor   : " << glGetString(GL_VENDOR) << endl;
  cout << "GL Renderer : " << glGetString(GL_RENDERER) << endl;
  cout << "GL Version  : " << glGetString(GL_VERSION) << endl;
}

int main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( winWidth, winHeight );
  glutCreateWindow( "Loading OBJ Models into VBO" );
  glewInit();
  getGLinfo();
  init();
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutReshapeFunc( reshape );
  glutMouseFunc(mouseCB);
  glutMotionFunc(mouseMotionCB);
  glutMainLoop();
  return 0;
}
