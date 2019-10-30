## Part3: Graphics Pipeline and Drawing

#### Creating a Render Pass

1. Definition of render pass: 

   Process of deferred shading:

   +  The first subpass draws the geometry with shaders that fill the G-buffer: store diffuse color in one texture, normal vectors in another, shininess and depth in yet another.  
   + Next for each light source, drawing is performed that reads some of the data(normal vectors, shininess, depth/position), calculates lighting and stores it in another texture.
   +  Final pass aggregates lighting data with diffuse color.

   So render pass is a set of data required to perform some drawing operations: storing data in textures and reading data from other textures. In Vulkan, a render pass represents a set of framebuffer attachments(images) required for drawing operations and a collection of subpasses that drawing operations will be ordered into. It is a construct that collects all color, depth and stencil attachments and operations modifying them in such a way that driver does not have to deduce this information by itself what may give a substantial optimization opportunities on some GPUs. 

   A subpass consists of drawing operations that use the same attachments. Each of these drawing operations may read from some input attachments and render data in some other attachments. A render pass also describes the dependencies between attachments: in one subpass we perform rendering into the texture, but in another this texture will be used as a source of data (be sampled from).

2. **Render Pass Attachment Description**

   To create a render pass, first we prepare an array with elements describing each attachment, regardless of the type of attachment. Struct that describes each attachment is ***VkAttachmentDescription***, which contains the following:

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

3. **Subpass Description**: The ***VkSubpassDescription*** structure is used to describe each subpass that our render pass will include. It includes:

   + flags: For future use.
   + pipelineBindPoint: Type of pipeline in which this subpass will be used.
   + inputAttachmentCount: Number of elements in the pInputAttachments array.
   + pInputAttachments: Array with elements describing which attachments are used as an input and can be read from inside shaders.
   + colorAttachmentCount: Number of elements in pColorAttachments and pResolverAttachments arrays.
   + pColorAttachments: Array describing attachments which will be used as color render targets
   + pResolveAttachments: Each element corresponds to an element from a color attachments array. Any such color attachment will be resolved to a given resolve attachment.
   + pDepthStencilAttachment: Description of an attachment that will be used for depth data.
   + preserveAttachmentCount: Number of elements in pPreserveAttachments Array.
   + pPreserveAttachments: Describling attachments that should be preserved. If a subpass doesn't use some of the attachments but we need their contents in later subpasses, specify these attachments here.

4. **Render Pass Creation**, the code is:

   ```
   vkRenderPassCreateInfo render_pass_create_info = {
     VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,    // VkStructureType                sType
     nullptr,                                      // const void                    *pNext
     0,                                            // VkRenderPassCreateFlags        flags
     1,                                            // uint32_t                       attachmentCount
     attachment_descriptions,                      // const VkAttachmentDescription *pAttachments
     1,                                            // uint32_t                       subpassCount
     subpass_descriptions,                         // const VkSubpassDescription    *pSubpasses
     0,                                            // uint32_t                       dependencyCount
     nullptr                                       // const VkSubpassDependency     *pDependencies
   };
   
   if( vkCreateRenderPass( GetDevice(), &render_pass_create_info, nullptr, &Vulkan.RenderPass ) != VK_SUCCESS ) {
     printf( "Could not create render pass!\n" );
     return false;
   }
   
   return true;
   ```

   We start by filling the ***VkRenderPassCreateInfo*** structure, which contains

   + sType, pNext, flags.
   + attachmentCount: Number of all different attachments used during whole render pass.
   + pAttachments: Array specifying all attachments used in a render pass.
   + subpassCount: Number of subpasses a render pass consist of.
   + pSubpasses: Array with descriptions of all subpasses.
   + dependencyCount: Number of elements in pDependencies array.
   + pDependencies: Array describing dependencies between pairs of subpasses.

   Dependencies describes what parts of the graphics pipeline use memory resource in what way. Each subpass may use resource in a different way. We use the ***vkCreateRenderPass*** function to create render pass.

#### Creating a Framebuffer

In the create of render pass, we have specified formats of all attachments and described how attachments will be used by each subpass. But we didn't specify WHAT attachments we will  be using or, in other words, what images will be used as these attachments. This done through a framebuffer.

A framebuffer describes specific images that the render pass operates on. In OpenGL, a framebuffer is a set of textures (attachments) we are rendering into. In Vulkan, it describes all the textures (attachments) used during the render pass, not only the images we are rendering into (color and depth/stencil attachments) but also images used as a source of data.

Before we can create a framebuffer, we must create image views for each image used as a framebuffer and render pass attachment. In Vulkan, images are not accessed directly. For this purpose, image views are used. **Image View** represent images, they "wrap" images and provide additional data for them.

1. **Creating Image Views**: We must create an image view for each of the swap chain images.

   ```
   const std::vector<VkImage> &swap_chain_images = GetSwapChain().Images;
   Vulkan.FramebufferObjects.resize( swap_chain_images.size() );
   
   for( size_t i = 0; i < swap_chain_images.size(); ++i ) {
     VkImageViewCreateInfo image_view_create_info = {
       VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
       nullptr,                                    // const void                    *pNext
       0,                                          // VkImageViewCreateFlags         flags
       swap_chain_images[i],                       // VkImage                        image
       VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
       GetSwapChain().Format,                      // VkFormat                       format
       {                                           // VkComponentMapping             components
         VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             r
         VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             g
         VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             b
         VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle             a
       },
       {                                           // VkImageSubresourceRange        subresourceRange
         VK_IMAGE_ASPECT_COLOR_BIT,                  // VkImageAspectFlags             aspectMask
         0,                                          // uint32_t                       baseMipLevel
         1,                                          // uint32_t                       levelCount
         0,                                          // uint32_t                       baseArrayLayer
         1                                           // uint32_t                       layerCount
       }
     };
   
     if( vkCreateImageView( GetDevice(), &image_view_create_info, nullptr, &Vulkan.FramebufferObjects[i].ImageView ) != VK_SUCCESS ) {
       printf( "Could not create image view for framebuffer!\n" );
       return false;
     }
   
   ```

   We use **vkCreateImageView** to create image view. This requires the struct ***VkImageViewCreateInfo***  which contains:

   + sType, pNext, flags
   + image: Handle to an image for which the view will be created.
   + viewType: View type must compatible with an image it is created for.
   + format: It must be compatible with the image's format but may not be the same format.
   + components: Mapping of an image components into a vector returned in the shader by texturing operations. This applies only to read operations (sampling), but since we are using an image as a color attachment, we must set the so-called identity mapping or just use "identity" value ***VK_COMPONENT_SWIZZLE_IDENTITY***.
   + subresourceRange: Describes the set of mipmap levels and array layers that will be accessible to a view. If image is mipmapped, we may specific mipmap level we want to render to.

2. **Specifying Framebuffer Parameters**: to create Framebuffer, we need a struct ***VkFramebufferCreateInfo*** which contains:

   + sType, pNext, flag
   + rendePass: Render pass this parameter will be compatible with
   + attachmentCount: Numbe of attachments in a framebuffer
   + pAttachments: Array of image views representing all attachments used in a framebuffer and render  pass. Echa element corresponds to each attachment in a render pass
   + width: Width of a framebuffer
   + height: Height of a framebuffer
   + layers: Number of layers in a framebuffer (OpenGL's layered rendering with geometry shaders, which could specify the layer into which fragments rasterized from a given polygon will be render)

   The framebuffer specifies what images are used as attachments on which the render pass operates. The number of images specified for a framebuffer must be the same as the number of attachments in a render pass. We may use a framebuffer not only with the specified render pass but also with all render passes that are compatible with the one specified.

   We call the **vkCreateFramebuffe** to create framebuffer.

### Creating a Graphics Pipeline

A pipeline is a collection of stages that process data one stage after another. In Vulkan there is currently a compute pipeline and a graphics pipeline. The compute pipeline allows us to perform some computational work, such as performing physics calculations for objects in games. The graphics pipeline is used for drawing operations.

In OpenGL there are multiple programmable stages (vertex, tessellation, fragment shaders and so on) and some fixed function stages (rasterizer, depth test, blending). In Vulkan, this is similar. But the whole pipelines' stage is gathered in one monolithic object. OpenGL allows us to change the state that influence rendering operations anytime we want, we can change parameters for each stage independently. We can set up shader programs, depth test, blending, and whatever state we want, and then we can render some objects. Next we can change just some small part of the state and render another object. In Vulkan, such operation can't be done. We must prepare the whole state and set up parameters for pipeline stages and group them in a pipeline object.

Why this? Changing just one single state of the whole pipeline may cause graphic hardware to perform many background operations lie state and error checking. This may cause applications to perform differently when executed on different graphics hardware.

In Vulkan, the state of the whole pipeline is to gather in one, single object. All the relevant statue and error checking is performed when the pipeline object is created. If error occurs, the pipeline can't be created.

The downside of this methodology is that we have create multiple pipeline object, multiple variation of pipeline objects when we are drawing many objects in a different way. If we want to draw objects using different shaders, we also have to create multiple pipeline object, one for each combination of shader programs.

1. **Creating a Shader Module**

   The first data to create graphics pipeline is a collection of all shader stages and shader programs that will be used during rendering with a given graphics pipeline bound.

   In OpenGL, we write shaders in GLSL. They are compiled and then linked into shader programs directly in our application. We can use or stop using a shader program anytime we want in out application.

   Vulkan accepts only a binary representation of shaders, an intermediate language called SPIR-V.  So we compile GLSL code into assembly offline. Then we prepare the SPIR-V assembly we can create a shader module from it. Such modules are then composed into an array of ***VkPipelineShaderStageInfo*** structures, which are used to create pipeline. Code is:
   
   ```c++
   const std::vector<char> code = Tools::GetBinaryFileContents( filename );
   if( code.size() == 0 ) {
     return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
   }
   
   VkShaderModuleCreateInfo shader_module_create_info = {
     VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
     nullptr,                                        // const void                    *pNext
     0,                                              // VkShaderModuleCreateFlags      flags
     code.size(),                                    // size_t                         codeSize
     reinterpret_cast<const uint32_t*>(&code[0])     // const uint32_t                *pCode
   };
   
   VkShaderModule shader_module;
   if( vkCreateShaderModule( GetDevice(), &shader_module_create_info, nullptr, &shader_module ) != VK_SUCCESS ) {
     printf( "Could not create shader module from a %s file!\n", filename );
     return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
   }
   
   return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>( shader_module, vkDestroyShaderModule, GetDevice() );
   
   ```
   
   The ***VkShaderModuleCreateInfo*** contains:
   
   + sType, pNext, flags.
   + codeSize: size in bytes of the code passed in pCode paramter.
   + pCode: Pointer to an array with source code.
   
   After we prepare a structure, use ***vkCreateShaderModule*** to create a shader module. To destroy the shader we created, use
   
   ```c++
   template<class T, class F>
   class AutoDeleter {
   public:
     AutoDeleter() :
       Object( VK_NULL_HANDLE ),
       Deleter( nullptr ),
       Device( VK_NULL_HANDLE ) {
     }
   
     AutoDeleter( T object, F deleter, VkDevice device ) :
       Object( object ),
       Deleter( deleter ),
       Device( device ) {
     }
   
     AutoDeleter( AutoDeleter&& other ) {
       *this = std::move( other );
     }
   
     ~AutoDeleter() {
       if( (Object != VK_NULL_HANDLE) && (Deleter != nullptr) && (Device != VK_NULL_HANDLE) ) {
         Deleter( Device, Object, nullptr );
       }
     }
   
     AutoDeleter& operator=( AutoDeleter&& other ) {
       if( this != &other ) {
         Object = other.Object;
         Deleter = other.Deleter;
         Device = other.Device;
         other.Object = VK_NULL_HANDLE;
       }
       return *this;
     }
   
     T Get() {
       return Object;
     }
   
     bool operator !() const {
       return Object == VK_NULL_HANDLE;
     }
   
   private:
     AutoDeleter( const AutoDeleter& );
     AutoDeleter& operator=( const AutoDeleter& );
     T         Object;
     F         Deleter;
     VkDevice  Device;
   };
   
   ```

2. **Preparing a Description of the Shade stages**

   Shader stages composite our graphics pipeline. The data that for shader stages describes what shader stages should be active when a given graphics pipeline is bound has a form of an array with elements of type ***VkPipelineShaderStageCreateInfo***. The code is:

   ```c++
   Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> vertex_shader_module = CreateShaderModule( "Data03/vert.spv" );
   Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule> fragment_shader_module = CreateShaderModule( "Data03/frag.spv" );
   
   if( !vertex_shader_module || !fragment_shader_module ) {
     return false;
   }
   
   std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos = {
     // Vertex shader
     {
       VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
       nullptr,                                                    // const void                                    *pNext
       0,                                                          // VkPipelineShaderStageCreateFlags               flags
       VK_SHADER_STAGE_VERTEX_BIT,                                 // VkShaderStageFlagBits                          stage
       vertex_shader_module.Get(),                                 // VkShaderModule                                 module
       "main",                                                     // const char                                    *pName
       nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
     },
     // Fragment shader
     {
       VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
       nullptr,                                                    // const void                                    *pNext
       0,                                                          // VkPipelineShaderStageCreateFlags               flags
       VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlagBits                          stage
       fragment_shader_module.Get(),                               // VkShaderModule                                 module
       "main",                                                     // const char                                    *pName
       nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
     }
   };
   ```

   The tool that compiles shader to assembly is "glslangValidator". The content of shader.vert is

   ```
   #version 400
   
   void main() {
       vec2 pos[3] = vec2[3]( vec2(-0.7, 0.7), vec2(0.7, 0.7), vec2(0.0, -0.7) );
       gl_Position = vec4( pos[gl_VertexIndex], 0.0, 1.0 );
   }
   ```

   The vertices are indexed using the Vulkan-specific "gl_VertextIndex" built-in variable. The shader.frag is

   ```glsl
   #version 400
   
   layout(location = 0) out vec4 out_Color;
   
   void main() {
     out_Color = vec4( 0.0, 0.4, 1.0, 1.0 );
   }
   ```

   In Vulkan's shaders (when transforming from GLSL to SPIR-V) layout qualifiers are required. Here we specify to what output (color) attachment we want to store the color values generated by fragment shader. For each enabled shader stage we need to prepare an instance of ***VkPipelineShaderStageCreateInfo*** structure which contains:

   + sType, pNext, flags.
   + stage: Type of shader stage we are decribing (like vertex, tessellation control, and so on).
   + module: Handle to a shader module that contains the shader for a given stage.
   + pName: Name of the entry pointer of the provided shader.
   + pSpecicalizationInfo: Pointer to ***VkSpecialization*** structure, leave for null.
   
3.  **Preparing Description of a Vertex Input**:

   We need to provide a description of the input data used for drawing. This is similar to OpenGL's vertex data: attributes, number of components, buffer from which to take data, data stride, or step rate. Because of the fact that vertex data is hardcoded into a vertex shader in this tutorial, we can almost entirely skip this step and fill the ***VkPipelineVertexInputStateCreateInfo*** with almost nulls and zero: The structure contains:

   + sType, pNext, flags
   + vertexBindingDescriptionCount: Number of elements in the pVertexBindingDescription array
   + pVertextBindDescriptions: Array with elements describing input vertex data (stride and stepping rate)
   + vertexAttributeDescriptionCount: Number of elements in the pVertexAttributeDescription array
   + pVertexAttributeDescriptions: element describing vertex attributes (location, format, offset)

4. **Preparing the Description of an Input Assembly**:

   Use the ***VKPipelineInputAssemblyStateCreateInfo*** to describe how vertices should be assembled into primitives.

5. **Preparing the Viewport's Description**:

   Output data specification start from here. Output data includes all the part of the graphics pipeline that connected with fragments, like rasterization, window (viewport), depth test and so on. The first set of data we must prepare is the state of viewport, which specifies to what part of the image we want to draw. The structure is ***VkViewPort***. The upper-left corner is the start.

   The scissor test, similar to OpenGL, restricts generation of fragments only to the specified rectangular area. In Vulkan this can't be turned off. This is a ***VkRect2D*** which contains ***VkOffset2D*** and ***VkExtent2D***.

   We use viewport and the scissor test to fill the ***VkPipelineViewportStateCreateInfo***

6. **Preparing the Rasterization State's Description**:

   We must specify how polygons are going to be rasterized (changed into fragments), which means whether we want fragments to be generated for whole polygons or just their edges or whether we want to see the front or back side or maybe both sides of the polygon (face culling). We can also provide depth bias parameters or indicate whether we want to enable depth clamp. This is encapsulated in ***VkPipelineRasterizationStateCreateInfo***.



