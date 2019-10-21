

# Beginning

### introduction

​	[source page](http://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-1 )

​     Way to use Vulkan: Use the Vulkan SDK, dynamically load Vulcan Loader library at runtime,  and load function pointers from it. This method provides more configuration abilities and more flexibility without the need to modify Vulkan application source code. We need to choose whether we want to statically link with a Vulkan Loader or whether we will load it dynamically and acquire function pointers by ourselves at runtime. The latter is chosen. Dynamically loading function points from the Vulkan Runtime library, use wglGetrocAddress() or GetProcAddress() on windows.

#### Loading VulKan Runtime Library and Acquiring Pointers to an Exported Function

1. Load library: The  Vulkan Runtime Library is usually installed along with the graphic that support it. Its name is vulcan-1.dll on Windows and libvulkan.so on Linux. The code is:

   ```c++
   #if defined(VK_USE_PLATFORM_WIN32_KHR)
       VulkanLibrary = LoadLibrary( "vulkan-1.dll" );
   #elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
       VulkanLibrary = dlopen( "libvulkan.so.1", RTLD_NOW );
   #endif
       if( VulkanLibrary == nullptr ) {
             std::cout << "Could not load Vulkan library!" << std::endl;
             return false;
           }
        return true;
   ```

   The Vulkan library are required to expose only one function that can be loaded with the standard techniques our OS possesses. The only function that must be exported is vkGetInstanceProcAddr(). This function is used to load all other Vulkan functions.

2.  The type of Vulkan API procedures:

   1. Global-level functions used to create a Vulkan instance.
   2. Instance-level functions: Check what Vulkan-capable hardware is available and what Vulkan features are exposed.
   3. Device-level functions: Responsible for performing jobs typically done in a 3D application.

3. Functions that can create a Vulkan instance:

   + vkCreateInstance: used to create Vulkan instance, which is equivalent of rendering context in OpenGL. It stores per-application state like enabled instance-level layers and extensions.

   + vkEnumerateInstanceExtensionProperties

   + vkEnumerateInstanceLayerProperties

     This two functions allows us to check what instance are available and what instance extensions are available. Validation Layer are divided into instance and device levels.

     Extension expose additional functionality that is not required by core specifications, and not all hardware vendors may implement them. Like layers, Extensions are divided into instance and device level, and extensions from different levels must be enabled separately. In OpenGL, all extensions are available in created contexts, using Vulkan we have to enable them before the functionality exposed by them can be used.
     
   4. Call the function vkGetIntanceProcAddr() to acquire addresses of instance-level procedures.  It takes tow parameters: an instance, and a function name. We don't have a instance yet so we provide null for the first parameter. The second parameter required by the vkGetInstanceProcAdd() is a name of a procedure address of which we want to acquire. We can only load global-level functions without and instance. 

4. Create a Vulkan instance. Use the loaded function vkCreateInstance() which take threee parameters:

   + The first parameter contains information about our application, the required Vulkan version, and the instance level layers and extensions we want to enable. 
   + The second parameter provides a pointer to a structure with list of different functions related to memory allocation. This usually used for debugging purposes but this feature is optional and we can rely on built-in memory allocation methods.
   + The third parameter is an address of a variable in which we want to store Vulkan instance handle.

   Detail code:

   ``````c++
   VkApplicationInfo application_info = {
     VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType
     nullptr,                                        // const void                *pNext
     "API without Secrets: Introduction to Vulkan",  // const char                *pApplicationName
     VK_MAKE_VERSION( 1, 0, 0 ),                     // uint32_t                   applicationVersion
     "Vulkan Tutorial by Intel",                     // const char                *pEngineName
     VK_MAKE_VERSION( 1, 0, 0 ),                     // uint32_t                   engineVersion
     VK_API_VERSION                                  // uint32_t                   apiVersion
   };
   
   VkInstanceCreateInfo instance_create_info = {
     VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
     nullptr,                                        // const void*                pNext
     0,                                              // VkInstanceCreateFlags      flags
     &application_info,                              // const VkApplicationInfo   *pApplicationInfo
     0,                                              // uint32_t                   enabledLayerCount
     nullptr,                                        // const char * const        *ppEnabledLayerNames
     0,                                              // uint32_t                   enabledExtensionCount
     nullptr                                         // const char * const        *ppEnabledExtensionNames
   };
   
   if( vkCreateInstance( &instance_create_info, nullptr, &Vulkan.Instance ) != VK_SUCCESS ) {
     std::cout << "Could not create Vulkan instance!" << std::endl;
     return false;
   }
   return true;
   ``````

5. Acquiring Pointers to instance-level function: 

   Instance-level functions allow us to create a logical device, which can be seen as a user view on a physical device. We may use many devices for different purpose(such as one for 3D graphics, one for physics calculations, and one for media decoding.) When use Vulkan, we must specify which device to perform the operations on. The instance-level function can check what devices and how many of them are available, what their capabilities are, and what operations the support. The code for loading instance-level functions is almost identical to the code loading global-level functions.

   Instance-level function operate on physical devices. A physical device refers to any physical graphics card that is installed on a computer running a Vulkan-enabled application that is capable of executing Vulkan commands. Such a device may expose and implement different Vulkan features, may have different capabilities. Instance-level functions allow us to check all these parameters to decide which physical device we want to use. After check, we create a logical device that represents our choice in the application.

   Instance-level function:

   + vkEnumeratePhysicalDevices
   + vkGetPhysicalDeviceProperties
   + vkGetPhysicalDeviceFeatures
   + vkGetPhysicalDeviceQueueFamilyProperties
   + vkCreateDevice
   + vkGetDeviceProcAddr
   + vkDestroyInstance

6. Creating a logical device

   1. check to see how many physical devices are available in the system

      ```c++
      uint32_t num_devices = 0;
      if( (vkEnumeratePhysicalDevices( Vulkan.Instance, &num_devices, nullptr ) != VK_SUCCESS) ||
          (num_devices == 0) ) {
        std::cout << "Error occurred during physical devices enumeration!" << std::endl;
        return false;
      }
      
      std::vector<VkPhysicalDevice> physical_devices( num_devices );
      if( vkEnumeratePhysicalDevices( Vulkan.Instance, &num_devices, &physical_devices[0] ) != VK_SUCCESS ) {
        std::cout << "Error occurred during physical devices enumeration!" << std::endl;
        return false;
      }
      ```

   2.  Check the properties of each device:

      ```c++
      VkPhysicalDevice selected_physical_device = VK_NULL_HANDLE;
      uint32_t selected_queue_family_index = UINT32_MAX;
      for( uint32_t i = 0; i < num_devices; ++i ) {
        if( CheckPhysicalDeviceProperties( physical_devices[i], selected_queue_family_index ) ) {
          selected_physical_device = physical_devices[i];
        }
      }
      ```

      Code of CheckPhysicalDeviceProperties:

      ```c++
      VkPhysicalDeviceProperties device_properties;
      VkPhysicalDeviceFeatures   device_features;
      
      vkGetPhysicalDeviceProperties( physical_device, &device_properties );
      vkGetPhysicalDeviceFeatures( physical_device, &device_features );
      
      uint32_t major_version = VK_VERSION_MAJOR( device_properties.apiVersion );
      uint32_t minor_version = VK_VERSION_MINOR( device_properties.apiVersion );
      uint32_t patch_version = VK_VERSION_PATCH( device_properties.apiVersion );
      
      if( (major_version < 1) &&
          (device_properties.limits.maxImageDimension2D < 4096) ) {
        std::cout << "Physical device " << physical_device << " doesn't support required parameters!" << std::endl;
        return false;
      }
      ```

      + Device Properties: Contains fields such as supported Vulkan API version, limits and so on. Limits describe how big textures can be created, how many samples in anti-aliasing are supported, or how many buffers in a given shader stage can be used.

      + Device Features: Additional hardware capabilities that are similar to extensions. They may not necessarily be supported by the driver and by default not enabled. During logical device creation, we must check whether feature is supported and enable it.

      + Queues, Queue Families, and Command Buffers:

        Queues is command buffer, in Vulkan the buffer is visible to the user and the user must specifically create and manage buffers for commands.

        Specific types of command may be processed by dedicated hardware, and that's why queues are called families. Each queues family may support different types of operations. Second Part of  CheckPhysicalDeviceProperties() function.

        ```c++
        uint32_t queue_families_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, nullptr );
        if( queue_families_count == 0 ) {
          std::cout << "Physical device " << physical_device << " doesn't have any queue families!" << std::endl;
          return false;
        }
        
        std::vector<VkQueueFamilyProperties> queue_family_properties( queue_families_count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, &queue_family_properties[0] );
        for( uint32_t i = 0; i < queue_families_count; ++i ) {
          if( (queue_family_properties[i].queueCount > 0) &&
              (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ) {
            queue_family_index = i;
            return true;
          }
        }
        
        std::cout << "Could not find queue family with required properties on physical device " << physical_device << "!" << std::endl;
        return false;
        ```
        
        The properties of each queue family contain queue flag, number of available queues in this family, time stamp support and image transfer granularity.
      
   3.  After we chose the physical device we can create logical device that will represent it in the rest of our application. The code is:
   
      ```c++
      if( selected_physical_device == VK_NULL_HANDLE ) {
        std::cout << "Could not select physical device based on the chosen properties!" << std::endl;
        return false;
      }
      
      std::vector<float> queue_priorities = { 1.0f };
      
      VkDeviceQueueCreateInfo queue_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // VkStructureType              sType
        nullptr,                                        // const void                  *pNext
        0,                                              // VkDeviceQueueCreateFlags     flags
        selected_queue_family_index,                    // uint32_t                     queueFamilyIndex
        static_cast<uint32_t>(queue_priorities.size()), // uint32_t                     queueCount
        &queue_priorities[0]                            // const float                 *pQueuePriorities
      };
      
      VkDeviceCreateInfo device_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,           // VkStructureType                    sType
        nullptr,                                        // const void                        *pNext
        0,                                              // VkDeviceCreateFlags                flags
        1,                                              // uint32_t                           queueCreateInfoCount
        &queue_create_info,                             // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
        0,                                              // uint32_t                           enabledLayerCount
        nullptr,                                        // const char * const                *ppEnabledLayerNames
        0,                                              // uint32_t                           enabledExtensionCount
        nullptr,                                        // const char * const                *ppEnabledExtensionNames
        nullptr                                         // const VkPhysicalDeviceFeatures    *pEnabledFeatures
      };
      
      if( vkCreateDevice( selected_physical_device, &device_create_info, nullptr, &Vulkan.Device ) != VK_SUCCESS ) {
        std::cout << "Could not create Vulkan device!" << std::endl;
        return false;
      }
      
      Vulkan.QueueFamilyIndex = selected_queue_family_index;
      return true;
      ```
   
      Logic device is created by calling vkCreateDevice function. It takes the handle to a physical device and an address of a structure that contains the information necessary for device creation. The structure is of type VkDeviceCreateInfo and contains the following field.
   
      * sType: standard type of a provided structure.
      
      * pNext: parameter pointing to an extension specific structure.
      
      * flags:  reserved for future which is set to zero.
      
      * queueCreateInfoCount: number of different queue families from which we create queues along with the device.
      
      * pQueueCreateInfos: point to an array of queuqCreateInfoCount elements specify queues we want to create.
      
        the rest is ignored here.
      
      VkDeviceQueueCreateInfo describes a queue family. Its fields include SType, PNext, flags, queueFamilyIndex, queueCount, pQueuePriorities. Different queue families may have similar meaning the may support similar types of operations. The higher of the queue priority, the more time the given queue may have for processing commands. But this is not guaranteed. And it don't influence execution order. Queues from different devices are treated independently.
      
   
7. Acquiring Pointes to Device-level function:

   We can use logical device we created to load functions from device level. 

   + There can be so many devices on a single computer, we can load universal procedures which is done with the vkGetInstanceProcAddr function. It returns the address of dispatch functions that perform jumps to proper implementions based on a provided logical device handle.

   + Loading functions for each logic device separately to avoid overhead by the upper method. We must remember that we can call the given function only with the device we loaded this function from. When using more devices we must load functions from each of these devices. This is done by vkGetDeviceProcAdd function along with a logical device handle.

   1. Retrieving Queues: Now that we have created a device, we need a queue that we can submit some commands to for processing. The vkGetDeviceQueue is used to ask for a queue handle. Code is 

      ```c++
      vkGetDeviceQueue(Vulkan.Device, Vulkan.QueueFamilyIndex, 0, &Vulkan.Queue)
      ```

      The first parameter is the logic device, the second is the queue family index and it must be one of the indices we've provided during logical device creation. One last parameter is a queue index from within a given family, it must be smaller than the total number of queues we requested from a given family.  When the function succeeds, it store a handle to a requested queue in the last variable. All work we want perform can be submitted for processing to the acquired.

8.  Resource cleanup: cleanup code is:

   ```
   if( Vulkan.Device != VK_NULL_HANDLE ) {
     vkDeviceWaitIdle( Vulkan.Device );
     vkDestroyDevice( Vulkan.Device, nullptr );
   }
   
   if( Vulkan.Instance != VK_NULL_HANDLE ) {
     vkDestroyInstance( Vulkan.Instance, nullptr );
   }
   
   if( VulkanLibrary ) {
   #if defined(VK_USE_PLATFORM_WIN32_KHR)
     FreeLibrary( VulkanLibrary );
   #elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
     dlclose( VulkanLibrary );
   #endif
   }
   ```

   We must ensure that before deleting any object, it is not being used by device. That's why there is a wait function which will block until all processing on queues of a given device is finished.

9. Conclusion: how to use Vulkan:
   1. Connect with Vulkan Runtime library and load global level function from it
   2. Create Vulkan instance and load instance-level functions.
   3. Check what physical devices are available and what are their features, properties, and capabilities.
   4. Create a logical device and describe what and how many queues must be created along with device
   5. Retrieve device level functions using newly created logical device handle.
   6. Retrieve queues and submit work for execution.



