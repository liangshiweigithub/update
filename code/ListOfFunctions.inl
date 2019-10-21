
// Exported functions, these functions are always exposed by vulkan libraries.
#if !defined(VK_EXPORTED_FUNCTION)
#define VK_EXPORTED_FUNCTION( fun )
#endif

VK_EXPORTED_FUNCTION(vkGetInstanceProcAddr)

#undef VK_EXPORTED_FUNCTION


#if !defined(VK_GLOBAL_LEVEL_FUNCTION)
#define VK_GLOBAL_LEVEL_FUNCTION
#endif

// Tutorial 01
VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance)

#undef VK_GLOBAL_LEVEL_FUNCTION


// Instance level funcions
#if !defined(VK_INSTANCE_LEVEL_FUNCTION)
#define VK_INSTANCE_LEVEL_FUNCION
#endif

// Tutorial 01

VK_INSTANCE_LEVEL_FUNCION(vkEnumeratePhysicalDevices)
VK_INSTANCE_LEVEL_FUNCION(vkGetPhysicalDeviceProperties)
VK_INSTANCE_LEVEL_FUNCION(vkGetPhysicalDeviceFeatures)
VK_INSTANCE_LEVEL_FUNCION(vkGetPhsysicalDeviceQueueFamilyProperties)
VK_INSTANCE_LEVEL_FUNCION(vkCreateDevice)
VK_INSTANCE_LEVEL_FUNCION(vkGetDeviceProcAddr)
VK_INSTANCE_LEVEL_FUNCION(vkDestroyInstance)

#undef VK_INSTANCE_LEVEL_FUNCION

