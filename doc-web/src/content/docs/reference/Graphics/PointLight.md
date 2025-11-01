---
title: PointLight
description: Nothing
---

# Nz::PointLight

Class description

## Constructors

- `PointLight()`
- `PointLight(`PointLight` const&)`
- `PointLight(`PointLight`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `ComputeContributionScore(Nz::BoundingVolumef const& boundingVolume)` |
| `void` | `FillLightData(void* data)` |
| `std::unique_ptr<`[`LightShadowData`](documentation/generated/Graphics/LightShadowData.md)`>` | `InstanciateShadowData(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& pipeline, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry)` |
| `float` | `GetAmbientFactor()` |
| `float` | `GetDiffuseFactor()` |
| `Nz::Color` | `GetColor()` |
| `Nz::Vector3f const&` | `GetPosition()` |
| `float` | `GetRadius()` |
| `void` | `UpdateAmbientFactor(float factor)` |
| `void` | `UpdateColor(`[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `UpdateDiffuseFactor(float factor)` |
| `void` | `UpdatePosition(Nz::Vector3f const& position)` |
| `void` | `UpdateRadius(float radius)` |
| `void` | `UpdateTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)` |
| PointLight`&` | `operator=(`PointLight` const&)` |
| PointLight`&` | `operator=(`PointLight`&&)` |
