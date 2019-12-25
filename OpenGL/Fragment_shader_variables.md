#### Fragment shader variables

+ **gl_FragCoord**: The x and y component are the window space coordinates of the fragment, originating form the bottom-left of the window. If the window size set by **glViewPort** is 800x600, the x values are between 0 and 800 and y are between 0 and 600. The z component is equal to the depth value of that particular fragment. Using the fragment shader we can calculate a different color value based on the window coordinate of fragment. For example:

  ```c
  void main()
  {
      if(gl_FragCoord.x < 400)
          return vec4(1.0, 0.0, 0.0, 1.0);
      else
          return vec4(0.0, 1.0, 0.0, 1.0);
  }
  ```

+ **gl_FrontFacing**:  If we are not using face culling (by enabling GL_FACE_CULL) then the gl_FrontFacing variable tells if the current fragment is part of a front-facing or a back-facing face. This is a bool value.

  ```c
  void main()
  {
      if(gl_FrontFacing)
          FragColor = texture(frontTexture, TexCoords);
      else
          FragColor = texture(backTexture, TexCoords);
  }
  ```

+ **gl_FragDepth**