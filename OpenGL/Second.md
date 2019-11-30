![avatar](..\image\texture_filtering.png)

Texture filtering can be set for magnifying and minifying operations. For example, we could use nearest neighbor filtering when textures are scaled downwards and linear filtering for upscaled textures.

```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
```

##### Mipmaps

When object are far away and probably produce only a few fragments, OpenGL has difficulties retrieving the right color value for its fragment from the high resolution texture, since it has to pick a texture color for a fragment that spans a large part of texture.

Mipmap is basically a collection of texture images where each subsequent texture is twice as small compared to the previous one.  After a certain distance threshold from the viewer, OpenGL will use a different mipmap texture that best suits the distance to the object. It is also filter between mipmap levels using NEAREST and LINERA. The options includes **GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINERA, GL_LINEAR_MIPMAP_LINEAR**.

```c++
glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER GL_LINEAR_MIPMAP_LINEAR)
glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
```

Since mipmaps are primarily used for when textures get downscaled. Texture magnification doesn't use mipmaps and giving it a mipmap filtering option will generate GL_INVALID_ENUM.

##### Generating a texture

```c++
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
// set the texture wrapping/filtering options (on the currently bound texture object)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// load and generate the texture
int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture" << std::endl;
}
stbi_image_free(data);
```

##### Applying textures

GLSL has a built-in data-type for texture objects called a **sampler** that takes as a postfix the texture type we want e.g. sampler1D, sampler2D. We can add a texture to the fragment shader by simply declaring a uniform sampler2D. Use the glsl's ***texture*** function samples the corresponding color value.

```
out vec4 FragColor;

int vec3 texCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	FragColor = mix(texture(texture1, texCoord), texture(texture2, textCoord), 0.2);
	// The mix returns (1-0.2)*first_val + 0.2*second_val, which is lerp
}
```

Using glUniform1i we can assign a location value to the texture sampler so we can set multiple textures at once in a fragment shader. This location of a texture is known as **texture unit**.  When there more than 1 texture, we must active it and then bind it like

```
glActiveTexture(GL_TEXTURE0)
glBindTexture(GL_TEXTURE_2D, texture);
glActvieTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, texture2);

// set image uniform
ourShader.use(); // don't forget to activate the shader before setting uniforms!  
glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
ourShader.setInt("texture2", 1); // or with shader class
```

Texture unit G_TEXTURE0 is always activated by default, so we can ignore it when there is only one texture.