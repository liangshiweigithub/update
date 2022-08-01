#ifdef WIN32
#include <windows.h>
#endif

#include "GL/glew.h"
#include "GL/glut.h"

#include "IL/ilut.h"

#include "glutil.h"
#include "vecmath.h"

#include <fstream>
#include <cstdlib>
#include <algorithm>


using std::min;
using std::max;

#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif

// Used for standard shading
GLuint shaderProgram;
// Used for drawing light and drawing to shadowmap
GLuint simpleShaderProgram; 

GLuint texture;

static float g_time = 0.0f;

void setCamera(Mtx4f &viewMatrix, Mtx4f &projectionMatrix);
void setLight(Mtx4f &viewMatrix, Mtx4f &projectionMatrix);

bool trigSpecialEvent = false;
bool g_paused = false;

// distance to look at point
static float cameraDistance = 30.0f;
// angle around y-axis
static float cameraYAngle = M_PI / 4.0f;
// angle to y-plane (plane with the y-axis as normal and containing origo)
static float cameraThetaAngle = M_PI / 6.0f;
// lights angle around y-axis
static Vec3f lightPosition; 
static float lightYAngle = M_PI / 4.0f;

// Model matrices
Mtx4f floorModelMatrix;
Mtx4f torusModelMatrix;

// Shadow Map globals
GLuint shadowmapTexture; 
GLuint shadowmapFBO; 
const int shadowmap_resolution = 1024;

static void initGL()
{
	//************************************
	//			Load Extensions
	//************************************
	glewInit();					// initiate glew, which provides easy access to OpenGL functions newer than version 1.1
	ilInit();					// initiate devIL (developers Image Library)
	ilutRenderer(ILUT_OPENGL);  // initiate devIL

	// Workaround for AMD, which hopefully will not be neccessary in the near future...
	if (!glBindFragDataLocation)
	{
		glBindFragDataLocation = glBindFragDataLocationEXT;
	}

	//************************************
	//		Specifying the object
	//************************************

	shaderProgram = loadShaderProgram("shading.vert", "shading.frag");
	glBindAttribLocation(shaderProgram, 0, "vertex"); 	
	glBindAttribLocation(shaderProgram, 2, "texCoordIn");
	glBindAttribLocation(shaderProgram, 3, "normalIn");
	glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
	linkShaderProgram(shaderProgram);

	simpleShaderProgram = loadShaderProgram("simple.vert", "simple.frag");
	glBindAttribLocation(simpleShaderProgram, 0, "vertex"); 	
	glBindFragDataLocation(simpleShaderProgram, 0, "fragmentColor");
	linkShaderProgram(simpleShaderProgram);

	CHECK_GL_ERROR();

	glUseProgram( shaderProgram );					
	//************************************
	//			Load Texture
	//************************************
	int texLoc = glGetUniformLocation( shaderProgram, "tex0" );
	glUniform1i( texLoc, 0 );
	texture = ilutGLLoadImage("white-marble.ppm");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	CHECK_GL_ERROR();


	//************************************
	// Set up model matrices
	//************************************
	floorModelMatrix.scale(10.0f, 1.0, 10.0f);
	Mtx4f ROT; 
	Mtx4f TRA; 
	ROT.rotX(M_PI/2.0f);
	TRA.translate(0.0f, 2.0f, 0.0f);
	torusModelMatrix = TRA * ROT;


	//*******************************************
	// Create shadowmap texture and framebuffer
	//*******************************************
	glGenTextures( 1, &shadowmapTexture );
	glBindTexture( GL_TEXTURE_2D, shadowmapTexture );		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowmap_resolution, shadowmap_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	Vec4f ones(1.0, 1.0, 1.0, 1.0); 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, ones.vec); 

	glGenFramebuffers(1, &shadowmapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE); 
	// Activate the default framebuffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


static void drawScene(const Mtx4f &viewMatrix, const Mtx4f &projectionMatrix,
					  const Mtx4f &lightViewMatrix, const Mtx4f &lightProjectionMatrix)
{
	// Shader Program
	glUseProgram( shaderProgram );				// Set the shader program to use for this draw call

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Send the lights modelview coordinates to the shaders
	Vec3f lightpos_mv = viewMatrix.multPnt(lightPosition); 
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, lightpos_mv.vec);

	// Bind shadow map texture to tex1
	glUniform1i( glGetUniformLocation( shaderProgram, "tex1" ), 1 );
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, shadowmapTexture);


	CHECK_GL_ERROR(); 
	/**************************
	 * Draw the floor
	 *************************/
	Mtx4f modelViewMatrix = viewMatrix * floorModelMatrix;	
	Mtx4f lightMatrix = viewMatrix; 
	lightMatrix.invert(); 
	lightMatrix = lightProjectionMatrix * lightViewMatrix * lightMatrix; 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightMatrix"), 1, false, lightMatrix.array);

	Mtx4f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	Mtx4f normalMatrix = modelViewMatrix;
	normalMatrix.invert(); 
	normalMatrix.transpose(); 
	// Update the matrices used in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelViewMatrix"),
		1, false, modelViewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix"), 
		1, false, modelViewProjectionMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "normalMatrix"),
		1, false, normalMatrix.array);
	drawQuad(); 
	/**************************
	 * Draw the torus
	 *************************/
	modelViewMatrix = viewMatrix * torusModelMatrix;	
	lightMatrix = viewMatrix; 
	lightMatrix.invert(); 
	lightMatrix = lightProjectionMatrix * lightViewMatrix * lightMatrix; 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightMatrix"), 1, false, lightMatrix.array);

	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix = modelViewMatrix;
	normalMatrix.invert(); 
	normalMatrix.transpose(); 
	// Update the matrices used in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelViewMatrix"),
		1, false, modelViewMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix"), 
		1, false, modelViewProjectionMatrix.array);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "normalMatrix"),
		1, false, normalMatrix.array);
	drawTorus(1.0, 0.4, 10, 40);

	glUseProgram( 0 );	
	CHECK_GL_ERROR();
}

static void drawShadowMap(const Mtx4f &viewMatrix, const Mtx4f &projectionMatrix)
{
	glPolygonOffset(2.5, 10);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,shadowmap_resolution, shadowmap_resolution); 

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// Shader Program
	GLint current_program; 
	glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
	glUseProgram( simpleShaderProgram );


	/**************************
	 * Draw the floor
	 *************************/
	Mtx4f modelViewMatrix = viewMatrix * floorModelMatrix;	
	Mtx4f modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "modelViewProjectionMatrix"), 
		1, false, modelViewProjectionMatrix.array);
	CHECK_GL_ERROR();
	drawQuad(); 
	/**************************
	 * Draw the torus
	 *************************/
	modelViewMatrix = viewMatrix * torusModelMatrix;	
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "modelViewProjectionMatrix"), 
		1, false, modelViewProjectionMatrix.array);
	drawTorus(1.0, 0.4, 10, 40);

	glUseProgram( current_program );	

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_POLYGON_OFFSET_FILL);
	CHECK_GL_ERROR();
}


static void drawLight(const Mtx4f &modelMatrix, const Mtx4f &viewMatrix, const Mtx4f &projectionMatrix)
{
	// Shader Program
	glUseProgram( simpleShaderProgram );				// Set the shader program to use for this draw call
	CHECK_GL_ERROR(); 

	// Draw the light as a quad
	Mtx4f ROT; 
	ROT.rotX(M_PI/2.0f); 
	Mtx4f modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix * ROT;
	glUniformMatrix4fv(glGetUniformLocation(simpleShaderProgram, "modelViewProjectionMatrix"), 
		1, false, modelViewProjectionMatrix.array);
	drawQuad(); 
	glUseProgram( 0 );	
	CHECK_GL_ERROR();
}


void display(void)
{
	CHECK_GL_ERROR();
	glClearColor(0.2,0.2,0.8,1.0);						// Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the color buffer and the z-buffer

	glEnable(GL_DEPTH_TEST);	// enable Z-buffering 
	glDisable(GL_CULL_FACE);		// disables not showing back faces of triangles 

	int w = glutGet((GLenum)GLUT_WINDOW_WIDTH);
	int h = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, w, h);								// Set viewport

	Mtx4f viewMatrix;
	Mtx4f projectionMatrix;
	// this call sets the view and projection matrices based on the camera variables
	setCamera(viewMatrix, projectionMatrix);

	// Now set up view and projection matrices for light
	Mtx4f lightViewMatrix; 
	Mtx4f lightProjectionMatrix; 
	setLight(lightViewMatrix, lightProjectionMatrix);
	drawShadowMap(lightViewMatrix, lightProjectionMatrix); 
	// Draw light for reference
	Mtx4f lightModelMatrix = lightViewMatrix; 
	lightModelMatrix.invert(); 
	drawLight(lightModelMatrix, viewMatrix, projectionMatrix);
	// we then pass these matrices to the scene drawing function.
	drawScene(viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix);


	CHECK_GL_ERROR();

	glutSwapBuffers();  // swap front and back buffer. This frame will now be displayed.
	CHECK_GL_ERROR();
}

void handleKeys(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:    /* ESC */
		exit(0); /* dirty exit */
		break;   /* unnecessary, I know */
	case 32:    /* space */
		break;
	}
}

void handleSpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		printf("Left arrow\n");
		break;
	case GLUT_KEY_RIGHT:
		printf("Right arrow\n");
		break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		break;
	}
}



void setCamera(Mtx4f &viewMatrix, Mtx4f &projectionMatrix)
{
	Vec3f viewat = Vec3f(0,0,0); 
	Vec3f viewup = Vec3f(0,1,0); 
	Vec3f yaxis  = Vec3f(0,1,0); 
	Vec3f viewpos = Vec3f(0,0,1); 
	Mtx3f rotmat;

	// Rotate around y-axis
	viewpos = Vec3f(0,0,cameraDistance); 
	rotmat.rotAxis(yaxis, cameraYAngle); 
	viewpos = rotmat * viewpos; 

	// Rotate around y-plane
	Vec3f tmpvec = viewpos.cross(yaxis); 
	tmpvec.normalize(); 
	rotmat.rotAxis(tmpvec, cameraThetaAngle); 
	viewpos = rotmat * viewpos; 

	// get window size from GLUT
	float w = (float)glutGet((GLenum)GLUT_WINDOW_WIDTH);
	float h = (float)glutGet((GLenum)GLUT_WINDOW_HEIGHT);

	// Compute projection matrix
	// field of view, aspect ratio, near clipping plane, far clipping plane 
	projectionMatrix = perspectiveMatrix(45.0f, w / h, 0.01f, 300.0f); 
	// Compute view matrix
	viewMatrix = lookAt(viewpos, viewat, viewup);
}

void setLight(Mtx4f &viewMatrix, Mtx4f &projectionMatrix)
{
	Vec3f viewpos = Vec3f(10.0f, 10.0f, 0.0f);
	Vec3f viewat = Vec3f(0,0,0); 
	Vec3f viewup = Vec3f(0,1,0); 
	Vec3f yaxis  = Vec3f(0,1,0); 
	Mtx3f rotmat;

	// Rotate around y-axis
	rotmat.rotAxis(yaxis, lightYAngle); 
	lightPosition = rotmat * viewpos; 

	// Compute projection matrix
	// field of view, aspect ratio, near clipping plane, far clipping plane 
	projectionMatrix = perspectiveMatrix(45.0f, 1.0, 5.0f, 100.0f); 
	// Compute view matrix
	viewMatrix = lookAt(lightPosition, viewat, viewup);
}


static bool leftDown = false;
static bool middleDown = false;
static bool rightDown = false;

static int prev_x = 0;
static int prev_y = 0;

void mouse(int button, int state, int x, int y)
{
	// debug logging of mouse interaction.
	static const char *buttonStr[] = { "GLUT_LEFT_BUTTON", "GLUT_MIDDLE_BUTTON", "GLUT_RIGHT_BUTTON" };
	static const char *stateStr[] = { "GLUT_DOWN", "GLUT_UP" };

	// reset the previous position, such that we only get movement performed after the button
	// was pressed.
	prev_x = x;
	prev_y = y;

	bool buttonDown = state == GLUT_DOWN;

	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(leftDown != buttonDown)
			trigSpecialEvent = !trigSpecialEvent;
		leftDown = buttonDown;
		break;
	case GLUT_MIDDLE_BUTTON:
		middleDown = buttonDown;
		break;
	case GLUT_RIGHT_BUTTON: 
		rightDown = buttonDown;
	default:
		break;
	}
}



void motion(int x, int y)
{
	int delta_x = x - prev_x;
	int delta_y = y - prev_y;

	// ZOOM
	if(middleDown)
	{
		cameraDistance -= float(delta_y) * 0.3f;
		// make sure cameraDistance does not become too small
		cameraDistance = max(0.1f, cameraDistance);
	}
	// ROTATE AROUND ORIGIN
	if(leftDown)
	{
		cameraYAngle	+= float(delta_x) * 0.3f * M_PI / 180.0f;
		cameraThetaAngle += float(delta_y) * 0.3f * M_PI / 180.0f;

		cameraThetaAngle  = min<float>(M_PI / 2.0f - 0.05f, cameraThetaAngle);
		cameraThetaAngle  = max<float>(-M_PI / 2.0f + 0.05f, cameraThetaAngle);

		// bring angle back into range [0, 2*pi)
		while (cameraYAngle >  M_PI * 2.0f) 
		{
			cameraYAngle -= M_PI * 2.0f;
		}
		while (cameraYAngle < -M_PI * 2.0f) 
		{
			cameraYAngle += M_PI * 2.0f;
		}
	}

	prev_x = x;
	prev_y = y;
}

void idle( void )
{
	// glutGet(GLUT_ELAPSED_TIME) returns the time since application start in milliseconds.

	// this is updated the first time we enter this function, otherwise we will take the
	// time from the start of the application, which can sometimes be long.
	static float startTime = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	// update the global time if the application is not paused.
	if (!g_paused)
	{
		g_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f - startTime;
	}

	// Here is a good place to put application logic.
	lightYAngle = 0.5 * g_time; 

	glutPostRedisplay(); 
	// Uncommenting the line above tells glut that the window 
	// needs to be redisplayed again. This forces the display to be redrawn
	// over and over again. 
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	/* open window of size 800x600 with double buffering, RGB colors, and Z-buffering */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutCreateWindow("Tutorial 3");
	glutKeyboardFunc(handleKeys);
	glutSpecialFunc(handleSpecialKeys);
	/* the display function is called once when the gluMainLoop is called,
	* but also each time the window has to be redrawn due to window 
	* changes (overlap, resize, etc). It can also be forced to be called
	* by making a glutPostRedisplay() call 
	*/
	glutDisplayFunc(display);	// This is the main redraw function
	glutMouseFunc(mouse);		// callback function on mouse buttons
	glutMotionFunc(motion);		// callback function on mouse movements
	glutIdleFunc( idle );

	glutDisplayFunc(display);	// Set the main redraw function

	initGL();

	glutMainLoop();  /* start the program main loop */

	return 0;          
}
