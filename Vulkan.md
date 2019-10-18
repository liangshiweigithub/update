

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

        Specific types of command may be processed by dedicated hardware, and that's why queues are called families. Each queues family may support different types of operations.

        

