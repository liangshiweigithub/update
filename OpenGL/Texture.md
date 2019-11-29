#### Texture

To use texture, each vertex should thus have a texture coordinate associated with them that specifies what part of the texture image to sample from. Retrieving the texture color using texture coordinates is called sampling.

##### Texture Wrapping

When the texture coordinate exceeds (1, 1), there are serval option:

+ **GL_REPEAT**: Repeats the texture image, ignore the integer part
+ **GL_MIRRORED_DEFAULT**: Same as GL_REPEAT but mirrors the image with repeat.
+ **GL_CLAMP_TO_DEDE**: Clamps the coordinates between 0 and 1. The higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
+ **GL_CLAMP_TO_BORDER**: Coordinates outside the range are now  given a user-specified border order.

```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT)
```

Each of the aforementioned options can be set per coordinate axis(s, t, r). It the **GL_CLAMP_TO_BORDER** option is chosen, we should specify a border color.

```
float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
glTexParamterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```

##### Texture Filtering

Texture coordinates do not depend on resolution but can be any floating point value, thus OpenGL has to figure out which texture pixel (texel) to map the texture coordinate to. 

+ **GL_NEAREST (nearest neighbor filtering)**: This is default. OpenGL selects the pixel which center is closest to the texture coordinate.
+ **GL_LINEAR (linear filtering)**: Takes an interpolated value from the texture coordinate's texels, approximating a color between the texels.