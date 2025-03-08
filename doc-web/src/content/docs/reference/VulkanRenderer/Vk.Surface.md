---
title: Vk.Surface
description: Nothing
---

# Nz::Vk::Surface

Class description

## Constructors

- `Surface(`[`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)`& instance)`
- `Surface(`Vk::Surface` const&)`
- `Surface(`Vk::Surface`&& surface)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(VkXlibSurfaceCreateInfoKHR const& createInfo, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(Display* display, `[`Window`](documentation/generated/Platform/Window.md)` window, VkXlibSurfaceCreateFlagsKHR flags, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(VkWaylandSurfaceCreateInfoKHR const& createInfo, VkAllocationCallbacks const* allocator)` |
| `bool` | `Create(wl_display* display, wl_surface* surface, VkWaylandSurfaceCreateFlagsKHR flags, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| `bool` | `GetCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceCapabilitiesKHR* surfaceCapabilities)` |
| `bool` | `GetFormats(VkPhysicalDevice physicalDevice, std::vector<VkSurfaceFormatKHR>* surfaceFormats)` |
| `bool` | `GetPresentModes(VkPhysicalDevice physicalDevice, std::vector<VkPresentModeKHR>* presentModes)` |
| `bool` | `GetSupportPresentation(VkPhysicalDevice physicalDevice, Nz::UInt32 queueFamilyIndex, bool* supported)` |
| `VkResult` | `GetLastErrorCode()` |
| Vk::Surface`&` | `operator=(`Vk::Surface` const&)` |
| Vk::Surface`&` | `operator=(`Vk::Surface`&&)` |
