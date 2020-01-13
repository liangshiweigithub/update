### Deferred Shading

Forward rendering or forward shading: Render an object, light it according to all light sources in a scene and then render the next object, and so on for each object in the scene. The flaw of this method is:

+ Each rendered object has to iterate over each light source for every rendered fragment which is a lot, this is quite heavy on performance.
+ Tends to waste a lot of fragment shader runs in scenes with a high depth complexity as most fragment shader outputs are overwritten.

Deferred shading is based on the idea that we defer or postpone most of the heavy rendering (like lighting) to a later stage. It consist of two passes:

+ **Geometry pass**: Render the scene once and retrieve all kinds of geometrical information from the objects that we store in a collection of textures called the **G-buffer**. The information includes position vectors, color vectors, normal vectors specular vectors and so on.
+ **Lighting pass**: Render a screen filled quad and calculate the screen's lighting for each fragment using the geometrical information stored in the G-buffer. Instead of taking each object all the way from the vertex shader to the fragment shader we decouple its advanced fragment processes to a later stage. During light calculations, we take all the required input variables from the corresponding G-buffer textures instead of the vertex shader.

The advantages are:

+ Whatever fragment ends up in the G-buffer is the actual fragment information that ends up as a screen pixel. This ensures that for each pixel we process in the lighting pass we only do this once.
+ Allows to render a much larger amount of light sources.

The disadvantage is:

+ G-buffer stores a relatively large amount of scene data in its texture colorbuffers which eats memory, especially position vectors require a high precision.
+ It doesn't support blending and MSAA no longer works.