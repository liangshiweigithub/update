Parallax mapping is a lot better in conveying a sense of depth and together with normal mapping gives incredibly realistic results while an illusion. Parallax mapping belongs to the family of **displacement mapping** that displace or offset vertices based on geometrical information stored inside a texture.

One way to do this is to take a plane with roughly 1000 vertices and displace each of these vertices based on a value in a texture that tells us the height of the plane at a specific area. Such a texture that contains height value per texel is called a height map. But this method need extra vertices.

The ideal behind parallax mapping is to alter the texture coordinates in such a way that it looks like a fragment's surface is higher or lower that it actually is, all based on the view direction and a heightmap.

![avatar](../image/parallax_mapping_scaled_height.png)

Parallax mapping aims to offset the texture coordinates at fragment position $A$ in such a way that we get texture coordinates at point $B$ ($\vec P$ points to view direction). When then use the texture coordinates at point $B$ for all subsequent texture samples, making it looks like the viewer is actually looking at point $B$. To get point $B$, parallax mapping tries to solve this by scaling the fragment to view direction vector $\vec V$ by the height at fragment $A$. So we're scaling the length of $\vec V$ to be equal to a sampled value from the heightmap $H(A)$ at the fragment position $A$. We take this vector $\vec P$ and take this vector coordinates that align with the plane as the texture coordinate offset. Issues of this method includes:

+ When the heights change rapidly over the surface the $\vec P$ will not close to $B$.
+ It's difficult to figure out which coordinates to retrieve from $\vec P$ when the surface is arbitrarily rotated in some way. What we'd rather do is parallax mapping in a different coordinate space where the x and y component of vector $\vec P$ always align with the texture's surface. The space is tangent space.

#### Parallax mapping

With parallax mapping it make more sense to use the inverse of the heightmap (which is known as depthmap) as it's easier to fake depth than height on flat surfaces.

![avatar](..\image\parallax_mapping_depth.png) 

We obtain vector $\vec P$ by subtracting vector $\vec V$ from the texture coordinates at point $A$. Parallax mapping is implemented in the fragment shader as the displacement effect differs all over a triangle's surface. The code is:

```c
#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoords;
layout (location=3) in vec3 aTangent;
layout (location=4) in vec3 aBittangent;

out VS_OUT{
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 T = normalize(mat3(model) * aTangent);
	vec3 B = normalize(mat3(model) * aBittangent);
	vec3 N = normalize(mat3(model) * aNormal);
	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentFragPos = TBN * vec3(model * vec4(aPos, 1.0));
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

The shader code is something like before. The code of parallax mapping is

```c
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}
```

As the viewDir vector is normalized viewDir.z will be somewhere in the range between 0.0 and 1.0. When viewDir is largely parallel to the surface its z component is close to 0.0 and the division returns a much large vector $\vec P$ compared to when viewDir is largely perpendicular to the surface. So basically we're increasing the size of $\vec P$ in such a way that it offsets the texture coordinates at a large scale when looking at a surface from an angle compared to when looking at it from the top. Some prefer to leave the division by viewDir.z out of  the equation as normal Parallax Mapping could produce undesirable results at angles. This is called Parallax Mapping with offset limiting.

At the edge of the parallax mapped plane the border is weird. This is because the displaced texture coordinates could oversample outside the range [0, 1] and this gives unrealistic results based on the texture's wrapping mode. One trick is to discard the fragment outside the default texture coordinate range.

```c
if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 	0.0 ||)
	discard;
```

#### Steep Parallax Mapping

Steep Parallax Mapping is an extension on top of Parallax mapping  in that it uses the same principles, but instead of 1 sample it takes multiple samples to better pinpoint vector $\vec P$ to $B$.

![avatar](..\image\parallax_mapping_steep_parallax_mapping_diagram.png)

The general idea of Steep Parallax Mapping is that it divides the total depth range into multiple layers of the same height/depth. We traverse the depth layers from the top down and for each layer we compare its depth value to the depth value stored in the depthmap. If the layer's depth value is less than the depthmap's value it means this layer's part of vector $\vec P$ is not below the surface. We continue this process until the layer's depth is higher than the value stored in the depthmap: this point is below the geometric surface.

##### Improvement

When looking straight onto a surface there isn't much texture displacement going on while there is a lot of displacement when looking at a surface from an angle. By taking les samples when looking straight at a surface and more samples when looking at an angle we only sample the necessary amount:

```c
const float minLayers = 8.0;
const float maxLayers = 32.0;
float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), 						viewDir)));
```

##### Problems

Because this technique is based on a finite number of samples we get aliasing effects and the clear distinction between layers can be easily be spotted. This problem can be reduced by take a larger number of samples, but this quickly becomes too heavy a burden on performance. There are several approaches that aim to fix this issue by not taking the first position that below the surface, but by interpolating between the position's tow closest depth layers to find a  much closer match to $B$. Tow of the approaches are **Relief Parallax Mapping** and **Parallax Occlusion Mapping**.

#### Parallax Occlusion Mapping

Parallax Occlusion Mapping is similar to Steep Parallax Mapping with an extra step the linear interpolation between the two depth layer's texture coordinates surrounding the interested point.

```c
[....] // steep parallax mapping code here
// get texture coordinates before collision
vec2 prevTexCoords = currentTexCoords - deltaTexCoords;

float afterDepth = currentDepthMapValue - currentLayerDepth;
float beforeDepth = texture(depthMap, prevTexCoords).r - (currentLayerDepth - layerDepth);

float weight = afterDepth / (afterDepth - beforeDepth);
vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
return finalTexCoords;
```

