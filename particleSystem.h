/*********************************************************************
* File:         particleSystem.h
* Author:       Krzysztof Koch  
* Date created: 04/10/2016
* Last mod:     04/10/2016
* Brief:        COMP37111 Assignment, include file for particleSystem.c
**********************************************************************/



/*********************************************************************
* Important includes
**********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

// Display list for coordinate axis 
GLuint axisList;


/*********************************************************************
* Particle definition
**********************************************************************/
typedef struct {
  GLdouble xpos, ypos, zpos;   	// position
  GLdouble xvel, yvel, zvel;   	// velocity
  GLdouble r, g, b;   			// color
  GLdouble alive;        	   	// 0 - dead, nonzero - alive
  GLdouble mass; 				// particle mass
} Particle;


/*********************************************************************
* Simulation parameters
**********************************************************************/

// Constant parameters
#define POINT_SIZE 1					// radius of point in pixels
#define EYE_X 0.0						// camera position
#define EYE_Y 0.0
#define EYE_Z 300.0
#define CENTER_X 0.0					// point at which camera is pointing
#define CENTER_Y 0.0
#define CENTER_Z 0.0

// Variable parameters
int AXIS_SIZE = 400; 			     
int AXIS_ENABLED = 0;
double GRAVITY = -0.01;



/*********************************************************************
* Fountain description 
**********************************************************************/

// Constant parameters
#define FOUNTAIN_X -120.0 				// fountain location
#define FOUNTAIN_Y -150.0
#define FOUNTAIN_Z 0.0
#define DEFAULT_NO_OF_WATERDROPS 1000	
#define MAX_NO_OF_WATERDROPS 100000
#define WATER_SPEED_MAX 4.5				// max initial vertical water speed
#define SIDE_SPLASH_MAX 0.6				// max distance of side water splash
#define WATER_DROP_COLOUR_R 0.0			// particle RGB colour 
#define WATER_DROP_COLOUR_G 0.0
#define WATER_DROP_COLOUR_B 1.0
#define WATER_DROP_MASS 10.0			// affects the impact of gravity

// Variable parameters
int NO_OF_WATERDROPS = DEFAULT_NO_OF_WATERDROPS;

// Water
Particle water[MAX_NO_OF_WATERDROPS];



/*********************************************************************
* Smoke description 
**********************************************************************/

// Constant parameters
#define SMOKE_EMITTER_X_MIN 110.0
#define SMOKE_EMITTER_X_MAX 130.0 	
#define SMOKE_EMITTER_Y -150.0
#define SMOKE_EMITTER_Z_MIN  -10.0	
#define SMOKE_EMITTER_Z_MAX -10.0	 	
#define MAX_NO_OF_SMOKE_PARTICLES 100000
#define DEFAULT_NO_OF_SMOKE_PARTICLES 1000	

// Variable parameters
int NO_OF_SMOKE_PARTICLES = DEFAULT_NO_OF_SMOKE_PARTICLES;

// Water
Particle water[MAX_NO_OF_WATERDROPS];



/*********************************************************************
* Function prototypes
**********************************************************************/
double myRandom(double);		// Return random double within range [-range,range]
void spawnParticles(void); 		// Spawn particles 
void drawParticles(void); 		// Render particles
void progressTime(void); 		// update particle parameters according to the laws 
void display(void); 			// openGL callback function
void keyboard(unsigned char, int, int); // keyboard callback function
void reshape(int, int); 		// window reshape callback function
void makeAxes(void);			// create a display list for drawing coord axis 
void initGraphics(int, char *argv[]); 	// initialisation function

