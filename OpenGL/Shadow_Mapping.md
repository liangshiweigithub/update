### Shadow Mapping

The idea behind shadow mapping: We render the scene from the light's point of view and everything we see from the light's perspective is lit and everything we can't see must be in shadow. Testing whether a fragment is lit by light is not efficient. So we use depth testing for shadow.

The value in the depth buffer corresponds to the depth of the fragment clamped to [0, 1] from the camera's point of view. We can render the scene from the light's perspective and store the resulting depth values in texture. Then we can sample the closest depth values as seen from the light's perspective. The depth values show the first fragment visible from the light's perspective. The texture that stores all the depth values are called **depth map or shadow map**.

Shadow mapping consists of two passes: first we render the depth map and in the second pass we render the scene as normal and use the generated depth map to calculate whether fragments are in shadow.

#### The depth map

The depth map or shadow map is the depth textures as rendered from the light's perspective. To generate it, use framebuffer.

```c
unsigned int depthMapFBO;
glGenFramebuffers(1, &depthMapFBO);

// create a 2D texture as the framebuffer's depth buffer
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
unsiged int depthMap;
glGenTexture(1, &depthMap);
glBindTexture(GL_TEXTURE_2D, &depthMap);

// the format is GL_DEPTH_COMPONENT
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WARP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WARP_T, GL_REPEAT);

// bind framebuffer
glBindFramebuffer(GL_FRAMEBUFFER, depthMAPFBO);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 							depthMap, 0);
// we don't need to use color buffer.
glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

We only care about depth values so the texture format is **GL_DEPTH_COMPONENT**. 1024 is the resolution of depth map. Depth information is the only needed thing so there is no need for color buffer. However, **a framebuffer is not complete without a color buffer** so we need to explicitly tell OpenGL we're not going to render any color data by setting both the read and draw buffer to **GL_NONE** with **glDrawBuffer** and **glReadBuffer**.

```c
// first pass renders the depth map
glViewPort(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
glClear(GL_DEPTH_BUFFER_BIT);
ConfigureShaderAndMatrices();
RenderScene();
glBindFramebuffer(GL_FRAMEBUFFER, 0);

// second pass render the scene as normal with normal mapping.
giViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glConfigureShaderAndMatrices();
glBindTexture(GL_TEXTURE_2D, depthMap);
RenderScene()
```

Shadow maps often have a different resolution compared to what we originally render the scene in so we need to change the viewport parameters to accommodate for the size of the shadow map by **glViewport**.

#### Light space transform

Because we're modeling a directional light source all its light rays are parallel. So we use orthographic projection matrix for light source. Because a projection matrix indirectly determines the range of what is visible. It you to make sure the size of the projection frustum correctly contains the objects you want to be in the depth map.

```c
float near_plane = 1.0f, far_plane = 7.5f;
glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 									near_plane, far_plane);
```

To view matrix is created by **glm::lookAt** function with the light source's position looking at the scene's center.

```c
glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), 
                                  glm::vec3( 0.0f, 0.0f,  0.0f), 
                                  glm::vec3( 0.0f, 1.0f,  0.0f));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;
```

#### Render to depth map

When we render the scene from the light's perspective we'd much rather use a simple shader that only transforms the vertices to light space and not much more. The fragment shader is an empty shader:

```c
#version 330 core

void main()
{
}
```

#### Rendering shadows

The code to check if a fragment is in shadow is executed in the fragment shader, but the light space transformation is done in the vertex shader.

The first thing to do to check whether a fragment is in shadow is transform the light-space fragment position in clip-space to normalized device coordinates. When we output a clip-space vertex position to gl_Position in the vertex shader, OpenGL automatically does a **perspective divide e.g. transform clip-space coordinates in the range [-w, w] to [-1, 1].**  So we first transform the fragPosLightSpace to normalized space coordinates. When using an orthographic projection matrix the w component of a vertex remains untouched so this step is quite meaningless.

The code that checks whether there's a shadow is like:

```c
float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from the light's perspective (range in [0,1])
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}
```

#### Improving shadow maps

##### shadow acne

Because the shadow map is limited by resolution, multiple fragments can sample the same value from the depth map when they're far away from the light source. It becomes an issue when the light source looks at an angle towards the surface as in that case the depth map is also rendered from an angle. Serval fragments then access the same tilted depth texel while some are above and some below the floor; we get a shadow discrepancy.

![avatar](..\image\shadow_mapping_acne_bias.png)

We can solve this issue with a small little hack called a **shadow bias** where we simply offset the depth of the surface by a small bias amount such that fragments are not incorrectly considered below the surface.

```c
float bias = 0.005;
float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
```

A shadow bias of 0.005 solves the issues of our scene by a large extent, but some surfaces that have a steep angle to the light source might still produce shadow acne. A more solid approach would be to change the amount of bias based on the surface angle towards the light:

```c
float bias = max(0.05 * (1- dot(normal, lightDir)), 0.005);
```

##### Peter panning

A disadvantage of using a shadow bias is that you're applying an offset to the actual depth of objects. As a result the bias might become large enough to see a visible offset of shadows compare to the actual object locations. Objects detached from their shadows is called **peter panning**.

![avatar](..\image\shadow_mapping_culling.png)

We solve this issue by using front face culling when rendering the depth map (by default OpenGL culls back-faces). Because we only need depth values for the depth map it shouldn't matter for solid objects whether we take the depth of their front faces or back faces (they only affects the shadow inside the object). **This trick only works for solid objects**.

##### Over sampling

Another visual discrepancy which you might like or dislike is that some regions outside the light's visible frustum are considered to be in shadow while they're not. This because projected coordinates (mainly because x and y coordinates) outside the light's are higher than 1.0 and will thus sample the depth texture outside its default range of [0, 1]. 

What we'd rather have is that all coordinates outside the depth map's range have a depth of 1.0 which as a result means these coordinates will never be in shadow.

```c
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
```

By this method if x or y are out the range [0, 1] the texture function returns depth of 1.0, so shadow will be 0.0.

For coordinates outside the far plane of the light's orthographic frustum (z is to big) it is still dark. We can solve this in fragment shader.

```c
float ShadowCalculation(vec4 fragPosLightSpace)
{
    [...]
    if(projCoords.z > 1.0)
        shadow = 0.0;  // outside the far plane, it is not in shadow
    
    return shadow;
} 
```

The result of all this mean that we are only have shadows where the projected fragment coordinates sit inside the depth map range so anything outside this range will have no visible shadows.

##### PCF

The depth map has a fixed resolution so the depth frequently spans more than one fragment per texel, which produces these jagged blocky edges. This can be reduces by increasing the depth map resolution or by trying to fit the light frustum as closely to the scene as possible.

Another (partial) solution to these jagged edges is called **PCF or percentage-closer filtering** which is a term that hosts many different filtering functions that produce softer shadows, making them appear less blocky or hard. The ideal is to sample more than once from the depth map, each time with slightly different texture coordinates. All the sub-results are then combined and averaged to get soft looking shadow. One simple implementation is:

```c
float shadow = 0.0;
vec2 texelSize = 1 / textureSize(shadowMap, 0);

for(int x=-1; x<=1; ++x)
{
    for(int y=-1; y<=1; ++y)
    {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * 									texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
}
shadow /= 9;
```

The **textureSize** returns a vec2 of the width and height of the given sampler texture as mipmap  level 0. Divided over 1 this returns the size of a single texel that use to offset texture coordinates.

##### Orthographic vs projection: difference between rendering the depth map

A orthographic projection matrix doesn't deform the scene with perspective so all view/light rays are parallel which makes it a great projection matrix for directional lights. A perspective projection matrix however does deform all vertices based on perspective which gives different result.

Another subtle difference with using a perspective projection matrix is that visualizing the depth buffer will often give an almost complete white result. To get a proper result we first to transform the non-linear depth values to linear.

```c
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    // FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}  
```

### 										

### 									Point Shadow

**omnidirectional shadow map** is used to generate dynamic shadows in all surrounding directions. This technique is perfect for lights as a real point light that would cast shadows in all directions.  The main difference between directional shadow and omnidirectional shadow is the depth map used. 

The depth map needed requires rendering a scene from all surrounding directions of a point light so we use **cubemap** instead of normal 2D depth map.

#### Light space transform

After setting the framebuffer and cubemap we need some way to transform all the scene's geometry to the relevant light spaces in all 6 directions of the light. A light space transformation matrix (projection and view matrix) T for each face is needed. Each light space transformation matrix uses same projection matrix. The field of view parameter is set to 90 degrees to make sure the viewing field is exactly large enough to properly fill a single face of the cubemap such that all the faces align correctly to each other at the edges. The view matrix is:

```c
std::vector<glm::mat4> shadowTransforms;
//right,left, top, bottom, near, far
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
              
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));
shadowTransforms.push_back(shadowProj * 
                 glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));
```

#### Depth shaders

To render depth values to a depth cubemap three shaders is needed: vertex, fragment and geometry. The vertex shader just transforms vertex to world space. The geometry shader is then responsible for transforming the vertices to the light space. It has a built-in variable called **gl_Layer** that specifies which cubemap face to emit a primitive to.

```c
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=18)out;

uniform mat4 shadowMatrices[6];
out vec4 FragPos;

void main()
{
    for(int face=0;face<6;++face)
    {
        gl_layer=face;
        for(int i=0;i<3;++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitve();
    }
}
```

We take a triangle as input and output a total of 6 triangles here. The fragment shader is

```c
#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    // map to [0, 1]
    lightDistance = lightDistance / far_plane;
    gl_FragDepth = lightDistance;
}
```

We take the distance between the fragment and the light source, map to [0, 1] and write it as the fragment's depth value.

