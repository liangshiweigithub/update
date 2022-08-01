#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #define VC_EXTRALEAN
  #define NOMINMAX //          - Macros min(a,b) and max(a,b)
#include <windows.h>
#endif // WIN32

#include <GL/glew.h>
#include <IL/ilut.h>

#include "vecmath.h"
#include "glutil.h"

#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

using std::vector; 

char *textFileRead(const char *fn) {
	FILE *fp;
	char *content = NULL;
	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");
		if (fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);
			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}



std::string GetShaderInfoLog(GLuint obj) {
	int logLength = 0;
	int charsWritten  = 0;
	char *tmpLog;
	std::string log;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {
		tmpLog = new char[logLength];
		glGetShaderInfoLog(obj, logLength, &charsWritten, tmpLog);
		log = tmpLog;
		delete[] tmpLog;
	}

	return log;
}



Mtx4f perspectiveMatrix(float fov, float aspectRatio, float n, float f) /* field of view, aspect ratio, near clipping plane, far clipping plane */
{
	// This matrix is created identically to gluPerspective()
	// and takes identical parameters.
	// Note that the returned matrix is stored transposed, i.e., 
	// row-by-row, which is standard for OpenGL

	Mtx4f m;
	m.loadIdentity();	// just to clear the matrix
	m.mtx[3][3] = 0.0f;
	float b = -1.0f / (f-n);
	float cotanFOV = 1.0f / tanf(fov*(float)M_PI/360.f);
	m.mtx[0][0] = cotanFOV / aspectRatio;
	m.mtx[1][1] = cotanFOV;
	m.mtx[2][2] = (f+n)*b;
	m.mtx[2][3] = -1;
	m.mtx[3][2] = 2.0f*n*f*b;
	return m;
}



Mtx4f lookAt(const Vec3f &eyePosition, const Vec3f &lookAt, const Vec3f &desiredUp)
{
	// implementation identical to that of MESA
    Vec3f forward, side, up;
    Mtx4f m;

    forward = lookAt - eyePosition;//;Vec3f(centerx - eyex, centery - eyey, centerz - eyez);

    up = desiredUp;

	forward.normalize();

    /* Side = forward x up */
	side = forward.cross(up);
	side.normalize();
    //cross(forward, up, side);
    //normalize(side);

    /* Recompute up as: up = side x forward */
	up = side.cross(forward);
    //cross(side, forward, up);

	m.loadIdentity();
    //__gluMakeIdentityf(&m[0][0]);
	m.mtx[0][0] = side[0];
    m.mtx[1][0] = side[1];
    m.mtx[2][0] = side[2];

    m.mtx[0][1] = up[0];
    m.mtx[1][1] = up[1];
    m.mtx[2][1] = up[2];

    m.mtx[0][2] = -forward[0];
    m.mtx[1][2] = -forward[1];
    m.mtx[2][2] = -forward[2];

    //glMultMatrixf(&m[0][0]);
    //glTranslated(-eyex, -eyey, -eyez);
	Mtx4f T;
  T.translate(-eyePosition[X], -eyePosition[Y], -eyePosition[Z]);

	m = m * T;
	return m;
}



GLuint loadCubeMap(const char* facePosX, const char* faceNegX, const char* facePosY, const char* faceNegY, const char* facePosZ, const char* faceNegZ)
{
	//********************************************
	//	Creating a local helper function in C++
	//********************************************
	class tempTexHelper {
	public:
		static void loadCubeMapFace(std::string filename, GLenum face)
		{
			ILuint image;
			ilGenImages(1, &image);
			ilBindImage(image);

			if(ilLoadImage(filename.c_str()) == IL_FALSE)   {
				std::cout << "Failed to load texture: " << filename << std::endl;
				ILenum Error;
				while ((Error = ilGetError()) != IL_NO_ERROR) 
					printf("%d: %s\n", Error, iluErrorString(Error));
			}
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			int s = std::max(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
			iluScale(s, s, ilGetInteger(IL_IMAGE_DEPTH));
			glTexImage2D(face, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		}
	};


	//************************************************
	//	Creating a texture ID for the OpenGL texture
	//************************************************
	GLuint textureID;
	glGenTextures(1, &textureID);

	//************************************************
	//	 Load the faces into the cube map texture
	//************************************************
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	tempTexHelper::loadCubeMapFace(facePosX, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	tempTexHelper::loadCubeMapFace(faceNegX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	tempTexHelper::loadCubeMapFace(facePosY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	tempTexHelper::loadCubeMapFace(faceNegY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	tempTexHelper::loadCubeMapFace(facePosZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	tempTexHelper::loadCubeMapFace(faceNegZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	//************************************************
	//			Set filtering parameters
	//************************************************
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Sets the type of mipmap interpolation to be used on magnifying and 
	// minifying the active texture. 
	// For cube maps, filtering across faces causes artifacts - so disable filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// In case you want filtering anyway, try this below instead
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use tri-linear mip map filtering
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);			  // or replace trilinear mipmap filtering with nicest anisotropic filtering


	CHECK_GL_ERROR();
	return textureID;
}


bool checkGLError(const char *file, int line)
{
  bool wasError = false;

  for (GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError())
  {
    wasError = true;
    const GLubyte* sError = gluErrorString(glErr);
    
    if (!sError)
    {
      sError = reinterpret_cast<const GLubyte *>(" (no message available)");
    }

    std::cerr << "GL Error #" << glErr << "(" << sError << ") " << " in File " << file << " at line: " << line << std::endl;

#if defined(_WIN32)
    std::stringstream ss;
    ss  << file << "(" << line << "): GL Error #" << glErr << "(" << sError << ") " << std::endl;

    // outputs error message to debug console, if a debugger is attached.
    OutputDebugStringA(ss.str().c_str());
#endif
  }
  return wasError;
}

// Error reporting
void fatal_error( std::string errorString, std::string title )
{
	if (title.empty())
	{
		title = "GL-Tutorial - Error";
	}
	if (errorString.empty())
	{
		errorString = "(unknown error)";
	}

	// On Win32 we'll use a message box. On !Win32, just print to stderr and abort()
#if defined(_WIN32)
	MessageBox( 0, errorString.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION );
#else
	fprintf( stderr, "%s : %s\n", title.c_str(), errorString.c_str() );
#endif

	abort();
}



GLuint loadShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	char *vs = textFileRead(vertexShader.c_str());
	char *fs = textFileRead(fragmentShader.c_str());

	// workaround for const correctness.
	const char *vv = vs;
	const char *ff = fs;
	glShaderSource(vShader, 1, &vv, NULL);
	glShaderSource(fShader, 1, &ff, NULL);
	free(vs);
	free(fs);

	glCompileShader(vShader);
	int errorFlag = -1;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &errorFlag);
	if(!errorFlag) {
		std::string err = GetShaderInfoLog(vShader);
		fatal_error( err );
		return 0;
	}

	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &errorFlag);
	if(!errorFlag) {
		std::string err = GetShaderInfoLog(fShader);
		fatal_error( err );
		return 0;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fShader);
	glDeleteShader( fShader );
	glAttachShader(shaderProgram, vShader);
	glDeleteShader( vShader );
	CHECK_GL_ERROR();

	return shaderProgram; 
}


void linkShaderProgram(GLuint shaderProgram)
{
	glLinkProgram(shaderProgram);
  GLint linkOk = 0;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkOk);
  if (!linkOk)
	{
	  std::string err = GetShaderInfoLog(shaderProgram);
	  fatal_error(err);
	  return;
  }
}


GLuint createAddAttribBuffer(GLuint vertexArrayObject, const void *data, const size_t dataSize, GLuint attributeIndex, GLsizei attributeSize, GLenum type, GLenum bufferUsage)
{
	GLuint buffer = 0;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, bufferUsage);
	CHECK_GL_ERROR();

	// Now attach buffer to vertex array object.
	glBindVertexArray(vertexArrayObject);
	glVertexAttribPointer(attributeIndex, attributeSize, type, false, 0, 0 );	
	glEnableVertexAttribArray(attributeIndex);
	CHECK_GL_ERROR();

	return buffer;
}

void drawTorus(float rout, float rin, int numc, int numt)
{   
	static GLuint torusVertexArrayObject = 0; 
	static int nofVertices = 0; 
	if(torusVertexArrayObject == 0)
	{
		vector<Vec3f> vertexdata; 
		vector<Vec2f> texturecoords; 
		vector<Vec3f> normaldata; 
		int i, j, k;   
		float s, t, twopi;   
		twopi = (float)(2 * M_PI);   
		for (i = 0; i < numc; i++) 
		{      
			for (j = 0; j <= numt; j++) {         
				for (k = 1; k >= 0; k--) {            
					s = float(i + k);            
					t = (float) j;
					float vx, vy, vz, tx, ty, tz, sx, sy, sz, nx, ny, nz, length; 
					float jangle = t*twopi/numt;
					float iangle = s*twopi/numc; 
					/* vertex position */
					vx = cos(jangle)*(rout+cos(iangle)*rin);
					vy = sin(jangle)*(rout+cos(iangle)*rin);
					vz = sin(iangle)*rin;
					/* tangent vector with respect to big circle */
					tx = -sin(jangle);
					ty = cos(jangle);
					tz = 0;
					/* tangent vector with respect to little circle */
					sx = cos(jangle)*(-sin(iangle));
					sy = sin(jangle)*(-sin(iangle));
					sz = cos(iangle);
					/* normal is cross-product of tangents */
					nx = ty*sz - tz*sy;
					ny = tz*sx - tx*sz;
					nz = tx*sy - ty*sx;
					/* normalize normal */
					length = sqrt(nx*nx + ny*ny + nz*nz);
					nx /= length;
					ny /= length;
					nz /= length;
					vertexdata.push_back(Vec3f(vx, vy, vz));
					texturecoords.push_back(Vec2f(s,t));
					normaldata.push_back(Vec3f(nx, ny, nz));
				}      
			}      
		}
		glGenVertexArrays(1, &torusVertexArrayObject); 
		createAddAttribBuffer(torusVertexArrayObject, vertexdata[0].vec, sizeof(Vec3f) * vertexdata.size(), 0, 3, GL_FLOAT); 
		createAddAttribBuffer(torusVertexArrayObject, texturecoords[0].vec, sizeof(Vec2f) * texturecoords.size(), 2, 2, GL_FLOAT); 
		createAddAttribBuffer(torusVertexArrayObject, normaldata[0].vec, sizeof(Vec3f) * texturecoords.size(), 3, 3, GL_FLOAT); 
		nofVertices = vertexdata.size(); 
	}
	glBindVertexArray(torusVertexArrayObject); 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, nofVertices); 
}

void drawQuad()
{   
	static GLuint quadVertexArrayObject = 0; 
	static int nofVertices = 4; 
	if(quadVertexArrayObject == 0)
	{
		glGenVertexArrays(1, &quadVertexArrayObject); 
		const float verts[] = {
			-1.0f,   0.0f, 1.0f,		// v0	-		v0	v2
			-1.0f,  0.0f, -1.0f,		// v1	-		|  /| 
			1.0f,   0.0f, 1.0f,			// v2	-		| / |
			1.0f,  0.0f, -1.0f			// v3	-		v1	v3
		};
		GLuint vertBuffer = createAddAttribBuffer(quadVertexArrayObject, verts, sizeof(verts), 0, 3, GL_FLOAT);
		const float colors[] = {
			1.0f, 0.0f, 0.0f,		
			0.0f, 1.0f, 0.0f,		
			0.0f, 0.0f, 1.0f,		
			1.0f, 0.0f, 1.0f		
		};
		GLuint colorBuffer = createAddAttribBuffer(quadVertexArrayObject, colors, sizeof(colors), 1, 3, GL_FLOAT);
		const float texcoords[] = {
			0.0f, 4.0f, 			// (u,v) for v0	
			0.0f, 0.0f,				// (u,v) for v1
			4.0f, 4.0f,				// (u,v) for v2
			4.0f, 0.0f				// (u,v) for v3
		};
		GLuint texcoordBuffer = createAddAttribBuffer(quadVertexArrayObject, texcoords, sizeof(texcoords), 2, 2, GL_FLOAT);
		const float normals[] = {
			0.0, 1.0, 0.0,
			0.0, 1.0, 0.0, 
			0.0, 1.0, 0.0, 
			0.0, 1.0, 0.0
		};
		GLuint normalsBuffer = createAddAttribBuffer(quadVertexArrayObject, normals, sizeof(normals), 3, 3, GL_FLOAT);
	}
	glBindVertexArray(quadVertexArrayObject); 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, nofVertices); 
}