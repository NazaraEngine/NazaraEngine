---
title: Vk.Device
description: Nothing
---

# Nz::Vk::Device

Class description

## Constructors

- `Device(`[`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)`& instance)`
- `Device(`Vk::Device` const&)`
- `Device(`Vk::Device`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vk::AutoCommandBuffer` | `AllocateCommandBuffer(Nz::QueueType queueType)` |
| `bool` | `Create(`[`Vk::PhysicalDevice`](documentation/generated/VulkanRenderer/Vk.PhysicalDevice.md)` const& deviceInfo, VkDeviceCreateInfo const& createInfo, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| `Nz::UInt32` | `GetDefaultFamilyIndex(Nz::QueueType queueType)` |
| [`VulkanDescriptorSetLayoutCache`](documentation/generated/VulkanRenderer/VulkanDescriptorSetLayoutCache.md)` const&` | `GetDescriptorSetLayoutCache()` |
| `std::vector<QueueFamilyInfo> const&` | `GetEnabledQueues()` |
| `QueueList&` | `GetEnabledQueues(Nz::UInt32 familyQueue)` |
| [`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)`&` | `GetInstance()` |
| [`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)` const&` | `GetInstance()` |
| `VkResult` | `GetLastErrorCode()` |
| `VmaAllocator` | `GetMemoryAllocator()` |
| `VkPhysicalDevice` | `GetPhysicalDevice()` |
| [`Vk::PhysicalDevice`](documentation/generated/VulkanRenderer/Vk.PhysicalDevice.md)` const&` | `GetPhysicalDeviceInfo()` |
| `PFN_vkVoidFunction` | `GetProcAddr(char const* name, bool allowInstanceFallback)` |
| `Nz::Vk::QueueHandle` | `GetQueue(Nz::UInt32 queueFamilyIndex, Nz::UInt32 queueIndex)` |
| `bool` | `IsExtensionLoaded(std::string const& extensionName)` |
| `bool` | `IsLayerLoaded(std::string const& layerName)` |
| `void` | `SetDebugName(VkObjectType objectType, Nz::UInt64 objectHandle, char const* name)` |
| `void` | `SetDebugName(VkObjectType objectType, Nz::UInt64 objectHandle, std::string_view name)` |
| `void` | `SetDebugName(VkObjectType objectType, Nz::UInt64 objectHandle, std::string const& name)` |
| `bool` | `WaitForIdle()` |
| Vk::Device`&` | `operator=(`Vk::Device` const&)` |
| Vk::Device`&` | `operator=(`Vk::Device`&&)` |
