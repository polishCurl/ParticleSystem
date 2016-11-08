/*********************************************************************
* File:         particleSystem.h
* Author:       Krzysztof Koch  
* Date created: 04/10/2016
* Last mod:     04/10/2016
* Brief:        COMP37111 Assignment, include file for particleSystem.c
**********************************************************************/



/*********************************************************************
* Import relevant libraries
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


/*********************************************************************
* Simulation parameters
**********************************************************************/

// Constant parameters
#define POINT_SIZE 3					// radius of point in pixels
#define RENDERING_METHOD 2
#define EYE_X 0.0						// camera position
#define EYE_Y 0.0
#define EYE_Z 500.0
#define CENTER_X 0.0					// point at which camera is pointing
#define CENTER_Y 0.0
#define CENTER_Z 0.0
#define WINDOW_WIDTH 1200				// window size
#define WINDOW_HEIGHT 800
#define DEFAULT_GRAVITY -9.81;			// default gravitational acceleration
#define AXIS_SIZE 400
#define MAX_NO_OF_PARTICLES 2000000		// maximum and default number of 
#define DEFAULT_NO_OF_PARTICLES 1000 	// particles in each particle syste,
#define INCREMENT_MULTIPLIER 1.1
#define DECREMENT_MULTIPLIER 0.9
#define TEXT_X -420
#define TEXT_Y 270


// Variable parameters			     
int axisEnabled = 0;
double gravity = DEFAULT_GRAVITY;



/*********************************************************************
* Waterdrop and water definitions
**********************************************************************/

// Constant parameters
#define FOUNTAIN_X -220.0 				// fountain location
#define FOUNTAIN_Y -240.0
#define FOUNTAIN_Z 0.0
#define WATER_SPEED_MEAN 11.0			// mean initial vertical velocity 
#define WATER_SPEED_VAR 1.2				// variance of initial vertical speed
#define WATER_SIDE_SPLASH_VAR 0.25		// max distance of side water splash
#define WATER_DROP_COLOUR_R 0.36		// water particle RGB colour 
#define WATER_DROP_COLOUR_G 0.71
#define WATER_DROP_COLOUR_B 1.0
#define WATER_DROP_MASS 0.03			// particle mass, affects the impact of gravity

// Waterdrop
typedef struct {
  GLdouble xpos, ypos, zpos;   			// position
  GLdouble xvel, yvel, zvel;   			// velocity
} Waterdrop;

// Water
typedef struct {
	Waterdrop particles[MAX_NO_OF_PARTICLES];	// array of particles
	int totalParticles; 				// current total number of particle
	int aliveParticles; 				// current number of alive particles
} Water;



/*********************************************************************
* Smoke particle and smoke definitions
**********************************************************************/

// Constant parameters
#define SMOKE_EMITTER_X 220.0			// smoke emitter location
#define SMOKE_EMITTER_Y -240.0
#define SMOKE_EMITTER_Z 0.0
#define SMOKE_EMITTER_SIZE 15.0			// variance of possible particle spawn locations
#define SMOKE_SPEED_MEAN 0.7			// starting vertical smoke speed		
#define SMOKE_SPEED_VAR 0.1				
#define SMOKE_CHAOS_SPEED_MEAN 0.0		// velocity of particle chaotic movement
#define SMOKE_CHAOS_SPEED_VAR 0.002		
#define SMOKE_SHADE 0.8					// initial colour of smoke (gray)
#define SMOKE_SHADE_CHANGE_MEAN 0.0015	// rate of smoke shade change
#define SMOKE_SHADE_CHANGE_VAR 0.007		
#define SMOKE_PARTICLE_MASS 0.00007		// affects the impact of gravity
#define SMOKE_COLOUR_CHANGE 0.05
#define SMOKE_DEATH_COLOUR_THRES 0.00001

// Smoke particle 
typedef struct {
  GLdouble xpos, ypos, zpos;   			// position
  GLdouble xvel, yvel, zvel;   			// velocity
  GLdouble r, g, b;   					// color
  int alive;        	   				// 0 - dead, nonzero - alive
} SmokeParticle;

// Smoke 
typedef struct {
	SmokeParticle particles[MAX_NO_OF_PARTICLES];	// array of particles
	int totalParticles; 				// current total number of particle
	int aliveParticles; 				// current number of alive particles
	double r;							// smoke initial colour
	double g;
	double b;
	double chaoticSpeed;				// speed of chaotic movement
} Smoke;



/*********************************************************************
* Variables for calculation of Frame Rate
**********************************************************************/
int frameCount = 0;
int currentTime, previousTime;
double fps;


/*********************************************************************
* String buffer for displaying performance data
**********************************************************************/
char stringBuffer[50];


/*********************************************************************
* Particle systems declaration
**********************************************************************/
Water fountain;
Smoke smokeEmitter;


/*********************************************************************
* Second random number generated using Box-Muller transform
**********************************************************************/
double boxMuller2Rand;



/*********************************************************************
* Function prototypes
**********************************************************************/
double uniformRandom(double);			// uniform random variable generator
double gaussianRandom(double, double);	// gaussian random variable generator
void initParticleSystem(void); 			// initialise the particle system
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
void displayData(void); 				// display characteristics of the simulation

