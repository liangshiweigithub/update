####                                                   															Colors

The colors we see in real life are not the colors the objects actually have, but are the colors reflected from the object (not absorbed by object).

####                                              														Basic Lighting

**Phone lighting model** consists of 3 components: ambient, diffuse and specular lighting. To create scenes we want to at least simulate these 3 light components.

##### Ambient light

Ambient light is a very simplistic model of global illumination. We use a small constant light color that we add to the final resulting color of the object's fragments.

```c
void main()
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0);
}
```

##### Diffuse lighting

Diffuse light is relative to the angle between surface normal and light direction. The resulting dot product of surface normal and light direction can be used to calculate the light's impact on the fragment's color.

##### Transformation of normal vectors

+ Normal vectors do not have a homogeneous coordinate, which means that translation do and should not have effect on the normal vectors.
+ If the model matrix would perform a non-uniform scale, the vertices would be changed in such a way that the normal vector is not perpendicular to the surface anymore. So we can't transform the normal vectors with such a model matrix because the normal vectors are not perpendicular to the corressponding surface anymore

To fix this problem, we use **normal matrix** instead of model matrix. This matrix is defined as **the transpose of the inverse of the upper-left corner of the model matrix which does not include the translation part**. The shader code is

```
Normal = mat3(transpose(inverse(model))) * aNormal
```

Inversing matrices is  not suggested to do in shader since they have to be done on each vertex of your scene.

##### Specular Lighting

Specular lighting is based on the reflective properties of light. To calculate the specular light, we first calculate a reflection vector by reflecting the light direction around the normal vector. Then we calculate the angular distance between this reflection vector and the view direction. The fragment shader is like

```c
#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
	
	// sepcular
	float specluarStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectedDir = reflect(-lightDir, norm);
	float specular = pow(max(dot(reflectedDir, viewDir),0), 32) * lightColor 
            
    vec3 result = (ambient + diffuse + sepcular) * objectColor;
    FragColor = vec4(result, 1.0);
} 
```

The 32 used to calculate specular is shinniess value of the highlight. The higher the shininess value of an object, the more it properly reflects the light instead of scattering it all around and thus the smaller the light becomes. 

We can implement the Phong lighting model in the vertex shader. The advantage of doing lighting in the vertex shader is that it is a lot more efficient since there are generally a lot less vertices than fragments. But this generates less realistic lighting. When the **Phong lighting model** is implemented in the vertex shader it is called **Gouraud shading** instead of **Phong shading**.

####                                                        												Material

Material is used to describe each of the 3 lighting components: ambient, diffuse and specular lighting.

```c
// definition of material in shader
struct Material{
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shinines;
}
uniform Material material;

void main()
{
	// ambient
	vec3 ambient = lightColor * material.ambient;
	
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0);
	vec3 diffuse = (diff * material.diffuse) * lightColor;
	
	// specular
	vec3 viewDir = (viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
	vec3 specular = spec * material.specular * lightColor;
	return vec4(ambient + diffuse + specular, 1.0);
} 
```

A struct in GLSL only acts as an encapsulation of uniform variable so if we want to fill the struct we stil have to set the individual uniforms like

```
lightShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
```

##### Light properties

Using the former shader, the object is way too bright. The reason for the object being too bright is that the ambient, diffuse and specular colors are reflected with full force from any light source. Light sources also have different intensities for their ambient, diffuse and specular components respectively. To solve the bright problem, we specify intensity vectors for each of the lighting comopoents. We restrict the ambient color by setting the light's ambient intensity to a lower value through a struct:

```c
// definition of light properties.
struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}
uniform Light light;

// calculate of light
vec3 ambient = light.ambient * material.ambient
```

A light source has a different intensity for its ambient, diffuse and specular light. 

+ The ambient light is usually set to a low intensity because the ambient color shouldn't be too dominant. 

+ The diffuse component of a light source is usually set to the exact color we'd like the light to have, often a bright white color.

+ The specular component is usually kept at vec3(1.0) shining at full intensity. 

  #### 										                                Lighting maps

Objects in real world consists of serval materials. We need to extend the previous system by introducing **diffuse and specular maps**. These allows us to influence the diffuse (and indirectly the ambient component since they're almost always the same) and the specular components of an object with much more precision.

##### Diffuse maps

A diffuse map is an image wrapped around an object that we can index for unique color values per fragment. By diffuse map, we can set the diffuse color of an object for each individual fragment. Using a diffuse map in shaders is exactly the same as with the texture tutorials. The texture is stored as a sampler2D. A sampler2D is a so called opaque type which means we can't instantiate these types, but only define them as uniforms. The shader code is:

```c
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 
```

 ##### Specular maps

 Each pixel of the specular map can be displayed as a color vector. In the fragment shader we then sample the corresponding color value and multiply this value with the light's specular intensity.

#### 											Light casters

A light source that casts light upon objects is called a light caster.

##### Directional Light

Directional light: All its light rays have the same direction.

```c
// definition of directional light
struct DirectionLight{
	vec3 direction;   // specify the direction instead of position
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

void main()
{
    vec3 lightDir = normalize(-light.direciton); // pay attention to the "-"
}
```

 Here we can use a vec4 to specify the position or direction property of the light. When using ,check is the forth is 0 or 1 to determine it is a vector or position component. When this affect the efficiency since it has dynamic judgement.

##### Point lights

A point light is a light source with a given position somewhere in a world that illuminates in all directions where the light rays fade out over distance. The attenuation equation is:
$$
F_{att}=\frac{1.0}{K_c+K_l*d+K_q*d^2}
$$
Here $$d$$ represents the distance from the fragment to the light source. $$K_c, K_l, k_q$$ are constants. Due to the quadratic term the light will diminish mostly at a linear fashion until the distance becomes large enough for the quadratic term to surpass the linear term and then the light intensity will decrease a lot faster.