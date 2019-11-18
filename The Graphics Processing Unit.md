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