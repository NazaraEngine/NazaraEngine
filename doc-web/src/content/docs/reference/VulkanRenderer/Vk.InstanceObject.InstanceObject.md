---
title: Vk.InstanceObject.InstanceObject
description: Nothing
---

# Nz::Vk::InstanceObject::InstanceObject

Class description

## Constructors

- `InstanceObject()`
- `InstanceObject(const InstanceObject<C, VkType, CreateInfo, ObjectType>&)`
- `InstanceObject(InstanceObject<C, VkType, CreateInfo, ObjectType>&& object)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)`& instance, CreateInfo const& createInfo, VkAllocationCallbacks const* allocator)` |
| `void` | `Destroy()` |
| `bool` | `IsValid()` |
| [`Vk::Instance`](documentation/generated/VulkanRenderer/Vk.Instance.md)`*` | `GetInstance()` |
| `VkResult` | `GetLastErrorCode()` |
| `InstanceObject<C, VkType, CreateInfo, ObjectType>&` | `operator=(const InstanceObject<C, VkType, CreateInfo, ObjectType>&)` |
| `InstanceObject<C, VkType, CreateInfo, ObjectType>&` | `operator=(InstanceObject<C, VkType, CreateInfo, ObjectType>&& object)` |
