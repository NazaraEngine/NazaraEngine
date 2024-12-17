---
title: VulkanRenderer
description: Nothing
---

# Nz::VulkanRenderer

Class description

## Constructors

- `VulkanRenderer()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`>` | `InstanciateRenderDevice(std::size_t deviceIndex, `[`RenderDeviceFeatures`](documentation/generated/Renderer/RenderDeviceFeatures.md)` const& enabledFeatures)` |
| `Nz::RenderAPI` | `QueryAPI()` |
| `std::string` | `QueryAPIString()` |
| `Nz::UInt32` | `QueryAPIVersion()` |
| `std::vector<`[`RenderDeviceInfo`](documentation/generated/Renderer/RenderDeviceInfo.md)`> const&` | `QueryRenderDevices()` |
| `bool` | `Prepare(`[`Renderer::Config`](documentation/generated/Renderer/Renderer.Config.md)` const& parameters)` |
