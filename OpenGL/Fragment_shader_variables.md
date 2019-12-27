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

