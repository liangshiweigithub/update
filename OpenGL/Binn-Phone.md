### Advanced Lighting

The flaw of Phong light model is that its specular reflections break down in certain conditions when the view direction and reflection direction is greater than 90 degree. This result the specular area immediately cut off.

#### Blinn-Phong

Blinn-Phong is an extension of the Phong  shading. Instead of relying on a reflection vector, it use **halfway vector** that is a unit vector exactly halfway between the view direction and the light direction. The closer this halfway vector aligns with the surface's normal vector, the higher the specular contribution.
$$
\vec H = \frac{\vec L + \vec V}{||\vec L + \vec V||}
$$
The calculation code is:

```c
vec3 lightDir = normalize(lightPos - FragPos);
vec3 viewDir = normalize(viewPos - FragPos);
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(normal, halfwayDir), 0), shinness);
vec3 specular = lightColor * spec;
```

The only difference between Blinn-Phong and Phong specular reflection is that we now measure the angle between the normal and the halfway vector compared to the angle between the view direction and reflection vector.

Another difference is that the angle between the halfway vector and the surface normal is often shorted than the angle between the view and reflection vector. So the specular shininess exponent has to be set a bit higher. A general rule is set between 2 and 4 times of Phone shininess exponent.