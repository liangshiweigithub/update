#### Colors

The colors we see in real life are not the colors the objects actually have, but are the colors reflected from the object (not absorbed by object).

#### Basic Lighting

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