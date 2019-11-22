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

##### Writing Shaders

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

##### Vertex Attributes Specification

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

##### Input Assembly State Specification

We are going to draw a quad. To do this, we must use triangle strip topology. We define it through ***VkPipelineInputAssemblyStateCreateInfo*** structure which cotains:

+ sType, pNext, flags
+ topology: Topology used for drawing vertices.
+ primitiveRestartEnable: Parameter defining whether we want to restart assembling a primitive by using a special value of vertex index.

##### Viewport State Specification:

Previously setting up step caused our image to be always the same size, no matter how big the window is. This time we use a dynamic state for the viewport and scissor test parameters. Don't specify pViewports and pScissors members in the ***VkPipelineViewPortStateCreateInfo***. The number of viewports and scissor test rectangle is same as before.

##### Dynamic State Specification:

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

##### Pipeline Object Creation:

Every pipeline state, which is specified as dynamic, must be set through a proper function call during command buffer recording.

### Vertex Buffer Creation

To use vertex attributes, apart from specifying them during pipeline creation, we need to prepare a buffer that will contain all the data for these attributes. From this buffer, the values from attributes will be read and provide to the vertex shader.

In Vulkan, buffer and image creation consists of at least two stages. First, we create the object itself. Next, we need to create a memory object, which will then be bound to the buffer (or image). From this memory object, the buffer will take its storage space.

To create a buffer object we call ***vkCreateBuffer***, It accepts a pointer to a variable of type ***VkBufferCreateInfo***, which defines parameters of created buffer. The code is:

```c++
VertexData vertex_data[] = {
  {
    -0.7f, -0.7f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f
  },
  {
    -0.7f, 0.7f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f
  },
  {
    0.7f, -0.7f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f
  },
  {
    0.7f, 0.7f, 0.0f, 1.0f,
    0.3f, 0.3f, 0.3f, 0.0f
  }
};

Vulkan.VertexBuffer.Size = sizeof(vertex_data);

VkBufferCreateInfo buffer_create_info = {
  VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType        sType
  nullptr,                                          // const void            *pNext
  0,                                                // VkBufferCreateFlags    flags
  Vulkan.VertexBuffer.Size,                         // VkDeviceSize           size
  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,                // VkBufferUsageFlags     usage
  VK_SHARING_MODE_EXCLUSIVE,                        // VkSharingMode          sharingMode
  0,                                                // uint32_t               queueFamilyIndexCount
  nullptr                                           // const uint32_t        *pQueueFamilyIndices
};

if( vkCreateBuffer( GetDevice(), &buffer_create_info, nullptr, &Vulkan.VertexBuffer.Handle ) != VK_SUCCESS ) {
  std::cout << "Could not create a vertex buffer!" << std::endl;
  return false;
}

```

The ***VkBufferCreateInfo*** constains:

+ sType, pNext
+ flags: Right now it allows creation of a buffer backed by a sparse memory.
+ size: Size , in bytes, of a buffer.
+ usage: Defines how we intend to use this buffer in future. We can specify that we want to use buffer as a uniform buffer, index buffer, source of data for transform (copy) operations and so on. 
+ sharingMode: Similar to swapchain images, defines whether a given buffer can be accessed by multiple queues at the same time (concurrent sharing mode) or by just a single queue (exclusive sharing mode). If a concurrent sharing mode is specified, we must provide indices of all queues that will have access to a buffer. For exclusive mode, we can still reference this buffer in different queues, but only in one at a time.
+ queueFamilyIndexCount: count of below
+ pQueueFamilyIndices: indices of all queues that will reference buffer (in concurrent sharing mode)

##### Buffer Memory Allocation:

```c++
VkMemoryRequirements buffer_memory_requirements;
vkGetBufferMemoryRequirements( GetDevice(), buffer, &buffer_memory_requirements );

VkPhysicalDeviceMemoryProperties memory_properties;
vkGetPhysicalDeviceMemoryProperties( GetPhysicalDevice(), &memory_properties );

for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i ) {
  if( (buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
    (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ) {

    VkMemoryAllocateInfo memory_allocate_info = {
      VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
      nullptr,                                    // const void                            *pNext
      buffer_memory_requirements.size,            // VkDeviceSize                           allocationSize
      i                                           // uint32_t                               memoryTypeIndex
    };

    if( vkAllocateMemory( GetDevice(), &memory_allocate_info, nullptr, memory ) == VK_SUCCESS ) {
      return true;
    }
  }
}
return false;

```

First we must check what the memory requirements for a created buffer are by the ***vkGetBufferMemoryRequirements***. The result is stored in ***VkMemoryRequirements*** and it contains information about required size, memory alignment, and supported memory types. Each device may have and expose different memory types--heaps of various sizes that have different properties. One memory type may be a device's local memory located on the GDDR chips (which is very fast). Another may be a shared memory that is visible both for a graphics card and a CPU (maybe slow).

To check what memory heaps and types are available, we need to call the ***vkGetPhysicalDeviceMemoryProperties***, which stores information in ***VkPhysicalDeviceMemoryProperties***. It contains:

+ memeoryHeapCount: Number of memory heaps exposed by a given device.
+ memoryHeaps: Each heap in it represents a memory of different size and properties.
+ memoryTypeCount: Number of different memory types exposed by a given device.
+ memoryTypes: Each element describes specific memory properties and contains an index of a heap that has these particular properties.

Buffer memory requirements have a field called memoryTypeBits and if a bit on a given index is set in this field, it means that for a given buffer for a given buffer we can allocate a memory of the type represented by that index. 

After finds we proper index, we prepare a variable of type ***VkMemoryAllocateInfo*** which contains:

+ sType, pNext
+ allocateSize: Minimum required memory size that should be allocated.
+ memoryTypeIndex: index of an memory type we want to use to create memory object.

Call the ***vkAllocateMemory*** to allocate the memory.

##### Binding a Buffer's Memory:

```
if( vkBindBufferMemory( GetDevice(), Vulkan.VertexBuffer.Handle, Vulkan.VertexBuffer.Memory, 0 ) != VK_SUCCESS ) {
  std::cout << "Could not bind memory for a vertex buffer!" << std::endl;
  return false;
}
```

After buffer memory allocation, we must bind it to our buffer by ***vkBindBufferMemory***. We provide a handle to buffer, handle to a memory object, and an offset. We can create larger memory objects and use it as a storage space for multiple buffers (or images) This is the recommended behavior. Creating larger memory objects means we are creating fewer memory objects. This allows driver to track fewer objects in general. Memory objects must be tracked by a driver because of OS requirements and security measures. Larger memory object don't cause big problems with memory fragmentation.

But when we allocate larger memory objects and bind them to multiple buffers, not all of them can be bounded at offset zero. This is what the offset for.

##### Uploading Vertex Data

After created a buffer and have a bound a memory that is host visible. We can map this memory, acquire a pointer to this memory, and use this pointer to copy data from our application to the buffer itself.

```
void *vertex_buffer_memory_pointer;
if( vkMapMemory( GetDevice(), Vulkan.VertexBuffer.Memory, 0, Vulkan.VertexBuffer.Size, 0, &vertex_buffer_memory_pointer ) != VK_SUCCESS ) {
  std::cout << "Could not map memory and upload data to a vertex buffer!" << std::endl;
  return false;
}

memcpy( vertex_buffer_memory_pointer, vertex_data, Vulkan.VertexBuffer.Size );

VkMappedMemoryRange flush_range = {
  VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,            // VkStructureType        sType
  nullptr,                                          // const void            *pNext
  Vulkan.VertexBuffer.Memory,                       // VkDeviceMemory         memory
  0,                                                // VkDeviceSize           offset
  VK_WHOLE_SIZE                                     // VkDeviceSize           size
};
vkFlushMappedMemoryRanges( GetDevice(), 1, &flush_range );

vkUnmapMemory( GetDevice(), Vulkan.VertexBuffer.Memory );

return true;
```

The ***vkMapMemory*** is used to map memory. In this call we must specify which memory object we want to map and a region to access. Region is defined by an offset from the beginning of a memory object's storage and size. We then acquire a pointer. We can use it to copy data from our application to the provided memory address.

After a memory copy operation and before unmap a memory (this is not needed), we need to tell the driver which parts of the memory was modified by our operations. This operation is called flushing. Through it we specify all memory ranges that our application copied data to. The ranges are defined by an array of ***VkMappedMemoryRange*** elements which contains:

+ sType, pNext
+ memory: Handle of a mapped and modified memory object.
+ offset: Offset at which a given range starts.
+ size: Size, in bytes, of an affected region. If the whole memory, from an offset to the end, was modified, we can use ***VK_WHOLE_SIZE***.

Call ***vkFlushMappedMemoryRanges*** to flash. After that, the driver will known which parts were modified and will reload them (refresh cache). Reloading usually occurs on barriers.

### Rendering Resources Creation

To record command buffers and submit them to queue in an efficient way, we need four types of resources: command buffers, semaphores, fences and framebuffer. Semaphores are used for internal queue synchronization. Fences allow the application to check if some specific situation occurred, if command buffer's execution after it was submitted to queue, has finished. If necessary, application can wait on a fence, until it is signaled. In general, semaphores are used to synchronize queues (GPU) and fences are used to synchronize application (CPU).

To render a single frame of animation we need at least one command buffer, two semaphores--one for a swapchain image acquisition (image available semaphore) and the other to signal that presentation may occur -- a fence, and a framebuffer. The fence is used to later to check whether we can record a given command buffer. We will keep several numbers of such rendering resources, which we call **virtual frame**. The number of these **virtual frames** should be independent of a number of swapchain image.

We are not allowed to record a command buffer that has been submitted to a queue until its execution in the queue is finished. During command buffer recording, we can use the "simultaneous use" flag, which allows us to record or resubmit a command buffer that has already been submitted. This may impact performance. A better way is to use fences and check whether a command buffer is not used any more. Tow or three virtual frames seems to be the most reasonable compromise between performance, memory usage, and input lag.

##### Command Pool Creation

Call the ***vkCreateCommandPool*** same as before. This time we provide two additional flag in ***VkCommandPoolCreatInfo*** which is:

+ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Indicates that command buffers, allocated from this pool, may be reset individually. Normally, without this flag, we can't record the same command buffer multiple times. It must be reset first. And, command buffers created from one pool may be reset only all at once. Specifying this flag allows us to reset command buffers individually, and it is done implicitly by calling the ***vkBeginCommandBuffer*** function.
+ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: This flag tells the driver that command buffers allocated from this pool will be living for a short amount of time, they will be often recorded and reset (re-recorded). This information helps optimize command buffer allocation and perform it more optimally.

##### Fence Creation:

The code is:

```c++
VkFenceCreateInfo fence_create_info = {
  VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,              // VkStructureType                sType
  nullptr,                                          // const void                    *pNext
  VK_FENCE_CREATE_SIGNALED_BIT                      // VkFenceCreateFlags             flags
};

for( size_t i = 0; i < Vulkan.RenderingResources.size(); ++i ) {
  if( vkCreateFence( GetDevice(), &fence_create_info, nullptr, &Vulkan.RenderingResources[i].Fence ) != VK_SUCCESS ) {
    std::cout << "Could not create a fence!" << std::endl;
    return false;
  }
}
return true;
```

To create a fence object we call the ***vkCreateFence***.  It accepts a variable of ***VkFenceCreateInfo*** which contains:

+ sType, pNext
+ flags: Allows for creating a fence that is already signaled.

A fence may have two states: signaled and unsignaled. The application checks whether a given fence is in a signaled state, or it may wait on a fence until the fence gets signaled. Signaling is done by the GUP after all operations submitted to the queue are processed. When we submit command buffers, we can provide a fence that will be signaled when a queue has finished executing all commands that were issued in this one submit operation. After the fence is signaled, it is the application's responsibility to reset it to an unsignaled state.

### Drawing

```c++
static size_t           resource_index = 0;
RenderingResourcesData ¤t_rendering_resource = Vulkan.RenderingResources[resource_index];
VkSwapchainKHR          swap_chain = GetSwapChain().Handle;
uint32_t                image_index;

resource_index = (resource_index + 1) % VulkanTutorial04Parameters::ResourcesCount;

if( vkWaitForFences( GetDevice(), 1, ¤t_rendering_resource.Fence, VK_FALSE, 1000000000 ) != VK_SUCCESS ) {
  std::cout << "Waiting for fence takes too long!" << std::endl;
  return false;
}
vkResetFences( GetDevice(), 1, ¤t_rendering_resource.Fence );

VkResult result = vkAcquireNextImageKHR( GetDevice(), swap_chain, UINT64_MAX, current_rendering_resource.ImageAvailableSemaphore, VK_NULL_HANDLE, &image_index );
switch( result ) {
  case VK_SUCCESS:
  case VK_SUBOPTIMAL_KHR:
    break;
  case VK_ERROR_OUT_OF_DATE_KHR:
    return OnWindowSizeChanged();
  default:
    std::cout << "Problem occurred during swap chain image acquisition!" << std::endl;
    return false;
}

if( !PrepareFrame( current_rendering_resource.CommandBuffer, GetSwapChain().Images[image_index], current_rendering_resource.Framebuffer ) ) {
  return false;
}

VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
VkSubmitInfo submit_info = {
  VK_STRUCTURE_TYPE_SUBMIT_INFO,                          // VkStructureType              sType
  nullptr,                                                // const void                  *pNext
  1,                                                      // uint32_t                     waitSemaphoreCount
  ¤t_rendering_resource.ImageAvailableSemaphore,    // const VkSemaphore           *pWaitSemaphores
  &wait_dst_stage_mask,                                   // const VkPipelineStageFlags  *pWaitDstStageMask;
  1,                                                      // uint32_t                     commandBufferCount
  ¤t_rendering_resource.CommandBuffer,              // const VkCommandBuffer       *pCommandBuffers
  1,                                                      // uint32_t                     signalSemaphoreCount
  ¤t_rendering_resource.FinishedRenderingSemaphore  // const VkSemaphore           *pSignalSemaphores
};

if( vkQueueSubmit( GetGraphicsQueue().Handle, 1, &submit_info, current_rendering_resource.Fence ) != VK_SUCCESS ) {
  return false;
}

VkPresentInfoKHR present_info = {
  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                     // VkStructureType              sType
  nullptr,                                                // const void                  *pNext
  1,                                                      // uint32_t                     waitSemaphoreCount
  ¤t_rendering_resource.FinishedRenderingSemaphore, // const VkSemaphore           *pWaitSemaphores
  1,                                                      // uint32_t                     swapchainCount
  &swap_chain,                                            // const VkSwapchainKHR        *pSwapchains
  &image_index,                                           // const uint32_t              *pImageIndices
  nullptr                                                 // VkResult                    *pResults
};
result = vkQueuePresentKHR( GetPresentQueue().Handle, &present_info );

switch( result ) {
  case VK_SUCCESS:
    break;
  case VK_ERROR_OUT_OF_DATE_KHR:
  case VK_SUBOPTIMAL_KHR:
    return OnWindowSizeChanged();
  default:
    std::cout << "Problem occurred during image presentation!" << std::endl;
    return false;
}
return true;
```

First we take the lease recently used rendering resource. Then we wait until the fence associated with this group is signaled. If it is, this means that we can safely take a command buffer and record it.

When a fence is finished, we reset the fence and perform normal drawing-related operations: we acquire an image, record operations rendering into an acquired image, submit the command buffer, and present an image.

##### Recording a Command Buffer

It is better and more convenient to create framebuffers on demand. Framebuffers operate on image views, which are created for a given, specific image. When a swapchain is recreated, old images are invalid, not existent. So we must recreate image views and also framebuffers.

Before recording a command buffer, we create a framebuffer for an image to which we will be rendering, and of the same size as that image. When we record command buffer that uses a render pass and framebuffer objects, the framebuffer must remain valid for the whole time the command buffer is processed by the queue.

First we define a variable of type ***VkCommandBufferBeginInfo*** and specify that a command buffer will be submitted only once. When we specify a ***VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT***, we can't submit a given command buffer more times.

Next define subresource range for image memory barriers. Then begins a render pass with the temporary framebuffer object. The bind graphics has two dynamic state. The viewport is set by ***vkCmdSetViewPort***, the  scissor is set by ***vkCmdSetScissor***. The last thing before we can draw anything is to bind appropriate vertex buffer, providing data for vertex attributes. We do this by the ***vkCmdBindVertexBuffers*** function call.