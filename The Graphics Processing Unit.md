### Processing Unit

Shader code is a small processor that does some relatively isolated task.

#### Data-Parallel Architectures

Various strategies are used by different processor architectures to avoid stall caused by accessing data. The GPU is optimized for throughput, defined as the maximum rate at which data can be processed. With less chip area dedicated to cache memory and control logic, latency for each shader core is generally considerably higher than what a CPU processor encounters. GPU uses the SIMD. Each pixel shader invocation for a fragment is called a ***thread***. It consists of a bit of memory for the input values to the shader, along with any register space needed for the shader execution. Threads that use the same shader program are bundled into groups, called ***warps*** by NVIDIA and ***wavefronts*** by AMD. A warp is scheduled for execution by some number of GUP shader cores, using SIMD-processing. Each thread is mapped to a SIMD lane. 

During warp execution when a memory fetch is encountered, The warp is swapped out for a different warp of all threads, which is then executed by the 32 cores. The shader program's structure is an important characteristic that influences efficiency. The more registers needed by the shader program associated with each thread, the fewer thread, and thus the fewer warps can be resident in the GPU. Warps that are resident are said to be "in fight", and this number is called the ***occupancy***. High occupancy means that there are many warps available for processing, so that idle processors are less likely.

Another factor effecting overall efficiency is dynamic branching, caused by "if" statements and loops. The "if" statement can cause ***thread divergence***, where a few threads may need to execute a loop iteration or perform an "if" path that the other threads in the warp do not, leaving them idle during this time.

##### The Programmable Shader Stage

Modern shader programs use a unified shader design. This means that the vertex, pixel, geometry, and tessellation-related shaders share a common programming model. A processor that implements this model is called a ***common-shader core*** in DirectX, and GPU with such cores  is said to have a unified shader architecture.

A ***draw call*** invokes the graphic API to draw a group of primitives, so causing the graphics pipeline to execute and run its shaders. Each programmable shader stage has two types of inputs: ***uniform inputs***, with values that remain constant throughout a draw call (can be changed between draw calls), and ***varying input***, data that come from the triangle's vertices or from rasterization.

The term ***flow control*** refers to the use of branching instructions to change the flow of code execution. There are two types of flow control. ***Static flow control*** branches are based on the values of uniform inputs. There is no thread divergence, since all invocations take the same code path. ***Dynamic flow control*** is based on the values of varying inputs, meaning that each fragment can execute the code differently. This costs performance. 

##### The Vertex Shader

The vertex shader provides a way to modify, create, or ignore values associated with each triangle's vertex, such as its color, normal, texture coordinates, and position. A vertex shader must always transforms vertices from model space to homogeneous clip space.

***Input assembly is usually presented as a process that happens before the vertex shader is executed.

##### The Tessellation Stage

The stage allows us to render a curved surface. The GPU takes each surface description and turn it into a representative set of triangles. It can save memory by a surface description instead of vertex. Besides, its ability to control the level of detail can also control performance.

##### The Pixel Shader

The piece of a triangle partially or fully overlapping the pixel is called ***fragment***. The values at the triangle's vertices, including the z-value used in the z-buffer, are interpolated across the triangle's surface for each pixel. These values are passed to the pixel shader, which then processes the fragment. The type of interpolation performed across the triangle is specified by the pixel shader program. Nowdays, instead of sending results of a pixel shader's program to just the color and z-buffer, multiple sets of values can be generated for each fragment and save to different buffers, each called a ***render target***. This is the idea of ***multiple render targets***.

