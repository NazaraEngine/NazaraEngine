---
title: Renderer
description: Nothing
---

# Nz::Renderer

Class description

## Constructors

- `Renderer(`[`Renderer::Config`](documentation/generated/Renderer/Renderer.Config.md)`)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RendererImpl`](documentation/generated/Renderer/RendererImpl.md)`*` | `GetRendererImpl()` |
| `std::shared_ptr<`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`>` | `InstanciateRenderDevice(std::size_t deviceIndex, `[`RenderDeviceFeatures`](documentation/generated/Renderer/RenderDeviceFeatures.md)` const& enabledFeatures)` |
| `Nz::RenderAPI` | `QueryAPI()` |
| `std::string` | `QueryAPIString()` |
| `Nz::UInt32` | `QueryAPIVersion()` |
| `std::vector<`[`RenderDeviceInfo`](documentation/generated/Renderer/RenderDeviceInfo.md)`> const&` | `QueryRenderDevices()` |
