---
title: Vk.DeviceObject.DeviceObject
description: Nothing
---

# Nz::Vk::DeviceObject::DeviceObject

Class description

## Constructors

- `DeviceObject()`
- `DeviceObject(const DeviceObject<C, VkType, CreateInfo, ObjectType>&)`
- `DeviceObject(DeviceObject<C, VkType, CreateInfo, ObjectType>&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, CreateInfo const& createInfo, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| `bool` | `IsValid()` |
| [`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`*` | `GetDevice()` |
| `VkResult` | `GetLastErrorCode()` |
| `DeviceObject<C, VkType, CreateInfo, ObjectType>&` | `operator=(const DeviceObject<C, VkType, CreateInfo, ObjectType>&)` |
| `DeviceObject<C, VkType, CreateInfo, ObjectType>&` | `operator=(DeviceObject<C, VkType, CreateInfo, ObjectType>&& object)` |
