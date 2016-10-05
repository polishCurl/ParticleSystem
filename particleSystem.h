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
#include <float.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

// Display list for coordinate axis 
GLuint axisList;

// Variables for calculating FPS
int frameCount = 0;
int currentTime, previousTime;
double fps;


/*********************************************************************
* Particle definition
**********************************************************************/
typedef struct {
  GLdouble xpos, ypos, zpos;   			// position
  GLdouble xvel, yvel, zvel;   			// velocity
  GLdouble r, g, b;   					// color
  int alive;        	   				// 0 - dead, nonzero - alive
  double mass; 							// particle mass
} Particle;


/*********************************************************************
* Simulation parameters
**********************************************************************/

// Constant parameters
#define POINT_SIZE 1					// radius of point in pixels
#define EYE_X 0.0						// camera position
#define EYE_Y 0.0
#define EYE_Z 500.0
#define CENTER_X 0.0					// point at which camera is pointing
#define CENTER_Y 0.0
#define CENTER_Z 0.0
#define WINDOW_WIDTH 1200				// window size
#define WINDOW_HEIGHT 800
#define DEFAULT_GRAVITY -0.03;			// default gravitational acceleration
#define AXIS_SIZE 400

// Variable parameters			     
int axisEnabled = 0;
double gravity = DEFAULT_GRAVITY;



/*********************************************************************
* Fountain description 
**********************************************************************/

// Constant parameters
#define FOUNTAIN_X -220.0 				// fountain location
#define FOUNTAIN_Y -240.0
#define FOUNTAIN_Z 0.0
#define DEFAULT_NO_OF_WATERDROPS 1000
#define MAX_NO_OF_WATERDROPS 1000000		
#define WATER_SPEED_MEAN 11.0			// mean initial vertical velocity 
#define WATER_SPEED_VAR 1.2				// variance of initial vertical speed
#define SIDE_SPLASH_VAR 0.25			// max distance of side water splash
#define WATER_DROP_COLOUR_R 0.36		// water particle RGB colour 
#define WATER_DROP_COLOUR_G 0.71
#define WATER_DROP_COLOUR_B 1.0
#define WATER_DROP_MASS 9.0				// particle mass, affects the impact of gravity

// Variable parameters
int noOfWaterdrops = DEFAULT_NO_OF_WATERDROPS;

// Water
Particle water[MAX_NO_OF_WATERDROPS];



/*********************************************************************
* Smoke description 
**********************************************************************/

// Constant parameters
#define SMOKE_EMITTER_X 220.0			// smoke emitter location
#define SMOKE_EMITTER_Y -240.0
#define SMOKE_EMITTER_Z 0.0
#define SMOKE_EMITTER_SIZE 10.0			// variance of possible particle spawn locations
#define DEFAULT_NO_OF_SMOKE_PARTICLES 1000	 	
#define SMOKE_SPEED_MEAN 0.7			// starting vertical smoke speed		
#define SMOKE_SPEED_VAR 0.1				
#define SMOKE_CHAOS_SPEED_MEAN 0.0		// velocity of particle chaotic movement
#define SMOKE_CHAOS_SPEED_VAR 0.008		
#define SMOKE_SHADE 0.8					// initial colour of smoke (gray)
#define SMOKE_SHADE_CHANGE_MEAN 0.001	// rate of smoke shade change
#define SMOKE_SHADE_CHANGE_VAR 0.01		
#define SMOKE_PARTICLE_MASS 0.015		// affects the impact of gravity
#define SMOKE_PARTICLE_LIFETIME_MEAN 400.0	// particle lifetime before it's removed	
#define SMOKE_PARTICLE_LIFETIME_STD 200.0


// Variable parameters
int noOfSmokeParticles = DEFAULT_NO_OF_SMOKE_PARTICLES;	// current number of particles
GLdouble smokeR = SMOKE_SHADE;			// smoke initial colour
GLdouble smokeG = SMOKE_SHADE;
GLdouble smokeB = SMOKE_SHADE;
GLdouble chaoticSpeed = SMOKE_CHAOS_SPEED_VAR;

// Smoke
Particle smoke[MAX_NO_OF_WATERDROPS];



/*********************************************************************
* Function prototypes
**********************************************************************/
double uniformRandom(double);			// uniform random variable generator
double gaussianRandom(double, double);	// gaussian random variable generator
void spawnParticles(void); 				// spawn particles 
void drawParticles(void); 				// render particles
void progressTime(void); 				// update particle parameters according to the laws 
void display(void); 					// openGL callback function
void keyboard(unsigned char, int, int); // keyboard callback function
void reshape(int, int); 				// window reshape callback function
void makeAxes(void);					// create a display list for drawing coord axis 
void initGraphics(int, char *argv[]); 	// initialisation function
void calculateFPS(void); 				// calculate the number of frames per second
void drawString (void*, float, float, char*); // draw string on screen

