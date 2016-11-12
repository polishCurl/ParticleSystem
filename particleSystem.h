/******************************************************************************
* File:         particleSystem.h
* Author:       Krzysztof Koch  
* Date created: 04/10/2016
* Last mod:     11/11/2016
* Brief:        Function prototypes and parameters for the particle system 
*				simulation
******************************************************************************/



/******************************************************************************
* Import relevant libraries
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include "SOIL.h"						// Library for loading textures from files

#ifdef MACOSX							// Include GLUT
    #include <GLUT/glut.h> 				// MACOSX
#else
    #include <GL/glut.h>					// Linux
#endif



/******************************************************************************
* Rendering method used
******************************************************************************/
#define RENDERING_METHOD 2



/******************************************************************************
* Simulation parameters
******************************************************************************/
#define POINT_SIZE_TEXTURE 100			// Point size for textured smoke rendering
#define POINT_SIZE 3					// Point size (non-textured rendering)
#define WINDOW_WIDTH 1450				// Window size
#define WINDOW_HEIGHT 800
#define DEFAULT_GRAVITY -9.81;			// Default gravitational acceleration
#define MAX_NO_OF_PARTICLES 2000000		// Maximum and default number of 
#define DEFAULT_NO_OF_PARTICLES 1000 	// particles in each particle system
#define INCREASE_VAL 1.1				// Multipliers for increasing and decreasing
#define DECREASE_VAL 0.9 				// simulation parameter values
#define TEXT_X -60						// Starting position of text to draw
#define TEXT_Y 510
#define FONT_HEIGHT 12					// Font height (used for drawing multiple lines)
#define DEG_TO_RAD 0.017453293 			// Degree to radian conversion



/******************************************************************************
* Waterdrop and water definitions
******************************************************************************/

// Constant parameters
#define WATER_FOUNTAIN_X -250.0 		// Fountain location
#define WATER_FOUNTAIN_Y 0.0
#define WATER_FOUNTAIN_Z 0.0
#define WATER_SPEED_MEAN 14.0			// Mean initial vertical velocity 
#define WATER_SPEED_VAR 1.5				// Variance of initial vertical speed
#define WATER_SIDE_SPLASH_VAR 0.25		// Max distance of side water splash
#define WATER_DROP_COLOUR_R 0.36		// Water particle RGB colour 
#define WATER_DROP_COLOUR_G 0.71
#define WATER_DROP_COLOUR_B 1.0
#define WATER_DROP_MASS 0.03			// Water particle mass, controls the impact of gravity

// Waterdrop
typedef struct {
    double xpos, ypos, zpos;   			// Position
    double xvel, yvel, zvel;   			// Velocity
} Waterdrop;

// Water
typedef struct {
	Waterdrop particles[MAX_NO_OF_PARTICLES];	// Array of particles
	int totalParticles; 				// Current total number of particles
	int aliveParticles; 				// Current number of alive particles
} Water;



/******************************************************************************
* Smoke particle and smoke definitions
******************************************************************************/

// Constant parameters
#define SMOKE_EMITTER_X 250.0			// Smoke emitter location
#define SMOKE_EMITTER_Y 0.0
#define SMOKE_EMITTER_Z 0.0
#define SMOKE_EMITTER_SIZE 15.0			// Variance of possible particle spawn locations
#define SMOKE_SPEED_MEAN 0.7			// Starting vertical smoke speed		
#define SMOKE_SPEED_VAR 0.1				
#define SMOKE_CHAOS_SPEED_MEAN 0.0		// Velocity of particle chaotic movement
#define SMOKE_CHAOS_SPEED_VAR 0.002		
#define SMOKE_CHAOS_VERTICAL_MUL 2.0	// The chaotic movement is more significant in Y dimension	
#define SMOKE_SHADE 0.8					// Initial colour of smoke (gray)
#define SMOKE_SHADE_CHANGE_MEAN 0.0015	// Rate of smoke shade change, mean and variance
#define SMOKE_SHADE_CHANGE_VAR 0.007	// this os for different shades of smoke
#define SMOKE_SHADE_INIT_VAR 0.02		// Initial smoke colour variance
#define SMOKE_INIT_ALHPA_MEAN 0.3		// Mean and variance of initial alpha value
#define SMOKE_INIT_ALPHA_VAR 0.1				
#define SMOKE_ALPHA_CHANGE 0.0001		// Rate of alpha change	
#define SMOKE_PARTICLE_MASS 0.00002		// affects the impact of gravity
#define SMOKE_COLOUR_CHANGE 0.05 		// How quickly colour is changed due to key presses
#define SMOKE_DEATH_THRES 0.0000001      // Threshold RGB colour and alpha value for killing particle
#define SMOKE_TEXTURE_NUMBER 25			// number of different smoke textures that can be used
#define SMOKE_WIND_DIRECTION_CHANGE 10 	// Delta angle of the wind after key press	
#define SMOKE_WIND_INIT_SPEED 0.1		// Initial wind speed and direction (angle)
#define SMOKE_WIND_INIT_DIRECTION 90.0		

// Smoke particle 
typedef struct {
    double xpos, ypos, zpos;   			// Position
    double xvel, yvel, zvel;   			// Velocity
    double r, g, b, alpha;   			// Current particle colour and aplha value
    int textureID;                      // ID of the texture applied to the particle
} SmokeParticle;

// Smoke 
typedef struct {
	SmokeParticle particles[MAX_NO_OF_PARTICLES];	// Array of particles
	int totalParticles; 				// Current total number of particle
	int aliveParticles; 				// Current number of alive particles
	double r;							// Smoke initial colour
	double g;
	double b;
	double chaoticSpeed;				// Speed of chaotic movement
	int textures[SMOKE_TEXTURE_NUMBER];	// Array of smoke texture IDs
} Smoke;



/******************************************************************************
* Particle systems declaration
******************************************************************************/
Water fountain;
Smoke smokeEmitter;



/******************************************************************************
* Current value of gravity acceleration
******************************************************************************/		     
double gravity;



/******************************************************************************
* Global vaiables for wind effect simulation
******************************************************************************/
int angle;
double windSpeed, xWind, zWind;


/******************************************************************************
* Camera views
******************************************************************************/

// Camera view definition
typedef struct {
	double eyeX, eyeY, eyeZ;            // position of camera
    double centerX, centerY, centerZ;   // point at which camera looks
    double upX, upY, upZ;               // "up" direction of camera
} CameraView;


// General view, shows both the fountain an smoke emitter together with parameter values
CameraView DEFAULT_VEW = {
    .eyeX = 0.0,
    .eyeY = 240.0,
    .eyeZ = 500.0,
    .centerX = 0.0,
    .centerY = 240.0,
    .centerZ = 0.0,
    .upX = 0.0,
    .upY = 1.0,
    .upZ = 0.0
};

// Only fountain shown
CameraView FOUNTAIN_VIEW = {
    .eyeX = WATER_FOUNTAIN_X + 400,
    .eyeY = WATER_FOUNTAIN_Y,
    .eyeZ = WATER_FOUNTAIN_Z,
    .centerX = WATER_FOUNTAIN_X,
    .centerY = WATER_FOUNTAIN_Y + 200,
    .centerZ = WATER_FOUNTAIN_Z,
    .upX = 0.0,
    .upY = 1.0,
    .upZ = 0.0
};

// Only smoke shown
CameraView SMOKE_VIEW = {
    .eyeX = SMOKE_EMITTER_X - 400,
    .eyeY = SMOKE_EMITTER_Y,
    .eyeZ = SMOKE_EMITTER_Z,
    .centerX = SMOKE_EMITTER_X,
    .centerY = SMOKE_EMITTER_Y + 200,
    .centerZ = SMOKE_EMITTER_Z,
    .upX = 0.0,
    .upY = 1.0,
    .upZ = 0.0
};

// View of the fountain from above
CameraView FOUNTAIN_TOP_VIEW = {
    .eyeX = WATER_FOUNTAIN_X,
    .eyeY = 600,
    .eyeZ = WATER_FOUNTAIN_Z,
    .centerX = WATER_FOUNTAIN_X,
    .centerY = WATER_FOUNTAIN_Y,
    .centerZ = WATER_FOUNTAIN_Z,
    .upX = 0.0,
    .upY = 0.0,
    .upZ = 1.0
};

// View of the smoke from above
CameraView SMOKE_TOP_VIEW = {
    .eyeX = SMOKE_EMITTER_X,
    .eyeY = 600,
    .eyeZ = SMOKE_EMITTER_Z,
    .centerX = SMOKE_EMITTER_X,
    .centerY = SMOKE_EMITTER_Y,
    .centerZ = SMOKE_EMITTER_Z,
    .upX = 0.0,
    .upY = 0.0,
    .upZ = 1.0
};

// Current view
CameraView *currentView;



/******************************************************************************
* Global variables for calculation of Frame Rate
******************************************************************************/
int frameCount, currentTime, previousTime;
double fps;



/******************************************************************************
* String buffer for displaying performance data
******************************************************************************/
char stringBuffer[50];



/******************************************************************************
* Second random number generated using Box-Muller transform (used for 
* improved performance)
******************************************************************************/
double boxMuller2Rand;



/******************************************************************************
* Function prototypes
******************************************************************************/
double uniformRandom(double);			// Uniform random variable generator
double gaussianRandom(double, double);	// Gaussian random variable generator
void initParticleSystem(void); 			// Initialise the particle system
void spawnParticles(void); 				// Spawn particles 
void drawParticles(void); 				// Render particles
void progressTime(void); 				// Update particle parameters according to the laws 
void display(void); 					// OpenGL callback function
void setView (void);					// Implement various camera views
void keyboard(unsigned char, int, int); // Keyboard callback function
void cursor_keys(int, int, int);  		// Special keys callback function
void reshape(int, int); 				// Window reshape callback function
void initGraphics(int, char *argv[]); 	// OpenGL initialisation function
void calculateFPS(void); 				// Calculate the number of frames per second
void drawString (void*, float, float, char*); // Draw string on screen
void displayData(void); 				// Display simulation parameters
void computeWind(void);					// Calculate wind vector
void createMenu(void);                  // Create menu interface
void menu(int);                         // Create menu entries

