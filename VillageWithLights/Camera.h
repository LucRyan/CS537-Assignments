//Camera Class
//#include <S>

#define PI 3.141592653
#pragma once

#include "Angel.h"

class CCamera
{
public:
	CCamera( void );
	CCamera( Angel::vec4 &, Angel::vec4 &, int );
	~CCamera( void );
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void pitchUp();
	void pitchDown();
	void yawLeft();
	void yawLeft( float );
	void yawRight();
	void yawRight( float );
	void rollLeft();
	void rollRight();
	void setEye( Angel::vec4 & );
	Angel::mat4 getLookAt();
	Angel::vec4 getEye();
	Angel::vec4 getAt();
private:
	Angel::vec4 n, v, u, eye;
	Angel::mat4 lookAt;
	GLfloat scaleVector, scaleAngle, cosine, sine, distant;
};
