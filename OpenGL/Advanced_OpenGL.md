### 									Advanced OpenGL

#### Depth testing

The depth buffer is a buffer that just like the color buffer (that stores all the fragment colors), stores information per fragment and has the same width and height as the color buffer. It is created automatically and stores its depth values as 16, 24 or 32 bit floats.

+ When depth testing is enabled OpenGL tests the depth value of a fragment against the content of the depth buffer. If this test  passes, the depth buffer is updated with the new depth value. If not, the fragment is discarded.

+ Depth testing is done in screen space after fragment shader has run (also stencil testing).
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

##### Depth test function

**glDepthFunc** is used to set comparison operator used for depth test. It parameters are

***GL_AWAYS, GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_EQUAL***. By default, **GL_LESS** is used that discards all the fragments that have a depth value higher than or equal the current depth buffer's value.

#####  Depth value precision

The z-values in the view space can be any value between the projection frustum's near and far value. We need some way to transform these view-space z-value to the range of [0, 1] and one way is to linearly transform them to [0, 1]. This is not commonly used.
$$
F_{depth} = \frac{z-near}{far-near}
$$

A non-linear version is
$$
F_{depth} = \frac{1/z - 1/near}{1/far-1/near}
$$
The values in the depth buffer are not linear in screen-space. The depth values are greatly determined by the small z-values thus giving enormous depth precision to the object close by.

##### Visualizing the depth buffer

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

##### Z-fighting

When two planes or triangles are so closely aligned to each other that the depth buffer does not have enough precision to figure out which one of the two shapes is in front of the other. The result is that two shapes are continually seem to switch which causes weird glitchy patterns. This is more common when object is far away. To prevent this:

+ Never place object so close.
+ Set the near plane as far as possible.
+ Use a higher precision depth buffer.