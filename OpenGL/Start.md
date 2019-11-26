##### State machine

OpenGL is by itself a large state machine: A collection of variables that define how OpenGL should currently operate. The state of OpenGL is commonly referred to as the OpenGL context. When using OpenGL, we often change its state by setting some options, manipulating some buffers and then render using the current context.

##### Objects

An  object in OpenGL is a collection of options that represents a subset of OpenGL's state. For example, we could have an object that represents the setting of the drawing window. We can visualize an object as a C-like struct: 

```c
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

##### GLFW

GLFW is a library, written in C, specifically targeted at OpenGL providing the bare necessities required for rendering goodies to the screen. It allows us to create an OpenGL context, define window parameters and handle user input.

A fragment in OpenGL is all the data required for OpenGL to render a single pixel.

##### Vertex Buffer Objects

Store a large of vertices in the GPU's memory. Like any object in OpenGL this buffer has a unique ID corresponding to that buffer. OpenGL has many types of buffer objects and the buffer type of vertex buffer object is ***GL_ARRAY_BUFFER***. Use the ***glBufferData*** to copy data to buffer memory. Its fourth parameter specifies how we want the graphics card to manage the given data, which has 3 types:

+ GL_STATIC_DRAW: the data most likely not change at all or very rarely.
+ GL_DYNAMIC_DRAW: change a lot
+ GL_STREAM_DRAW: change every time it is drawn

```c++
unsigned int VBD;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW)
```

##### Vertex Shader:

The input of the vertex shader is specified by **in** keyword. To set the output of the vertex shader we have to assign the position data to predefined **gl_Position** function.

```
unsigned int vertexShader;
//create vertex shader
vertexShader = glCreateShader(GL_VERTEX_SHADER);
//attach shader resource to shader object and compile.
glShaderResource(vertexShader, 1 &vertexShaderResource, NULL);
glCompileShader(vertexShader);

// check if compile success
int success;
char infLo[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
if(!success)
{
	glGetShaderInfLog(vertexShader, 512, NULL, infoLog);
	std::cout<<infoLog<<std::end;
}
```

The use of fragment shade is the same.

##### Shader program

A shade program object is the final linked version of multiple shaders combined. To use the recently compiled shaders we have to link them to a shader program object and then activate this shader program when rendering object. When linking the shaders into program it links the outputs of each shader to the inputs of next shader.