##### State machine

OpenGL is by itself a large state machine: A collection of variables that define how OpenGL should currently operate. The state of OpenGL is commonly referred to as the OpenGL context. When using OpenGL, we often change its state by setting some options, manipulating some buffers and then render using the current context.

##### Objects

An  object in OpenGL is a collection of options that represents a subset of OpenGL's state. For example, we could have an object that represents the setting of the drawing window. We can visualize an object as a C-like struct: 

```c
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

##### GLFW

GLFW is a library, written in C, specifically targeted at OpenGL providing the bare necessities required for rendering goodies to the screen. It allows us to create an OpenGL context, define window parameters and handle user input.