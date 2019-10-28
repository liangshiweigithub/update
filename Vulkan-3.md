## Part3: Graphics Pipeline and Drawing

#### Creating a Render Pass

1. Definition of render pass: 

   Process of deferred shading:

   +  The first subpass draws the geometry with shaders that fill the G-buffer: store diffuse color in one texture, normal vectors in another, shininess in depth in yet another.  
   + Next for each light source, drawing is performed that reads some of the data(normal vectors, shininess, depth/position), calculates lighting and stores it in another texture.
   +  Final pass aggregates lighting data with diffuse color.

   So render pass is a set of data required to perform some drawing operations: storing data in textures and reading data from other textures. In Vulkan, a render pass represents a set of framebuffer attachments(images) required for drawing operations and a collection of subpasses that drawing operations will be ordered into. It is a construct that collects all color, depth and stencil attachments and operations modifying them in such a way that driver does not have to deduce this information by itself what may give a substantial optimization opportunities on some GPUs. 

   A subpass consists of drawing operations that use the same attachments. Each of these drawing operations may read from some input attachments and render data in some other attachments. A render pass also describes the dependencies between attachments: in one subpass we perform rendering into the texture, but in another this texture will be used as a source of data (be sampled from).

2. ***vkCreateRenderPass*** is used to create a render pass, which requires a pointer to a structure describing all the attachments involved in rendering and all the subpasses forming the render pass. Struct that describes each attachment is ***VkAttachmentDescription***, which contains the following:

   + flags: Describes additional properties of an attachment. Only aliasing flag is available, which informs the driver that the attachment shares the same physical memory with another.
   + format: Format of an image used for the attachment.
   + samples: Number of samples of the image. If use multisampling, the value is bigger than 1.
   + loadOp: specifies what to do with the image's contents at the beginning of a render pass, whether we want them cleared, preserved, or don't care. This parameter also refers to depth part of depth/stencil images.
   + storeOp: Informs the driver what to do with the image's contents after render pass. This parameter also refers to the depth part of depth/stencil images.
   + stencilLoadOp: The same as loadOp but for the stencil part of depth/stencil image.
   + stencilStoreOp: for stencil part of depth/stencil images, for color attachment this is ignored.
   + initialLayout: The layout the given attachment will have when the render pass starts.
   + finalLayout: The layout the driver automatically transition the given image into at the end of pass.

   Layout is an internal memory arrangement of an image. Image data may be organized in such a way that neighboring "image pixels" are also neighbors in memory, which can increase cache hit when read. Image layout may be changed using image memory barriers. Initial layout informs the hardware about the layout the application "provides" the given attachment with.

