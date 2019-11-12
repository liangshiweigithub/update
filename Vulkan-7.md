### Uniform Buffers

In Vulkan, uniform variables other than opaque types like samplers can't be declared in a global scope (as in OpenGL); they must be accessed from within uniform buffers. Buffers can be used for many different purposes -- they can be a source of vertex data; we can keep vertex indices in them so they are used as index buffers; they can contain shader uniform data, or we can store data inside buffers from within shaders and use them as storage buffers.

To create a buffer that can be used as a source of shader uniform data, we need to create a buffer with the ***VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT*** usage.

##### Copying Data to Buffers

The next is to upload appropriate data to our uniform buffer. In it we will store 16 elements of a 4x4 matrix. Each uniform variable must be placed at an appropriate offset, counting from the beginning of a buffer's memory. To transfer data to uniform buffer, we use a staging buffer -- it is created with ***VK_BUFFER_USAGE__TRANSFER_SRC_BIT*** usage and is backed by a memory supporting ***VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT*** property. Frequent mapping and unmapping may impact performance of our application. In Vulkan, resources can be mapped all the time and it doesn't impact our application in any way. So, if we want to frequently transfer data from using staging resources, we should map them only once and keep the acquired pointer for future use.

#### Preparing Descriptor Set Layout

Descriptor sets is the interface between our application and a pipeline through which we can provide resources used by shaders. Code is:

```c++
  std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
  {
    0,                                         // uint32_t           binding
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // VkDescriptorType   descriptorType
    1,                                         // uint32_t           descriptorCount
    VK_SHADER_STAGE_FRAGMENT_BIT,              // VkShaderStageFlags stageFlags
    nullptr                                    // const VkSampler *pImmutableSamplers
  },                                                                  
  {                                                                   
    1,                                         // uint32_t           binding
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType   descriptorType
    1,                                         // uint32_t           descriptorCount
    VK_SHADER_STAGE_VERTEX_BIT,                // VkShaderStageFlags stageFlags
    nullptr                                    // const VkSampler *pImmutableSamplers
  }
};

VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, // VkStructureType  sType
  nullptr,                                             // const void      *pNext
  0,                                                   // VkDescriptorSetLayoutCreateFlags flags
  static_cast<uint32_t>(layout_bindings.size()),       // uint32_t         bindingCount
  layout_bindings.data()                               // const VkDescriptorSetLayoutBinding *pBindings
};

if( vkCreateDescriptorSetLayout( GetDevice(), &descriptor_set_layout_create_info, nullptr, &Vulkan.DescriptorSet.Layout ) != VK_SUCCESS ) {
  std::cout << "Could not create descriptor set layout!" << std::endl;
  return false;
}
return true;
```

When we define uniform variables, we need to specify the same binding values as one provided during layout creation:

layout( set=S, binding=B ) uniform <variable type> <variable names>.

For example:

layout(set=0, binding=1) uniform u_UniformBuffer {
    mat4 u_ProjectionMatrix;
};

layout(set=0, binding=0) uniform sampler2D u_Texture;