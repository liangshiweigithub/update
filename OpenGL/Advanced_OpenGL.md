### 																Advanced OpenGL

### Depth testing

The depth buffer is a buffer that just like the color buffer (that stores all the fragment colors), stores information per fragment and has the same width and height as the color buffer. It is created automatically and stores its depth values as 16, 24 or 32 bit floats.

+ When depth testing is enabled OpenGL tests the depth value of a fragment against the content of the depth buffer. If this test  passes, the depth buffer is updated with the new depth value. If not, the fragment is discarded.

+ Depth testing is done in screen space after fragment shader has run (and after stencil testing).
+ The screen space coordinates relate directly to the viewport defined by OpenGL ***glViewPort*** and can be accessed via GLSL's built-in ***gl_FragCoord*** variable in the fragment shader. The x and y components of the ***gl_FragCoord*** represent the fragment's screen-space coordinates ((0, 0) being the bottom-left corner). It also contains a z-component which contains the actual depth value of the fragment.

Early depth testing: This technique allows the depth test to run before the fragment shader runs. Wherever it is clear a fragment is never going to be visible we can prematurely discard the fragment. The restriction is the fragment shader for early depth testing is that you shouldn't write to the fragment's depth value.

```c++
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// not write the depth buffer
glDepthMask(GL_FALSE);
```

+ Use the **glEnable** to enable depth testing.
+ If enables depth testing you should also clear the depth buffer before each render iteration using the GL_DPETH_BUFFER_BIT.
+ If you want to perform the depth test on all fragments but not update the depth buffer. This done by setting its depth mask to GL_FALSE.

#### Depth test function

**glDepthFunc** is used to set comparison operator used for depth test. It parameters are

***GL_AWAYS, GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_EQUAL***. By default, **GL_LESS** is used that discards all the fragments that have a depth value higher than or equal the current depth buffer's value.

#### Depth value precision

The z-values in the view space can be any value between the projection frustum's near and far value. We need some way to transform these view-space z-value to the range of [0, 1] and one way is to linearly transform them to [0, 1]. This is not commonly used.
$$
F_{depth} = \frac{z-near}{far-near}
$$

A non-linear version is
$$
F_{depth} = \frac{1/z - 1/near}{1/far-1/near}
$$
The values in the depth buffer are not linear in screen-space. The depth values are greatly determined by the small z-values thus giving enormous depth precision to the object close by.

#### Visualizing the depth buffer

The built-in ***gl_FragCoord*** in the fragment shader contains the depth value of that particular fragment. Output this depth value of the fragment as a color we could display the depth values of all the fragments in the scene

```c
// pixel shader shows the depth color
void main()
{
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
```

This is code convert the z value to linear.

```c
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{   
    // divide by far for demonstration
    float depth = LinearizeDepth(gl_FragCoord.z) / far; 
    FragColor = vec4(vec3(depth), 1.0);
}
```

#### Z-fighting

When two planes or triangles are so closely aligned to each other that the depth buffer does not have enough precision to figure out which one of the two shapes is in front of the other. The result is that two shapes are continually seem to switch which causes weird glitchy patterns. This is more common when object is far away. To prevent this:

+ Never place object so close.

+ Set the near plane as far as possible.

+ Use a higher precision depth buffer.

  

### Stencil testing

Stencil test has the ability to discarding fragments. It is based on the stencil buffer which contains 8 bit per stencil value. We can set these stencil value and then discard or keep fragments whenever a particular fragment has a certain stencil value. Steps to use stencil:

+ Enable writing to the stencil buffer.
+ Render objects, updating the content of the stencil buffer.
+ Disable writing to the stencil buffer
+ Render (other object), this time discard certain fragments based on the content of stencil buffer.

By using the stencil buffer we can discard certain fragments based on the fragments of other drawn objects in the scene.

```c
glEnable(GL_STENCIL_TEST);

while (rendring)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    renderSomething();
}
```

The ***glStencilMask*** allows to set a bitmask that is **AND**ed with the stencil value about to be written to the buffer. By default the musk is $0xFF$ which will not effect the write value.

```c
// each bit is written
glStencilMask(0xFF);
// each bit ends up as 0 in the stencil buffer
glStencilMask(0x00);
```

##### Stencil functions

```c
glStencilFunc(GLenum func, GLint ref, GLuint mask);
```

+ func: sets the stencil test function. This test function is applied to the stored stencil value and the ***glStencilFunc***'s ref value. Possible options are ***GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEUAL, GL_ALWAYS***.
+ ref: specifies the reference value for the stencil test. The stencil buffer's content is compared to this value.
+ mask: specifies a mask that is ANDed with both the reference value and the stored stencil value before the test compares them. Initially set to 0xff.

The ***glStenciFunc*** describes what OpenGL should do with the content of the stencil buffer. While the ***glStencilOp*** defines how to update the stencil buffer.

```
glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
```

+ sfail: action to take is the stencil test fails.
+ dpfail: action to take is the stencil test passes, but the depth test fails.
+ dppass: action to take if both the stencil and the depth test pass.

The actions includes ***GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT***.

##### Object Outline

1. Set the stencil func to ***GL_ALWAYS*** before drawing the objects, update the stencil buffer with 1 whenever the objects fragments are rendered.

2. Render Object. (the stencil buffer is also changed).

3. Disable stencil writing and depth testing.

4. Scale each object to outline by a small amount.

5. Use a different fragment shader that output the border color.

6. Draw the object again, but only if their fragment's stencil values are not equal to 1.

7. Enable stencil writing and depth testing again.

   

### Blending

Blending is a technique to implement transparency within objects. The amount of transparency of an object is defined by its color's **alpha** value. Enable blending is:

```c
glEnable(GL_BLEND);
```

The blending equation is:
$$
C_{result} = C_{source} * F_{source} + C_{destination} * F_{destination}
$$

+ $C_{source}$: Color vector that originates from the texture.
+ $C_{destination}$: Color vector that is currently stored in the color buffer.
+ $F_{source}$: Source color factor. Can be set.
+ $F_{destination}$: Destination color factor. Can be set.

The ***glBlendFunc(GLenum sfactor, GLenum dfactor)*** function set the option for the source and destination factor. Options are ignored here.

```
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

The ***glBlendFuncSeparate*** set different options for the RGB and alpha channel individually.

```c
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
```

##### Order of Drawing transparent object

Because depth testing works a bit tricky combined with blending. When writing to the depth buffer, the depth test does not core if the fragment has transparency or not so the transparent parts are written to the depth buffer as any other value. The result is that the entire quad of the window is checked for depth testing regardless of transparency. So the transparency object will block each other.

+ Draw all opaque objects first.
+ Sort all the transparent objects.
+ Draw all the transparent objects in sorted order. ( for farthest to nearest )

The sort is cost of time. Advance techniques like ***order independent transparency*** can be used.



### Face culling

Face culling checks all the faces that are front facing towards the viewer and renders those while discarding all the faces that are back facing. By default, triangles defined with counter-clockwise vertices are processed as front-facing triangles. Enable face culling uses:

```c
glEnable(GL_CULL_FACE); // enable face culling
glCullFace(GL_FRONT);  // face to cull.
glFrontFace(GL_CCW);  // prefer counter clockwise winding
```

The initial value of ***glCullFace*** is ***GL_BACK***. There are also other type: ***GL_FRONT, GL_FRONT_AND_BACK***. **Face culling works with closed shapes like a cube.**

The ***gFrontFace*** can set front face according to ***GL_CCW, GL_CW***, which are counter-clockwise and clock-wise.

Remember: to specify vertices in a counter-clockwise winding order you need to visualize the triangle as if you're in front of the triangle and from that point of view, is where you set their order.



### Framebuffers

The combination of a color buffer, a depth buffer and stencil buffer is framebuffer. All the operation before is done on the top of render buffers attached to the default framebuffer. The default framebuffer is created and configured when create window. OpenGL gives the flexibility to define our own framebuffers.

##### Create framebuffer

```c
unsigned int fbo;
glGenFrameBuffers(1, &fbo);
// bind the framebuffer
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
...prepare operations
// check status
if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    doSomethin;
glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer.
glDeleteFramebuffer(1, &fbo);
```

The using of framebuffer is same as before. First create a framebuffer object, bind it, do some operation and unbind it. Except for ***GL_FRAMEBUFFER***, there are also ***GL_READ_FRAMEBUFFER*** and ***GL_DRAW_FRAMEBUFFER*** which we read data or render data from/to. Before using the framebuffer, we have to:

+ Attach at least one buffer (color, depth or stencil)
+ There should be at least one color attachment.
+ All attachments should be complete as well (reserved memory)
+ Each buffer should have the same number of samples.

The ***glCheckFramebufferStatus*** checks the currently bound framebuffer is successfully setup. An **attachment** is a memory location that can act as a buffer for the framebuffer, think of it as an image. There two type attachment: **textures** or **renderbuffer** objects.

##### Texture attachments

When attaching a texture to a framebuffer, all rendering commands will write to the texture as if  it was a normal color/depth or stencil buffer. The advantage of using textures is that the result of all rendering operations will be stored as a texture image that can easily used in shaders.

```c
// code to create a texture attachments
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// attach the texture to framebuffer as color attachment.
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

// attach the texture as depth attachment
unsigned int depthTex;
glGenTextures(1, &depthTex);
glBindTexture(GL_TEXTURE_2D, depthTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

unsigned int stencilTex;
glGenTextures(1, &stencilTex);
glBindTexture(GL_TEXTURE_2D, stencilTex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_COMPONENT, 800, 600, 0, GL_STENCIL_COMPONENT, GL_UNSIGNED_BYTE, NULL);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

// attach depth buffer and stencil buffer as a single texture
unsigned int stencilDepthTex;
glGenTextures(1, &stencilDepthTex);
glBindTexture(GL_TEXTURE_2D, stencilDepthTex);

glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, null);

glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
```

During the texture creation, we only allocating memory and not actually filling it. Filling the texture will happen as soon as we render to the framebuffer. Use the ***glFramebufferTexture2D*** to attach the texture to framebuffer. It has

+ target: the framebuffer we're targeting.
+ attachment: type of attachment going to attach (this example is color attachment). 0 suggests we can attach more than 1 attachment.
+ textarget: type of texture you want to attach.
+ texture: the actual texture to attach
+ level: the mipmap level

Note the method of attaching depth and stencil textures. The last method set the depth and stencil buffer  as  a single texture. Each 32 bit value of the texture then consist for 24 bits of depth information and 8 bits of stencil information.

##### Renderbuffer object attachment

A renderbuffer object stores its data in OpenGL's native rendering format (without any conversion to texture-specific formats) making them faster as a writable storage medium. Renderbuffer objects are generally write-only, thus can't read from them like texture-access. The  ***glReadPixels*** can read from them but returns a specific area of pixels from the currently bound framebuffer, not directly from the attachment itself.

Render buffer object are generally write-only so they are often used as depth and stencil attachments. When we're not sampling from these buffers, a renderbuffer object is generally preferred.

```c
unsigned int rbo;
glGenRenderbuffers(1, &rbo);
glBindRenderbuffer(GL_RENDERBUFFER, rbo);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

//attach the renderbuffer
glFramebufferRenderbuffer(GL_FRAMBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
```

##### Post processing

##### Inversion

```c
void main()
{
	FragColor = vec4(vec3(1 - texture(screenTexture, TexCoors)), 1.0);
}
```

##### Grayscale

Remove all colors from the scene except the white, gray and black colors effectively grayscaling the entire image.

```c
// version 1: averaging results
void main()
{
	FragColor = texture(screenTexture, TexCoords);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);
}

// version: weighted channels. Because human eye tends to be more sensitive to // green color and at least blue.
void main()
{
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}
```

##### Kernel effects

A kernel is a small matrix-like array of values centered on the current pixel that multiplies surrounding pixel values by its kernel values and adds them all together to form a single value.

```c
void KernaleEffect()
{
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0, offset),
		vec2(offset, offset),
		vec2(-offset, 0),
		vec2(0, 0),
		vec2(offset, 0),
		vec2(-offset, -offset),
		vec2(0, -offset),
		vec2(offset, -offset)
	);
	
	float kernal[9] = float[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);
	vec3 sampleTex[9];
	for(int i=0;i<9;++i)
		sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
	vec3 col = vec3(0.0);
	for(int i=0;i<9;++i)
		col += sampleTex[i] * kernal[i];
	FragColor = vec4(col, 1.0f);
}
```

##### Blur

A kernel that creates a blur effect is defined as below
$$
\left[ \begin{matrix} 1&&2&&1\\2&&4&&2\\1&&2&&1\end{matrix} \right] / 16
$$

##### Edge detection

The kernel matrix is:
$$
\left[ \begin{matrix} 1&&1&&1\\1&&-8&&1\\1&&1&&1\end{matrix} \right]
$$




### Cubemaps

A cubemap is a combination of multiple textures mapped into a single texture. A cubemap is basically a texture that contains 6 individual 2D textures that each form one side of a cube: a textured cube. A cube maps have the useful property that they can be indexed/sampled using a direction vector.

#### Creating a cubemap

```c
unsigned int textureID;
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
```

The creation of cubemap is similar to normal textures, except that we bind it to **GL_TEXTURE_CUBE_MAP**. For each face of cubemap, we call **glTexImage2D** with their parameters. We have to set the texture target parameter to a specific face of the cubemap which is:

|         Texture target         | Orientation |
| :----------------------------: | :---------: |
| GL_TEXTURE_CUBE_MAP_POSITIVE_X |    right    |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_X |    left     |
| GL_TEXTURE_CUBE_MAP_POSITIVE_Y |     top     |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_Y |   bottom    |
| GL_TEXTURE_CUBE_MAP_POSITIVE_Z |    back     |
| GL_TEXTURE_CUBE_MAP_NEGATIVE_Z |    front    |

The code initialize the cubemap face like:

```c
int width, height, nrChannels;
unsigned char *data;
for(GLuint i=0;i<textures_faces.size();++i)
{
	data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}
```

The shader code that uses cubemap is

```c
in vec3 textureDir;
uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, textureDir);
}
```

#### Skybox

A skybox is a large cube that encompasses the entire scene and contains 6 images of surrounding environment.

##### Displaying a skybox

A cubemap used to texture a 3D cube can be sampled using the positions of the cube as the texture coordinates. When a cube is centered on the origin(0, 0, 0) each of its positions vector is also a direction vector from the origin. **To draw the skybox we're going to draw it as the first object in the scene and disable depth writing. This way the skybox will always be draw at the background of all the other objects**.

##### An optimization: Early depth testing

If render the skybox first we're running the fragment shader for each pixel on the screen even though only a small part of the skybox will eventually be visible. These fragments could have easily been discarded using **early depth testing**.  We have to change the depth function a little by setting it to GL_LEQUAL instead of GL_LESS when drawing skybox.

The shader is:

```c
void mian()
{
    TexCoords = aPas;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // here ensures the z is always 1 after devide the w.
}
```

#### Environment mapping

Using a cubemap with an environment, we could give objects reflective or refractive properties. Techniques that use an environment cubemap like this are called **environment mapping** techniques.

##### Reflection

The vertex shader that calculates reflection is

```c
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

The fragment shader is:

```c
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
```

##### Refraction

Refraction is the change in direction of light due to the change of the material the light flows through. The refractive index determines the amount light distorts/blends of a material where each material has its own refractive index. With right combination of lighting, reflection, refraction and vertex movement you can create pretty neat water graphic. The code is

```c
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{   
	float ratio = 1.0/1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
```

##### Dynamic environment maps

The calculation so far doesn't include the actual scene with possible moving objects. If we have a mirror like objects with multiple surrounding objects, only the skybox would be visible in the mirror.

Using framebuffers it is possible to create a texture of the scene for all 6 different angles from the object in question and store those in a cubemap each render iteration. We can use this cubemap to create realistic reflection and refractive surfaces that include all object. This is called **dynamic environment mapping** but this cost a lot.



### Advanced Data

A buffer in OpenGL is only an object that manages a certain piece of memory. Binding a buffer to a specific buffer target gives a meaning to a buffer. OpenGL internally stores a buffer per target and based on the target, processes the buffers differently. Way to get data in a buffer includes:

+ **glBufferData**: Allocates a piece of memory and adds data into this memory. If passes NULL as its data argument, the function would only allocate memory and not fill it.

+ **glBufferSubData**: This function can fill specific regions of the buffer instead of filling the entire buffer. This function expects a buffer target, an offset, the size of the data and the actual data as its arguments.  The buffer should have enough allocated memory so a call to glBufferData is necessary before calling glBufferSubData on the buffer.

  ```c
  // update range: [24, 24 + sizeof(data)]
  glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data); 
  ```

+ **glMapBuffer**: Ask for a pointer to the buffer's memory and directly copy the data to the buffer by yourself. By calling **glMapBuffer** OpenGL returns a pointer to the currently bound buffer's memory for us to operate.

  ```c
  float data[] = [....];
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); // get butter ptr
  // copy data into memory
  memcpy(ptr, data, sizeof(data));
  //tell OpenGL we are done with the pointer.
  glUnmapBuffer(GL_ARRAY_BUFFER);
  ```

#### Batching vertex attributes

**glVertexAttribPointer** is used to specify the attribute layout of vertex array buffer's content. Within the vertex array buffer we interleaved the attributes. That is, the position, normal or texture coordinates were placed next to each other.

What we could also do is batch all the vector data into large chunks per attribute type instead of interleaving them. Instead of an interleaved layout 123123 we take a batched approach 112233. This is done by **glBufferSubData**.

```c
float positions[] = {....};
float normals[] = {...};
float tex[] = {....};

// fill the buffer.
glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(position), sizeof(normals), &normals);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(position) + sizeof(normals), 						sizeof(tex), &tex);

// update vertex attribute pointers
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)						  (sizeof(positions)));
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)							(sizeof(positions) + sizeof(normals)));
```

#### Copying buffers

The **glCopyBufferSubData** allows to copy the data from one buffer to another buffer with relative case.  The function prototype is:

```
void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr 							readoffset, GLintptr writeoffset, GLsizeiptr size);
```

The readtarget and writetarget parameters expect to give the buffer targets that we want to copy from and to. For example copy data from **VERTEX_ARRAY_BUFFER** to **VERTEX_ELEMENT_ARRAY_BUFFER**. When the buffers are bind to same target, we use **GL_COPY_READ_BUFFER** and **GL_COPY_WRITE_BUFFER**  as the buffer target.

```c
float vertexData[] = {...};
glBindBuffer(GL_ARRAY_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 								sizeof(vertexData));
```



### GLSL's built-in variables

#### Vertex Shader variables

+ **gl_Position**: The clip space output position vector of the vertex shader.

+ **gl_PointSize**: GL_POINTS is one of the render primitives we can choose in which case each single vertex is a primitive and rendered as a point. gl_PointSize is used to set the size of points being rendered via OpenGL's gl_PointSize function. First enable the attribute:

  ```c
  glEnable(GL_PROGRAM_POINT_SIZE);
  ```

  The shader code is:

  ```c
  void main()
  {
  	gl_Position = projection * view * model * vec4(aPos, 1.0);
      gl_PointSize = gl_Position.z;
  }
  ```

+ **gl_VertexID**: This integer variable **gl_VertexID** holds the current ID of the vertex we are drawing. When doing indexed rendering this variable holds the current index of the vertex we are drawing. If without indices, this variable holds the number of the currently processed vertex since the start of the render call.

#### Fragment shader variables

+ **gl_FragCoord**: The x and y component are the window space coordinates of the fragment, originating form the bottom-left of the window. If the window size set by **glViewPort** is 800x600, the x values are between 0 and 800 and y are between 0 and 600. The z component is equal to the depth value of that particular fragment. Using the fragment shader we can calculate a different color value based on the window coordinate of fragment. For example:

  ```c
  void main()
  {
      if(gl_FragCoord.x < 400)
          return vec4(1.0, 0.0, 0.0, 1.0);
      else
          return vec4(0.0, 1.0, 0.0, 1.0);
  }
  ```

+ **gl_FrontFacing**:  If we are not using face culling (by enabling GL_FACE_CULL) then the gl_FrontFacing variable tells if the current fragment is part of a front-facing or a back-facing face. This is a bool value.

  ```c
  void main()
  {
      if(gl_FrontFacing)
          FragColor = texture(frontTexture, TexCoords);
      else
          FragColor = texture(backTexture, TexCoords);
  }
  ```

+ **gl_FragDepth**: This variable is used to set the depth value of the fragment within the shader (we can't set the depth value by gl_FragCoord) .  Writing to gl_FragDepth is of peformance penalty. Beside, early depth testing is disabled.

#### Interface blocks

Interface block allows to group the data sent from vertex to fragment shader.

```c
# vertex shader
out VS_OUTPUT
{
    vec2 TexCoords;
}vs_out;

void main()
{
    vs_out.TexCoords = aTexCoords;
}

# fragment shader

in VS_OUT
{
    vec2 TexCoords;
}fs_in;

void main()
{
    FragColor = texture(texture1, fs_in.TexCoords);
}
```

#### Uniform buffer objects

Uniform buffer object allows to declare a set of global uniform variables that remain the same over several shader programs.

```c
layout(location=1) in vec3 aPos;
layout(std140) uniform Matrices
{
	mat4 projection;
    mat4 view;
};
uniform mat4 model;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

Variables in uniform black can be directly accessed without the block name as a prefix. So here Matrices.projection = projection.

#### Uniform block layout

Uniform block layout says that currently defined uniform block uses a specific memory layout for its content. The content of uniform block is stored in a buffer object.

The std140 layout explicitly states the memory layout for each variable type by stating their respective offsets governed by a set of rules. Each variable has a base alignment which is equal to the space a variable takes within a uniform block. The aligned byte offset of a variable must be equal to a multiple of its base alignment. Each scaler has a base alignment of N (N is four). Vector is either 2N or 4N. Array of scalars of vectors has base alignment of vec4. So is matrices and struct.

#### Using uniform buffers

```c
unsigned int ubo;
glGenBuffers(1, &ubo);
// bind to GL_UNIFORM_BUFFER
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
// allocate memory without initialization
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW);

// bind uniform block to bind point
// first get the index of the uniform block
unsigned int light_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
glUniformBlockBinding(ShaderA.ID, light_index, 2);

// bind uniform buffer to bind point
glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo);
glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, 152);
```

Bind the buffer object to **GL_UNIFORM_BUFFER**. In the OpenGL context there is a number of **bind points** defined where we can link a uniform buffer to. Linking a uniform buffer to a binding point and link the uniform block in the shader to the same binding point links those to each other. The **glUniformBlockBinding** that takes a program object as its first argument, a uniform block index and the binding point to link to. From OpenGL 4.2 we can set uniform block point by shader like this:

```
layout(std140, binding=2) uniform Lights{...};
```



### Instancing

Instancing is a technique where we draw many object at once with a single render call, saving us all the CPU->GPU communications each time we need to render an object; this only have to be done once. To render using instancing all we need to do is change the render calls **glDrawArrays** and **glDrawElements** to **glDrawArraysInstanced** and **glDrawElementsInstanced** respectively. The instances sent to GPU will be rendered with a single call. Because rendering the same object a thousand time is of no use because the object is rendered at the same position. So the function seems useless. So OpenGL offers a build-in variable in the vertex shader called **gl_InstanceID**. When drawing via one of the instanced rendering calls, **gl_InstanceID** is incremented for each instance staring from 0.

#### Drawing example

```c
// example shader code
uniform vec2 offsets[100];

// c++ code that set the uniform variable
shader.setVec2(("offsets[" + index + "]").c_str(), translations[i];
```

#### Instanced arrays

The previous drawing method will hit a limit on the amount of uniform data we can send to shaders. Instanced arrays that is defined as a vertex attribute that is only updated whenever the vertex shader renders a new instance.

With vertex attributes, each run of the vertex shader will cause GLSL to retrieve the next set of vertex attributes that belong to the current vertex. When defining a vertex attribute as an instanced array however, the vertex shader only updates the content of the vertex attribute per instance instead of per vertex. The vertex shader is like this

```c
#version 330 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aColor;
// define the attributes as usual.
layout(location=2) in vec3 aOffset;

out vec3 fColor;

void main()
{
	fColor = aColor;
	gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
}
```

The instanced array is a vertex attribute, we also need to store its content  in a vertex buffer object and configure its attribute pointer.

```c
unsigned int intanceVBO;
glGenBuffers(1, &instanceVBO);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATCI_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);

//set vertex attribute pointer
glEnableVertexAttribArray(2);
glBIndBuffer(GL_ARRAY_BUFFER, instanceVBO);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glVertexAttribDivisor(2, 1);
```

The **glVertexAttribDivisor** tells OpenGL when to update the content of a vertex attribute. Its first parameter is the vertex attribute in question and the second is **attribute divisor**. The meaning the number is:

+ 0: Update the content of the vertex attribute each iteration of the vertex shader.
+ 1: Update the content when start to render a new instance.
+ 2 or other: Update the content every 2 instance and so on.

### Anti Aliasing

The effect of seeing the pixel formations an edge is composed of, is called aliasing.

#### Multisampling

The rasterizer takes all the vertices belonging to a single primitive and transforms this to a set of fragments. The rasterizer will first calculate weather the pixel belongs to a primitive. If so, a fragment will be generated for that covered pixel.

Multisampling does not use a single sampling point for determine coverage of the triangle, but use multiple sample points: **subsamples**. 

When determine the color of the pixel, **MSAA (multiple sample anti-aliasing)** run the the fragment shader only once. The fragment shader is run with the vertex data interpolated to the center of the pixel and the resulting color is then stored inside each of the covered subsamples. Once the color buffer's subsamples are filled with all the colors of the primitives we've rendered, all these colors are then averaged per pixel resulting in a single color per pixel. So when using multi-sampling, the color buffer is multiplied too. The depth and stencil test also make use of multiple sample points.

#### MSAA in OpenGL

If we want to use MSAA in OpenGL, we need a new type of buffer that can store a given amount of multisamples which is **multisample buffer**

To use MASS, before create window, we need to hint GLFW that we'd like to use a multisample buffer.

```
glfwWindowHint(GLFW_SAMPLES, 4);

// truely enable
glEnable(GL_MULTISAMPLE);
```

#### Off-screen MSAA

Because GLFW takes care of creating the multisampled buffer before, enabling MASS is quiet easy. If we want to use our own framebuffer however, for some off-screen rendering, we have to take care of creating multisampled buffers.

##### Multisampled texture attachments

To create a texture that supports storage of multiple sample points we use **glTexImage2DMultisample** that accepts **GL_TEXTURE_2D_MULTISAMPLE**.

```c
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
// samples is the number of sample we want.
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, 							height, GL_TRUE);

// bind to framebuffer
glFrameBufferTexture2D(GL_FRAMBUFFER, GL_COLOR_ATTACHMENT0, 					                  GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
```

##### Multisampled renderbuffer objects

We use **glRenderbufferStorage** instead of **glRenderbufferStorageMultisample**.

```c
glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,                                        width, height);
```

##### Render to multisampled framebuffer

Because a multisampled buffer is special we can't directly use their buffer images for other operations like sampling them in a shader. We need to **downscale or resolve** the image. Resolving a multisampled framebuffer is generally done via **glBlitFramebuffer** that transfers a given source region defined by 4 screen-space coordinates to a given target region also defined by 4 screen-space coordinates.

```c
glBindFramebuffer(GL_READ_FRAMBUFFER, multisampledFBO);
glBindFramebuffer(GL_DRAW_FRAMBUFFER, 0);
glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,                                       GL_COLOR_BUFFER_BIT, GL_NEAREST);
```

To sample the multisampled framebuffer content in fragment shader, we also need to blit the multisampled buffers to a different FBO with a non-multisampled texture attachment. 

```c
unsigned int msFBO = CreateFBOWithMultiSampledAttachments();
// then create another FBO with a normal texture color attachment
...
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,                            screenTexture, 0);
...
while (!glfwWindowShouldClose(window))
{
    ...
    glBindFramebuffer(msFBO);
    ClearFrameBuffer();
    DrawScene();
    
    // now resolve multisampled buffer into intermediate FBO;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,                                       GL_COLOR_BUFFER_BIT, GL_NEAREST);
    // now scene is stored as 2D texture image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ClearFramebuffer();
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    DrawPostProcessingQuad();
}
```

#### Custom Anti-Aliasing algorithm

It is possible to directly pass a multisampled texture image to the shaders instead of first resolving them. To retrieve the color value per subsample we first define the texture uniform sampler as a **sampler2DMS**. Then use **texelFetch** to retrieve the color value.

```
uniform sampler2DMS screenTextureMS;
// fetch the 4th subsample.
vec4 colorSample = texelFetch(ScreenTextureMS, TexCoords, 3);
```

