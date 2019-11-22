## Part 2: Swap chain

1. To draw something, we must integrate the Vulkan pipeline with the application and API that the OS provides. In OpenGL we acquire Device Context that is associated with the application's window. We create a 32-bit color surface with a 24-bit depth buffer and a support for double buffering. In Vulkan there is no default frame buffer. If we want to display something we can create a set of buffers to which we can render. These buffer along with their properties, similar to Direct3D*, are called a swap chain. To display we give buffers back to the presentation engine.  In OpenGL we first have to define the surface format and associate it with a window, then create Rendering Context. In Vulkan, we first create an instance, a device, and then create a swap chain.

2. Swap chain is treated as extension, the reason is:

   1. Vulkan can be used for many different purpose, including performing mathematical operations, not just display an image(which requires swap chain).
   2. Every OS displays image in a different way. The surface on which you can render may be implemented differently, can have a different format, and can be differently represented in the OS, there is no one universal way to do it.

3. Swap chain provides render targets that integrates with OS specific code. The swap chain creation and usage is an extension, we have to ask for the extension during both instance and device creation. If a given instance and device doesn't support these extensions, the instance and/or device creation will fail.

4. In the case of swap-chain support, three extensions are involved: two from an instance level and one from device level.

   1. The ***VK_KHR_surface*** extension is defined at the instance level. It describes a "surface" object, which is a logical representation of an application window. This extension allows us to check different parameters(capabilities, supported formats, size) of a surface and query whether the given physical device support a swap chain. This extension also defines methods to destroy any such surface.
   2. ***VK_KHR_win32_surface*** on windows and ***VK_KHR_xlib_surface*** or ***VK_KHR_xcb_surface***. This allows us to create a surface that represents the application's window in a given OS.

5. **Check whether an instance extension is supported**: ***vkEnumerateInstanceExtensionProperties*** This function enumerates all available instance general extensions, if the first parameter is null, or instance layer extensions if we set the first parameter to the name of given layer. Code is

   ```c++
   uint32_t extensions_count = 0;
   if( (vkEnumerateInstanceExtensionProperties( nullptr, &extensions_count, nullptr ) != VK_SUCCESS) ||
       (extensions_count == 0) ) {
     std::cout << "Error occurred during instance extensions enumeration!" << std::endl;
     return false;
   }
   
   std::vector<VkExtensionProperties> available_extensions( extensions_count );
   if( vkEnumerateInstanceExtensionProperties( nullptr, &extensions_count, &available_extensions[0] ) != VK_SUCCESS ) {
     std::cout << "Error occurred during instance extensions enumeration!" << std::endl;
     return false;
   }
   
   std::vector<const char*> extensions = {
     VK_KHR_SURFACE_EXTENSION_NAME,
   #if defined(VK_USE_PLATFORM_WIN32_KHR)
     VK_KHR_WIN32_SURFACE_EXTENSION_NAME
   #elif defined(VK_USE_PLATFORM_XCB_KHR)
     VK_KHR_XCB_SURFACE_EXTENSION_NAME
   #elif defined(VK_USE_PLATFORM_XLIB_KHR)
     VK_KHR_XLIB_SURFACE_EXTENSION_NAME
   #endif
   };
   
   for( size_t i = 0; i < extensions.size(); ++i ) {
     if( !CheckExtensionAvailability( extensions[i], available_extensions ) ) {
       std::cout << "Could not find instance extension named \"" << extensions[i] << "\"!" << std::endl;
       return false;
     }
   }
   ```

   if we prepare a place for a smaller amount of extensions, the function will return **VK_INCOMPLETE**. Each element in available_extensions contains the name of extension and its version.

6. Enabling an instance-level Extension we need to prepare an array with the names of all extensions we want to enable and pass it to ***VkInstanceCreateInfo***. When create instance success, we can load instance-level functions including additional, extension-specific functions.

7. **Create a Presentation Surface:**  Call ***vkCreate???SurfaceKHR*** function which accepts Vulkan instance, a pointer to a OS-specific structure, a point to optional memory allocation handling functions and a pointer to a variable to store created surface. The structure is ***Vk???SurfaceCreateInfoKHR*** which contains sTye, pNext, flags, hInstance/connection/dpy and hwnd/window(handle to application window)

8. **Check whether a device extension is supported**. 

   The extension is called ***VK_KHR_swapchain***, and it defines the actual support, implementation and usage of a swap chain. The check function is ***vkEnumerateDeviceExtensionProperties***. It behaves identically to the function querying instance extension. The only difference is that it takes an additional physical device handle in the first parameter. The code is:

   ```c++
   uint32_t extensions_count = 0;
   if( (vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extensions_count, nullptr ) != VK_SUCCESS) ||
       (extensions_count == 0) ) {
     std::cout << "Error occurred during physical device " << physical_device << " extensions enumeration!" << std::endl;
     return false;
   }
   
   std::vector<VkExtensionProperties> available_extensions( extensions_count );
   if( vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extensions_count, &available_extensions[0] ) != VK_SUCCESS ) {
     std::cout << "Error occurred during physical device " << physical_device << " extensions enumeration!" << std::endl;
     return false;
   }
   
   std::vector<const char*> device_extensions = {
     VK_KHR_SWAPCHAIN_EXTENSION_NAME
   };
   
   for( size_t i = 0; i < device_extensions.size(); ++i ) {
     if( !CheckExtensionAvailability( device_extensions[i], available_extensions ) ) {
       std::cout << "Physical device " << physical_device << " doesn't support extension named \"" << device_extensions[i] << "\"!" << std::endl;
       return false;
     }
   }
   ```

   We get supported extension names and look for the device swap-chain extension. If there is none there is no point in further checking the device's properties.

9. **Check whether presentation to a given surface is supported**:

   The ***vkGetPhysicalDeviceSurfaceSupportKHR*** function  is used to check whether a given queue family from a given physical device support a swap chain or, to be more precise, whether it supports presenting images to a given surface. The code is:

   ```c++
   uint32_t graphics_queue_family_index = UINT32_MAX;
   uint32_t present_queue_family_index = UINT32_MAX;
   
   for( uint32_t i = 0; i < queue_families_count; ++i ) {
     vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, i, Vulkan.PresentationSurface, &queue_present_support[i] );
   
     if( (queue_family_properties[i].queueCount > 0) &&
         (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ) {
       // Select first queue that supports graphics
       if( graphics_queue_family_index == UINT32_MAX ) {
         graphics_queue_family_index = i;
       }
   
       // If there is queue that supports both graphics and present - prefer it
       if( queue_present_support[i] ) {
         selected_graphics_queue_family_index = i;
         selected_present_queue_family_index = i;
         return true;
       }
     }
   }
   
   // We don't have queue that supports both graphics and present so we have to use separate queues
   for( uint32_t i = 0; i < queue_families_count; ++i ) {
     if( queue_present_support[i] ) {
       present_queue_family_index = i;
       break;
     }
   }
   
   // If this device doesn't support queues with graphics and present capabilities don't use it
   if( (graphics_queue_family_index == UINT32_MAX) ||
       (present_queue_family_index == UINT32_MAX) ) {
     std::cout << "Could not find queue family with required properties on physical device " << physical_device << "!" << std::endl;
     return false;
   }
   
   selected_graphics_queue_family_index = graphics_queue_family_index;
   selected_present_queue_family_index = present_queue_family_index;
   return true;
   ```

   The ***vkGetPhysicalDeviceSurfaceSupportKHR*** requires to provide a physical device handle, the queue family index we want to check, and the surface handle we want to render into. if support is available, ***VK_TRUE*** will be stored at a given address, otherwise ***VK_FALSE*** is stored.

10. **Create a Device with a Swap Chain Extension Enabled**:

    Fill a variable of ***VkDeviceCreateInfo*** type with ***VkDeviceQueueCreateInfo***. Then ask for the third extension related to a swap chain-- a device-level ***VK_KHR_swapchain*** extension. After create device, we can load device level functions and get device queues. Code to create Device:                      

    ```c++
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<float> queue_priorities = { 1.0f };
    
    queue_create_infos.push_back( {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType              sType
      nullptr,                                          // const void                  *pNext
      0,                                                // VkDeviceQueueCreateFlags     flags
      selected_graphics_queue_family_index,             // uint32_t                     queueFamilyIndex
      static_cast<uint32_t>(queue_priorities.size()),   // uint32_t                                     queueCount
      &queue_priorities[0]                              // const float                 *pQueuePriorities
    } );
    
    if( selected_graphics_queue_family_index != selected_present_queue_family_index ) {
      queue_create_infos.push_back( {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // VkStructureType              sType
        nullptr,                                        // const void                  *pNext
        0,                                              // VkDeviceQueueCreateFlags     flags
        selected_present_queue_family_index,            // uint32_t                     queueFamilyIndex
        static_cast<uint32_t>(queue_priorities.size()), // uint32_t                     queueCount
        &queue_priorities[0]                            // const float                 *pQueuePriorities
      } );
    }
    
    std::vector<const char*> extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    VkDeviceCreateInfo device_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,             // VkStructureType                    sType
      nullptr,                                          // const void                        *pNext
      0,                                                // VkDeviceCreateFlags                flags
      static_cast<uint32_t>(queue_create_infos.size()), // uint32_t                           queueCreateInfoCount
      &queue_create_infos[0],                           // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
      0,                                                // uint32_t                           enabledLayerCount
      nullptr,                                          // const char * const                *ppEnabledLayerNames
      static_cast<uint32_t>(extensions.size()),         // uint32_t                           enabledExtensionCount
      &extensions[0],                                   // const char * const                *ppEnabledExtensionNames
      nullptr                                           // const VkPhysicalDeviceFeatures    *pEnabledFeatures
    };
    
    if( vkCreateDevice( Vulkan.PhysicalDevice, &device_create_info, nullptr, &Vulkan.Device ) != VK_SUCCESS ) {
      std::cout << "Could not create Vulkan device!" << std::endl;
      return false;
    }
    
    Vulkan.GraphicsQueueFamilyIndex = selected_graphics_queue_family_index;
    Vulkan.PresentQueueFamilyIndex = selected_present_queue_family_index;
    return true;
    ```

11. **Creating a Semaphore** : Last step before we can move to swap chain creation and usage. Semaphore are objects that used for queue synchronization. They may be signaled or unsignaled. One queue may signal a semaphore(change its state from unsignaled to signaled) when some operations are finished, and another queue may wait on semaphore until it becomes signaled. Call ***vkCreateSemophore*** to create it:

    ```c++
    VkSemaphoreCreateInfo semaphore_create_info = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
      nullptr,                                      // const void*              pNext
      0                                             // VkSemaphoreCreateFlags   flags
    };
    
    if( (vkCreateSemaphore( Vulkan.Device, &semaphore_create_info, nullptr, &Vulkan.ImageAvailableSemaphore ) != VK_SUCCESS) ||
        (vkCreateSemaphore( Vulkan.Device, &semaphore_create_info, nullptr, &Vulkan.RenderingFinishedSemaphore ) != VK_SUCCESS) ) {
      std::cout << "Could not create semaphores!" << std::endl;
      return false;
    }
    
    return true;
    ```

12. **Creating a swap chain**

    To create a swap chain, we call the ***vkCreateSwapchainKHR*** function.  It requires us to provide an address of a variable of type ***VkSwapchainCreateInfoKHR***, which informs the driver about the properties of a swap chain that is being created.  This info is get by check device properties.

    1. **Acquiring Surface Capabilities** by ***vkGetPhysicalDeviceSurfaceCapabilitiesKHR***

       ```c++
       VkSurfaceCapabilitiesKHR surface_capabilities;
       if( vkGetPhysicalDeviceSurfaceCapabilitiesKHR( Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &surface_capabilities ) != VK_SUCCESS ) {
         std::cout << "Could not check presentation surface capabilities!" << std::endl;
         return false;
       }
       ```

       Acquired capabilities contains import information about ranges(limits) that are supported by the swap chain, that is minimal and maximal number of images, minimal and maximal dimensions of images, or supported transform.

    2. **Acquiring Supported Surface Formats** by ***vkGetPhysicalDeviceSurfaceFormatsKHR***

       Not all platforms are compatible with typical image formats like non-linear 32-bit RGBA.

       ```c++
       uint32_t formats_count;
       if( (vkGetPhysicalDeviceSurfaceFormatsKHR( Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &formats_count, nullptr ) != VK_SUCCESS) ||
           (formats_count == 0) ) {
         std::cout << "Error occurred during presentation surface formats enumeration!" << std::endl;
         return false;
       }
       
       std::vector<VkSurfaceFormatKHR> surface_formats( formats_count );
       if( vkGetPhysicalDeviceSurfaceFormatsKHR( Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &formats_count, &surface_formats[0] ) != VK_SUCCESS ) {
         std::cout << "Error occurred during presentation surface formats enumeration!" << std::endl;
         return false;
       }
       ```

    3. **Acquiring Supported Present Modes**: The present mode defines whether an application will wait for v-sync or whether it will display an image immediately when it is available (which will probably lead to image tearing). The function is ***vkGetPhysicalDeviceSurfacePresentModesKHR***.

       ```c++
       uint32_t present_modes_count;
       if( (vkGetPhysicalDeviceSurfacePresentModesKHR( Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &present_modes_count, nullptr ) != VK_SUCCESS) ||
           (present_modes_count == 0) ) {
         std::cout << "Error occurred during presentation surface present modes enumeration!" << std::endl;
         return false;
       }
       
       std::vector<VkPresentModeKHR> present_modes( present_modes_count );
       if( vkGetPhysicalDeviceSurfacePresentModesKHR( Vulkan.PhysicalDevice, Vulkan.PresentationSurface, &present_modes_count, &present_modes[0] ) != VK_SUCCESS ) {
         std::cout << "Error occurred during presentation surface present modes enumeration!" << std::endl;
         return false;
       }
       ```

    4. **Selecting the Number of Swap Chain Images**

       A swap chain consists of multiple images. Several images are required for the presentation engine to work properly, that is, one image is presented on the screen, another image waits in a queue for the next v-sync, and a third image is available for the application to render into.

       Application may request more images. We must ensure that the requested number of swap chain images is not smaller than the minimal required number of images and not greater than the maximal supported number of images. Too many images will require much more memory. Too small may cause stalls in the application. Code is:

       ```
       uint32_t image_count = surface_capabilities.minImageCount + 1;
       if( (surface_capabilities.maxImageCount > 0) &&
           (image_count > surface_capabilities.maxImageCount) ) {
         image_count = surface_capabilities.maxImageCount;
       }
       return image_count;
       ```

       The minImageCount value in the surface capabilities gives the required minimum number of images for the swap chain to work properly.

    5. **Selecting a Format for Swap Chain Image**.

       Choosing a format for the images depends on the type of processing/rendering we want to do, that is, if we want to blend the application window with the desktop contents, an alpha value may be required. We must also know what color space is available and if we operate on linear or sRGB colorspace.

       ```
       // If the list contains only one entry with undefined format
       // it means that there are no preferred surface formats and any can be chosen
       if( (surface_formats.size() == 1) &&
           (surface_formats[0].format == VK_FORMAT_UNDEFINED) ) {
         return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
       }
       
       // Check if list contains most widely used R8 G8 B8 A8 format
       // with nonlinear color space
       for( VkSurfaceFormatKHR &surface_format : surface_formats ) {
         if( surface_format.format == VK_FORMAT_R8G8B8A8_UNORM ) {
           return surface_format;
         }
       }
       
       // Return the first format from the list
       return surface_formats[0];
       ```

       If the array of supported format contains only one value with an undefined format, the platform doesn't have any preference. We can use any image format we want.

    6. **Selecting the Size of the Swap Chain Images**

       Typically the size of swap chain images will be identical to the window size. We can choose other sizes, but we must fit into image size constraints. The info fit into the current application window's size is available in the surface capabilities structure. The value of "-1" indicates the application's window size will be determined by the swap chain size, so we can choose whatever dimension we want.

       ```c++
       // Special value of surface extent is width == height == -1
       // If this is so we define the size by ourselves but it must fit within defined confines
       if( surface_capabilities.currentExtent.width == -1 ) {
         VkExtent2D swap_chain_extent = { 640, 480 };
         if( swap_chain_extent.width < surface_capabilities.minImageExtent.width ) {
           swap_chain_extent.width = surface_capabilities.minImageExtent.width;
         }
         if( swap_chain_extent.height < surface_capabilities.minImageExtent.height ) {
           swap_chain_extent.height = surface_capabilities.minImageExtent.height;
         }
         if( swap_chain_extent.width > surface_capabilities.maxImageExtent.width ) {
           swap_chain_extent.width = surface_capabilities.maxImageExtent.width;
         }
         if( swap_chain_extent.height > surface_capabilities.maxImageExtent.height ) {
           swap_chain_extent.height = surface_capabilities.maxImageExtent.height;
         }
         return swap_chain_extent;
       }
       
       // Most of the cases we define size of the swap_chain images equal to current window's size
       return surface_capabilities.currentExtent;
       ```

    7. **Selecting Swap Chain Usage Flags**

       Usage flags defines how a given image may be used in Vulkan. If we want an image to be sampled (used in shaders) it must be created with "sampled" usage. If the image should be used as a depth render target, it must be created with "depth and stencil" usage. For a swap chain we want to render into the image (use it as a render target), so we must specify "color attachment" usage with VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT enum. In Vulkan this usage is always available for swap chains, so we can always set it without any additional checking. Code:

       ```
       // Color attachment flag must always be supported
       // We can define other usage flags but we always need to check if they are supported
       if( surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ) {
         return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
       }
       std::cout << "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain!" << std::endl
       return static_cast<VkImageUsageFlags>(-1);
       ```

       The "transfer destination" usage is required for image clear operation.

    8. **Selecting Pre-Transformations**

       On some platform we may want our image to be transformed. During swap chain creation we must specify what transformations should be applied to image prior to presenting. This info is stored in surface capabilities called "supportedTransforms".

       ```c++
       // Sometimes images must be transformed before they are presented (i.e. due to device's orienation
       // being other than default orientation)
       // If the specified transform is other than current transform, presentation engine will transform image
       // during presentation operation; this operation may hit performance on some platforms
       // Here we don't want any transformations to occur so if the identity transform is supported use it
       // otherwise just use the same transform as current transform
       if( surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
         return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
       } else {
         return surface_capabilities.currentTransform;
       }
       ```

    9. **Selecting Presentation Mode**

       Presentation modes determines the way images will be processed internally by the presentation engine and displayed on the screen. 

       1. single buffer: The draw operations were visible.
       2. double buffer: Prevent the visibility of drawing operations. During presentation, the contents of the second image was copied into the first image (earlier) or (later) the image was swapped which means that their pointers was exchanged. 
       3. Tearing was another issue with displaying images, so the ability to wait for the vertical blank signal was introduced if we want to avoid it. But waiting introduced another proble: input lag. So double buffering was changed into tripple buffering in which we were drawing into two back buffers interchangeably and during v-sync the most recent one was used for presentation.
    
       Presentation mode determines how to deal with all these issues, how to present iamges on the screen and whether we want to use v-sync. Available presentation mode are
    
       + **IMMEDIATE**: Present request are applied immediately and tearing may be observed. The presentation engine doesn't use any queue for holding swap chain image.
    
       + **FIFO**: This mode is most popular to OpenGL's buffer swapping with a swap interval set to 1.
    
         The image is displayed only on vertical blanking periods, so no tearing should be visible. The presentation engine maintains a queue for images to display. If all images are in the queue, the application has to wait until v-sync releases the currently displayed image. Only after that does it become available to the application and program may render image into it. Must be available in all Vulkan implementations.
    
       + **FIFO RELAXED**: Similar to FIFO, but when the image is displayed longer than one blanking period it may be released immediately without waiting for another v-sync signal(so if we are rendering frames with lower frequency than screen's refresh rate, tearing may be visible)
       
       + **MAILBOX**: Most similar to mentioned triple buffering. The queue size if one compared to FIFO. Internally, the presentation engine uses the queue with only a single element. One image is displayed and one waits in the queue. If application want to present another image it is not append to the end of the queue but replaces the one that waits.
       
       For movies we want all frame to be displayed in a proper order. So FIFO mode is the best choice. For game MAILBOX mode always display the recently generated frame, so there is no tearing and input lag is minimized.
    
    10. **Creating a Swap Chain** by ***vkCreateSwapChainKHR*** Code is
    
        ```c++
        uint32_t                      desired_number_of_images = GetSwapChainNumImages( surface_capabilities );
        VkSurfaceFormatKHR            desired_format = GetSwapChainFormat( surface_formats );
        VkExtent2D                    desired_extent = GetSwapChainExtent( surface_capabilities );
        VkImageUsageFlags             desired_usage = GetSwapChainUsageFlags( surface_capabilities );
        VkSurfaceTransformFlagBitsKHR desired_transform = GetSwapChainTransform( surface_capabilities );
        VkPresentModeKHR              desired_present_mode = GetSwapChainPresentMode( present_modes );
        VkSwapchainKHR                old_swap_chain = Vulkan.SwapChain;
        
        if( static_cast<int>(desired_usage) == -1 ) {
          return false;
        }
        if( static_cast<int>(desired_present_mode) == -1 ) {
          return false;
        }
        
        VkSwapchainCreateInfoKHR swap_chain_create_info = {
          VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
          nullptr,                                      // const void                    *pNext
          0,                                            // VkSwapchainCreateFlagsKHR      flags
          Vulkan.PresentationSurface,                   // VkSurfaceKHR                   surface
          desired_number_of_images,                     // uint32_t                       minImageCount
          desired_format.format,                        // VkFormat                       imageFormat
          desired_format.colorSpace,                    // VkColorSpaceKHR                imageColorSpace
          desired_extent,                               // VkExtent2D                     imageExtent
          1,                                            // uint32_t                       imageArrayLayers
          desired_usage,                                // VkImageUsageFlags              imageUsage
          VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
          0,                                            // uint32_t                       queueFamilyIndexCount
          nullptr,                                      // const uint32_t                *pQueueFamilyIndices
          desired_transform,                            // VkSurfaceTransformFlagBitsKHR  preTransform
          VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
          desired_present_mode,                         // VkPresentModeKHR               presentMode
          VK_TRUE,                                      // VkBool32                       clipped
          old_swap_chain                                // VkSwapchainKHR                 oldSwapchain
        };
        
        if( vkCreateSwapchainKHR( Vulkan.Device, &swap_chain_create_info, nullptr, &Vulkan.SwapChain ) != VK_SUCCESS ) {
          std::cout << "Could not create swap chain!" << std::endl;
          return false;
        }
        if( old_swap_chain != VK_NULL_HANDLE ) {
          vkDestroySwapchainKHR( Vulkan.Device, old_swap_chain, nullptr );
        }
        
        return true;
        ```
    
        Images in Vulkan can be referenced by queues. This means that we can create commands that use these images. These commands are stored in command buffers, and these command buffers are submitted to queue.

13. **Image Presentation**

    Swap chain images belong to and are owned by the swap chain. This means that the application can't use these images until it asks for them. After finishes using the image it should return it by presenting it. If not we will soon run out of images and nothing will display on the screen. Acquiring access may require waiting when there are not enough images. The availability of images may depend on many factors: internal implementation, OS, number of created images, number of images the application want to use at a single time and on the selected presentation time. Code is 

    ```c++
    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR( Vulkan.Device, Vulkan.SwapChain, UINT64_MAX, Vulkan.ImageAvailableSemaphore, VK_NULL_HANDLE, &image_index );
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
    ```

    To access an image, call the ***vkAcquireNextImageKHR*** function. We must specify a swap chain from which we want to use an image, a timeout, a semaphore, and a fence object. Images are processed or referenced by commands stored in command buffers. In Vulkan, creating command buffers and submitting them to queues is the only way to cause operations to be performed by the device.
    When command buffers are submitted to queues, all their commands start being processed. But a queue can't use an image until it is allowed to, and the semaphore we created earlier is for internal queue synchronization--before the queue starts processing commands that reference a given image, it should wait on this semaphore. There are two synchronization mechanisms for accessing swap chain images. (1) a timeout, which may block an application but doesn't stop queue processing. (2) a semaphore, which doesn't block the application but blocks selected queues. Before the processing will start, we should tell the queue to wait. Code is:

    ```c++
    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit_info = {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,                // VkStructureType              sType
      nullptr,                                      // const void                  *pNext
      1,                                            // uint32_t                     waitSemaphoreCount
      &Vulkan.ImageAvailableSemaphore,              // const VkSemaphore           *pWaitSemaphores
      &wait_dst_stage_mask,                         // const VkPipelineStageFlags  *pWaitDstStageMask;
      1,                                            // uint32_t                     commandBufferCount
      &Vulkan.PresentQueueCmdBuffers[image_index],  // const VkCommandBuffer       *pCommandBuffers
      1,                                            // uint32_t                     signalSemaphoreCount
      &Vulkan.RenderingFinishedSemaphore            // const VkSemaphore           *pSignalSemaphores
    };
    
    if( vkQueueSubmit( Vulkan.PresentQueue, 1, &submit_info, VK_NULL_HANDLE ) != VK_SUCCESS ) {
      return false;
    }
    ```

    In this example we telling the queue to wait only on one semaphore, which will be signaled by the presentation engine when the queue can safely start processing commands referencing the swap chain image.

    After we have submitted a command buffer, all the processing starts in the background. Next, we want to present a rendered image. Presenting means that we want our image to be displayed and we are giving it back to the swap chain. Code is

    ```c++
    VkPresentInfoKHR present_info = {
      VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,           // VkStructureType              sType
      nullptr,                                      // const void                  *pNext
      1,                                            // uint32_t                     waitSemaphoreCount
      &Vulkan.RenderingFinishedSemaphore,           // const VkSemaphore           *pWaitSemaphores
      1,                                            // uint32_t                     swapchainCount
      &Vulkan.SwapChain,                            // const VkSwapchainKHR        *pSwapchains
      &image_index,                                 // const uint32_t              *pImageIndices
      nullptr                                       // VkResult                    *pResults
    };
    result = vkQueuePresentKHR( Vulkan.PresentQueue, &present_info );
    
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

    An image is presented by calling the ***vkQueuePresentKHR***. Each operation that is performed by calling ***vkQueue...*** is appended to the end of the queue for processing.

14. **Checking What Images Were Created in a Swap Chain**

    pass for temporary
    
    **Recreating a Swap Chain**: The properties of the surface, platform, or application window properties changed in such a way that the current the swap chain can't be used anymore. If the return value is ***VK_SUBOPTIMAL_KHR***, The swap chain sometimes can still be used, but it may no longer the optimal choice. If it is ***VK_ERROR_OUT_OF_DATE_KHR***, we must recreate the swap chain.

15. **Command  Buffers**: Commands are encapsulated inside command buffers. Submitting such buffers to queues causes devices to start processing commands that were recorded in them.

    1. **Creating Command Buffer Memory Pool**: To prepare space for commands we create a pool from which the buffer can allocate its memory. Command buffer is not connected with any queue or queue family, but the memory pool from which buffer allocates memory is. So each command buffer that takes memory from a given memory pool can only be submitted to a queue from a queue family which the memory pool was created. Code is 

       ```
       VkCommandPoolCreateInfo cmd_pool_create_info = {
         VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,     // VkStructureType              sType
         nullptr,                                        // const void*                  pNext
         0,                                              // VkCommandPoolCreateFlags     flags
         Vulkan.PresentQueueFamilyIndex                  // uint32_t                     queueFamilyIndex
       };
       
       if( vkCreateCommandPool( Vulkan.Device, &cmd_pool_create_info, nullptr, &Vulkan.PresentQueueCmdPool ) != VK_SUCCESS ) {
         std::cout << "Could not create a command pool!" << std::endl;
         return false;
       }
       ```

       Call the ***vkCreateComandPool*** to create tool for command buffer.

    2. **Allocating Command Buffers**:  The code is:

       ```c++
       uint32_t image_count = 0;
       if( (vkGetSwapchainImagesKHR( Vulkan.Device, Vulkan.SwapChain, &image_count, nullptr ) != VK_SUCCESS) ||
           (image_count == 0) ) {
         std::cout << "Could not get the number of swap chain images!" << std::endl;
         return false;
       }
       
       Vulkan.PresentQueueCmdBuffers.resize( image_count );
       
       VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {
         VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType              sType
         nullptr,                                        // const void*                  pNext
         Vulkan.PresentQueueCmdPool,                     // VkCommandPool                commandPool
         VK_COMMAND_BUFFER_LEVEL_PRIMARY,                // VkCommandBufferLevel         level
         image_count                                     // uint32_t                     bufferCount
       };
       if( vkAllocateCommandBuffers( Vulkan.Device, &cmd_buffer_allocate_info, &Vulkan.PresentQueueCmdBuffers[0] ) != VK_SUCCESS ) {
         std::cout << "Could not allocate command buffers!" << std::endl;
         return false;
       }
       
       if( !RecordCommandBuffers() ) {
         std::cout << "Could not record command buffers!" << std::endl;
         return false;
       }
       return true;
       ```

       Use the ***vkAllocateCommandBuffers*** to create command buffer. 

    3.  **Recording Command Buffers**: Code of first part:

       ```
       uint32_t image_count = static_cast<uint32_t>(Vulkan.PresentQueueCmdBuffers.size());
       
       std::vector<VkImage> swap_chain_images( image_count );
       if( vkGetSwapchainImagesKHR( Vulkan.Device, Vulkan.SwapChain, &image_count, &swap_chain_images[0] ) != VK_SUCCESS ) {
         std::cout << "Could not get swap chain images!" << std::endl;
         return false;
       }
       
       VkCommandBufferBeginInfo cmd_buffer_begin_info = {
         VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
         nullptr,                                      // const void                            *pNext
         VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, // VkCommandBufferUsageFlags              flags
         nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
       };
       
       VkClearColorValue clear_color = {
         { 1.0f, 0.8f, 0.4f, 0.0f }
       };
       
       VkImageSubresourceRange image_subresource_range = {
         VK_IMAGE_ASPECT_COLOR_BIT,                    // VkImageAspectFlags                     aspectMask
         0,                                            // uint32_t                               baseMipLevel
         1,                                            // uint32_t                               levelCount
         0,                                            // uint32_t                               baseArrayLayer
         1                                             // uint32_t                               layerCount
       };
       ```

       We need to prepare a variable of structured type ***VkCommandBufferBeginInfo***. It contains the information necessary in more typical rendering scenarios.

       Tow types of command buffer:

       + Primary command buffers (similar to drawing lists) are independent, individual "begins" and they may be submitted to queues.
       + Second command buffers may only be referenced from with primary command buffers and can not be submitted directly to queues.

       The ***VkImageSubresourceRange*** specifies parts of the image that our operations will be performed on. Our image consists of only one mipmap level and one array level.

    4. **Image Layouts and Layout Transitions**: Images may be used as render targets, as textures that can be sampled from inside shaders, or as a data resource for copy/blit operation. We must specify different usage flag during image creation for different types of operations. Depending on the type of operations, images may be differently allocated or may have a different layout in memory. We can use a general layout that is supported by all operations, but it may not provide the best performance. For different usage we should use dedicated layouts. To perform different operations on image, we must change the images current layout before we can perform each type of operation. The swap-chain-images have ***VK_IMAGE_LAYOUT_PRESENT_SOURCE_KHR*** layouts (designed for presentation engine to display on the screen). Before using these images, we need to change their layouts to ones compatible with desired operations. After processing the images, we need to transition their layout back to the ***VK_IMAGE_LAYOUT_PRESENT_SOURCE_KHR*** otherwise the presentation engine will not be able to use these images and undefine behavior may occur. ***Image memory barriers*** are use to do this.

    5. **Recording Command Buffers**: The record code is:

       ```c++
       for( uint32_t i = 0; i < image_count; ++i ) {
         VkImageMemoryBarrier barrier_from_present_to_clear = {
           VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
           nullptr,                                    // const void                            *pNext
           VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
           VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
           VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                          oldLayout
           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
           Vulkan.PresentQueueFamilyIndex,             // uint32_t                               srcQueueFamilyIndex
           Vulkan.PresentQueueFamilyIndex,             // uint32_t                               dstQueueFamilyIndex
           swap_chain_images[i],                       // VkImage                                image
           image_subresource_range                     // VkImageSubresourceRange                subresourceRange
         };
       
         VkImageMemoryBarrier barrier_from_clear_to_present = {
           VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
           nullptr,                                    // const void                            *pNext
           VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          srcAccessMask
           VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          dstAccessMask
           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          oldLayout
           VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                          newLayout
           Vulkan.PresentQueueFamilyIndex,             // uint32_t                               srcQueueFamilyIndex
           Vulkan.PresentQueueFamilyIndex,             // uint32_t                               dstQueueFamilyIndex
           swap_chain_images[i],                       // VkImage                                image
           image_subresource_range                     // VkImageSubresourceRange                subresourceRange
         };
       
         vkBeginCommandBuffer( Vulkan.PresentQueueCmdBuffers[i], &cmd_buffer_begin_info );
         vkCmdPipelineBarrier( Vulkan.PresentQueueCmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear );
       
         vkCmdClearColorImage( Vulkan.PresentQueueCmdBuffers[i], swap_chain_images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &image_subresource_range );
       
         vkCmdPipelineBarrier( Vulkan.PresentQueueCmdBuffers[i], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present );
         if( vkEndCommandBuffer( Vulkan.PresentQueueCmdBuffers[i] ) != VK_SUCCESS ) {
           std::cout << "Could not record command buffers!" << std::endl;
           return false;
         }
       }
       
       return true;
       ```

       First we need to prepare two memory barriers.  Memory barriers are used to change three different things in case of images which need a variable of type ***VkImageMemoryBarrier*** which has:

       + sType: type of structure, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER.
       + pNext
       + srcAccessMask: Type of memory operations done on the image before memory barrier.
       + dstAccessMask: Type of memory operations that will take place after memory barrier.
       + oldLayout: Layout from which we are transitioning.
       + newLayout: A layout that is compatible with operations that we will perform after barrier.
       + srcQueueFamilyIndex: queue family index that was referencing the image previously.
       + dstQueueFamilyIndex: queue family index that will reference the image after memory barrier.
       + image: handle to image
       + subresourceRange: describing parts of an image we want to perform transitions on.

       In this example before the first barrier and after the second barrier only the presentation engine has access to the image which only reads from the image. So the srcAccessMask (first barrier) and dstAccessMask in the second barrier to ***AK_ACCESS_MEMORY_READ_BIT***. We only clear an image (the so-called "transfer" operation), so ***VK_ACCESS_MEMORY_WRITE_BIT*** is used.

       **vkBegineCommandBuffer** is used to start recording our commands. Then **vkCmdPipelineBarrier**  is used to set up barrier to change image layout. After barrier we can safely perform any operations that is compatible with the layout we have transitioned images to. The **vkCmdClearColorImage** function is used to clear image to specific color. The last is to call **vkEndCommandBuffer** to inform that we have ended recording a command buffer. If there were errors, we can't use the command buffer and need to record it again.

16. **Clean up**:

    Note: Destroying a command pool implicitly frees all command buffers allocated from a given pool.

17. **Conclusion**: process of this tutorial:
    * Enable proper instance level functions.
    * Create an application window's Vulkan representation called a surface.
    * Choose a device with a queue family that supported presentation and create a logical device which enables device extensions.
    * Create a swap chain. To do that we first acquired a set of parameters describing our surface and then choose value for proper swap chain creation.
    * Create and record command buffers, which also includes image's layout transitions for which image memory barrier were used.
    * Present image, which include acquiring an image, submitting a command buffer.

