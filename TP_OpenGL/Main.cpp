// ----------------------------------------------
// Informatique Graphique 3D & R�alit� Virtuelle.
// Travaux Pratiques
// Introduction � OpenGL
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <GL/glut.h>
#include <math.h>


using namespace std;

// App parameters
static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static string appTitle ("Informatique Graphique & Realite Virtuelle - Travaux Pratiques - Introduction a OpenGL");
static GLint window;
static unsigned int screenWidth;
static unsigned int screenHeight;

// Camera parameters
static float fovAngle;
static float aspectRatio;
static float nearPlane;
static float farPlane;
static float camPhi; // Expressing the camera position in polar coordinate, in the frame of the target
static float camTheta;
static float camDist2Target;
static float camTargetX;
static float camTargetY;
static float camTargetZ;

GLuint texture; // Identifiant opengl de la texture
static float currentTime = 0.0; // Le temps courant en milliseconds
static float acceleration = 0.0;
static float passedTime = 0.0;
static float v = 0.0;
static bool initFlag = true;
static float deltaT;

static bool move = false;
static float startPoint_x = 0.0;
static float endPoint_x = 0.0;
static float x_move = 0.0;
static float startPoint_y = 0.0;
static float endPoint_y = 0.0;
static float y_move = 0.0;

static float zoomFactor = 1.0;
static bool zoom = false;

static bool rotate = false;
static float rotate_phi = 0.0;
static float rotate_theta = 0.0;


void polar2Cartesian (float phi, float theta, float d, float & x, float & y, float & z) {
	x = d*sin (theta) * cos (phi);
    y = d*cos (theta);
	z = d*sin (theta) * sin (phi);
}

void printUsage () {
	std::cerr << std::endl // send a line break to the standard error output
		 << appTitle << std::endl
         << "Author : Tamy Boubekeur" << std::endl << std::endl
         << "Usage : ./main [<file.off>]" << std::endl
         << "Cammandes clavier :" << std::endl
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
		 		 << " w: Toggle wireframe mode" << std::endl
         << " <drag>+<left button>: rotate model" << std::endl
         << " <drag>+<right button>: move model" << std::endl
         << " <drag>+<middle button>: zoom" << std::endl
				 << " [0][1][2][3]: light model" << std::endl
				 << " +, - : modify acceleration of spheres" << std::endl
				 << " s : stop animated model" << std::endl
				 << " r: reset initial display" << std::endl
         << " q, <esc>: Quit" << std::endl << std::endl;
}

void genCheckerboard(unsigned int width, unsigned int height, unsigned char * image){
	glEnable (GL_TEXTURE_2D); // Activation de la texturation 2D
	glGenTextures (1, &texture); // Génération d’une texture OpenGL
	glBindTexture (GL_TEXTURE_2D, texture); // Activation de la texture comme texture courante
	// les 4 lignes suivantes paramètre le filtrage de texture ainsi que sa répétition au-delà du carré unitaire
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// La commande suivante remplit la texture (sur GPU) avec les données de l’image
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

}

void init () {
	// OpenGL initialization
    glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
    glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
	  glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // Background color

		//Lumière
		glEnable(GL_LIGHTING);

		GLfloat light_position0[4] = {10.0f, 10.0f, 10.0f, 1.0f};
		GLfloat color0[4] = {1.0f, 1.0f, 0.9f, 1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, light_position0); // On place la source Nº0 en (10, 10, 10)
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color0); // On lui donne légèrement orangée
		glLightfv(GL_LIGHT0, GL_SPECULAR, color0); // Une hérésie, mais OpenGL est conçu comme cela
		//glEnable(GL_LIGHT0); // On active la source 0

		GLfloat light_position1[4] = {-10.0f, 0.0f, -1.0f, 1.0f};
		GLfloat color1[4] = {1.0f, 1.0f, 0.9f, 1.0f};
		glLightfv(GL_LIGHT1, GL_POSITION, light_position1); // On place la source Nº0 en (10, 10, 10)
		glLightfv(GL_LIGHT1, GL_DIFFUSE, color1); // On lui donne légèrement orangée
		glLightfv(GL_LIGHT1, GL_SPECULAR, color1); // Une hérésie, mais OpenGL est conçu comme cela

		GLfloat light_position2[4] = {-10.0f, 0.0f, -1.0f, 1.0f};
		GLfloat color2[4] = {0.1f, 1.0f, 0.9f, 1.0f};
		glLightfv(GL_LIGHT2, GL_POSITION, light_position2); // On place la source Nº0 en (10, 10, 10)
		glLightfv(GL_LIGHT2, GL_DIFFUSE, color2); // On lui donne légèrement orangée
		glLightfv(GL_LIGHT2, GL_SPECULAR, color2); // Une hérésie, mais OpenGL est conçu comme cela

		GLfloat light_position3[4] = {-10.0f, 0.0f, -1.0f, 1.0f};
		GLfloat color3[4] = {1.0f, 0.2f, 0.5f, 1.0f};
		glLightfv(GL_LIGHT3, GL_POSITION, light_position3); // On place la source Nº0 en (10, 10, 10)
		glLightfv(GL_LIGHT3, GL_DIFFUSE, color3); // On lui donne légèrement orangée
		glLightfv(GL_LIGHT3, GL_SPECULAR, color3); // Une hérésie, mais OpenGL est conçu comme cela

		// Camera initialization
		fovAngle = 45.f;
		nearPlane = 0.01;
		farPlane = 10.0;
		camPhi = M_PI/2.0;
		camTheta = M_PI/2.0;
		camDist2Target = 5.0;
		camTargetX = 0.0;
		camTargetY = 0.0;
		camTargetZ = 0.0;

		//Initialize image
		unsigned int width = 4;
		unsigned int height = 4;
		unsigned char image [ 4 * width * height] = {0,0,255,255,
																								 255,0,0,255,
																								 0,255,0,255,
																								 0,0,0,255,
																								 128, 128,128,255,
																								 13,5,88,255,
																								 68, 79, 156, 255,
																								 200, 100, 30, 255,
																									0,0,255,255,
																									255,0,0,255,
																									0,255,0,255,
																									0,0,0,255,
																									128, 128,128,255,
																									13,5,88,255,
																									68, 79, 156, 255,
																									200, 100, 30, 255};

		genCheckerboard(width, height, image);

}

void setupCamera () {
	glMatrixMode (GL_PROJECTION); // Set the projection matrix as current. All upcoming matrix manipulations will affect it.
	glLoadIdentity ();
	gluPerspective (fovAngle, aspectRatio, nearPlane, farPlane); // Set the current projection matrix with the camera intrinsics
	glMatrixMode (GL_MODELVIEW); // Set the modelview matrix as current. All upcoming matrix manipulations will affect it.
	glLoadIdentity ();
	float camPosX, camPosY, camPosZ;
	polar2Cartesian (camPhi+ rotate_phi, camTheta + rotate_theta, camDist2Target/zoomFactor, camPosX, camPosY, camPosZ);
	camPosX += camTargetX ;
	camPosY += camTargetY ;
	camPosZ += camTargetZ;
	gluLookAt (camPosX , camPosY, camPosZ, camTargetX + x_move*2.0/screenWidth, camTargetY+y_move*2.0/screenHeight, camTargetZ, 0.0, 1.0, 0.0); // Set up the current modelview matrix with camera transform
}

void reshape (int w, int h) {
    screenWidth = w;
		screenHeight = h;
		aspectRatio = static_cast<float>(w)/static_cast<float>(h);
		glViewport (0, 0, (GLint)w, (GLint)h); // Dimension of the drawing region in the window
		setupCamera ();
}

void reset(){
	acceleration = 0.0;
	currentTime = 0.0;
	passedTime = 0.0;
	deltaT = 0.0;
	v=0.0;
	initFlag = true;
	x_move = 0.0;
	y_move =0.0;
	move = false;
	zoom = false;
	zoomFactor = 1.0;
	rotate = 0.0;
	rotate_phi = 0.0;
	rotate_theta = 0.0;
}

void glSphere(float x, float y, float z, float r){
	float x1, y1, z1;
	float d = r;
	float theta = M_PI;
	float phi = 2.0 * M_PI;
	float resTheta = 100;
	float resPhi = 100;

	glMatrixMode(GL_MODELVIEW); // inidque que l'on va désormais altérer la matrice modèle-vue
	glPushMatrix(); // pousse la matrice courante sur un pile
	x = x + v + 1.0/2.0 * deltaT*deltaT*acceleration;
	y = y + v + 1.0/2.0 * deltaT*deltaT*acceleration;
	z = z + v + 1.0/2.0 * deltaT*deltaT*acceleration;
	glTranslatef(x,y,z); // applique une translation à la matrice


	glBegin(GL_TRIANGLES);

	for(int i=0; i<resPhi; i++){
		for(int j=0; j<resTheta; j++){
			float nPhi = phi*i/resPhi;
			float nTheta = theta*j/resTheta;

			float nPhi2 = phi*(i+1)/resPhi;
			float nTheta2 = theta*(j+1)/resTheta;

			// Triangle M-M2-M1
			//M
			polar2Cartesian(nPhi, nTheta, d, x1, y1, z1);
			glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
			glTexCoord2f(nTheta/theta, nPhi/phi);
			glVertex3f(x1, y1, z1);
			//M2
			polar2Cartesian(nPhi2, nTheta2, d, x1, y1, z1);
			glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
			glTexCoord2f(nTheta2/theta, nPhi2/phi);
			glVertex3f(x1, y1, z1);
			//M1
			polar2Cartesian(nPhi, nTheta2, d, x1, y1, z1);
			glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
		  glTexCoord2f(nTheta2/theta, nPhi/phi);
			glVertex3f(x1, y1, z1);

			// Triangle M-M3-M2
			//M
		  polar2Cartesian(nPhi, nTheta, d, x1, y1, z1);
		  glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
			glTexCoord2f(nTheta/theta, nPhi/phi);
			glVertex3f(x1, y1, z1);
			//M3
			polar2Cartesian(nPhi2, nTheta, d, x1, y1, z1);
			glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
			glTexCoord2f(nTheta/theta, nPhi2/phi);
			glVertex3f(x1, y1, z1);
			//M2
			polar2Cartesian(nPhi2, nTheta2, d, x1, y1, z1);
			glColor3f(x1, y1, z1);
			glNormal3f(x1, y1, z1);
			glTexCoord2f(nTheta2/theta, nPhi2/phi);
			glVertex3f(x1, y1, z1);
		}
	}
	glEnd();

	glPopMatrix();	//replace la matrice modèle vue courante original


}

void glSphereWithMat(float x, float y, float z, float r,
										float difR, float difG, float difB,
										float specR, float specG, float specB,
										float shininess){
	GLfloat material_color[4] = {difR, difG, difB, 1.0f};
	GLfloat material_specular[4] = {specR, specG, specB,1.0};
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glBindTexture(GL_TEXTURE_2D, texture);
	glSphere(x, y, z, r);
}

void drawTriangle(){
		glBegin(GL_TRIANGLES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(1.0, 1.0, 0.0);
		glEnd();
}


void display () {
    setupCamera ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

		if(0)
			drawTriangle();

		if(0)
			glSphere(0.0, 0.0, 0.0, 1.0);
		if(0){

			glSphere(0.0, 1.5, 0.0, 0.5);
			glSphere(0.5, 0.5, 0.5, 0.5);
			glSphere(-0.5, 0.5, 0.5, 0.5);
			glSphere(-1.0, -0.5, 1.0, 0.5);
			glSphere(0.0, -0.5, 1.0, 0.5);
			glSphere(1.0, -0.5, 1.0, 0.5);

		}


		if(1){
			glSphereWithMat(0.0, 1.5, 0.0, 0.5,
							       1.0, 0.0, 0.0,
										 1.0, 1.0, 1.0,
										 1.0);
			glSphereWithMat(0.5, 0.5, 0.5, 0.5,
											0.0, 1.0, 0.0,
											0.6, 0.40, 1.0,
											0.0);
			glSphereWithMat(-0.5, 0.5, 0.5, 0.5,
											0.5, 1.6, 0.7,
											0.8, 0.4, 0.1,
											0.2);
		}

    glFlush (); // Ensures any previous OpenGL call has been executed
    glutSwapBuffers ();  // swap the render buffer and the displayed (screen) one
}

void keyboard (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
	case 'w':
		GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
        break;
    case 'q':
    case 27:
        exit (0);
        break;
		case '0':
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHT3);
			glEnable(GL_LIGHT0);
			break;

		case '1':
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHT3);
			glEnable(GL_LIGHT1);
			break;

		case '2':
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT3);
			glEnable(GL_LIGHT2);
			break;

		case '3':
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glEnable(GL_LIGHT3);
			break;

		case '+':
			acceleration = acceleration + 0.0001;
			break;

		case '-':
			if(acceleration-0.0001 < 0.0){
				acceleration = 0.0;
			}else{acceleration = acceleration - 0.0001;}
			break;

		case 's':
			acceleration = 0.0;
			break;

		case 'r':
			reset();
			break;

     default:
        printUsage ();
        break;
    }
    glutPostRedisplay ();
}

void mouse (int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state==GLUT_DOWN){
		startPoint_x = x;
		startPoint_y = y;
		rotate = true;
	}
	if(button == GLUT_LEFT_BUTTON && state==GLUT_UP ){
		endPoint_x = x;
		endPoint_y = y;
		if(endPoint_x>startPoint_x)
			rotate_phi = rotate_phi + M_PI*(endPoint_x - startPoint_x)/screenWidth;
		else{rotate_phi = rotate_phi - M_PI*(startPoint_x-endPoint_x)/screenWidth;}
		if(endPoint_y>startPoint_y)
			rotate_theta = rotate_theta + M_PI*(endPoint_y - startPoint_y)/screenHeight;
		else{rotate_theta = rotate_theta - M_PI*(startPoint_y - endPoint_y)/screenHeight;}
		setupCamera();
		rotate=false;
	}
	if(button == GLUT_RIGHT_BUTTON && state==GLUT_DOWN){
	  startPoint_x = x;
		startPoint_y = y;
		move = true;
	}
	if(button == GLUT_RIGHT_BUTTON && state==GLUT_UP ){
		endPoint_x = x;
		endPoint_y = y;
		x_move = x_move + startPoint_x - endPoint_x;
		y_move = y_move + endPoint_y - startPoint_y;
		setupCamera();
		move=false;
	}
	if(button == GLUT_MIDDLE_BUTTON && state==GLUT_DOWN){
		startPoint_y = y;
		zoom = true;
	}
	if(button == GLUT_MIDDLE_BUTTON && state==GLUT_UP){
		endPoint_y = y;
		if(startPoint_y<endPoint_y){
			zoomFactor =1.0/sqrt((endPoint_y-startPoint_y)*(endPoint_y-startPoint_y));
		}else{
			zoomFactor =sqrt((endPoint_y-startPoint_y)*(endPoint_y-startPoint_y));
			}
		setupCamera();
		zoom=false;
	}
}

void motion (int x, int y) {
	if(move==true){
		endPoint_x = x;
		endPoint_y = y;
		x_move = x_move + startPoint_x - endPoint_x;
		y_move = y_move + endPoint_y - startPoint_y;
		startPoint_x = endPoint_x;
		startPoint_y = endPoint_y;
		setupCamera();
	}
	if(zoom==true){
		endPoint_y = y;
		if(startPoint_y<endPoint_y){
			zoomFactor =1.0/sqrt((endPoint_y-startPoint_y)*(endPoint_y-startPoint_y));
		}else{
			zoomFactor =sqrt((endPoint_y-startPoint_y)*(endPoint_y-startPoint_y));
			}
		setupCamera();
	}
	if(rotate == true){
		endPoint_x = x;
		endPoint_y = y;
		if(endPoint_x>startPoint_x)
			rotate_phi = rotate_phi + M_PI*(endPoint_x - startPoint_x)/screenWidth;
		else{rotate_phi = rotate_phi - M_PI*(startPoint_x-endPoint_x)/screenWidth;}
		if(endPoint_y>startPoint_y)
			rotate_theta = rotate_theta + M_PI*(endPoint_y - startPoint_y)/screenHeight;
		else{rotate_theta = rotate_theta - M_PI*(startPoint_y - endPoint_y)/screenHeight;}
		setupCamera();
	}
}

// This function is executed in an infinite loop. It updated the window title
// (frame-per-second, model size) and ask for rendering
void idle () {
	glutPostRedisplay();
	if(initFlag == false){
		currentTime = glutGet((GLenum)GLUT_ELAPSED_TIME);
		passedTime = glutGet((GLenum)GLUT_ELAPSED_TIME);
		deltaT = currentTime-passedTime;
		initFlag = true;
	}
	passedTime = currentTime;
	currentTime = glutGet((GLenum)GLUT_ELAPSED_TIME);
	deltaT = currentTime - passedTime;
	v = v+ acceleration*deltaT;
}

int main (int argc, char ** argv) {
    glutInit (&argc, argv); // Initialize a glut app
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); // Setup a RGBA framebuffer to display, with a depth buffer (z-buffer), in double buffer mode (fill a buffer then update the screen)
    glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT); // Set the window app size on screen
    window = glutCreateWindow (appTitle.c_str ()); // create the window
    init (); // Your initialization code (OpenGL states, geometry, material, lights, etc)
    glutReshapeFunc (reshape); // Callback function executed whenever glut need to setup the projection matrix
		glutDisplayFunc (display); // Callback function executed when the window app need to be redrawn
    glutKeyboardFunc (keyboard); // Callback function executed when the keyboard is used
    glutMouseFunc (mouse); // Callback function executed when a mouse button is clicked
		glutMotionFunc (motion); // Callback function executed when the mouse move
		glutIdleFunc (idle); // Callback function executed continuously when no other event happens (good for background procesing or animation for instance).
    printUsage (); // By default, display the usage help of the program
    glutMainLoop ();
    return 0;
}
