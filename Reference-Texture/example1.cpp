// rotating cube with threetexture objects
// change textures with 1, 2, and 3 keys


#include "Angel.h"
#include "glm.h"

const int  NumTriangles = 12; // (6 faces)(2 triangles/face)
const int  NumVertices  = 3 * NumTriangles;
const int  TextureSize  = 512;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

#include<iostream>
using namespace std;

// Texture objects and storage for texture image
GLuint textures[3];

//GLubyte image[TextureSize][TextureSize][3];
GLubyte* image0; 
GLubyte* image1;
GLubyte* image2;

// Vertex data arrays
point4  points[NumVertices];
color4  quad_colors[NumVertices];
vec2    tex_coords[NumVertices];

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLuint   theta;

//----------------------------------------------------------------------------

int Index = 0;

void
quad( int a, int b, int c, int d )
{
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

    color4 colors[8] = {
	color4( 0.0, 0.0, 0.0, 1.0 ),  // black
	color4( 1.0, 0.0, 0.0, 1.0 ),  // red
	color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),  // green
	color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),  // white
	color4( 1.0, 1.0, 1.0, 1.0 )   // cyan
    };

    quad_colors[Index] = colors[a];
    points[Index] = vertices[a];
    tex_coords[Index] = vec2( 0.0, 0.0 );
    Index++;
    
    quad_colors[Index] = colors[a];
    points[Index] = vertices[b];
    tex_coords[Index] = vec2( 0.0, 1.0 );
    Index++;
    
    quad_colors[Index] = colors[a];
    points[Index] = vertices[c];
    tex_coords[Index] = vec2( 1.0, 1.0 );
    Index++;
    
    quad_colors[Index] = colors[a];
    points[Index] = vertices[a];
    tex_coords[Index] = vec2( 0.0, 0.0 );
    Index++;
    
    quad_colors[Index] = colors[a];
    points[Index] = vertices[c];
    tex_coords[Index] = vec2( 1.0, 1.0 );
    Index++;
    
    quad_colors[Index] = colors[a];
    points[Index] = vertices[d];
    tex_coords[Index] = vec2( 1.0, 0.0 );
    Index++;
}

//----------------------------------------------------------------------------

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

void
init()
{
    colorcube();
   
       
    image0 = glmReadPPM("crate_texture.ppm", 512, 512);
	image1 = glmReadPPM("brks.ppm", 256, 256);
	image2 = glmReadPPM("checker.ppm", 128, 128);

    // Initialize texture objects
 
    glGenTextures( 3, textures );

	glActiveTexture( GL_TEXTURE0 ); //See p 469 Red Book 7th ed
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0,
		  GL_RGB, GL_UNSIGNED_BYTE, image0 );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glBindTexture( GL_TEXTURE_2D, textures[2] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
		  GL_RGB, GL_UNSIGNED_BYTE, image2 );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, textures[1] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0,
		  GL_RGB, GL_UNSIGNED_BYTE, image1 );
	glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );



    

   glActiveTexture( GL_TEXTURE0 );

    glBindTexture( GL_TEXTURE_2D, textures[2] );
   

    // Create a vertex array object
    GLuint vao;


   cout << "before" << endl;
if (!GLEW_ARB_vertex_array_object) 
     std::cerr << "ARB_vertex_array_object not available." << std::endl; 

   glGenVertexArrays( 1, &vao );


    cout << "after" << endl;
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER,
		  sizeof(points) + sizeof(quad_colors) + sizeof(tex_coords),
		  NULL, GL_STATIC_DRAW );
    glBindVertexArray( vao );
    // Specify an offset to keep track of where we're placing data in our
    //   vertex array buffer.  We'll use the same technique when we
    //   associate the offsets with vertex attribute pointers.
    GLintptr offset = 0;
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points), points );
    offset += sizeof(points);

    glBufferSubData( GL_ARRAY_BUFFER, offset,
		     sizeof(quad_colors), quad_colors );
    offset += sizeof(quad_colors);
    
    glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader71.glsl", "fshader71.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    offset = 0;
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );
    offset += sizeof(points);

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );
    offset += sizeof(quad_colors);

    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );

    // Set the value of the fragment shader texture sampler variable
    //   ("texture") to the the appropriate texture unit. In this case,
    //   zero, for GL_TEXTURE0 which was previously set by calling
    //   glActiveTexture().
    glUniform1i( glGetUniformLocation(program, "texture"), 0 );

    theta = glGetUniformLocation( program, "theta" );
    
    glEnable( GL_DEPTH_TEST );
    
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniform3fv( theta, 1, Theta );

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN ) {
	switch( button ) {
	    case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
	    case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
	    case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
	}
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
    Theta[Axis] += 0.01;

    if ( Theta[Axis] > 360.0 ) {
	Theta[Axis] -= 360.0;
    }
    
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int mousex, int mousey )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	case '1':
		glActiveTexture( GL_TEXTURE0 );
	    glBindTexture( GL_TEXTURE_2D, textures[0] );
	    break;

	case '2':
		glActiveTexture( GL_TEXTURE0 );
	    glBindTexture( GL_TEXTURE_2D, textures[1] );
	    break;

	case '3':
		glActiveTexture( GL_TEXTURE1 );
	    glBindTexture( GL_TEXTURE_2D, textures[2] );
	    break;
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

// Get OpenGL version information
void getGLinfo()
{
  cout << "GL Vendor   : " << glGetString(GL_VENDOR) << endl;
  cout << "GL Renderer : " << glGetString(GL_RENDERER) << endl;
  cout << "GL Version  : " << glGetString(GL_VERSION) << endl;
}

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Textured Cube" );
    getGLinfo();
  glewInit();
    init();
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    glutIdleFunc( idle );

    glutMainLoop();
    return 0;
}
