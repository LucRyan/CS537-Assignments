//CS 537: G. Kamberov
// A GLSL "Small Labirynth" 
// The program opens two windows showing the labyrinth and a 
// scaled version version of it in separate windows
// The goal is to illustrate: 
//     Basic handling of multiple objects
//     Basic windows handling using freeGLUT (or GLUT)
//     The need to think about proper adjustment of the MVT 
//     
// The labyrinth is built using two 1D meshes (line strips) defined by vertices 
// on a 3 by 3 grid
//
// We use the default orthographic projection
//
//Requirements 
//GLSL version 110
//Angel's header files
//The Mac/Windows initShader.cpp
//Shaders (included in the distro): vshader00_v110.glsl and vshader00_v110.glsl
//
//



#include "stdafx.h"
void m_glewInitAndVersion(void);

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

GLfloat t1 = 1.0/3.0;
GLfloat t2 = 2.0/3.0;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

//two walls: one has 1 segment the other has 7 segments
GLuint buffers[2];
GLuint vPosition;

//wall segment 0
point4 points0[2] = {
    point4( 0.0, 0.0,  0.0, 1.0 ),
    point4( 1.0, 0.0,  0.0, 1.0 )
};

//wall segment 1
point4 points1[8] = {
    point4(  t2,  t1, 0.0, 1.0),
    point4( 0.0,  t1, 0.0, 1.0), 
	point4( 0.0, 1.0, 0.0, 1.0), 
	point4( 1.0, 1.0, 0.0, 1.0), 
	point4( 1.0,  t2, 0.0, 1.0), 
	point4(  t1,  t2, 0.0, 1.0), 
	point4( 1.0,  t2, 0.0, 1.0), 
	point4( 1.0,  t1, 0.0, 1.0) 
};

//initial state snakes
point4  snake1[16] = {
    point4( 0.0,  0.0, 0.0, 1.0),
    point4( 0.0,  t1, 0.0, 1.0), 
	point4( 0.0,  t2, 0.0, 1.0), 
	point4( 0.0, 1.0, 0.0, 1.0), 
	point4(  t1, 1.0, 0.0, 1.0),
	point4(  t1,  t2, 0.0, 1.0),
    point4(  t1,  t1, 0.0, 1.0),
    point4(  t1, 0.0, 0.0, 1.0),
    point4(  t2, 0.0, 0.0, 1.0), 
	point4(  t2,  t1, 0.0, 1.0), 
	point4(  t2,  t2, 0.0, 1.0), 
	point4(  t2, 1.0, 0.0, 1.0),
    point4( 1.0, 1.0, 0.0, 1.0), 
    point4( 1.0,  t2, 0.0, 1.0), 
    point4( 1.0,  t1, 0.0, 1.0), 
	point4( 1.0, 0.0, 0.0, 1.0)
};

point4  snake2[16] = {
    point4( 0.0,  0.0, 0.0, 1.0),
    point4(  t1, 0.0, 0.0, 1.0),
    point4(  t2, 0.0, 0.0, 1.0), 
    point4( 1.0, 0.0, 0.0, 1.0),
    point4( 1.0,  t1, 0.0, 1.0),
    point4(  t1,  t1, 0.0, 1.0),
	point4(  t2,  t1, 0.0, 1.0), 
    point4( 0.0,  t1, 0.0, 1.0), 
	point4( 0.0,  t2, 0.0, 1.0), 
	point4(  t1,  t2, 0.0, 1.0),
	point4(  t2,  t2, 0.0, 1.0), 
    point4( 1.0,  t2, 0.0, 1.0),
    point4( 1.0, 1.0, 0.0, 1.0),
    point4(  t2, 1.0, 0.0, 1.0),
	point4(  t1, 1.0, 0.0, 1.0),
	point4( 0.0, 1.0, 0.0, 1.0)
};

//selectable plate
point4 plate[4]= {
    point4(  t1,  t1, 0.0, 1.0),
	point4(  t2,  t1, 0.0, 1.0), 
	point4(  t2,  t2, 0.0, 1.0),
	point4(  t1,  t2, 0.0, 1.0)
};




// RGBA colors
color4 blue_opaque = color4( 0.0, 0.0, 1.0, 1.0 );

//uniform variable locations
GLuint color_loc;
GLuint model_view_loc;

mat4 ctmat = Angel::mat4(1.0);



//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUniform4fv(color_loc, 1, blue_opaque);
	
	ctmat = Angel::mat4(1.0)*Angel::Scale(0.5, 0.5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ctmat);

	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 16);

	glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 16);
	
	glFlush();
}

// OpenGL initialization
void
init()
{
	


    // Create and initialize a buffer object
    
    glGenBuffers( 2, &buffers[0] );
	
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(snake1),  snake1, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(snake2),  snake2, GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader00_v110.glsl", "fshader00_v110.glsl" );
    glUseProgram( program );

    
    vPosition = glGetAttribLocation( program, "vPosition" );
	
    glEnableVertexAttribArray( vPosition );
    
	
	color_loc = glGetUniformLocation(program, "color");
	model_view_loc = glGetUniformLocation(program, "modelview");

    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 0.0, 1.0 ); 
}

//----------------------------------------------------------------------------


void
display2( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUniform4fv(color_loc, 1, blue_opaque);
	
	ctmat = Angel::mat4(1.0)*Angel::Scale(0.5, 0.5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ctmat);

	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 16);

	glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 16);

	// glDisable( GL_DEPTH_TEST );

	color4 plate_color = color4(0.0, 1.0, 0.0, 1.0);
    glUniform4fv(color_loc, 1, plate_color);
	glBindBuffer( GL_ARRAY_BUFFER, buffers[2] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glDrawArrays( GL_LINE_STRIP, 0, 4);
	
	glFlush();
}


void init2()
{
	


    // Create and initialize a buffer object
    
    glGenBuffers( 3, &buffers[0] );
	
    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(snake1),  snake1, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(snake2),  snake2, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[2] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(plate),  plate, GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader00_v110.glsl", "fshader00_v110.glsl" );
    glUseProgram( program );

    
    vPosition = glGetAttribLocation( program, "vPosition" );
	
    glEnableVertexAttribArray( vPosition );
    
	
	color_loc = glGetUniformLocation(program, "color");
	model_view_loc = glGetUniformLocation(program, "modelview");

    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 0.0, 1.0 ); 
}

//----------------------------------------------------------------------------



int 
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize( 256, 256 );
/*
    int noscaled = glutCreateWindow( "CS 537 Idiot's Labyrinth" );
	glutInitWindowPosition(50, 50);
	m_glewInitAndVersion();
	init();
	glutDisplayFunc( display);
    glutKeyboardFunc( keyboard );
*/
	int roomSelect = glutCreateWindow( "CS 537 Delect a Room to Connect" );
	glutInitWindowPosition(450,450);
	m_glewInitAndVersion();
	init2();
	glutDisplayFunc( display2);
    glutKeyboardFunc( keyboard );

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