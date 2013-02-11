
#include "stdafx.h"
#include "Camera.h"
#include "LightAndMaterial.h"
#include "Models.h"

#include "World.h"

extern GLuint glutPro, arrayPro, skyPro, carPro, cubeMapPro; 
extern GLuint buffer;
extern GLuint vao;
extern GLuint vPosition, vNormal, vTexCoord;
extern Angel::mat4 projmat;  //projection matrix
extern stack<Angel::mat4> modelViewStack;  //model view matrix stack
extern CCamera camera;
extern int switchCam;  //0:fly 1:inCar 2:followCar
extern int index ;
extern bool switcher;
// Texture objects and storage for texture image
extern GLuint textures[2];
extern GLuint cubemap;
//GLubyte image[TextureSize][TextureSize][3];
extern GLubyte *grassTex, *skyTex, *roadTex, *castleTex, *wallTex; 
extern GLubyte *frontTex, *backTex, *leftTex, *rightTex, *topTex, *bottomTex;
extern float skyRotate;
extern Angel::vec4 carPosition;  //the terrain's high is -2.0
extern Angel::vec4 carForward;
extern float carRotation;
extern float carTurn;

void begin() {
}

void cam() {
	if ( switchCam == 1 ) {
		camera.setEye(Angel::vec4(carPosition.x + sin(carRotation/180*PI)*0.4, 
			carPosition.y + 0.8, carPosition.z + cos(carRotation/180*PI)*0.4, 1.0 ));
	}
	if ( switchCam == 2 ) {
		camera.setEye(Angel::vec4(carPosition.x + sin(carRotation/180*PI)*5.0,
			carPosition.y + 2.0, carPosition.z + cos(carRotation/180*PI)*5.0, 1.0 ));
	}
	modelViewStack.top() = camera.getLookAt();
	Print(modelViewStack.top());
}

void sky() {
	glUseProgram( skyPro );
	glBindVertexArray( skyDome->vao[0] );
	glBindBuffer( GL_ARRAY_BUFFER, skyDome->vbo[0] );
	vPosition = glGetAttribLocation( skyPro, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );
	vTexCoord = glGetAttribLocation( skyPro, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(GLfloat)*4*skyDome->numPointsInVBO 
		+ sizeof(GLfloat)*4*skyDome->numPointsInVBO) );
	glUniform1i( glGetUniformLocation(skyPro, "texture"), 0 );

	glBindTexture( GL_TEXTURE_2D, textures[0] );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0,
		GL_RGB, GL_UNSIGNED_BYTE, skyTex );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( camera.getEye().x, camera.getEye().y-0.3, camera.getEye().z, 0.0 ) );
	modelViewStack.top() *= Angel::RotateZ( 90.0 );
	modelViewStack.top() *= Angel::RotateX( skyRotate );
	modelViewStack.top() *= Angel::Scale( 2.0, 2.0, 2.0 );
	glUniformMatrix4fv(glGetUniformLocation( skyPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());

	glDisable( GL_DEPTH_TEST );
	glmDrawVBO(skyDome);
	glEnable( GL_DEPTH_TEST );
}

void ground() {
	glUseProgram( arrayPro );

	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	vPosition = glGetAttribLocation( arrayPro, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) ); 
	vNormal = glGetAttribLocation( arrayPro, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3) ); 
	vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3*2) );	
	glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
		GL_RGB, GL_UNSIGNED_BYTE, grassTex );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( 0.0, -2.001, 0.0, 0.0 ) );
	modelViewStack.top() *= Angel::Scale( 6.0, 0.01, 6.0 );
	glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
		Angel::Scale( 1/6.0, 1/0.01, 1/6.0 ) );
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, ground_a );
	glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, ground_d );
	glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, ground_s );
	glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), ground_sh );
	glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

	glDrawArrays(GL_TRIANGLES	, 0, NumCubeMesh); 
}

void car() {
	// 	if ( switchCam == 0 )  //if get off the car, don't render
	// 			return;
	glUseProgram( carPro );

	GLMgroup* group = carMod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < carMod->numgroups; ++i ) {
		glBindVertexArray( carMod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, carMod->vbo[i] );
		vPosition = glGetAttribLocation( carPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( carPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );

		material = &carMod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(carPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(carPro, "LightPosition"), 1, distant_light_position );
		if ( i == 14 ) {  //glass... don't know why I can't find it by name
			//glUniform1f( glGetUniformLocation(carPro, "alpha"), 0.5 ); 
			//the transparent materials of model are not correct
			glUniform1f( glGetUniformLocation(carPro, "alpha"), 1.0 ); 
		}
		else
			glUniform1f( glGetUniformLocation(carPro, "alpha"), 1.0 ); 

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( carPosition );
		modelViewStack.top() *= Angel::RotateY( -180.0 + carRotation );
		modelViewStack.top() *= Angel::Scale( 2.0, 2.0, 2.0 );
		glUniformMatrix4fv( glGetUniformLocation(carPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateY( -180.0 + carRotation )*Angel::Scale( 1.0/2.0, 1.0/2.0, 1.0/2.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( carPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void house0() {
	glUseProgram( arrayPro );

	GLMgroup* group = house0Mod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < house0Mod->numgroups; ++i ) {
		glBindVertexArray( house0Mod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, house0Mod->vbo[i] );
		vPosition = glGetAttribLocation( arrayPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( arrayPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );
		vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, 
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3*2) );	
		glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0,
			GL_RGB, GL_UNSIGNED_BYTE, castleTex );

		material = &house0Mod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 10.0, 12.5, -20.0, 0 ) );
		modelViewStack.top() *= Angel::Scale( 20.0, 20.0, 20.0 );
		glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::Scale( 1.0/20.0, 1.0/20.0, 1.0/20.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
	modelViewStack.top() *= Angel::Translate( Angel::vec4( 10.0, 12.5, -20.0, 0 ) );  //for children
}

void dude() {
	glUseProgram( carPro );

	GLMgroup* group = dudeMod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < dudeMod->numgroups; ++i ) {
		glBindVertexArray( dudeMod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, dudeMod->vbo[i] );
		vPosition = glGetAttribLocation( carPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( carPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );

		material = &dudeMod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(carPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(carPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 6.0, -0.8, -8.0, 0 ) );
		modelViewStack.top() *= Angel::RotateY( -30 );
		modelViewStack.top() *= Angel::Scale( 1.0, 1.0, 1.0 );
		glUniformMatrix4fv( glGetUniformLocation(carPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateY( 30 )*Angel::Scale( 1.0/1.0, 1.0/1.0, 1.0/1.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( carPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void tree0() {
	glUseProgram( arrayPro );

	GLMgroup* group = tree0Mod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < tree0Mod->numgroups; ++i ) {
		glBindVertexArray( tree0Mod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, tree0Mod->vbo[i] );
		vPosition = glGetAttribLocation( arrayPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( arrayPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );
		vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3*2) );	

		glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,GL_RGB, GL_UNSIGNED_BYTE, grassTex );

		material = &tree0Mod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 15.0, 3.0, -8.0, 0 ) );
		modelViewStack.top() *= Angel::RotateX( -90 );
		modelViewStack.top() *= Angel::Scale( 5.0, 5.0, 5.0 );
		glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateX( -90 )*Angel::Scale( 1.0/5.0, 1.0/5.0, 1.0/5.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void tree1() {
	glUseProgram( arrayPro );

	GLMgroup* group = tree0Mod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < tree0Mod->numgroups; ++i ) {
		glBindVertexArray( tree0Mod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, tree0Mod->vbo[i] );
		vPosition = glGetAttribLocation( arrayPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( arrayPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );
		vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, 
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3*2) );	

		glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
			GL_RGB, GL_UNSIGNED_BYTE, grassTex );

		material = &tree0Mod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 0.0, 3.0, -5.0, 0 ) );
		modelViewStack.top() *= Angel::RotateX( -90 );
		modelViewStack.top() *= Angel::RotateZ( 45 );
		modelViewStack.top() *= Angel::Scale( 5.0, 5.0, 5.0 );
		glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateX( -90 )*Angel::RotateZ( 45 )*Angel::Scale( 1.0/5.0, 1.0/5.0, 1.0/5.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void tree2() {
	glUseProgram( arrayPro );

	GLMgroup* group = tree0Mod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < tree0Mod->numgroups; ++i ) {
		glBindVertexArray( tree0Mod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, tree0Mod->vbo[i] );
		vPosition = glGetAttribLocation( arrayPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( arrayPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );
		vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, 
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3*2) );	

		glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
			GL_RGB, GL_UNSIGNED_BYTE, grassTex );

		material = &tree0Mod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( -15.0, 3.0, -20.0, 0 ) );
		modelViewStack.top() *= Angel::RotateX( -90 );
		modelViewStack.top() *= Angel::Scale( 5.0, 5.0, 5.0 );
		glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateX( -90 )*Angel::Scale( 1.0/5.0, 1.0/5.0, 1.0/5.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void tree3() {
	glUseProgram( arrayPro );

	GLMgroup* group = tree0Mod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < tree0Mod->numgroups; ++i ) {
		glBindVertexArray( tree0Mod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, tree0Mod->vbo[i] );
		vPosition = glGetAttribLocation( arrayPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( arrayPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );
		vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, 
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3*2) );	

		glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
			GL_RGB, GL_UNSIGNED_BYTE, grassTex );

		material = &tree0Mod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 20.0, 3.0, 0.0, 0 ) );
		modelViewStack.top() *= Angel::RotateX( -90 );
		modelViewStack.top() *= Angel::RotateZ( -30 );
		modelViewStack.top() *= Angel::Scale( 5.0, 5.0, 5.0 );
		glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateX( -90 )*Angel::RotateZ( -30 )*Angel::Scale( 1.0/5.0, 1.0/5.0, 1.0/5.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void road() {
	glUseProgram( arrayPro );

	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	vPosition = glGetAttribLocation( arrayPro, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) ); 
	vNormal = glGetAttribLocation( arrayPro, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3) ); 
	vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3*2) );	
	glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0,
		GL_RGB, GL_UNSIGNED_BYTE, roadTex );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( 10.0, -2.0, 25.0, 0.0 ) );
	modelViewStack.top() *= Angel::Scale( 0.5, 0.01, 3.0 );
	glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
		Angel::Scale( 1/10.0, 1/0.01, 1/10.0 ) );
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, wall_a );
	glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, wall_d );
	glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, wall_s );
	glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), wall_sh );
	glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, distant_light_position );

	glDrawArrays(GL_TRIANGLES	, 0, NumCubeMesh); 
}

void room0() {
	glUseProgram( arrayPro );

	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	vPosition = glGetAttribLocation( arrayPro, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) ); 
	vNormal = glGetAttribLocation( arrayPro, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3) ); 
	vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3*2) );	
	glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallTex );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( -10.5, -10.75, -0.5, 0.0 ) );
	modelViewStack.top() *= Angel::Scale( 0.35, 0.35, 0.35 );
	glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
		Angel::Scale( 1.0, 1.0, 1.0 ) );
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, pillar_a );
	glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, pillar_d );
	glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, pillar_s );
	glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), pillar_sh );
	glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, point_light_position );
	if ( !switcher ){
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, dark_a );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, dark_d );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, dark_s );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), dark_sh );
	}

	glDrawArrays(GL_TRIANGLES	, 0, NumCubeMesh); 
}

void vase() {
	glUseProgram( carPro );

	GLMgroup* group = vaseMod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < vaseMod->numgroups; ++i ) {
		glBindVertexArray( vaseMod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, vaseMod->vbo[i] );
		vPosition = glGetAttribLocation( carPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( carPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );

		material = &vaseMod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(carPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(carPro, "LightPosition"), 1, distant_light_position );
		if ( !switcher ){
			glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, dark_a );
			glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, dark_d );
			glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, dark_s );
			glUniform1f( glGetUniformLocation(carPro, "Shininess"), dark_sh );
		}

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( -3.0, -5.0, 0.0, 0 ) );
		modelViewStack.top() *= Angel::RotateY( -30 );
		modelViewStack.top() *= Angel::Scale( 3.0, 3.0, 3.0 );
		glUniformMatrix4fv( glGetUniformLocation(carPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateY( 30 )*Angel::Scale( 1.0/3.0, 1.0/3.0, 1.0/3.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( carPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
}

void venus() {
	glUseProgram( carPro );

	GLMgroup* group = venusMod->groups;
	GLMmaterial* material;

	for ( int i = 0; i < venusMod->numgroups; ++i ) {
		glBindVertexArray( venusMod->vao[i] );
		glBindBuffer( GL_ARRAY_BUFFER, venusMod->vbo[i] );
		vPosition = glGetAttribLocation( carPro, "vPosition" );
		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );
		vNormal = glGetAttribLocation( carPro, "vNormal" );
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(GLfloat)*4*group->numtriangles*3) );

		material = &venusMod->materials[group->material];
		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
		glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, 
			distant_light_ambient * ambient );
		glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, 
			distant_light_diffuse * diffuse );
		glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, 
			distant_light_specular * specular );
		glUniform1f( glGetUniformLocation(carPro, "Shininess"), 
			material->shininess );
		glUniform4fv( glGetUniformLocation(carPro, "LightPosition"), 1, distant_light_position );
		if ( !switcher ){
			glUniform4fv( glGetUniformLocation(carPro, "AmbientProduct"), 1, dark_a );
			glUniform4fv( glGetUniformLocation(carPro, "DiffuseProduct"), 1, dark_d );
			glUniform4fv( glGetUniformLocation(carPro, "SpecularProduct"), 1, dark_s );
			glUniform1f( glGetUniformLocation(carPro, "Shininess"), dark_sh );
		}

		modelViewStack.push(modelViewStack.top());
		modelViewStack.top() *= Angel::Translate( Angel::vec4( 4.0, -5.0, -1.0, 0 ) );
		modelViewStack.top() *= Angel::RotateY( -30 );
		modelViewStack.top() *= Angel::Scale( 3.0, 3.0, 3.0 );
		glUniformMatrix4fv( glGetUniformLocation(carPro, "NormalMatrix"), 1, GL_TRUE,
			Angel::RotateY( 30 )*Angel::Scale( 1.0/3.0, 1.0/3.0, 1.0/3.0 ) );
		glUniformMatrix4fv(glGetUniformLocation( carPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
		modelViewStack.pop();
		glDrawArrays( GL_TRIANGLES, 0, group->numtriangles*3 );	
		group = group->next;
	}
	modelViewStack.top() *= Angel::Translate( Angel::vec4( 4.0, -5.0, -1.0, 0 ) );
}

void transparentBall() {
	glUseProgram( glutPro );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( 0.0, 0.2, 0.0, 0 ) );
	modelViewStack.top() *= Angel::RotateY( -30 );
	modelViewStack.top() *= Angel::Scale( 3.0, 3.0, 3.0 );
	glUniformMatrix4fv( glGetUniformLocation(glutPro, "NormalMatrix"), 1, GL_TRUE,
		Angel::RotateY( 30 )*Angel::Scale( 1.0/3.0, 1.0/3.0, 1.0/3.0 ) );
	glUniformMatrix4fv(glGetUniformLocation( glutPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform4fv( glGetUniformLocation(glutPro, "AmbientProduct"), 1, snow_a );
	glUniform4fv( glGetUniformLocation(glutPro, "DiffuseProduct"), 1, snow_d );
	glUniform4fv( glGetUniformLocation(glutPro, "SpecularProduct"), 1, snow_s );
	glUniform1f( glGetUniformLocation(glutPro, "Shininess"), snow_sh );
	glUniform4fv( glGetUniformLocation(glutPro, "LightPosition"), 1, point_light_position );
	if ( !switcher ){
		glUniform4fv( glGetUniformLocation(glutPro, "AmbientProduct"), 1, dark_a );
		glUniform4fv( glGetUniformLocation(glutPro, "DiffuseProduct"), 1, dark_d );
		glUniform4fv( glGetUniformLocation(glutPro, "SpecularProduct"), 1, dark_s );
		glUniform1f( glGetUniformLocation(glutPro, "Shininess"), dark_sh );
	}
	glutSolidCube(2.0);
}

void room1() {
	glUseProgram( arrayPro );

	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	vPosition = glGetAttribLocation( arrayPro, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) ); 
	vNormal = glGetAttribLocation( arrayPro, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3) ); 
	vTexCoord = glGetAttribLocation( arrayPro, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubeMesh)/3*2) );	
	glUniform1i( glGetUniformLocation(arrayPro, "texture"), 0 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0,
		GL_RGB, GL_UNSIGNED_BYTE, wallTex );

	modelViewStack.top() *= Angel::Translate( Angel::vec4( 10.6, -10.75, -0.5, 0.0 ) );
	modelViewStack.top() *= Angel::Scale( 0.35, 0.35, 0.35 );
	glUniformMatrix4fv( glGetUniformLocation(arrayPro, "NormalMatrix"), 1, GL_TRUE,
		Angel::Scale( 1.0, 1.0, 1.0 ) );
	glUniformMatrix4fv(glGetUniformLocation( arrayPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, pillar_a );
	glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, pillar_d );
	glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, pillar_s );
	glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), pillar_sh );
	glUniform4fv( glGetUniformLocation(arrayPro, "LightPosition"), 1, point_light_position );
	if ( !switcher ){
		glUniform4fv( glGetUniformLocation(arrayPro, "AmbientProduct"), 1, dark_a );
		glUniform4fv( glGetUniformLocation(arrayPro, "DiffuseProduct"), 1, dark_d );
		glUniform4fv( glGetUniformLocation(arrayPro, "SpecularProduct"), 1, dark_s );
		glUniform1f( glGetUniformLocation(arrayPro, "Shininess"), dark_sh );
	}

	glDrawArrays(GL_TRIANGLES	, 0, NumCubeMesh); 
}

void cubeMap() {
	if(!switcher)
		return;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, rightTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, leftTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, topTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, bottomTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, frontTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ,0,GL_RGB,
		512,512,0,GL_RGB,GL_UNSIGNED_BYTE, backTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glUseProgram( cubeMapPro );
	modelViewStack.top() *= Angel::Translate( Angel::vec4( -5.0, 1.0, 7.0, 0.0 ) );
	modelViewStack.top() *= Angel::RotateX(180);
	glUniformMatrix4fv(glGetUniformLocation( cubeMapPro, "ModelView" ), 1, GL_TRUE, modelViewStack.top());
	glUniform1ui(glGetUniformLocation(cubeMapPro, "cubeMap"), cubemap);
	glutSolidSphere(2, 20, 20);
}

void buildTree() {
	//malloc
	ptr_begin = (t_ptr)malloc( sizeof( treenode ) );
	ptr_sky = (t_ptr)malloc( sizeof( treenode ) );
	ptr_car = (t_ptr)malloc( sizeof( treenode ) ); 
	ptr_ground = (t_ptr)malloc( sizeof( treenode ) );
	ptr_camera = (t_ptr)malloc( sizeof( treenode ) );
	ptr_house0 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_dude = (t_ptr)malloc( sizeof( treenode ) );
	ptr_tree0 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_tree1 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_tree2 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_tree3 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_road = (t_ptr)malloc( sizeof( treenode ) );
	ptr_room0 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_room1 = (t_ptr)malloc( sizeof( treenode ) );
	ptr_vase = (t_ptr)malloc( sizeof( treenode ) );
	ptr_venus = (t_ptr)malloc( sizeof( treenode ) );
	ptr_ball = (t_ptr)malloc( sizeof( treenode ) );
	ptr_cubeMap = (t_ptr)malloc( sizeof( treenode ) );

	ptr_begin->f = begin;
	ptr_begin->sibling = NULL;
	ptr_begin->child = ptr_camera;

	ptr_camera->f = cam;
	ptr_camera->sibling = NULL;
	ptr_camera->child = ptr_sky;

	ptr_sky->f = sky;
	ptr_sky->sibling = ptr_ground;
	ptr_sky->child = NULL;

	ptr_ground->f = ground;
	ptr_ground->sibling = ptr_house0;
	ptr_ground->child = NULL;

	ptr_house0->f = house0;
	ptr_house0->sibling = ptr_car;
	ptr_house0->child = ptr_room0;

	ptr_car->f = car;
	ptr_car->sibling = ptr_dude;
	ptr_car->child = NULL;

	ptr_dude->f = dude;
	ptr_dude->sibling = ptr_tree0;
	ptr_dude->child = NULL;

	ptr_tree0->f = tree0;
	ptr_tree0->sibling = ptr_tree1;
	ptr_tree0->child = NULL;

	ptr_tree1->f = tree1;
	ptr_tree1->sibling = ptr_tree2;
	ptr_tree1->child = NULL;

	ptr_tree2->f = tree2;
	ptr_tree2->sibling = ptr_tree3;
	ptr_tree2->child = NULL;

	ptr_tree3->f = tree3;
	ptr_tree3->sibling = ptr_road;
	ptr_tree3->child = NULL;

	ptr_road->f = road;
	ptr_road->sibling = NULL;
	ptr_road->child = NULL;

	ptr_room0->f = room0;
	ptr_room0->sibling = ptr_room1;
	ptr_room0->child = ptr_vase;

	ptr_vase->f = vase;
	ptr_vase->sibling = ptr_venus;
	ptr_vase->child = NULL;

	ptr_venus->f = venus;
	ptr_venus->sibling = NULL;
	ptr_venus->child = ptr_ball;

	ptr_ball->f = transparentBall;
	ptr_ball->sibling = ptr_cubeMap;
	ptr_ball->child = NULL;

	ptr_cubeMap->f = cubeMap;
	ptr_cubeMap->sibling = NULL;
	ptr_cubeMap->child = NULL;

	ptr_room1->f = room1;
	ptr_room1->sibling = NULL;
	ptr_room1->child = NULL;

}

void loadTexture() {
	skyTex = glmReadPPM("sky.ppm", 512, 512);
	grassTex = glmReadPPM("grass.ppm", 128, 128);
	roadTex = glmReadPPM("road.ppm", 128, 128);
	castleTex = glmReadPPM("castle256.ppm", 256, 256);
	wallTex = glmReadPPM("wall.ppm", 256, 256);

	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, textures[0] );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	// 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	// 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	// 	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );

	frontTex = glmReadPPM("front.ppm", 512, 512);
	backTex = glmReadPPM("back.ppm", 512, 512);
	leftTex = glmReadPPM("left.ppm", 512, 512);
	rightTex = glmReadPPM("right.ppm", 512, 512);
	topTex = glmReadPPM("top.ppm", 512, 512);
	bottomTex = glmReadPPM("bottom.ppm", 512, 512);

	//glEnable(GL_TEXTURE_CUBE_MAP);
}

void loadModel() {
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cubeMesh), cubeMesh, GL_STREAM_DRAW ); 

	//Loading the model
	skyDome=glmReadOBJ("sphere.obj");
	glmUnitize(skyDome);
	glmFacetNormals(skyDome);
	glmVertexNormals(skyDome, 90.0);
	glmSpheremapTexture(skyDome);
	glmLoadInVBO(skyDome);

	carMod=glmReadOBJ("porsche.obj");
	glmUnitize(carMod);
	glmFacetNormals(carMod);
	glmVertexNormals(carMod, 90.0);
	glmLinearTexture(carMod);
	glmLoadGroupsInVBO(carMod);

	house0Mod=glmReadOBJ("castle.obj");
	glmUnitize(house0Mod);
	glmFacetNormals(house0Mod);
	glmVertexNormals(house0Mod, 90.0);
	glmLinearTexture(house0Mod);
	glmLoadGroupsInVBO(house0Mod);

	dudeMod=glmReadOBJ("al.obj");
	glmUnitize(dudeMod);
	glmFacetNormals(dudeMod);
	glmVertexNormals(dudeMod, 90.0);
	glmLinearTexture(dudeMod);
	glmLoadGroupsInVBO(dudeMod);

	tree0Mod=glmReadOBJ("palmtree.obj");
	glmUnitize(tree0Mod);
	glmFacetNormals(tree0Mod);
	glmVertexNormals(tree0Mod, 90.0);
	glmLinearTexture(tree0Mod);
	glmLoadGroupsInVBO(tree0Mod);

	vaseMod=glmReadOBJ("flowers.obj");
	glmUnitize(vaseMod);
	glmFacetNormals(vaseMod);
	glmVertexNormals(vaseMod, 90.0);
	glmLinearTexture(vaseMod);
	glmLoadGroupsInVBO(vaseMod);

	venusMod=glmReadOBJ("venus.obj");
	glmUnitize(venusMod);
	glmFacetNormals(venusMod);
	glmVertexNormals(venusMod, 90.0);
	glmLinearTexture(venusMod);
	glmLoadGroupsInVBO(venusMod);

	// 	GLMgroup* group = carMod->groups;
	// 	GLMmaterial* material;
	// 	for ( int i = 0; i < carMod->numgroups; ++i ) {
	// 		cout << i << ": " << group->name << endl;
	// 		cout << group->numtriangles << endl;
	// 		cout << *group->triangles << endl;
	// 		material = &house0Mod->materials[group->material];
	// 		Angel::vec4 ambient( material->ambient[0], material->ambient[1], material->ambient[2], material->ambient[3]);
	// 		Angel::vec4 diffuse( material->diffuse[0], material->diffuse[1], material->diffuse[2], material->diffuse[3]);
	// 		Angel::vec4 specular( material->specular[0], material->specular[1], material->specular[2], material->specular[3]);
	// 		cout << ambient << " " << i << endl;
	// 		cout << diffuse << " " << i << endl;
	// 		cout << specular << " " << i << endl;
	// 		group = group->next;
	// 	}
}



void quad( int a, int b, int c, int d ) {
	Angel::vec4 n;
	n = normalize( cross(cube[b] - cube[a], cube[c] - cube[b]) );
	cubeMesh[index] = cube[a]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 0, 1, 0, 0 );
	index++;
	cubeMesh[index] = cube[b]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 0, 0, 0, 0 );
	index++;
	cubeMesh[index] = cube[c]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 1, 0, 0, 0 );
	index++;

	n = normalize( cross(cube[c] - cube[a], cube[d] - cube[c]) );
	cubeMesh[index] = cube[a]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 0, 1, 0, 0 );
	index++;
	cubeMesh[index] = cube[c]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 1, 0, 0, 0 );
	index++;
	cubeMesh[index] = cube[d]; 
	cubeMesh[index+NumCubeMesh] = n; 
	cubeMesh[index+NumCubeMesh*2] = Angel::vec4( 0, 0, 0, 0 );
	index++;
}

void makeCube() {
	quad( 1, 0, 3, 2 );
	quad( 2, 3, 7, 6 );
	quad( 3, 0, 4, 7 );
	quad( 6, 5, 1, 2 );
	quad( 4, 5, 6, 7 );
	quad( 5, 4, 0, 1 );
}
