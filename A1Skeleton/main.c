#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>
#include "Vector3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"
#include "Matrix3D.h"

#define PI 3.141592654
#define E 2.71828

//Constants
const int vWidth = 1000;
const int vHeight = 1000;
const int meshSize = 40;

//Camera Variables
float Xangle = 0.0; //Xangle of rotation
float Yangle = 0.0; //Yangle of rotation
float lx = 0.0, ly = 0.0, lz = -1; //camera's direction
float x = 0.0, y = 10, z = 30.0; // XZ position
float deltaXAngle = 0.0f;
float deltaYAngle = 0.0f;
float deltaMove = 0;
float deltaXMove = 0;
int xOrigin = -1;
int yOrigin = -1;
int cameraSwitch = 0;

//Camera2 Variables
float cx = 0.0, cy = 0.0, cz = 0.0;

//Hero Variables
float heroAngle = 0.0;
float hlx = 0.0, hlz = 0.0;
float heroX = 0.0, heroY = 0.0, heroZ = 0.0;
float speed = 0.1;


// Light positions
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };

//Light properties
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

//Foe properties
static GLfloat foe_diffuse[] = { 0.243, 0.635, 0.956, 1.0 };
static GLfloat foe_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat foe_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

//hero properties
static GLfloat hero_diffuse[] = { 0.960, 0.078, 0, 1.0 };
static GLfloat hero_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat hero_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

//A quad mesh object, given by the provided QuadMesh.c file
static QuadMesh groundMesh;

//list of hole positions
Vector3D holes[3];

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int mx, int my);
void functionKeys(int key, int x, int y);
void releaseKey(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void drawFoe(void);
void drawHero(void);
void drawAxes(void);

//global camera functions
void computePosition(float deltaMove);
void computeXPosition(float deltaMove);

//player camera function
void computePosition2();

//drawHero helper functions
void drawBicep(void);
void drawForearm(void);
void drawHead(void);
void drawWheelnAxle(void);

void printInstructions(void);


int main(int argc, char** argv)
{

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 1000) / 2, 0);
	glutCreateWindow("Assignment 3");


	initOpenGL(vWidth, vHeight);

	//Register Callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutIdleFunc(display);

	glutSpecialUpFunc(releaseKey);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	//Enter GLUT event processing cycle
	glutMainLoop();
	return 1;
}

//Set up OpenGL. Sets up lighting, other OpenGL stuff, and the ground floor
void initOpenGL(int w, int h)
{
	//Light1 Properties setup
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	//Light2 Properties setup, which is the same as Light1
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	//Light1 and Light2 Position setup
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	//Enable Lights
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.498f, 0.737f, 0.843f, 0.0f);// Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	//Ground setup
	Vector3D origin = NewVector3D(-(meshSize / 2), 0.0f, (meshSize / 2)); //doesn't refer to centroid, refers to one of the corners of the mesh
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.176f, 0.823f, 0.274f);
	Vector3D specular = NewVector3D(0.4f, 0.04f, 0.04f);

	//List of holes
	holes[0] = NewVector3D(10, 0, 10);
	holes[1] = NewVector3D(-10, 0, 5);
	holes[2] = NewVector3D(-15, 0, -15);

	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, meshSize, meshSize, dir1v, dir2v);
	memcpy(groundMesh.holes, holes,sizeof(groundMesh.holes));
	//b, a
	ComputeGauss(&groundMesh,-10.0f, 20.0f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
}

//sets up camera, sets materials and does the modeling transforms
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	if (deltaMove)
		computePosition(deltaMove);
	if (deltaXMove)
		computeXPosition(deltaXMove);
	
	if(cameraSwitch == 0)
		gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
	if (cameraSwitch == 1) {
		computePosition2();
		gluLookAt(cx - 15, 20, cz, heroX- 15, 0, heroZ, 0, 1, 0);
	}

	//Drawing Global axes
	drawAxes();

	heroY = getY(&groundMesh, heroX - 15, heroZ);


	//Hero
	glPushMatrix();
		//printf("Y:%f\n", getY(&groundMesh, heroX - 15, heroZ));
		glTranslatef(heroX - 15, heroY, heroZ);
		glRotatef(heroAngle, 0.0, 1.0, 0.0);
		glScaled(0.7, 0.7, 0.7);
		drawHero(0.243, 0.635, 0.956);
	glPopMatrix();

	//Foe
	glPushMatrix();
		glTranslated(15.0, 0.0, 0.0);
		glRotated(180.0, 0.0, 1.0, 0.0);
		glScaled(0.7, 0.7, 0.7);
		drawFoe();
	glPopMatrix();
	
	//Draw Mesh
	DrawMeshQM(&groundMesh, meshSize);
	glutSwapBuffers(); //Double buffering, swap buffers
}

//Called whenever user resizes the window, including at initialization
//set up viewport, projection, then change to modelview matrix mode
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int mx, int my)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w': //up
		if (hlx == 0) {
			heroX += speed;
		}
		else if (hlz == 0) {
			heroZ += speed;
		}
		else {
			heroX += hlx * speed;
			heroZ += hlz * speed;
		}
		printf("up");
		break;
	case 's': //down
		if (hlx == 0) {
			heroX -= speed;
		}
		else if (hlz == 0) {
			heroZ -= speed;
		}
		else {
			heroX -= hlx * speed;
			heroZ -= hlz * speed;
		}
		
		printf("down");
		break;
	case 'a': //left
		heroAngle += 2.0;
		hlx = cos((heroAngle)*(PI/180));
		hlz = -sin((heroAngle)*(PI/180));

		printf("heroAngle : %f\n", heroAngle);
		//printf("calculated: hlx = %f hlz = %f\n", hlx, hlz);
		//printf("actual: hlx = %f hlz = %f\n", cos(heroAngle), sin(heroAngle));

		break;
	case 'd': //right
		heroAngle -= 2.0;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));


		//printf("hlx = %f hlz = %f\n", hlx, hlz);
		break;

	case 'c':
		if (cameraSwitch == 0) {
			cameraSwitch = 1;
			printf("camera 1");
		}
		else if (cameraSwitch == 1) {
			cameraSwitch = 0;
			printf("camera 0");
		}
		break;
	case 'f':
		glutFullScreen();
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

void functionKeys(int key, int x, int y)
{
	float fraction = 0.7f;

	switch (key)
	{
	case GLUT_KEY_F1: //Help
		printInstructions();
		break;
	case GLUT_KEY_DOWN:
		deltaMove = -0.5f;
		break;
	case GLUT_KEY_UP:
		deltaMove = 0.5f;
		break;
	case GLUT_KEY_LEFT:
		deltaXMove = -0.5f;
		printf("Left\n");
		break;
	case GLUT_KEY_RIGHT:
		deltaXMove = 0.5f;
		printf("Right\n");
		break;
	}
}

void releaseKey(int key, int x, int y)
{
	switch (key) 
	{
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: 
		deltaMove = 0;
		break;
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		deltaXMove = 0;
		break;
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			Xangle += deltaXAngle;
			Yangle += deltaYAngle;
			xOrigin = -1;
			yOrigin = -1;
		}
		else if(state == GLUT_DOWN){
			xOrigin = x;
			yOrigin = y;
		}
	}
}

void mouseMove(int x, int y)
{
	if (xOrigin >= 0) {
		deltaXAngle = (x - xOrigin) * 0.003f;
		deltaYAngle = (y - yOrigin) * 0.003f;

		lx = sin(Xangle + deltaXAngle);
		ly = -sin(Yangle + deltaYAngle);
		lz = -cos(Xangle + deltaXAngle);
	}
}

void drawFoe(void)
{

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, foe_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, foe_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, foe_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

	glPushMatrix();
		//glRotatef(shoulderYaw, 0.0, 1.0, 0.0);
		glPushMatrix();
			//Shoulder Transformations
			//glRotatef(shoulderPitch, 0.0, 0.0, 1.0);
			//Base Shoulder Sphere
			glPushMatrix();
				glutSolidSphere(2.0, 20.0, 50);
				//drawAxes();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0, 7.0, 0.0);
				glScalef(1.0, 10.0, 1.0);
				glutSolidCube(1.0);
			glPopMatrix();

			//Elbow Transformations
			glTranslatef(0.0, 12.0, 0.0);
			//glRotatef(elbowPitch, 0.0, 0.0, 1.0);
			//Elbow Sphere
			glPushMatrix();
				glutSolidSphere(1.0, 20, 50);
				//drawAxes();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(5.0, 0.0, 0.0);
				glScalef(10.0, 1.0, 1.0);
				glutSolidCube(1.0);
			glPopMatrix();
			//Head
			glPushMatrix();
				glTranslatef(11.0, 0.0, 0.0);
				glScalef(4.0, 0.5, 3.0);
				glutSolidCube(1.0);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void drawHero(void) 
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, hero_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, hero_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hero_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
	
	glPushMatrix();
		//local axes
		//drawAxes();
		glPushMatrix();
			//Shoulder Transformations
			//glRotatef(shoulderPitch, 0.0, 0.0, 1.0);
			glPushMatrix();
				glPushMatrix();
					glTranslated(0, 2, 0);
					glutSolidSphere(2.0, 20.0, 50);	//Base Shoulder Sphere
					
				glPopMatrix();

				glPushMatrix();
					glTranslated(0, 1, 0);
					glScalef(4, 1, 3);
					glutSolidCube(2); //Base Body
				glPopMatrix();

				glPushMatrix();
					glTranslatef(-2, 0, 0);
					drawWheelnAxle();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(2, 0, 0);
					drawWheelnAxle();
				glPopMatrix();

			glPopMatrix();

			drawBicep();

			//Elbow Transformations
			glTranslatef(0.0, 12.0, 0.0);
			//glRotatef(elbowPitch, 0.0, 0.0, 1.0);
			glPushMatrix(); //Elbow Sphere
				glutSolidSphere(1.0, 20, 50); 
			glPopMatrix();

			drawForearm();

			drawHead();

		glPopMatrix();
	glPopMatrix();

}

void drawAxes(void)
{
	static GLfloat axes_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
	static GLfloat axes_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat axes_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, axes_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, axes_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, axes_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);
	glEnd();
}

void computePosition(float deltaMove)
{
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
	if (y >= 0)
		y += deltaMove * ly * 0.1f;
	else
		y = 0;
}

void computeXPosition(float deltaMove)
{
	x += deltaXMove * 0.1f;
}

void computePosition2()
{
	float radius = 15;
	cx = heroX + (-radius * hlx);
	cz = heroZ + (radius * -hlz);
}

void drawBicep(void)
{
	glPushMatrix();
		glTranslatef(0.0, 7.0, 0.0);
		glScalef(1.0, 10.0, 1.0);
		glutSolidCube(1.0);
	glPopMatrix();
}

void drawForearm(void)
{
	glPushMatrix();
		glTranslatef(5.0, 0.0, 0.0);
		glScalef(10.0, 1.0, 1.0);
		glutSolidCube(1.0);
	glPopMatrix();
}

void drawHead(void)
{
	glPushMatrix();
		glTranslatef(11.0, 0.0, 0.0);
		glScalef(4.0, 0.5, 3.0);
		glutSolidCube(1.0);
	glPopMatrix();
}

void drawWheelnAxle(void)
{
	glPushMatrix();
	glTranslated(0, 1.5, 4.5);
	glutSolidTorus(0.5, 1, 15, 10);
	glPopMatrix();

	glPushMatrix();
		glTranslated(0, 1.5, 0);
		glutSolidCone(0.5,6.0,30,20);
	glPopMatrix();

	glPushMatrix();
		glTranslated(0, 1.4, 4.5);
		glScalef(0.125, 2.5, 0.125);
		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
		glTranslated(0, 1.5, 4.5);
		glRotated(90, 0, 0, 1);
		glScalef(0.125, 2.5, 0.125);
		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 1.5, 0);
	glRotated(180, 0, 1, 0);
	glutSolidCone(0.5, 6.0, 30, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 1.4, -4.5);
	glScalef(0.125, 2.5, 0.125);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 1.5, -4.5);
	glRotated(90, 0, 0, 1);
	glScalef(0.125, 2.5, 0.125);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 1.5, -4.5);
	glutSolidTorus(0.5, 1, 15, 10);
	glPopMatrix();
}

void printInstructions(void)
{
	printf("\n\nINSTRUCTIONS\nYou are a Battle Robot, destroy your foe, fight to the death.\n");
	printf("\t-'f' key: Enter Fullscreen Mode\n");
	printf("\t-'esc' key: Exit the Game\n\n");
	printf("CAMERA CONTROLS: There are three available cameras, that you can switch between using the 'C' key\n");
	printf("\t1. Gloabl Camera: enables you to move camera freely about the game\n");
	printf("\t\t-Use mouse to Pan and Tilt camera in place\n");
	printf("\t\t-Use the UP, DOWN, LEFT, RIGHT arrow keys to translate camera around the world\n");
	printf("\t2. Third Person Camera: This Camera follows the robot\n");
	printf("\t3. FPV Camera: This Camera provides a first person view in the robots perspective\n");
	printf("ROBOT CONTROLS\n");
	printf("\t-'w' key: moves robot forward\n");
	printf("\t-'s' key: moves robot backwards\n");
	printf("\t-'a' key: rotates robot counter clockwise\n");
	printf("\t-'d' key: rotates robot clockwise\n");
}







