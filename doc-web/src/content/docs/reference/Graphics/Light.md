---
title: Light
description: Nothing
---

# Nz::Light

Class description

## Constructors

- `Light(Nz::UInt8 lightType)`
- `Light(`Light` const&)`
- `Light(`Light`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `ComputeContributionScore(Nz::BoundingVolumef const& boundingVolume)` |
| `void` | `EnableShadowCasting(bool castShadows)` |
| `void` | `FillLightData(void* data)` |
| `Nz::BoundingVolumef const&` | `GetBoundingVolume()` |
| `Nz::UInt8` | `GetLightType()` |
| `Nz::PixelFormat` | `GetShadowMapFormat()` |
| `Nz::UInt32` | `GetShadowMapSize()` |
| `std::unique_ptr<`[`LightShadowData`](documentation/generated/Graphics/LightShadowData.md)`>` | `InstanciateShadowData(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& pipeline, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry)` |
| `bool` | `IsShadowCaster()` |
| `void` | `UpdateShadowMapFormat(Nz::PixelFormat format)` |
| `void` | `UpdateShadowMapSettings(Nz::UInt32 size, Nz::PixelFormat format)` |
| `void` | `UpdateShadowMapSize(Nz::UInt32 size)` |
| `void` | `UpdateTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)` |
| Light`&` | `operator=(`Light` const&)` |
| Light`&` | `operator=(`Light`&&)` |
