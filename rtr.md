### Render pipeline

The main function of the pipeline is to generate, or render, a two-dimensional image, given a virtual camera, three-dimensional objects, light sources, and more. A coarse division of the real-time rendering pipeline into four stages -- ***application, geometry processing, rasterization, and pixel processing.*** A functional stage has a certain to perform but does not specify the way that task is executed in the pipeline.

#### Application stage

Tasks performed include collision detection, global acceleration algorithms, animation, physics simulation and so on.

#### Geometry processing

Deals with transforms, projections and other types of geometry processing. This stage computes what is to be drawn, how it should be drawn, and where to draw. It is responsible for most of the per-triangle and per-vertex operations which is divided into four stages: ***Vertex Shading***, ***Projection***, ***Clipping***, ***Screen Mapping***.

##### Vertex Shading

Two main tasks of vertex shading:

+ To compute the position for a vertex.
+ To evaluate whatever the programmer may like to have as vertex out data, such as normal and texture coordinates.

The following is optional:

###### Tessellation

With tessellation, a curved surface can be generated with an appropriate number of triangles. Vertices can be used to describe a curved surface, such as a ball. Such surfaces can be specified by a set of patches, and each patch is made of a set of vertices. The tessellation stage consists of a series of stage itself -- hull shader, tessellator, and domain shader -- that converts these sets of patch vertices into larger sets of vertices that are then used to make new sets of triangles. The camera can be used to determine how many triangles are generated: many when the patch is close, few when it is far.

###### Geometry shader

It is like the tessellation shader in that it takes in primitives of various sorts and can produce new vertices. The creation is limited in scope and the types of output primitives are much more limited. The typical example is fireworks explosion.

###### Stream output

This stage lets us use the GPU as a geometry engine. Instead of sending our processed vertices down the rest of the pipeline to be rendered to the screen, at this pointer we can optionally output these to an array for further processing. These data can be used by the CPU.

##### Clipping

Primitives that are partially inside the view volume that require clipping.

##### Screen Mapping

Only the clipped primitives inside the view volume are passed on to the screen mapping stage, and the coordinates are still three dimensional when entering this stage. The x and y coordinates are transformed (by scaling operation) to form ***screen coordinates***. Screen coordinates together with the z-coordinates are also called ***window coordinates***.  The z coordinates are mapped to 0,1

#### Rasterization 

Takes input as three vertices, forming a triangle, and finds all pixels that are considered inside that triangle.

#### Pixel processing

 Executes a program per pixel to determine its color and may perform depth testing to see whether it is visible or not. This stage is divided into ***pixel shading*** and ***merging***.

##### Pixel Shading

Any per-pixel shading computations are performed here, using the interpolated shading data as input. The end result is one or more colors to be passed on to the next. This stage is executed by programmable GPU cores.

##### Merging

The information for each pixel is stored in the color buffer, which is a rectangular array of colors. It is the responsibility of the merging stage to combine the fragment color produced by the pixel shading stage with the color currently stored in the buffer. This stage is not fully programmable. It is highly configurable. This stage is also responsible for resolving visibility by the z-buffer.

The ***alpha channel*** is associated with the color buffer and stores a related opacity value for each pixel. In older APIs, the alpha channel was also used to discard pixels selectively via the alpha test feature. Nowadays a discard operation can be used to trigger a discard. This type of test can be used to ensure that fully transparent fragments do not affect the z-buffer.

The ***stencil buffer*** is an offscreen buffer used to record the locations of the rendered primitive. It typically contains 8 bits per pixel. Primitives can be rendered into the stencil buffer using various functions, and the buffer's contents can then be used to control rendering into the color buffer and z-buffer. Assume that a filled circle has been drawn into the stencil buffer. This can be combined with an operator that allows rendering of subsequent primitives into the color buffer only where the circle is present. All these functions at the end of the pipeline are called ***raster operations*** (ROP) or ***blend operations***. It is possible to mix the color currently in the color buffer with the color of the pixel being processed inside a triangle. This can enable effects such as transparency or the accumulation of color samples.

Points, lines, and triangles are the rendering primitives from which a model or an object is built. 