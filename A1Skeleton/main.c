#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"
#include "Matrix3D.h"

//Constants
const int vWidth = 1000;
const int vHeight = 1000;
const int meshSize = 100;

//Camera Variables
float Xangle = 0.0; //Xangle of rotation
float Yangle = 0.0; //Yangle of rotation
float lx = 0.0, ly = 0.0, lz = -1; //camera's direction
float x = 0.0, y = 10, z = 60.0; // XZ position

float deltaXAngle = 0.0f;
float deltaYAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;
int yOrigin = -1;


// Light positions
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };

//Light properties
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

//A quad mesh object, given by the provided QuadMesh.c file
static QuadMesh groundMesh;

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int mx, int my);
void functionKeys(int key, int x, int y);
void releaseKey(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void drawRobotArm(void);
void drawAxes(void);
void computePosition(float deltaMove);

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

	glutIgnoreKeyRepeat(1);
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
	Vector3D origin = NewVector3D(-50.0f, 0.0f, 50.0f); //doesn't refer to centroid, refers to one of the corners of the mesh
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.176f, 0.823f, 0.274f);
	Vector3D specular = NewVector3D(0.4f, 0.04f, 0.04f);

	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 100.0, 100.0, dir1v, dir2v);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
}

//sets up camera, sets materials and does the modeling transforms
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (deltaMove)
		computePosition(deltaMove);


	gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);

	//Drawing Global axes
	drawAxes();
	glPushMatrix();
	drawRobotArm();
	glPopMatrix();
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
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

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
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

void functionKeys(int key, int x, int y)
{
	float fraction = 0.7f;

	switch (key)
	{
	case GLUT_KEY_F1: //Help
		printf("HELP");
		break;
	case GLUT_KEY_DOWN:
		deltaMove = -0.5f;
		break;
	case GLUT_KEY_UP:
		deltaMove = 0.5f;
		break;
	}
}

void releaseKey(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: 
		deltaMove = 0;
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

void drawRobotArm(void)
{
	glPushMatrix();
	//glRotatef(shoulderYaw, 0.0, 1.0, 0.0);
	glPushMatrix();
	//Shoulder Transformations
	//glRotatef(shoulderPitch, 0.0, 0.0, 1.0);
	//Base Shoulder Sphere
	glPushMatrix();
	glutSolidSphere(2.0, 20.0, 50);
	drawAxes();
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
	drawAxes();
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

void drawAxes(void)
{
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
}





