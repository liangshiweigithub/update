### Shading Basics

##### Shading Models

The first step in determining the appearance of a rendered object is to choose a shading model to describe how the object's color should vary based on factors such as surface orientation, view direction, and lighting. Shading model often have properties used to control appearance variation. Setting the values of these properties is the next step in determining object appearance.

The ***Gooch shading model*** was designed to increase legibility of details in technical illustrations. The mathematical definition of the model is:
$$
c_{shaded} = s*c_{highlight} + (1-s)(tc_{warm}+(1-t)*c_{cool})\\
c_{cool}=(0,0,0.55)+0.25*c_{surface}\\
c_{warm}=(0.3,0.3,0)+0.23*c_{surface}\\
c_{highlight}=(1,1,1)\\
t=\frac{n*l+1}{2}\\
r=2(n*l)n-l\\
s=(100(r*v)-97)^{\overline+}
$$
Linear interpolation appears so often in shaders that it is a built-in function, called ***lerp*** and ***mix***, in every shader language we have seen.

##### Light Sources

The impact of lighting on our example shading model was quite simple; it provided a dominant direction for shading. The next step in the lighting complexity is for the shading model to react to the presence or absence of light in a binary way.

It is a small step from the binary presence or absence of light to a continuous scale of light intensities. This could be expressed as a simple interpolation between absence and full presence, which implies a bounded range for the intensity, perhaps 0 to 1, or as an unbounded quality that affects the shading in some other way. A common option for the latter is to factor the shading model into lit and unlit parts, with the light intensity k<sub>light</sub> linearly scaling the lit part:
$$
c_{shaded} = f_{unlit}(n,v) + k_{light}f_{lit}(l,n,v)
$$
This easily extends to an RGB light color c<sub>light</sub>,
$$
c_{shaded}=f_{unlit}(n,v)+c_{light}f_{lit}(l,n,v)
$$
and to multiple light sources:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}c_{light_{i}}f_{lit}(l_{i},n,v)
$$
**The unlit part f<sub>unlit</sub>(n, v) of shading model often expresses some form of lighting that does not come directly from explicitly placed light sources, such as light from the sky or light bounced from surrounding objects.**

The effect of light on a surface can be visualized as a set of rays, with the density of rays hitting the surface corresponding to the light intensity for surface shading purposes. The ray density is proportional to the dot product of **l** and **n** when it is positive. Negative values corresponds to light ray coming from behind the surface, which has no effect. So, before multiplying the light's shading by the lighting dot product, we first clamp the dot product to 0:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}(l_{i}*n)^{+}c_{light_{i}}f_{lit}(l_{i},n,v)
$$
The simplest possible choice for the function f<sub>lit</sub>, is to make it a constant color,
$$
f_{lit}() = c_{surface}
$$
The final function is:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}(l\cdot n)^+c_{light}c_{surface}
$$
The lit part of this model is corresponds to the ***Lambertian*** shading model. This model works in the context of ideal diffusely reflecting surfaces. In the equations we can see that a light source interacts with the shading model via two parameters: the vector **l** pointing toward the light and the light color c<sub>light</sub>.

