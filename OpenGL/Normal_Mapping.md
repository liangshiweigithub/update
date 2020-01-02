### Normal Mapping

The technique to use per-fragment normals compared to per-surface is called normal mapping or bump mapping. Use a 2D texture to store per-fragment data. The normal vectors range between -1 and 1, map them to [0, 1] is like:

```c
vec3 rgb_normal = normal * 0.5 + 0.5;
```

Normal map usually have a blueish tint. This is because all the normals closely pointing outward towards the positive z-axis which is (0, 0, 1): a blueish color. The slight deviations in color represent normal vectors that are slightly offset from the positive z direction. The normal map gives a scene of depth.

#### Tangent space

Normal vectors in a normal map are expressed in tangent space where normals always point roughly in the positive z direction. Tangent space is a space that is local to the surface of a triangle: the normals are relative to the local reference frame of the individual triangles. 

Using **TBN** we can then transform normal vectors from this local tangent space to world or view coordinates. The letters of **TBN** depict a tangent, bittangent, and normal vector. Up vector is the surface's normal vector. The right and forward vector of the normal map are the tangent and bittangent vector.

![avatar](../image/normal_mapping_surface_edges.png)

The edge of the triangle $E_1$ and $E_2$ can be represented as:
$$
E_1 = \Delta U_1T + \Delta V_1B\\
E_2 = \Delta U_2T + \Delta V_2B
$$
 which is
$$
(E_{1x}, E_{1y}, E_{1z}) = \Delta U_1(T_x, T_y, T_z) + \Delta V_1(B_x, B_y, B_z)\\
(E_{2x}, E_{2y}, E_{2z}) = \Delta U_2(T_x, T_y, T_z) + \Delta V_2(B_x, B_y, B_z)
$$
Transform into matrix calculation:
$$
\left[
\begin{matrix}
E_{1x} && E_{1y} && E_{1z}\\
E_{2x} && E_{2y} && E_{2z}\\
\end{matrix}
\right]
=
\left[
\begin{matrix}
\Delta U_1 && \Delta V_1\\
\Delta U_2 && \Delta V_2
\end{matrix}\right] 

\left[
\begin{matrix}
T_x && T_y && T_z\\
B_x && B_y && B_z
\end{matrix}
\right]
$$
So we get the tangent and bittangent vector:
$$
\left[
\begin{matrix}
T_x && T_y && T_z\\
B_x && B_y && B_z
\end{matrix}
\right]
=
\left[
\begin{matrix}
\Delta U_1 && \Delta V_1\\
\Delta U_2 && \Delta V_2
\end{matrix}\right]^{-1}

\left[
\begin{matrix}
E_{1x} && E_{1y} && E_{1z}\\
E_{2x} && E_{2y} && E_{2z}\\
\end{matrix}
\right]
$$

$$
\left[
\begin{matrix}
T_x && T_y && T_z\\
B_x && B_y && B_z
\end{matrix}
\right]
=
\frac{1}{\Delta U_1 \Delta V_2 - \Delta U_2 \Delta V_1}
\left[
\begin{matrix}
\Delta V_2 && -\Delta V_1\\
-\Delta U_2 && \Delta U_1
\end{matrix}\right]

\left[
\begin{matrix}
E_{1x} && E_{1y} && E_{1z}\\
E_{2x} && E_{2y} && E_{2z}\\
\end{matrix}
\right]
$$

The complete code is:

```c
// positions
glm::vec3 pos1(-1.0,  1.0, 0.0);
glm::vec3 pos2(-1.0, -1.0, 0.0);
glm::vec3 pos3( 1.0, -1.0, 0.0);
glm::vec3 pos4( 1.0,  1.0, 0.0);
// texture coordinates
glm::vec2 uv1(0.0, 1.0);
glm::vec2 uv2(0.0, 0.0);
glm::vec2 uv3(1.0, 0.0);
glm::vec2 uv4(1.0, 1.0);
// normal vector
glm::vec3 nm(0.0, 0.0, 1.0); 

glm::vec3 edge1 = pos2 - pos1;
glm::vec3 edge2 = pos3 - pos2;
glm::vec2 deltaUV1 = uv2 - uv1;
glm::vec2 deltaUV2 = uv3 - uv2;

float f = 1 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
tangent1 = glm::normalize(tangent1);

bittangent1.yx = f * (-deltaUV2.x * edge.x + deltaUV1.x * edge2.x);
bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
bitangent1 = glm::normalize(bitangent1);

```

Because a triangle is always flat we only need to calculate a single tangent/bittangent pair per triangle. If triangles shader vertices, we usually average the vertex properties like normals and tangents/bittangents for each vertex to get a more smooth result.

#### Tangent space normal mapping

To get normal mapping working we have to create a TBN matrix shaders.

```c
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 aBitTangent;

void main()
{
    [...]
    // transform the T, B , N to proper coordinates system.
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitTangent, 0.0)));
    vec3 N = normalize(vec3(model) * vec4(aNormal, 0.0)));
    mat3 TBN = mat3(T, B, N);
}
```

The two ways of using TBN matrix is:

+ Take a TBN matrix that transforms any vector from tangent to world space, give it to the fragment shader and transform the sampled normal from tangent space to world space using the TBN matrix; the normal is then in the same space as the other lighting variables.
+ Take the inverse of the TBN matrix that transforms any vector from world space to tangent space and use this matrix to transform other relevant lighting variables to tangent space.

Transforming vectors from world to tangent space has an added advantage in that we can transform all the relevant vectors to tangent space in the vertex shader instead of in the fragment shader. This works because ***LightPos*** and ***viewPos*** do not change each fragment run  and for FragPos we also calculate its tangent-space position in the vertex shader and let fragment interpolation do this work. Basically, there is no need to transform any vector to tangent space in the fragment shader.

So instead of sending the inverse of the BTN matrix to the fragment shader, we send a tangent-space light position, view position and vertex position to the fragment shader. This is a nice optimization as vertex shader runs considerably less than the fragment shader. The code is:

```c
out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;
 
[...]
  
void main()
{    
    [...]
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vec3(model * vec4(aPos, 0.0));
}  
```

#### Complex objects

For complex object, Assimp has a very useful configuration bit we can set when loading a model called **aiProcess_CalcTangentSpace**. If the bit is supplied to Assimp's ReadFile function Assimp calculates smooth tangent and bittangent vectors for each of the loaded vertices. 

The **aiProcess_CalcTangentSpace** doesn't always work because calculating tangents is based on texture coordinates. If the texture coordinate is tricked, this will not work.

#### Gram-Schmidt process

When tangent vectors are calculated on larger meshes that share a considerable amount of vertices the tangent vectors are generally averaged to give nice and smooth results when normal mapping is applied to these surfaces. The TBN vectors could end up non-perpendicular to each other. Using a mathematical trick called the Gram-Schmidt we can re-orthogonalize the BTN vector such that each vector is again perpendicular to each other.

```
mat3 normalMatrix = transpose(inverse(mat3(model)));
vec3 T = normalize(vec3(normalMatrix * vec4(aTangent, 0.0)));
vec3 N = normalize(vec3(normalMatrix * vec4(aNormal, 0.0)));
T = normalize(T - dot(T, N) * N);
vec3 B = cross(N, T);
mat3 TBN = mat3(T, B, N);
```

