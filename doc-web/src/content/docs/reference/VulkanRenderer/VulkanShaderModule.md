---
title: VulkanShaderModule
description: Nothing
---

# Nz::VulkanShaderModule

Class description

## Constructors


## Methods

| Return type | Signature |
| ----------- | --------- |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::Module const& shaderModule, nzsl::ShaderWriter::States const& states)` |
| `bool` | `Create(`[`Vk::Device`](documentation/generated/VulkanRenderer/Vk.Device.md)`& device, nzsl::ShaderStageTypeFlags shaderStages, Nz::ShaderLanguage lang, void const* source, std::size_t sourceSize, nzsl::ShaderWriter::States const& states)` |
| [`Vk::ShaderModule`](documentation/generated/VulkanRenderer/Vk.ShaderModule.md)` const&` | `GetHandle()` |
| `std::vector<Stage> const&` | `GetStages()` |
| `void` | `UpdateDebugName(std::string_view name)` |
