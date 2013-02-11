#include "stdafx.h"

float modelViewMatrix[16];
float projectionMatrix[16];
int viewport[4];

glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
glGetIntegerv(GL_VIEWPORT, viewport);

float nearPlaneLocation[3];
gluUnProject(x, y, 0.0, modelViewMatrix, projectionMatrix,
	viewport, &nearPlaneLocation[0], &nearPlaneLocation[1], 
	&nearPlaneLocation[2]);

float farPlaneLocation[3];
gluUnProject(x, y, 1.0, modelViewMatrix, projectionMatrix,
	viewPort, &farPlaneLocation[0], &farPlaneLocation[1],
	&farPlaneLocation[2]);

Vector3d nearPoint(nearPlaneLocation[0], nearPlaneLocation[1],
	nearPlaneLocation[2]);
Vector3d farPoint(farPlaneLocation[0], farPlaneLocation[1],
	farPlaneLocation[2]);