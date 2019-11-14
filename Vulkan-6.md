### Descriptor Sets

In Vulkan, there are only two resource types in which we can store data: buffers and images. Each of them can be provided to shaders, in which case we call such resources descriptors. We can't provide them to shaders directly. They are aggregated in wrapper or container objects called descriptor sets. We can place multiple resources in a single descriptor set but we need to do it according to a predefined structure of such set. This structure defines the contents of a single descriptor set--types of resources that are placed inside it, number of each of these resource types, and their order. This structure is named **descriptor set layout**. Similar descriptions need to be specified when we write shader programs. Together they form an interface between API and the programmable pipeline (shaders).

When we have prepared a layout, and created a descriptor set, we can fill it. In this way we define specific objects that we we want to use in shaders. After that, before issuing drawing commands inside a command buffer, we need to bind such a set to the command buffer. This allows us to use the resources from inside the shader source source code.

#### Creating an Image

Images represent a continuous area of memory, which is interpreted according to the rules defined during image creation. In Vulkan, there have 1D, 2D and 3D three type of image. Image may have mipmaps (level of detail), many array layers (at least one is required), or samples per frame. All these parameters are specified during image creation. To create an image, ***VkImageCreateInfo*** is required, which contains:

+ sType, pNext
+ flags: describes additional properties of an image. Through this parameter we can specify that the image can be backed by a sparse memory. But a more interesting value is ***VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT***, which allows us to use the image as a cubemap.
+ imageType: Basic types: 1D, 2D or 3D
+ format: Format of the image: number of its components, number of bits for each component, and a data type.
+ extent: Size of the image (number of texels/pixels) in each dimension.
+ mipLevels: Number of level of detail (mipmaps).
+ arrayLayers: Number of array layers.
+ samples: Number of per texel samples.
+ tiling: Defines the inner memory structure of the image: linear or optimal.
+ usage: Defines all the ways in which we want to use an image during its overall lifetime.
+ shadringMode: Specifies whether an image will be accessed by queues from multiple family 
+ queueFamilyIndexCount and pQueueFamilyIndices.
+ initialLayout: Memory layout image will be created with. We can only provide an undefined or preinitialized layout.

Layout defines an image's memory layout and is strictly connected with the way in which we want to use an image. Each specific usage has its own memory layout. Before we can use an image in a given way we need to perform a layout transition. We use the ***vkCreateImage*** to create the image we want.

##### Allocating Image Memory and 

Similar to buffers, image don't have their own memory, so we need to bind memory to them. After allocating the memory, we call ***vkBindImageMemory*** to bind the memory.

##### Creating Image View:

When we want to use an image in application we rarely provide the image's handle. Image views are used instead. They provide an additional layer that interprets the contents of an image for the purpose of using it in a specific context.

##### Copying Data to an Image

Use a staging buffer to copy data to our image. First we use the **vkmapMemory** function to copy data to the staging buffer. Then we create a command and submit it to a queue. We start the command buffer recording operation and then change the layout of the image.

Next, we can copy the data itself. We use a ***VkBufferImageCopy*** structure to describe the data to copy and call the ***vkCmdCopyBufferToImage*** to copy the image. The last thing is to perform layout transition to transform the layout to ***VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPITMAL***.

#### Creating a Sampler

Inside a shader, we usually created variables of type sampler2D, which also combined both images and their sampling parameters (samplers). In Vulkan, we need to create images and samplers separately.

Samplers defines the way in which image data is read inside shaders: when filtering is enabled, whether we want to use mipmaps (or maybe a specific subrange of mipmaps), or what kind of addressing mode we want to use(clamping or wrapping). ***VkSamplerCreateInfo*** has the following number:

+ sType, pNext, flag
+ magFilter: Type of filtering (nearest and linear) used for magnification.
+ minFilter: Type of filtering (nearest and linear) used for minification.
+ mipmapMode: Type of filtering (nearest and linear) used for mipmap lookup.
+ addressmodeU: Addressing mode for U
+ addressModeV:
+ addressModeW:
+ mipLodBias: Value of bias added to mipmap's level of detail calculations. If we want to offset fetching data from a specific mipmap, we can provide a value other than 0.0
+ anisotropyEnable: Parameter defining whether anisotropic filtering should be used.
+ maxAnisotropy: Maximal allowed value used for anisotropic filtering.
+ compareEnable: Enables comparison against a reference value during texture lookups.
+ compareOp: Type of comparison performed during lookups if the compareEnable parameter  is set to true.
+ minLod: Minimal allowed level of detail used during data fetching. If the calculated level of detail (mipmap level) is greater than this value, it will be clamped.
+ maxLod: Maximal allowed level of detail used during data fetching.
+ borderColor: Specifies predefined color of border pixels. Border color is used when address mode includes clamping to border colors. 
+ unnormalizedCoordinates: Usually we provide texture coordinates using a normalized range. When set to true, this parameter allows us to specify that we want to use unnormalized coordinates and address texture using texels range.

Sampler object is created by calling the ***vkCreateSampler*** function.

#### Using Descriptor Sets

Resources used inside shaders are called descriptions. In Vulkan we have 11 types of descriptors.

**Samplers** - Define the way image data is read. Inside shaders, samplers can be used with multiple images.

**Sampled images** - Defines images from which we can read data inside shaders. We can read data from a single image using different samplers.

**Combined image samplers** - These descriptors combine both sampler and sampled image as one object. From the API perspective, we still need to create both a sampler and an image, but inside shader they appear as a single object.

**Storage images** - This descriptor allows us to both read and store data inside an image.

**Input attachments** - This is a specific usage of render pass's attachments.  When we want to read data from an image which is used as an attachment inside the same render pass, we can only do it through an input attachment. This way we do not need to end a render pass and start another one, but we are restricted to only fragment shaders, and to only a single location per fragment shader instance.

**Uniform buffers** - Uniform buffers allows us to read data from uniform variables. In Vulkan, such variable can't be placed inside the global scope, we need to use uniform buffers.

**Storage buffers** - Storage buffers allows us to both read and store data inside variable.

**Uniform texel buffers** - These allow the contents of buffers to be treated as if they contain texture data, they are interpreted as texels with a selected number of components and format. In this way, we can access very large arrays of data.

**Storage texel buffers** - Similar to uniform texel buffers. Not only can they be used for reading, but they can alos be used for storing data.

All of the above descriptors are creted from sampler, images, or buffers.

##### Creating a Descriptor Set Layout

Preparing resources to be used by shaders should begin with creating a descriptor set layout. Descriptor sets are opaque object in which we store handles of resources. Layouts define the structure of descriptor sets - what type of descriptors they contain, how many descriptors of each type there are and what their order is. To create a descriptor set layout, first fill the ***VkDescriptorSetLayoutBinding*** which contains:

+ binding - Index of a descriptor within a given set. All descriptor from a single layout must have a unique binding. This same binding is also used inside shaders to access a descriptor.
+ descriptorType - The type of descriptor. (sampler, uniform buffer and so on)
+ descriptorCount - Number of descriptors of a selected type accessed as an array.
+ stageFlags: Set of flags defining all shader stages that will have access to a given descriptor.
+ plmmutableSamplers - Affects only samplers that should be permanently bound into the layout.

We then provide the ***VkDescriptorSetLayoutBinding*** to ***VkDescriptorSetLayoutCreateInfo***, call the ***vkCreateDescriptorSetLayout*** to create a descriptor set layout.

##### Creating a Descriptor Pool

Descriptor sets are allocated from pools. Before we can allocate a descriptor set, we need to create a descriptor pool. Creating a descriptor pool involves specifying how many descriptor sets can be allocated from it. At the same time, we also need to specify what types of descriptors, and how many of them can be allocated from the pool across all sets. During descriptor pool creation we define the total number of descriptors and total number of sets that can be allocated from it. The code is:

```
VkDescriptorPoolSize pool_size = {
  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // VkDescriptorType               type
  1                                               // uint32_t                       descriptorCount
};

VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
  VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,  // VkStructureType                sType
  nullptr,                                        // const void                    *pNext
  0,                                              // VkDescriptorPoolCreateFlags    flags
  1,                                              // uint32_t                       maxSets
  1,                                              // uint32_t                       poolSizeCount
  &pool_size                                      // const VkDescriptorPoolSize    *pPoolSizes
};

if( vkCreateDescriptorPool( GetDevice(), &descriptor_pool_create_info, nullptr, &Vulkan.DescriptorSet.Pool ) != VK_SUCCESS ) {
  std::cout << "Could not create descriptor pool!" << std::endl;
  return false;
}
```

##### Allocating Descriptor Sets

Prepare a ***VkDescriptorSetAllocateInfo*** which contains:

+ sType, pNext
+ descriptorPool: Handle of descriptor tool to allocate memory
+ descriptorSetCount and pSetLayouts: Each element for this array must contain a descriptor set layout that defines the inner structure of the allocated descriptor set.

Use the ***vkAllocateDescriptorSets*** to allocate descriptor set.

##### Updating Descriptor Sets

This step means that we tell the driver which resources should be used for descriptors inside the set. We can update a descriptor set in two ways:

+ By writing to the descriptor set - this way we provide new resources.
+ By copying data from another descriptor set. If we have a previously updated descriptor set and if we also want to use some of its descriptors in another descriptor we can copy them.

For each descriptor type we need to prepare two structures.  One is ***VkWriteDescriptorSet***. Depending on the type of descriptor we want to update, we need to prepare a variable of type ***VkDescriptorImageInfo***, ***VkDescriptorBufferInfo*** or ***VkBufferView***.

##### Creating a Pipeline Layout

We have prepared specific resources that are almost ready to be used inside shaders, but descriptor sets are used to store handles of specific resources. These handles are provided during command buffer recording. We need to prepare information for the other side of the barricade: the driver also needs to know what types of resources the given pipeline needs to accessed to. This information is crucial when we create a pipeline as it may impact its internal structure or maybe even a shader compilation. And this information is provided in a so-called pipeline layout.

The pipeline layout stores information about resource types that the given pipeline has access to. These resources involve descriptors and push constants ranges. To create a pipeline layout and prepare information about the types of resources accessed by the pipeline, we need to provide an array of descriptor set layouts. This is done through ***VkPipelineLayoutCreateInfo***.

+ sType, flags
+ setLayoutCount:
+ pSetLayouts: Array with descriptor set layouts.
+ pushConstantRangeCount:
+ pPushConstantRanges: Array with elements describing push constant ranges.

Use the ***vkCreatePipelineLayout*** to create pipeline layout.

##### Binding Descriptor Sets

We can have multiple different descriptor sets or multiple, similar descriptor sets (with the same layouts), but they may contain different resource handles. Which of these descriptors are used during rendering is defined during command buffer recording. Before we can draw anything, we need to set up a valid state.

If a pipeline uses descriptor resources (when shaders access images or buffers), we need to bind descriptor sets by calling the ***vkCmdBindSescriptorSets*** function. For this function we provide a handle of the pipeline layout and array of descriptor set handles. We bind descriptor sets to specific indices.

##### Accessing Descriptors in Shaders

The address we use inside shaders like this:

layout( set=S, binding=B) uniform <variable type> <variable name>

Set defines an index that the given descriptor set was bound to through the ***vkCmdBindDescriptorSet***. ***Binding*** specifies the index of a resource within the provided set and corresponds to the binding defined during descriptor set layout creation and corresponds to the binding defined during descriptor set layout creation.