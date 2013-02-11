#include "stdafx.h"
#include "Models.h"
#include "Light.h"

int const NumVertices = 36; 

Model::Model(char* fileName, 
			 stack<mat4> *mvStack, 
			 vec4 transVec, vec3 rotVec, vec3 scalVec,
			 GLuint program, bool texFlag, GLubyte *textures ){

	_model = glmReadOBJ(fileName); //Loading the model
	if (!_model) exit(0); // See if it success

	glmUnitize(_model);	// Normalize vertices
	glmFacetNormals(_model); // Compute facet normals
	glmVertexNormals(_model, 90.0); // Compute vertex normals
	glmLinearTexture(_model); //Map the texture to Model
	glmLoadGroupsInVBO(_model); // Load the model (vertices and normals) into a vertex buffer

	_group = _model->groups;
	_mvStack = mvStack;
	_transVec = transVec;
	_rotVec = rotVec;
	_scalVec = scalVec;
	_program = program;
	_texFlag = texFlag;
	_textures = textures;
}

Model::~Model(){
	delete(_group); 
	delete(_mvStack);
	delete(_transVec); 
	delete(_rotVec );
	delete(_scalVec);
	delete(_textures);
}
	
void Model::render(){

	glUseProgram( _program );
	
	for ( int i = 0; i < _model->numgroups; ++i ) {
		glBindVertexArray( _model->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, _model->vbo[i] );

		//Set the vPosition
		GLuint vPosition = glGetAttribLocation( _program, "vPosition" );				// Get the vPosition location from Shader
		glEnableVertexAttribArray( vPosition );											// Enable vPosition Array
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) ); // 

		//Set the vNormal
		GLuint vNormal = glGetAttribLocation( _program, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*4*_model->groups->numtriangles*3) );

		//Set the vTexture Coordinates
		if(_texFlag){	
			GLuint vTexCoord = glGetAttribLocation( _program, "vTexCoord" );
			glEnableVertexAttribArray( vTexCoord );
			glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*4*_model->groups->numtriangles*3*2) );	
			glUniform1i( glGetUniformLocation(_program, "texture"), 0 );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, _textures );
		}

		//Set _materials and light
		_material = &_model->materials[_model->groups->material];
		Angel::vec4 ambient( _material->ambient[0], _material->ambient[1], _material->ambient[2], _material->ambient[3]);
		Angel::vec4 diffuse( _material->diffuse[0], _material->diffuse[1], _material->diffuse[2], _material->diffuse[3]);
		Angel::vec4 specular( _material->specular[0], _material->specular[1], _material->specular[2], _material->specular[3]);
		glUniform4fv( glGetUniformLocation(_program, "AmbientProduct"), 1,  distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(_program, "DiffuseProduct"), 1,  distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(_program, "SpecularProduct"), 1, distant_light_specular * specular );
		glUniform1f ( glGetUniformLocation(_program, "Shininess"), _material->shininess );
		glUniform4fv( glGetUniformLocation(_program, "LightPosition"), 1, distant_light_position );
	

		_mvStack->push(_mvStack->top());
		_mvStack->top() *= Angel::Translate( _transVec );
		_mvStack->top() *= Angel::RotateX( _rotVec.x );
		_mvStack->top() *= Angel::RotateY( _rotVec.y );
		_mvStack->top() *= Angel::RotateZ( _rotVec.z );
		_mvStack->top() *= Angel::Scale( _scalVec );

		glUniformMatrix4fv( glGetUniformLocation(_program, "NormalMatrix"), 1, GL_TRUE, 
							Angel::RotateX( _rotVec.x )*
							Angel::RotateY( _rotVec.y )*
							Angel::RotateZ( _rotVec.z )*
							Angel::Scale( 1.0/_scalVec.x, 1.0/_scalVec.y, 1.0/_scalVec.z ) );

		glUniformMatrix4fv(glGetUniformLocation( _program, "ModelView" ), 1, GL_TRUE, _mvStack->top());
	
		glDrawArrays( GL_TRIANGLES, 0, _model->groups->numtriangles*3 );	
		_group = _model->groups->next;
		_mvStack->pop();
	}
}
