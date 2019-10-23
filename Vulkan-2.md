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

   if we prepare a place for a smaller amount of extensions, the function will return VK_INCOMPLETE. Each element in available_extensions contains the name of extension and its version.

6. Enabling an instance-level Extension we need to prepare an array with the names of all extensions we want to enable and pass it to ***VkInstanceCreateInfo***. When create instance success, we can load instance-level functions including additional, extension-specific functions.

7. **Create a Presentation Surface:**  Call ***vkCreate???SurfaceKHR*** function which accepts Vulcan instance, a pointer to a OS-specific structure, a point to optional memory allocation handling functions and a pointer to a variable to store created surface. The structure is ***Vk???SurfaceCreateInfoKHR*** which contains sTye, pNext, flags, hInstance/connection/dpy and hwnd/window(handle to application window)

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

    Fill a variable of VkDeviceCreateInfo type with VkDeviceQueueCreateInfo. Then ask for the third extension related to a swap chain-- a device-level ***VK_KHR_swapchain*** extension. After create device, we can load device level functions and get device queues. Code to create Device:

    ```c++
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::vector<float> queue_priorities = { 1.0f };
    
    queue_create_infos.push_back( {
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType              sType
      nullptr,                                          // const void                  *pNext
      0,                                                // VkDeviceQueueCreateFlags     flags
      selected_graphics_queue_family_index,             // uint32_t                     queueFamilyIndex
      static_cast<uint32_t>(queue_priorities.size()),   // uint32_t                     queueCount
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

    To create a swap chain, we call the ***vkCreateSwapchainKHR*** function.  It requires us to provide an address of a variable of type VkSwapchainCreateInfoKHR, which informs the driver about the properties of a swap chain that is being created.  This info is get by check device properties.

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

       Presentation modes determines the way images will be processed internally by the presentation egine and displayed on the screen. 

       1. single buffer: The draw operations were visible.
       2. dubble buffer: Prevent the visibility of drawing operations. During presentation, the contents of the second image was copied into the first image (earlier) or (later) the image was swapped which means that their pinters was exchanged. 
       3. Tearing was another issue with displaying images, so the ability to wait for the vertical blank signal was introduced if we want to avoid it. But waiting introduced another proble: input lag. So double buffering was changed into tripple buffering in which we were drawing into two back buffers intechangeably and during v-sync the most recent one was used for presentation.
    
       Presentation mode determines how to deal with all these issues, how to present iamges on the screen and whether we want to use v-sync. Available presentation mode are
    
       + **IMMEDIATE**: Present request are applied immediately and tearing may be observed. The presentation engine doesn't use any queue fro holding swap chain image.
    
       + **FIFO**: This mode is most popular to OpenGL's buffer swapping with a swap interval set to 1.
    
         The image is displayed only on vertical blanking periods, so no tearing should be visible. The presentation engine maintains a queue for images to display. If all images are in the queue, the application has to wait until v-sync releases the currently displayed image. Only after that does it become available to the application and program may render image into it. Must be available in all Vulkan implementations.
    
          
    
    
    
    



