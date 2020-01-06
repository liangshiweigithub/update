**omnidirectional shadow map** is used to generate dynamic shadows in all surrounding directions. This technique is perfect for lights as a real point light that would cast shadows in all directions.  The main difference between directional shadow and omnidirectional shadow is the depth map used. 

The depth map needed requires rendering a scene from all surrounding directions of a point light so we use **cubemap** instead of normal 2D depth map.

#### Light space transform

After setting the framebuffer and cubemap we need some way to transform all the scene's geometry to the relevant light spaces in all 6 directions of the light. A light space transformation matrix (projection and view matrix) T for each face is needed.