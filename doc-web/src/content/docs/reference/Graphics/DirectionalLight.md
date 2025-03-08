---
title: DirectionalLight
description: Nothing
---

# Nz::DirectionalLight

Class description

## Constructors

- `DirectionalLight()`
- `DirectionalLight(`DirectionalLight` const&)`
- `DirectionalLight(`DirectionalLight`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `ComputeContributionScore(Nz::BoundingVolumef const& boundingVolume)` |
| `void` | `FillLightData(void* data)` |
| `std::unique_ptr<`[`LightShadowData`](documentation/generated/Graphics/LightShadowData.md)`>` | `InstanciateShadowData(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& pipeline, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry)` |
| `float` | `GetAmbientFactor()` |
| `float` | `GetDiffuseFactor()` |
| `Nz::Color` | `GetColor()` |
| `Nz::Vector3f const&` | `GetDirection()` |
| `Nz::Quaternionf const&` | `GetRotation()` |
| `void` | `UpdateAmbientFactor(float factor)` |
| `void` | `UpdateColor(`[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `UpdateDiffuseFactor(float factor)` |
| `void` | `UpdateDirection(Nz::Vector3f const& direction)` |
| `void` | `UpdateRotation(Nz::Quaternionf const& rotation)` |
| `void` | `UpdateTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)` |
| DirectionalLight`&` | `operator=(`DirectionalLight` const&)` |
| DirectionalLight`&` | `operator=(`DirectionalLight`&&)` |
