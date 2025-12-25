---
title: Graphics
description: Nothing
---

# Nz::Graphics

Class description

## Constructors

- `Graphics(`[`Graphics::Config`](documentation/generated/Graphics/Graphics.Config.md)` config)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`RenderPipeline`](documentation/generated/Renderer/RenderPipeline.md)`> const&` | `GetBlitPipeline(bool transparent)` |
| `std::shared_ptr<`[`RenderPipelineLayout`](documentation/generated/Renderer/RenderPipelineLayout.md)`> const&` | `GetBlitPipelineLayout()` |
| [`Graphics::DefaultMaterials`](documentation/generated/Graphics/Graphics.DefaultMaterials.md)` const&` | `GetDefaultMaterials()` |
| [`Graphics::DefaultTextures`](documentation/generated/Graphics/Graphics.DefaultTextures.md)` const&` | `GetDefaultTextures()` |
| [`MaterialPassRegistry`](documentation/generated/Graphics/MaterialPassRegistry.md)`&` | `GetMaterialPassRegistry()` |
| [`MaterialPassRegistry`](documentation/generated/Graphics/MaterialPassRegistry.md)` const&` | `GetMaterialPassRegistry()` |
| `Nz::MaterialInstanceLoader&` | `GetMaterialInstanceLoader()` |
| `Nz::MaterialInstanceLoader const&` | `GetMaterialInstanceLoader()` |
| `Nz::MaterialLoader&` | `GetMaterialLoader()` |
| `Nz::MaterialLoader const&` | `GetMaterialLoader()` |
| `Nz::PixelFormat` | `GetPreferredDepthFormat()` |
| `Nz::PixelFormat` | `GetPreferredDepthStencilFormat()` |
| `std::shared_ptr<`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`> const&` | `GetRenderDevice()` |
| [`RenderPassCache`](documentation/generated/Renderer/RenderPassCache.md)` const&` | `GetRenderPassCache()` |
| [`TextureSamplerCache`](documentation/generated/Graphics/TextureSamplerCache.md)`&` | `GetSamplerCache()` |
| `std::shared_ptr<nzsl::FilesystemModuleResolver>&` | `GetShaderModuleResolver()` |
| `std::shared_ptr<nzsl::FilesystemModuleResolver> const&` | `GetShaderModuleResolver()` |
| `void` | `RegisterComponent(`[`AppFilesystemComponent`](documentation/generated/Core/AppFilesystemComponent.md)`& component)` |
