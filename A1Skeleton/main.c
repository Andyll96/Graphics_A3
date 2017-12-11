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
#include "Player.h"

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
Player hero;
float heroAngle = 0.0;
float hlx = 0.0, hlz = 0.0;
float heroX = 0.0, heroY = 0.0, heroZ = 0.0;
float speed = 0.1;
float turnSpeed = 2;

//arm controls for both robots
float elbowPitch = 0.0;
bool elbowAscend = true;
float shoulderPitch = 0.0;
bool shoulderAscend = true;

//Foe Variables
Player foe;
float foeAngle = 0.0;
float flx = 0.0, flz = 0.0;
float foeX = 0.0, foeY = 0.0, foeZ = 0.0;
float foeSpeed = 0.025;
float foeTurnSpeed = 2;
float foeWeaponX = 0.0, foeWeaponY = 0.0, foeWeaponZ = 0.0;


bool fullscreen = false;


// Light positions
static GLfloat light_position0[] = { -20.0F, 25.0F, 15.0F, 1.0F };
static GLfloat light_position1[] = { 15.0F, 15.0F, 0.0F, 1.0F };

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
void keyRelease(int key, int x, int y);
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
void drawArm(void);

void printInstructions(void);

void foeDrive(void);


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

	glutKeyboardUpFunc(keyRelease);
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
	
	//random starting angle for foe
	srand(time(NULL));
	foeAngle = (rand() % 360 + 1);
	flx = cos((foeAngle)*(PI / 180));
	flz = -sin((foeAngle)*(PI / 180));

	//List of holes
	holes[0] = NewVector3D(10, 0, 10);
	holes[1] = NewVector3D(-10, 0, 5);
	holes[2] = NewVector3D(-15, 0, -15);

	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, meshSize, meshSize, dir1v, dir2v);
	memcpy(groundMesh.holes, holes,sizeof(groundMesh.holes));
	//b, a
	ComputeGauss(&groundMesh, -5.0f, 10.0f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

	hero = newPlayer('h', -15, 0, 0);
	foe = newPlayer('f', 15, 0, 0);
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
	else if (cameraSwitch == 1) {
		computePosition2();
		gluLookAt(cx - 15, heroY + 20, cz, heroX- 15, 0, heroZ, 0, 1, 0);
	}
	else if (cameraSwitch == 2) {
		gluLookAt(heroX - 15, 10 + heroY, heroZ, (heroX - 15) +hlx, 10 + heroY, heroZ + hlz, 0, 1, 0);
	}

	//Drawing Global axes
	drawAxes();

	heroY = getY(&groundMesh, heroX - 15, heroZ);

	if (heroX - 15 >= meshSize / 2) {
		heroX = (meshSize / 2) + 15;
		heroAngle += 135;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		baseHit(&hero);
		heroX--;
	}
	if (heroX - 15 <= -(meshSize / 2)) {
		heroX = -(meshSize / 2) + 15;
		heroAngle += 135;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		baseHit(&hero);
		heroX++;
	}
	if (heroZ >= meshSize / 2) {
		heroZ = (meshSize / 2);
		heroAngle += 135;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		baseHit(&hero);
		heroZ--;
	 }
	if (heroZ <= -(meshSize / 2)) {
		heroZ = -(meshSize / 2);
		heroAngle += 135;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		baseHit(&hero);
		heroZ++;
	}


	//Hero
	hero.Angle = heroAngle;
	hero.elbowAngle = elbowPitch;
	hero.shoulderAngle = shoulderPitch;
	glPushMatrix();
		setPosition(&hero, heroX - 15, heroY, heroZ);//for the heroData, including the weapon
		glTranslatef(heroX - 15, heroY, heroZ);
		glRotatef(heroAngle, 0.0, 1.0, 0.0);
		glScaled(0.7, 0.7, 0.7);
		drawHero(0.243, 0.635, 0.956);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(hero.weaponPosition.x, hero.weaponPosition.y, hero.weaponPosition.z);
		glutWireCube(1.0);
	glPopMatrix();

	foeDrive();
	
	//Foe
	foe.Angle = foeAngle;
	foe.elbowAngle = elbowPitch;
	foe.shoulderAngle = shoulderPitch;
	glPushMatrix();
		setPosition(&foe, 15 + foeX, foeY, foeZ);
		glTranslated(15.0 + foeX, foeY, foeZ);
		glRotated(180.0 + foeAngle, 0.0, 1.0, 0.0);
		glScaled(0.7, 0.7, 0.7);
		drawFoe();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(foe.weaponPosition.x, foe.weaponPosition.y, foe.weaponPosition.z);
	glutWireCube(1.0);
	glPopMatrix();
	
	//Draw Mesh
	DrawMeshQM(&groundMesh, meshSize);
	glutSwapBuffers(); //Double buffering, swap buffers

	if (collisionDetect(&hero, &foe) == 0)
	{
		baseHit(&hero);
		heroAngle += 135;
		speed = 0;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		heroX -= 6;


		baseHit(&foe);
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeX += 6;
	}
	else if (collisionDetect(&hero, &foe) == 1)
	{
		heroAngle += 135;
		speed = 0;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		heroX -= 6;


		weaponHit(&foe);
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeX += 6;
	}
	else if (collisionDetect(&hero, &foe) == 2)
	{
		weaponHit(&hero);
		heroAngle += 135;
		speed = 0;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));
		heroX -= 6;


		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeX += 6;
	}
	checkWinner(&hero, &foe);
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
		speed += 0.025;
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
		speed += 0.025;
		break;
	case 'a': //left
		heroAngle += turnSpeed;
		hlx = cos((heroAngle)*(PI/180));
		hlz = -sin((heroAngle)*(PI/180));

		turnSpeed += 0.5;
		break;
	case 'd': //right
		heroAngle -= turnSpeed;
		hlx = cos((heroAngle)*(PI / 180));
		hlz = -sin((heroAngle)*(PI / 180));

		turnSpeed += 0.5;
		break;

	case 'c':
		if (cameraSwitch == 0) {
			cameraSwitch = 1;
			printf("Third Person Camera 1\n");
		}
		else if (cameraSwitch == 1) {
			cameraSwitch = 2;
			printf("First Person Camera\n");
		}
		else if (cameraSwitch == 2) {
			cameraSwitch = 0;
			printf("Free World Camera\n");
		}
		break;
	case 'f':
		fullscreen = !fullscreen;
		if (fullscreen) {
			glutFullScreen();
		}
		else {
			glutReshapeWindow(vWidth, vHeight);
			glutPositionWindow((glutGet(GLUT_SCREEN_WIDTH) - 1000) / 2, 0);
		}
		break;
	case 'r': //because for some keyboards, the release function doesn't register
		turnSpeed = 0;
		speed = 0;
		printf("Speed Emergency Reset\n");
		break;
	case 'e':
		if (elbowPitch == 60)
			elbowAscend = false;
		else if (elbowPitch == -60)
			elbowAscend = true;
		if (elbowAscend)
			elbowPitch += 2;
		else
			elbowPitch -= 2;
		break;
	case 'q':
		if (shoulderPitch == 32)
			shoulderAscend = false;
		else if (shoulderPitch == -32)
			shoulderAscend = true;
		if (shoulderAscend)
			shoulderPitch += 2;
		else
			shoulderPitch -= 2;
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
		break;
	case GLUT_KEY_RIGHT:
		deltaXMove = 0.5f;
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

void keyRelease(int key, int x, int y)
{
	switch (key)
	{
	case 'w':
		turnSpeed = 0;
		speed = 0;
		printf("w release\n");
		break;
	case 's':
		turnSpeed = 0;
		speed = 0;
		printf("s release\n");
		break;
	case 'a':
		turnSpeed = 0;
		speed = 0;
		printf("a release\n");
		break;
	case 'd':
		turnSpeed = 0;
		speed = 0;
		printf("d release\n");
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
		//local axes
		//drawAxes();
		glPushMatrix();
			
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

			drawArm();

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

			drawArm();

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

//camera helper
void computePosition(float deltaMove)
{
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
	if (y >= 0)
		y += deltaMove * ly * 0.1f;
	else
		y = 0;
}

//camera helper
void computeXPosition(float deltaMove)
{
	x += deltaXMove * 0.1f;
}

//camera2 helper
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

void drawArm(void) {
	glPushMatrix();
	//Shoulder Transformations
	glRotatef(shoulderPitch, 0.0, 0.0, 1.0);
	drawBicep();

	//Elbow Transformations
	glTranslatef(0.0, 12.0, 0.0);
	glRotatef(elbowPitch, 0.0, 0.0, 1.0);
	glPushMatrix(); //Elbow Sphere
	glutSolidSphere(1.0, 20, 50);
	glPopMatrix();

	drawForearm();

	drawHead();
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

void foeDrive(void)
{

	foeY = getY(&groundMesh, foeX + 15, foeZ);

	if (flx == 0) {
		foeX -= foeSpeed;
	}
	else if (flz == 0) {
		foeZ -= foeSpeed;
	}
	else {
		foeX -= flx * foeSpeed;
		foeZ -= flz * foeSpeed;
	}

	
	if (foeX >= (meshSize / 2) - 15) {
		foeX = (meshSize / 2)-15;
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeX--;
	}
	if (foeX <= -(meshSize / 2) - 15) {
		foeX = -(meshSize / 2) - 15;
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeX++;
	}
	if (foeZ >= (meshSize) / 2) {
		foeZ = (meshSize) / 2;
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeZ--;
	}
	if (foeZ <= -(meshSize/2))
	{
		foeZ = -(meshSize / 2);
		foeAngle += 135;
		flx = cos((foeAngle)*(PI / 180));
		flz = -sin((foeAngle)*(PI / 180));
		foeZ++;
	}
}

