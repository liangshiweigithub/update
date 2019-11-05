## Vertex Attributes

In real situations, every frame of animation is different, so we can't prerecord all the rendering commands. We should record and submit the command buffer as late as possible to minimize input lag and acquire as recent input data as possible. Record the command buffer just before it is submitted to the queue. We should not record the same command buffer until the graphics card finishes processing it after it was submitted. This moment is signaled through a fence. Waiting on a fence every frame is a waste of time. So we need more command buffers used interchangeably.

### Specifying Render Pass Dependencies

The information about how the image was used (that is, what types of operations occurred in connection with an image), and when it was used (which parts of a rendering pipeline were using an image). This information can be specified both in the image memory barrier and the render pass description.

When we create a render pass and provide a description for it, the same information is specified through subpass dependencies. This additional data is crucial for a driver to optimally prepare an implicit barrier.

```c++
std::vector<VkSubpassDependency> dependencies = {
  {
    VK_SUBPASS_EXTERNAL,                            // uint32_t                       srcSubpass
    0,                                              // uint32_t                       dstSubpass
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           srcStageMask
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           dstStageMask
    VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  srcAccessMask
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  dstAccessMask
    VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
  },
  {
    0,                                              // uint32_t                       srcSubpass
    VK_SUBPASS_EXTERNAL,                            // uint32_t                       dstSubpass
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           srcStageMask
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           dstStageMask
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  srcAccessMask
    VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  dstAccessMask
    VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
  }
};

VkRenderPassCreateInfo render_pass_create_info = {
  VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,        // VkStructureType                sType
  nullptr,                                          // const void                    *pNext
  0,                                                // VkRenderPassCreateFlags        flags
  1,                                                // uint32_t                       attachmentCount
  attachment_descriptions,                          // const VkAttachmentDescription *pAttachments
  1,                                                // uint32_t                       subpassCount
  subpass_descriptions,                             // const VkSubpassDescription    *pSubpasses
  static_cast<uint32_t>(dependencies.size()),       // uint32_t                       dependencyCount
  &dependencies[0]                                  // const VkSubpassDependency     *pDependencies
};

if( vkCreateRenderPass( GetDevice(), &render_pass_create_info, nullptr, &Vulkan.RenderPass ) != VK_SUCCESS ) {
  std::cout << "Could not create render pass!" << std::endl;
  return false;
}

```

When an attachment is used in one specific way in a given subpass, but in another way in another subpass, we can create a memory barrier or we can provide a subpass dependency that describes the intended usage of an attachment in these two pass. The latte is good.

We can specify dependencies between render passes (by providing the number of a given subpass) and operations outside of them (by providing ***VK_SUBPASS_EXTERNAL***). We specify this data in ***VkSubPassDepency*** which contains:

+ srcSubpass: Index of a first subpass or ***VK_SUBPASS_EXTERNAL*** if we want to indicate dependency between subpass and operations outside of a render pass.
+ desSubpass: Index of second subpass or ***VK_SUBPASS_EXTERNAL***.
+ srcStageMask: Pipeline stage during which a given attachment was used before.
+ dstStageMask: Pipeline stage during which a given attachment will be used later.
+ srcAccessMask: Types of memory operations that occurred in a source subpass or before a render pass
+ desAccessMask: Types of memory operations that occurred in a dest subpass or after a render pass.
+ dependencyFlags: Flag describing the type of dependency.

### Graphics Pipeline Creation

#### Writing Shaders

```glsl
#version 450

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;

void main() {
    gl_Position = i_Position;
    v_Color = i_Color;
}

```

In Vulkan, all attributes must have a location layout qualifier. When we specify a description of the vertex attributes in Vulkan API, the names of these attributes don't matter, only their indices/locations. In OpenGL we could ask for a location of an attribute with a given name. In Vulkan location layout qualifiers are the only way to go.

shader.frag

```
#version 450

layout(location = 0) in vec4 v_Color;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = v_Color;
}
```

#### Vertex Attributes Specification

We specify the vertex input state creation, for which we specify a variable of type ***VkPipelineVertexInputStateCreateInfo***. In this variable we provide pointers to structures, which define the type of vertex input data and number and layout of our attributes.

***VkVertexInputBindingDescription*** is used to specify the binding (general memory information) of vertex data. It contains:

+ binding: Index of a binding with which vertex data will be associated.
+ stride: The distance in bytes between two consecutive elements.
+ inputRate: Defines how data should be consumed, per vertex or per instance.

When creating a vertex buffer, we bind it to a chosen slot before rendering operations. The slot number is this binding and we describe how data in this slot is aligned in memory and how it should be consumed.

Next step is to define all vertex attributes. We must specify a location (index) for each attribute (the same as in a shader source code, in location layout qualifier), source of data (binding from which data will be read), format (data type and number of components) and offset at which data for this specific attribute can be found (offset from the beginning of a data for a given vertex, not from the beginning of all vertex data). This information is provided through the ***VkVertexInputAttributeDescription*** structure. It contains:

+ location: Index of an attribute, the same as defined by the location layout specifier in a shader source code.
+ binding: The number of the slot from which data should be read (source of data like VBO in OpenGL)
+ format: Data type and number of components per attribute.
+ offset: Beginning of data for a given attribute.

The last is prepare vertex input state description by filling a variable of type ***VkPipelineVertexInputStateCreateInfo*** which contains:

+ SType, pNext, flags
+ vertexBindingDescriptionCount
+ pVertexBindingDescriptions: Array describing all bindings defined for a given pipeline (buffers from which values of all attributes are read)
+ vertexAttrbuteCount
+ pVertexAttributeDescription: Array with elements specifying all vertex attributes.

#### Input Assembly State Specification

We are going to draw a quad. To do this, we must use triangle strip topology. We define it through ***VkPipelineInputAssemblyStateCreateInfo*** structure which cotains:

+ sType, pNext, flags
+ topology: Topology used for drawing vertices.
+ primitiveRestartEnable: Parameter defining whether we want to restart assembling a primitive by using a special value of vertex index.

#### Viewport State Specification:

Previously setting up step caused our image to be always the same size, no matter how big the window is. This time we use a dynamic state for the viewport and scissor test parameters. Don't specify pViewports and pScissors members in the ***VkPipelineViewPortStateCreateInfo***. The number of viewports and scissor test rectangle is same as before.

#### Dynamic State Specification:

When we create a pipeline, we can specify which parts of it are always static, defined through structures at a pipeline creation, and which are dynamic, specified by proper function calls during command buffer recording. This allows us to lower the number of pipeline objects that differ only with small details like width, blend constants, or stencil parameter. The code is:

```c++
std::vector<VkDynamicState> dynamic_states = {
  VK_DYNAMIC_STATE_VIEWPORT,
  VK_DYNAMIC_STATE_SCISSOR,
};

VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
  VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // VkStructureType                                sType
  nullptr,                                                      // const void                                    *pNext
  0,                                                            // VkPipelineDynamicStateCreateFlags              flags
  static_cast<uint32_t>(dynamic_states.size()),                 // uint32_t                                       dynamicStateCount
  &dynamic_states[0]                                            // const VkDynamicState                          *pDynamicStates
};

```

This is done by using structure of type ***VkPipelineDynamicStateCreateInfo***, which contains:

+ sTYpe, pNext, flags.
+ dynamicStateCount:
+ pDynamicStates: Array contains enums, specifying which parts of a pipeline should be marked as dynamic. Each element of it is of type ***VkDynamicState***.

#### Pipeline Object Creation:

Every pipeline state, which is specified as dynamic, must be set through a proper function call during command buffer recording.