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

```c++
unsigned int shaderProgram;
shaderProgram = glCreateProgram();

//Attach shader
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

// check error
glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if(!success){
    glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
}

// use the program
glUseProgram(shaderProgram);

// delete the shader objects 
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

Don't forget to delete the shader objects once we have linked them into the program object; we no longer need them anymore.

##### Linking Vertex Attributes

The vertex shader allows us to specify any input we want  in the form of vertex attributes and while this allows for great flexibility, it does mean we have to manually specify what part of our input data goes to which vertex attribute in the  vertex shader. This means we have to specify how OpenGL should interpret the vertex data before rendering.

```c++
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

The function glVertexAttribuPointer has quite a few parameters:

+ The first specifies which vertex attribute we want to configure. We specified the location of the position vertex attribute in the vertex shader with ***layout(location = 0)***. This sets the location of the vertex attribute to 0 and since we pass data to this vertex attribute, we specify 0.
+ The next specifies the size of the vertex attribute' element
+ The third is the type of data
+ The next argument specifies if we want the data to be normalized.
+ The fifth is the stride of vertex attributes.
+ The last void* is the offset of where the position data begins in the buffer.

##### Vertex Array Object

A vertex array object (**VAO**) can be bound just like a vertex buffer object and any subsequent vertex attribute calls from that point on will be stored inside the VAO. So when configuring vertex attribute pointers you only have to make those calls once and whenever we want to draw object, we can just bind the corresponding VAO. All the state is set inside VAO. A vertex array object stores:

+ Calls to glEnableVertexAttribArray or glDisableVertexAttribArray
+ Vertex attribute configurations via glVertexAttribPointer
+ Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.

```
unsigned int VAO;
glGenVertexArrays(1, &VAO);

glBindVertexArray(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// draw code
glUserProgram(shaderProgram);
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);
```

##### Element Buffer Object

EBO is a buffer, just like a vertex buffer, that stores indices that OpenGL uses to decide what vertices to draw.

#### GLSL

A Shader contains a list of input and output variables, uniforms and its main function. When talking specifically about the vertex shader each input variable is also known as **vertex attribute**. The maximum allowed attributes can be retrieved by querying **GL_MAX_VERTEX_ATTRIBUTE**.

```
int nr;
glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
```

##### Vectors

It has ***vecn, bvecn, ivecn, uvecn, dvecn*** form. We can use x,y,z,w to access its component. Also, ***rgba*** for colors and ***stpq*** for texture coordinates, accessing the same components.

The vector datatype allows for some interesting and flexible component selection called **swizzling**. Swizzling allow for the follow syntax:

```
vec2 a;
vec4 b = a.xyxx;
vec3 c= v.zyw;
vec4 d = vec4(a, 0, 0);
```

##### In and out

They are used to specify inputs and outputs on the individual shaders. Each shader can specify inputs and outputs using these keywords and whenever the output variable matches an input variable of the next shader stage they're passed along. To define how the vertex data is organized we specify the input variables with location metadata so we can configure the vertex attributes on the CPU. A fragment shader requires a vec4 color output variable.

##### Uniforms

Uniforms are another way to pass data from application on the CPU to the shaders on the GPU.

+ Uniforms are global, meaning that a uniform variable is unique per shader program object, and can accessed from any shader at any stage in the shader program.
+ Uniforms keep their values until they are reset or update.

```c++
# code set the uniform variable named "ourColor"
float timeValue = glfwGetTime();
float greenValue = (sin(timeValue)/2.0f) + 0.5f;
int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
glUseProgram(shaderProgram);
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

if ***glGetUnifromLocation*** fails, it returns -1. Because glUniform4f sets the uniform on the currently active shader program, so we should use the program first.

