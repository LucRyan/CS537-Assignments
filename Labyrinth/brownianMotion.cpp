#include "stdafx.h"

using namespace std;

GLfloat boxWidth = 450.0; //box
GLfloat boxHeight = 450.0;
GLfloat jump = ((boxWidth > boxHeight) ? boxHeight : boxWidth)/50; // 1/50 the shortest edge...
class Point2D { public: GLfloat x, y; };
list<Point2D> pointList; //linklist to store vertice
enum Status {NORMAL, FINISHED, PAUSED}; //status
float lastAngel; //record the angel when the point hits the edge
Status nowStatus, tmpStatus; //tmpStatus is a container for PAUSED


void generatePoint(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) 
{
	//srand(time(0));
	if (pointList.empty()) //for the first time
	{
		Point2D p;  //the first point		
		while (1) {  //random x
			p.x = rand();
			//cout << p.x << endl;
			if (p.x>x1 && p.x<x2)
				break;
		}
		while (1) {  //random y
			p.y = rand();
			//cout << p.y << endl;
			if (p.y<y1 && p.y>y2)
				break;
		}
		/*
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(4);
		glBegin(GL_POINTS);
			glVertex2f(pt.x,pt.y);
		glEnd();
		*/
		pointList.push_back(p);
		//cout << pointList.front().x << endl;
		//cout << pointList.front().y << endl;
	}
	
	//for the rest times
	if (nowStatus != NORMAL)
		return; //if PAUSED or FINISHED, no more generate->
	
	GLfloat angle = rand()%360; //random an angle for a direction
	Point2D pt; //new point position
	pt.x = pointList.back().x + cos(angle)*jump; //new point with the random angel
	pt.y = pointList.back().y + sin(angle)*jump;
	
	if(pt.x<=x1||pt.x>=x2||pt.y>=y1||pt.y<=y2) { //if new point is out off box
		nowStatus = FINISHED; //finished
		lastAngel = angle; //record the hit angle, using for the last part of the line and the cross
	}
	pointList.push_back(pt);
}



void drawLines(list<Point2D> *pt, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	Point2D lastPoint;  //record last point
	
	//draw first big point
	glPointSize(4); 
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POINTS);  
	glVertex2f(pt->front().x, pt->front().y);
	glEnd();
	glPointSize(1);

	//draw lines
	if(nowStatus == FINISHED) {
		//cout << "finished";
		lastPoint = pt->back();
		pt->pop_back();
		//make the hit point be the last point
		if(lastPoint.x <= x1) {
			lastPoint.x = x1;
			lastPoint.y = pt->back().y + tan(lastAngel)*(lastPoint.x-pt->back().x);
		}
		if(lastPoint.x >= x2) {
			lastPoint.x = x2;
			lastPoint.y = pt->back().y + tan(lastAngel)*(pt->back().x-lastPoint.x);
		}
		if(lastPoint.y >= y1) {
			lastPoint.y = y1;
			lastPoint.x = pt->back().x + (lastPoint.y-pt->back().y)/tan(lastAngel);
		}
		if(lastPoint.y <= y2) {
			lastPoint.y = y2;
			lastPoint.x = pt->back().x + (pt->back().y-lastPoint.y)/tan(lastAngel);
		}
		pt->push_back(lastPoint);
	}
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for ( list<Point2D>::iterator i = pt->begin(); i != pt->end(); ++i )
		glVertex2f(i->x,i->y);
	glEnd();

	//draw the last cross
	if (nowStatus == FINISHED) {
		glLineWidth(2);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2f(pt->back().x-2.0, pt->back().y+2.0);
			glVertex2f(pt->back().x+2.0, pt->back().y-2.0);	
		glEnd();
		glBegin(GL_LINES);
			glVertex2f(pt->back().x-2.0, pt->back().y-2.0);
			glVertex2f(pt->back().x+2.0, pt->back().y+2.0);
		glEnd();
		glLineWidth(1);
	}

	//draw pause point
	if (nowStatus == PAUSED){
		glPointSize(4); 
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_POINTS);  
		glVertex2f(pt->back().x, pt->back().y);
		glEnd();
		glPointSize(1);
	}

	//do the refresh
	if (nowStatus == NORMAL)
		glutPostRedisplay();
}
