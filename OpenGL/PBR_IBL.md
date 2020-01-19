### Diffuse irradiance

IBL or **image base lighting** is a collection of techniques to light objects, by treating the surrounding environment as one big light source. This is accomplished by manipulating a cubemap environment map: treating each cubemap pixel as a light emitter.

In the pbr quation, solving the integral requires us to sample the environment map from not just one direction, but all possible directions $w_i$ over the hemisphere $\Omega$ which is far to expensive. The diffuse part of the light is:
$$
L_o(p, w_o) = k_d\frac{c}{\pi}\int_{\Omega}{}L_i(p, w_i)n*w_idw_i
$$
This gives an integral that only depends on $w_i$ (assuming $p$ is at the center of the environment map).  With this we can calculate or pre-compute a new cubemap that stores in each sample direction $w_o$ the diffuse integral's result by convolution. The cubemap, that for each sample direction $w_o$ stores the integral result, can be thought of as the pre-computed sum of all indirect diffuse light of the scene hitting some surface aligned along direction $w_o$. This is known as **irradiance map**.

#### PRB and HDR

##### The radiance HDR file format

The radiance file format (.hdr) stores a full cubemap with all 6 faces as floating point data allowing anyone to specify color values outside the 0.0 and 1.0 range to give lights their correct color intensity.

##### From Equiretangular to Cubemap

Render a unit cube and project the equirectangular to it. The shader code is:

```c
#version 330 core

out vec4 FragColor;
in vec3 localPos;

uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(localPos);
    vec3 color = texture(equirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
```

#### Cubemap convolution

![avatar](../image/ibl_spherical_integrate.png)

The integral of the reflectance equation resolves around the solid angle $dw$ which is difficult to work with. We'll integrate over its equivalent spherical coordinates $\theta$ and $\phi$.
$$
L_o(p, \phi_o,\theta_o) = k_d\frac{c}{\pi}\int_{\phi=0}^{2\pi}\int_{\theta=0}^{\frac{1}{2}pi}L_i(p, \phi_i, \theta_i)cos(\theta)sin(\theta)d\phi d\theta
$$

$$
L_o(p,\phi_o, \theta_o) = 
        k_d\frac{c}{\pi} \frac{1}{n_1 n_2} \sum_{\phi = 0}^{n_1} \sum_{\theta = 0}^{n_2} L_i(p,\phi_i, \theta_i) \cos(\theta) \sin(\theta)  d\phi d\theta
$$

The fragment shader is

```c
#version 330 core

out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube enviromentMap;
const float PI = 3.14159;

void main()
{
    vec3 N = normalize(WorldPos);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up = cross(N, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;

    for(float phi = 0.0;phi<2*PI;phi+=sampleDelta)
    {
        for(float theta=0.0;theta<0.5*PI;theta+=sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            irradiance += texture(enviromentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    FragColor = vec4(irradiance, 1.0);
}
```

As the ambient light comes from all directions within the hemisphere oriented around the normal N there's no single halfway vector to determine the Fresnel response. So we calculate the Fresnel from the angle between the normal and view vector. However, earlier we used the micro-surface halfway vector, influenced by the roughness of the surface, as input to Fresnel equation. As we don't take any roughness into account, the surface's reflective ratio will always end up relatively high. We alleviate the issue by injecting the roughness term.

```c
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughtness)
{
    return F0+(max(1-roughness, F0)-F0)*pow(1.0 - cosTheta, 5);
}

vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0), F0, roughness);
vec3 kD = 1-kS;
vec3 irradiance = texture(irradianceMap, N).rgb;
vec3 diffuse = irradiance * albedo;
vec3 ambient = (kD * diffuse) * ao;
```

### Specular IBL

The Cook-Torrance specular portion isn't constant vector over the integral and is dependent on the incoming light direction, but also the incoming view direction. Here Epic Games propose **split sum approximation** to calculate the integral.

The **split sum approximation** splits the specular part of the reflectance equation into two separate part that we can individually convolute and later combine in the PBR shader for specular indirect image based lighting.
$$
L_o(p,w_o)=\int_{\Omega}{}L_i(p, w_i)dw_i*\int_{\Omega}f_r(p,w_i,w_o)n*w_idw_i
$$
The first part is **pre-filtered environment map** which is a pre-computed environment convolution map, but this time taking roughness into account. For increasing roughness levels, the environment map is convoluted with more scattered sample vectors, creating more blurry reflections. For each roughness level we convolute, we store the sequential blurrier result in the pre-filtered map's mipmap levels.

We generate the sample vectors and their scattering strength using the normal distribution function of the Cook-Torrance BRDF that takes as input both a normal and view direction. As we don't known beforehand the view direction when convoluting the environment map, Epic Games makes a further approximation by assuming the view direction is always equal to the output sample direction $w_o$.

```
vec3 N = Normalize(w_o);
vec3 R = N;
vec3 V = R;
```

The second part of the equation equals the BRDF part of the specular integral. If we pretend the incoming radiance is completely white for every direction we can pre-calculate the BRDF's response given an input roughness and an input angle between the normal n and light $w_i$ or $n\cdot w_i$. Epic Games stores the pre-computed BRDF's response to each normal and light direction combination on varying roughness values in a 2D lookup texture(LUT) known as **BRDF integration map**.

The lookup texture is generated by treating the horizontal texture coordinate of a plane as the BRDF's input $n*w_i$ and its vertical texture coordinate as the input roughness value. With this BRDF integration map and the pre-filtered environment map we can combine both to get the result of the specular integral:

```c
float lod = getMipLevelFromRoughness(roughness);
vec3 prefilteredColor = textureCubeLod(PrefilteredEnvMap, refVec, lod);
vec2 envBRDF = texture2D(BRDFIntegrationMap, vec2(NdotV, roughness)).xy;
vec3 indirectSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
```

#### Pre-filtering an HDR environment map

The difference between how we convoluted an irradiance map is that we account for roughness and store sequentially rougher reflections in the pre-filtered map's mip levels. To make sure we allocate enough memory for its mip levels we call **glGenerateMipmap**.

```c
unsigned int prefilterMap;
glGenTextures(1, &prefilterMap);
glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
for(unsigned int i=0;i<6;++i)
{
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITION_X+i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
}
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// attension point
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

```

We plan to sample the prefilterMap its mipmaps you'll need to make sure its minification filter is set to **GL_LINEAR_MIPMAP_LINEAR**. The resolution of the base mip level increase if there are a large number of smooth material.

![avatar](../image/ibl_specular_lobe.png)

For specular reflections, based on the roughness of a surface, the light reflects closely or roughly around a reflection vector $r$ over a normal $n$. The general shape of possible outgoing light reflections is known as the **specular lobe**. As roughness increase, the specular lobe's size increases; and the shaped of the specular lobe changes on varying incoming light directions.

When it comes to the microsurface model, we can imagine the specular lobe as the reflection orientation about the microfacet halfway vectors given some incoming light direction. It make sense to generate the sample vectors in a similar fashion as most would otherwise be wasted. This is **importance sampling**.

##### Monte Carlo integration and importance sampling

Monte Carlo integration revolves mostly around a combination of statics and probability theory. It helps in discretely solving the problem of figuring out some statistic or value of a population without having to take all of the population into consideration.

The **law of large numbers** is that if a smaller set of size N of truly random samples from the total population is measured, the result will relatively close to the true answer and get closer as the number of samples N increases.

Monte Carlo integration builds on the law of large numbers.
$$
O =  \int\limits_{a}^{b} f(x) dx 
      = 
      \frac{1}{N} \sum_{i=0}^{N-1} \frac{f(x)}{pdf(x)}
$$
To solve the integral, we take N random samples over the population a to b, add them together and divide by the total number of samples to average them. The $pdf$ is **probability density function** that stands the probability a specific sample occurs over the total sample set.

The **unbiased** estimation means that given an ever-increasing amount of samples we will eventually converge to the exact solution of the integral. Monte Carlo estimation are biased, meaning the samples aren't completely random, but forward to a specific value or direction. These biased Monte Carlo estimators have a faster rate of convergence meaning they can converge to the exact solution at a much faster rate.

The **low-discrepancy sequences** generate random samples, but each sample is more evenly distributed. When using low-discrepancy sequence for generating Monte Carlo sample vectors, the process is known as **Quasi-Monte Carlo integration**. It has a faster rate of convergence.

Generate sample vectors in some region constrained by the roughness oriented around the microfacet's halfway vector. This is importance sampling. By combining Quasi-Monte Carlo sampling with a low discrepancy sequence and biasing the sample vectors using importance sampling we get a high rate of convergence.

##### A low-discrepancy sequence

We'll pre-compute the specular portion of indirect reflectance equation using importance sampling given a random low-discrepancy sequence based on the Quasi-Monte Carlo method. The sequence is Hammersley Sequence.

The program that get a Hammersley sequence sample i over N total samples

```c
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
```

##### GGX Importance sampling

The sample vectors will be generated biased towards the general reflection orientation of the microsurfaces halfway vector based on the surface's roughness. The sampling process is similar as before: begin a loop, generate a random (low-discrepancy) sequence value, take the sequence value to generate a sample vector in tangent space, transform to world space and sample the scene's radiance.

To build a sample vector, We take the NDF (normal distribution function) and combine the GGX NDF in the spherical sample vector process to orientation and bias the sample vector towards the specular lobe

```c
vec3 ImportanceSampleGGX(vec3 Xi, vec3 N, float roughtness)
{
    float a = roughness * roughness;
    
    float phi = 2 * PI * Xi.x;
    float cosTheta = sqrt((1.0-Xi.y)/(1.0+(a*a-1.0)*Xi.y));
    float sinTheta = sqrt(1.0-cosTheta*cosTheta);
    
    vec3 H;
    H.x = cos(phi)*sinTheta;
    H.y = cos(phi)*sinTheta;
    H.z = cosTheta;
    
    vec3 up = abs(N.z) < 0.999?vec3(0.0, 0.0, 1.0): vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bittangent * H.y + N * H.z;
    return normalize(smapleVec);
}
```

The final code of the pre-filter convolution shader is:

```C
#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
  
void main()
{		
    vec3 N = normalize(localPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMap, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}
```

#### Pre-filter convolution artifacts

##### Cubemap seams at high roughness

The lower mip levels are both of a lower resolution and the pre-filter map is convoluted with a much larger sample lobe, the lack of between cube face filtering becomes apparent.

```
glEnable(GL_TEXTURE_CUBE_MAP_SAMPLES);
```

##### Bright dots in the pre-filter convolution

Convoluting the specular reflections requires a large number of samples to properly account for the wildly varying nature of HDR environmental reflections. On some environment the sample might not be enough at some of the rougher mip levels in which case dotted patterns emerge around bright areas.

We can reduce this artifact by not directly sampling the environment map, but sampling a mip level of the environment map based on the integral' PDF and roughness;

```c
float D = DistributionGGX(NdotH, roughness);
float pdf = (D * NdotH/(4.0*HDotV)) + 0.00001;

float res = 512.0;
float saTexel = 4.0 * PI/(6.0 * resolution * resolution);
float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.00001);
float mipLevel = roughness == 0.0?0.0:0.5*log2(saSample/saTexel);
```

#### Pre-computing the BRDF

The process of deducing the equation is ignored.

