---
title: Vk.Instance
description: Nothing
---

# Nz::Vk::Instance

Class description

## Constructors

- `Instance()`
- `Instance(`Vk::Instance` const&)`
- `Instance(`Vk::Instance`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(Nz::RenderAPIValidationLevel validationLevel, VkInstanceCreateInfo const& createInfo, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(Nz::RenderAPIValidationLevel validationLevel, std::string const& appName, Nz::UInt32 appVersion, std::string const& engineName, Nz::UInt32 engineVersion, Nz::UInt32 apiVersion, std::vector<const char *> const& layers, std::vector<const char *> const& extensions, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| `bool` | `EnumeratePhysicalDevices(std::vector<VkPhysicalDevice>* physicalDevices)` |
| `PFN_vkVoidFunction` | `GetDeviceProcAddr(VkDevice device, char const* name)` |
| `Nz::UInt32` | `GetApiVersion()` |
| `VkResult` | `GetLastErrorCode()` |
| `bool` | `GetPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<VkExtensionProperties>* extensionProperties)` |
| `VkPhysicalDeviceFeatures` | `GetPhysicalDeviceFeatures(VkPhysicalDevice device)` |
| `VkFormatProperties` | `GetPhysicalDeviceFormatProperties(VkPhysicalDevice device, VkFormat format)` |
| `bool` | `GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* imageFormatProperties)` |
| `VkPhysicalDeviceMemoryProperties` | `GetPhysicalDeviceMemoryProperties(VkPhysicalDevice device)` |
| `VkPhysicalDeviceProperties` | `GetPhysicalDeviceProperties(VkPhysicalDevice device)` |
| `bool` | `GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>* queueFamilyProperties)` |
| `PFN_vkVoidFunction` | `GetProcAddr(char const* name)` |
| `Nz::RenderAPIValidationLevel` | `GetValidationLevel()` |
| `void` | `InstallDebugMessageCallback()` |
| `bool` | `IsExtensionLoaded(std::string const& extensionName)` |
| `bool` | `IsLayerLoaded(std::string const& layerName)` |
| `bool` | `IsValid()` |
| Vk::Instance`&` | `operator=(`Vk::Instance` const&)` |
| Vk::Instance`&` | `operator=(`Vk::Instance`&&)` |
