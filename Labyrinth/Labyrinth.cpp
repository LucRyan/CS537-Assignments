#include "stdafx.h"
#include "GLFont.h"

using namespace std;

#define MAX_CHAR        128
//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
int gridNum = 50; //Maxium Number is 50.
class Point2D { public: GLfloat x, y; };
class Line2D { public: Point2D start, end;};
class Square2D { public: Point2D bottomLeft, bottomRight, topLeft, topRight;};
Point2D *points = new Point2D[(gridNum + 1) * (gridNum + 1)];
Line2D *lines = new Line2D[gridNum * (gridNum + 1) * 2];
bool *linesValidation = new bool[gridNum * (gridNum + 1) * 2];
Square2D *squares = new Square2D[gridNum * gridNum];

int *outsideLines = new int[gridNum * 4]; //{0, 1, 2, 3, 16, 17, 18, 19, 20, 21, 22, 23, 36, 37, 38, 39};
int exitLine, entranceLine;
bool backgroundDraw = true;
bool fly = false;
int baseCell, selectedCell;
int collisionedLine = -1;
GLfloat jump;

list<Point2D> pointList, winList; //linklist to store vertice
enum Status {NORMAL, FINISHED, PAUSED, SUCCEED}; //status
float lastAngel; //record the angel when the point hits the edge
Status nowStatus, tmpStatus; //tmpStatus is a container for PAUSED

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void glInit(); 
void labyrinthInit();
void display();
void drawGrid();
void updateLabyrinth();
void eraseLine(int lineNumber);
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunction(int button, int state, int x, int y);
int getSquareNum(int x, int y);
int* getConnectCells(int cellNum);
int getConnectLine(int baseCell, int connectCell);
void generatePoint(Line2D line[]); 
void drawLines(list<Point2D> *pt);
int collisionDetect(Point2D p1, Point2D p2, Point2D p3, Point2D p4);
void fontInitialize();
void getGridNum();

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
int main(int argc, char **argv)
{	
	
	glutInit(&argc, argv);
	getGridNum();
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Labyrinth ver 1.9.5 -- 'R': Run and Restart. 'P': Pause. 'Q' and ESC: Quit.");

	labyrinthInit();
	glInit();

	glutMouseFunc(mouseFunction);
	glutKeyboardFunc(keyboardFunc);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

void getGridNum(){
	char answer;
	cout << "------------    Labyrinth ver 1.9.5     ---------------" << endl << "* "<< endl ;

	cout << "* Please input the Grids Number." << endl;
	cout << "* For Best Play Experience it will between 2 and 30: " << endl << "* "<< endl << "* ";
	cin >> gridNum;
	cout << "* "<<endl;

	if (gridNum > 30 && gridNum <= 50 || gridNum < 2 && gridNum > 0){
		
		cout << "* Are you sure about this? Y/N " << endl<< "* "<< endl << "* ";
		cin >> answer;
		cout << "* "<<endl;
		if (answer == 'n' || answer == 'N')
		{
			cout << "* Please input the Grids Number." << endl<< "* " << endl << "* ";
			int temp;
			cin >> temp;
			cout << "* "<<endl;
			if ( temp < 50 && temp > 0 ){
				gridNum = temp;
			}else
			{
				gridNum = 4;
				cout << "* The Number has been set to 4 as default." << endl<< "* " << endl << "* ";
			}
		}else
		{

		}
	}else if ( gridNum > 50 || gridNum < 1 ){
		cout << "* The Number must between 1 and 50." << endl<< "* " << endl << "* ";
		int temp = 0;
		cin >> temp;
		cout << "* "<<endl;
		if ( temp < 50 && temp > 0 ){
			gridNum = temp;
			cout<<"123";
		}else{
			gridNum = 4;
			cout << "* The Number has been set to 4 as default." << endl<< "* " << endl << "* ";
		}
	}
	cout << "-------------------------------------------------------" << endl << endl;
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void glInit() 
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, SCREENWIDTH, 0.0, SCREENHEIGHT);
	
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* rendererVersion = glGetString(GL_RENDERER);
	const GLubyte* OpenGLVersion =glGetString(GL_VERSION);
	const GLubyte* gluVersion= gluGetString(GLU_VERSION);

	cout << "Vendor: " << name << endl;
	cout << "Renderer: " << rendererVersion << endl;
	cout << "OpenGLVersion: " << OpenGLVersion << endl;
	cout << "gluVersion: " << gluVersion << endl;
}


//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void labyrinthInit()
{
	int count = 0;
	//Initialize the points.
	for(int i = 1; i <= ( 1 + gridNum ); i++){
		for(int j = 1; j <= gridNum + 1; j++){
			points[count].x = i * (SCREENWIDTH  / ( 2 + gridNum ));
			points[count].y = j * (SCREENHEIGHT / ( 2 + gridNum ));
			count++;
		}
	}

	count = 0;
	//Initialize the lines.
	for(int i = 0; i < gridNum + 1; i++){
		for(int j = 0; j < gridNum; j++){
			lines[count].start = points[i * ( 1 + gridNum ) + j];
			lines[count].end = points[i * ( 1 + gridNum ) + j + 1];
			linesValidation[count] = true;
			count++;
		}
	}

	for(int i = 0; i < ( 1 + gridNum ); i++){
		for(int j = 0; j <  gridNum ; j++){
			lines[count].start = points[i + ( 1 + gridNum ) * j];
			lines[count].end = points[i + ( 1 + gridNum ) * j + ( 1 + gridNum )];
			linesValidation[count] = true;
			count++;
		}
	}

	count = 0;
	//Initialize the Outside lines;
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < gridNum; j++){
			outsideLines[count] = j + (i * gridNum * gridNum) ;
			count++;
		}
		for(int j = 0; j < gridNum; j++){
			outsideLines[count] = j + (i * gridNum * gridNum) + (gridNum + 1) * gridNum ;
			count++;
		}
	}


	//Initialize the entrance and exit.
	srand(time(0));
	exitLine = rand() % ( 4 * gridNum);
	entranceLine = rand() % ( 4 * gridNum);
	while(exitLine == entranceLine){
		exitLine = rand() % ( 4 * gridNum);
		entranceLine = rand() % ( 4 * gridNum);
	}
	eraseLine(outsideLines[exitLine]);
	eraseLine(outsideLines[entranceLine]);

	//cout<< outsideLines[exitLine]<< "    " << outsideLines[entranceLine] <<endl;
	count = 0;

	//Initialize the Rectangles.
	for(int i = 0; i < gridNum; i++){
		for(int j = 0; j < gridNum; j++){
			squares[count].bottomLeft = points[(1 + gridNum ) * i + j];
			squares[count].bottomRight = points[(1 + gridNum ) * i + j + (1 + gridNum )];
			squares[count].topLeft = points[(1 + gridNum ) * i + j + 1];
			squares[count].topRight = points[(1 + gridNum ) * i + j + (2 + gridNum )];
			count++;
		}
	}

	jump = (SCREENWIDTH - (SCREENWIDTH / (gridNum + 2))) / 50.00;
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void display()
{
	if(backgroundDraw)
	glClear(GL_COLOR_BUFFER_BIT);
	
	if(!fly){
		drawGrid();
		updateLabyrinth();
	}

	if(fly){
		//glClear(GL_COLOR_BUFFER_BIT);
		drawGrid();
		updateLabyrinth();
		generatePoint(lines);
		drawLines(&pointList);
	}

	glFlush();
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void drawGrid(){
	
	backgroundDraw = false;
	glLineWidth(2);
	glColor3f(1.0, 0.0, 0.0);

	for(int i = 0; i < (gridNum + 1); i++){
		glBegin(GL_LINE_STRIP);
		for(int j = 0; j < (gridNum + 1); j++){
			glVertex2f(points[i * (gridNum + 1) + j].x, points[i * (gridNum + 1) + j].y);
		}
		glEnd();
		glBegin(GL_LINE_STRIP);
		for(int j = 0; j < (gridNum + 1); j++){
			glVertex2f(points[i + (gridNum + 1) * j].x, points[i + (gridNum + 1) * j].y);
		}
		glEnd();
	}
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void eraseLine(int lineNumber){
	linesValidation[lineNumber] = false;
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void updateLabyrinth(){
	for(int i = 0; i < (2 *((gridNum + 1) * gridNum )); i++){
		if(!linesValidation[i]){
			glLineWidth(2);
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_LINE_STRIP);
			glVertex2f(lines[i].start.x, lines[i].start.y);
			glVertex2f(lines[i].end.x, lines[i].end.y);
			glEnd();
		}
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void mouseFunction(int button, int state, int x, int y){
	int x1, y1, x2, y2;
	int	lineNum = -1;
	int* connectedCell;

	//Handle right button click.
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		x1 = x;
		y1 = SCREENHEIGHT - y;
		baseCell = getSquareNum(x1, y1);
		//cout << selectedCell << "    "<< baseCell<< endl;
		if (baseCell >= 0 && baseCell < gridNum * gridNum){
			connectedCell = getConnectCells(getSquareNum(x1, y1));
			glClear(GL_COLOR_BUFFER_BIT);
			for(int i = 0; i < 4; i++){
				if(connectedCell[i] <  gridNum * gridNum && connectedCell[i] >= 0){
					//cout<< connectedCell[i]<<endl;
					glColor3f(0.0, 1.0, 0.0);
					glBegin (GL_POLYGON);
					glVertex2f(squares[connectedCell[i]].bottomLeft.x, squares[connectedCell[i]].bottomLeft.y);
					glVertex2f(squares[connectedCell[i]].bottomRight.x, squares[connectedCell[i]].bottomRight.y);
					glVertex2f(squares[connectedCell[i]].topRight.x, squares[connectedCell[i]].topRight.y);
					glVertex2f(squares[connectedCell[i]].topLeft.x, squares[connectedCell[i]].topLeft.y);
					glEnd();
				}
			}
			glutPostRedisplay();
		}
	}

	//Handle left button click.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		x2 = x;
		y2 = SCREENHEIGHT - y;
		selectedCell = getSquareNum(x2, y2);
		//cout << selectedCell << "    "<< baseCell<< endl;
		glClear(GL_COLOR_BUFFER_BIT);
		if (selectedCell  >= 0 && selectedCell  <  gridNum * gridNum &&
			baseCell >= 0 && baseCell <  gridNum * gridNum){
			//cout << selectedCell << "    "<< baseCell<< endl;
			lineNum = getConnectLine(baseCell, selectedCell);
			//cout <<getConnectLine(baseCell, selectedCell)<< endl;
			//cout << lineNum << endl;
			if(lineNum <  ( gridNum * (gridNum + 1) * 2 ) && lineNum >=0){
				eraseLine(lineNum);
				glutPostRedisplay();
			}
		}
		baseCell = -1;
	}
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
int getSquareNum(int x, int y){
	for(int i = 0; i <  gridNum * gridNum ; i++){
		if( x - squares[i].bottomLeft.x > 0 && 
			y - squares[i].bottomLeft.y > 0 &&
			x - squares[i].topRight.x < 0 && 
			y - squares[i].topRight.y < 0 ){
				return i;
			}
	}
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
void keyboardFunc(unsigned char key, int x, int y) {
	switch (key) {  
		case 'r':  //reset
		case 'R':  
			//cout << "r" << endl;
			fly = true;
			glClear(GL_COLOR_BUFFER_BIT);
			pointList.clear(); 
			tmpStatus = NORMAL;
			nowStatus = NORMAL;
			lastAngel = 0.0;
			glutPostRedisplay();
			break;  
		case 'p':  //pause
		case 'P':  
			//cout << "p" << endl;
			if (nowStatus != PAUSED) { //use the container to store the nowStatus(normal or finished)
				tmpStatus = nowStatus;
				nowStatus = PAUSED;
			}
			else if (nowStatus == PAUSED) {
				nowStatus = tmpStatus;  //restore the status
				glutPostRedisplay();
			}
			//cout << nowStatus << endl;
			break; 
		case 27:
		case 'q':
		case 'Q':
			exit(0); 
			break;
		default:
			break;
	}  
}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//

int collisionDetect(Point2D p1, Point2D p2, Point2D p3, Point2D p4)
{
	if((p1.y - p2.y)/(p1.x-p2.x)==(p3.y - p4.y)/(p3.x-p4.x)&&(p1.y-p2.y)/(p1.x-p2.x)!=(p1.y-p4.y)/(p1.x-p4.x))   
         return 0;
    else
    {
		if(((p1.x-p3.x)*(p2.x-p4.x)>0&&(p1.x-p4.x)*(p2.x-p3.x)>0)||((p1.y-p3.y)*(p2.y-p4.y)>0&&(p1.y-p4.y)*(p2.y-p3.y)>0))  
        return 0;
        
		else
        return 1;
    }
}


//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//

void generatePoint(Line2D line[]) 
{
	if (pointList.empty()) //for the first time
	{
		Point2D p;  //the first point		
		while (1) {  //random x
			p.x = rand();
			//cout << p.x << endl;
			if (p.x < line[ gridNum * ( gridNum + 1 ) * 2 - 1].end.x && p.x > line[0].start.x)
				break;
		}
		while (1) {  //random y
			p.y = rand();
			//cout << p.y << endl;
			if (p.y < line[gridNum * ( gridNum + 1 ) * 2 - 1].end.y && p.y > line[0].start.y)
				break;
		}
		pointList.push_back(p);
	}
	
	//for the rest times
	if (nowStatus != NORMAL)
		return; //if PAUSED or FINISHED, no more generate->

	GLfloat angle = rand() % 360; //random an angle for a direction

	//cout<< angle << "   "  <<  cos(angle)*jump << endl;
	Point2D pt; //new point position
	pt.x = pointList.back().x + cos(angle)*jump; //new point with the random angel
	pt.y = pointList.back().y + sin(angle)*jump;
	//cout<< pt.x << "    "<< pt.y << endl;

	if( pt.x > line[gridNum * ( gridNum + 1 ) * 2 - 1].end.x || pt.x < line[0].start.x)
	{
		nowStatus = SUCCEED;
		//cout << endl << "Congratulations! You Win! The fly get out at (" << pt.x << "," << pt.y << "). " << endl;
	}else if( pt.y > line[gridNum * ( gridNum + 1 ) * 2 - 1].end.y || pt.y < line[0].start.y)
	{
		nowStatus = SUCCEED;
		//cout << endl << "Congratulations! You Win! The fly get out at (" << pt.x << "," << pt.y << "). " << endl;
	}
	for(int i = 0; i < gridNum * ( gridNum + 1 ) * 2 ; i++){
		if(linesValidation[i]){
			if(collisionDetect(pointList.back(), pt, line[i].start, line[i].end)) { //
				collisionedLine = i;
				//cout<< "collisioned Line:" << i << ".  " << endl;
				nowStatus = FINISHED; //finished
				lastAngel = angle; //record the hit angle, using for the last part of the line and the cross
			}
		}
	}
	pointList.push_back(pt);
}


void drawLines(list<Point2D> *pt){
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
		if( collisionedLine >= 0 && collisionedLine < gridNum * ( gridNum + 1 )){
			if( lastPoint.x <= lines[collisionedLine].end.x ||
				lastPoint.x >= lines[collisionedLine].end.x )
			{
				lastPoint.x = lines[collisionedLine].end.x;
				lastPoint.y = pt->back().y + tan(lastAngel)*(lastPoint.x-pt->back().x);
			}
		}
		if( collisionedLine >= gridNum * ( gridNum + 1 ) && collisionedLine < gridNum * ( gridNum + 1 ) * 2){
			if( lastPoint.y <= lines[collisionedLine].end.y ||
				lastPoint.y >= lines[collisionedLine].end.y )
			{
				lastPoint.y = lines[collisionedLine].end.y;
				lastPoint.x = pt->back().x + (lastPoint.y-pt->back().y)/tan(lastAngel);
			}
		}
		pt->push_back(lastPoint);
	}
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for ( list<Point2D>::iterator i = pt->begin(); i != pt->end(); ++i )
		glVertex2f(i->x,i->y);
	glEnd();

	//draw the last cross
	if(nowStatus == SUCCEED){
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


//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
int getConnectLine(int baseCell, int selectedCell){
	int lineNum = 0;

	for(int i = 0; i < gridNum * ( gridNum + 1 ) * 2 ; i++){
		if( lines[i].start.x == squares[baseCell].bottomLeft.x && // The line when we select LEFT connected cell
			lines[i].start.x == squares[selectedCell].bottomRight.x &&
			lines[i].start.y == squares[baseCell].bottomLeft.y &&
			lines[i].start.y == squares[selectedCell].bottomRight.y &&
			lines[i].end.x == squares[baseCell].topLeft.x &&
			lines[i].end.x == squares[selectedCell].topRight.x &&
			lines[i].end.y == squares[baseCell].topLeft.y &&
			lines[i].end.y == squares[selectedCell].topRight.y)
		{
			return lineNum = i;
		}

		if( lines[i].start.x == squares[baseCell].bottomRight.x && // The line when we select RIGHT connected cell
			lines[i].start.x == squares[selectedCell].bottomLeft.x &&
			lines[i].start.y == squares[baseCell].bottomRight.y &&
			lines[i].start.y == squares[selectedCell].bottomLeft.y &&
			lines[i].end.x == squares[baseCell].topRight.x &&
			lines[i].end.x == squares[selectedCell].topLeft.x &&
			lines[i].end.y == squares[baseCell].topRight.y &&
			lines[i].end.y == squares[selectedCell].topLeft.y)
		{
			return lineNum = i;
		}

		if( lines[i].start.x == squares[baseCell].topLeft.x &&  // The line when we select TOP connected cell
			lines[i].start.x == squares[selectedCell].bottomLeft.x &&
			lines[i].start.y == squares[baseCell].topLeft.y &&
			lines[i].start.y == squares[selectedCell].bottomLeft.y &&
			lines[i].end.x == squares[baseCell].topRight.x &&
			lines[i].end.x == squares[selectedCell].bottomRight.x &&
			lines[i].end.y == squares[baseCell].topRight.y &&
			lines[i].end.y == squares[selectedCell].bottomRight.y)
		{
			return lineNum = i;
		}

		if( lines[i].start.x == squares[baseCell].bottomLeft.x &&  // The line when we select TOP connected cell
			lines[i].start.x == squares[selectedCell].topLeft.x &&
			lines[i].start.y == squares[baseCell].bottomLeft.y &&
			lines[i].start.y == squares[selectedCell].topLeft.y &&
			lines[i].end.x == squares[baseCell].bottomRight.x &&
			lines[i].end.x == squares[selectedCell].topRight.x &&
			lines[i].end.y == squares[baseCell].bottomRight.y &&
			lines[i].end.y == squares[selectedCell].topRight.y)
		{
			return lineNum = i;
		}
	}

		

}

//-------------------------------------------------------------------------------//
//																				 //
//-------------------------------------------------------------------------------//
int* getConnectCells(int cellNum){
	int* connectedCells = new int[4];

	for(int i = 0; i < gridNum * gridNum; i++){
		if( squares[cellNum].bottomLeft.x == squares[i].bottomRight.x && //The left connected squre
			squares[cellNum].bottomLeft.y == squares[i].bottomRight.y &&
			squares[cellNum].topLeft.x == squares[i].topRight.x &&
			squares[cellNum].topLeft.y == squares[i].topRight.y)
		{
			connectedCells[0] = i;
		}
		if( squares[cellNum].bottomRight.x == squares[i].bottomLeft.x && //The Right connected squre
			squares[cellNum].bottomRight.y == squares[i].bottomLeft.y &&
			squares[cellNum].topRight.x == squares[i].topLeft.x &&
			squares[cellNum].topRight.y == squares[i].topLeft.y)
		{
			connectedCells[1] = i;
		}
		if( squares[cellNum].topRight.x == squares[i].bottomRight.x && //The Top connected squre
			squares[cellNum].topRight.y == squares[i].bottomRight.y &&
			squares[cellNum].topLeft.x == squares[i].bottomLeft.x &&
			squares[cellNum].topLeft.y == squares[i].bottomLeft.y)
		{
			connectedCells[2] = i;
		}
		if( squares[cellNum].bottomRight.x == squares[i].topRight.x && //The Top connected squre
			squares[cellNum].bottomRight.y == squares[i].topRight.y &&
			squares[cellNum].bottomLeft.x == squares[i].topLeft.x &&
			squares[cellNum].bottomLeft.y == squares[i].topLeft.y)
		{
			connectedCells[3] = i;
		}
	}

	return connectedCells;
	
}
