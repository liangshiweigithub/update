### 									Advanced OpenGL

#### Depth testing

The depth buffer is a buffer that just like the color buffer (that stores all the fragment colors), stores information per fragment and has the same width and height as the color buffer. It is created automatically and stores its depth values as 16, 24 or 32 bit floats.

When depth testing is enabled OpenGL tests the depth value of a fragment against the content of the depth buffer.