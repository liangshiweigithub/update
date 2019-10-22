## Part 2: Swap chain

1. To draw something, we must integrate the Vulkan pipeline with the application and API that the OS provides. In OpenGL we acquire Device Context that is associated with the application's window. We create a 32-bit color surface with a 24-bit depth buffer and a support for double buffering. In Vulkan there is no default frame buffer. If we want to display something we can create a set of buffers to which we can render. These buffer along with their properties, similar to Direct3D*, are called a swap chain. To display we give buffers back to the presentation engine.  In OpenGL we first have to define the surface format and associate it with a window, then create Rendering Context. In Vulkan, we first create an instance, a device, and then create a swap chain.

2. Swap chain is treated as extension, the reason is:

   1. Vulkan can be used for many different purpose, including performing mathematical operations, not just display an image(which requires swap chain).
   2. Every OS displays image in a different way. The surface on which you can render may be implemented differently, can have a different format, and can be differently represented in the OS, there is no one universal way to do it.

3. Swap chain provides render targets that integrates with OS specific code. The swap chain creation and usage is an extension, we have to ask for the extension during both instance and device creation. If a given instance and device doesn't support these extensions, the instance and/or device creation will fail.

4. In the case of swap-chain support, three extensions are involved: two from an instance level and one from device level.

   1. The ***VK_KHR_surface*** extension is defined at the instance level. It describes a "surface" object, which is a logical representation of an application window. This extension allows us to check different parameters(capabilities, supported formats, size) of a surface and query whether the given physical device support a swap chain. This extension also defines methods to destroy any such surface.
   2. ***VK_KHR_win32_surface*** on windows and ***VK_KHR_xlib_surface*** or ***VK_KHR_xcb_surface***. This allows us to create a surface that represents the application's window in a given OS.

5. Check whether an instance extension is supported: ***vkEnumerateInstanceExtensionProperties*** This function enumerates all available instance general extensions, if the first parameter is null, or instance layer extensions if we set the first parameter to the name of given layer. Code is

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

7. Create a Presentation Surface:  Call ***vkCreate???SurfaceKHR*** function which accepts Vulcan instance, a pointer to a OS-specific structure, a point to optional memory allocation handling functions and a pointer to a variable to store created surface. The structure is ***Vk???SurfaceCreateInfoKHR*** which contains sTye, pNext, flags, hInstance/connection/dpy and hwnd/window(handle to application window)

8. Check whether a device extension is supported. The extension is called ***VK_KHR_swapchain***, and it defines the actual support, implementation and usage of a swap chain. The check function is ***vkEnumerateDeviceExtensionProperties***. It behaves identically to the function querying instance extension. The only difference is that it takes an additional physical device handle in the first parameter. The code is:

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

9. Check whether presentation to a given surface is supported:

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

