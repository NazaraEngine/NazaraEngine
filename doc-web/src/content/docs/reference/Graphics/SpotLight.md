---
title: SpotLight
description: Nothing
---

# Nz::SpotLight

Class description

## Constructors

- `SpotLight()`
- `SpotLight(`SpotLight` const&)`
- `SpotLight(`SpotLight`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `float` | `ComputeContributionScore(Nz::BoundingVolumef const& boundingVolume)` |
| `void` | `FillLightData(void* data)` |
| `float` | `GetAmbientFactor()` |
| `float` | `GetDiffuseFactor()` |
| `Nz::Color` | `GetColor()` |
| `Nz::Vector3f const&` | `GetDirection()` |
| `Nz::RadianAnglef` | `GetInnerAngle()` |
| `Nz::RadianAnglef` | `GetOuterAngle()` |
| `Nz::Vector3f const&` | `GetPosition()` |
| `Nz::Quaternionf const&` | `GetRotation()` |
| `float` | `GetRadius()` |
| `std::unique_ptr<`[`LightShadowData`](documentation/generated/Graphics/LightShadowData.md)`>` | `InstanciateShadowData(`[`FramePipeline`](documentation/generated/Graphics/FramePipeline.md)`& pipeline, `[`ElementRendererRegistry`](documentation/generated/Graphics/ElementRendererRegistry.md)`& elementRegistry)` |
| `void` | `UpdateAmbientFactor(float factor)` |
| `void` | `UpdateColor(`[`Color`](documentation/generated/Core/Color.md)` color)` |
| `void` | `UpdateDiffuseFactor(float factor)` |
| `void` | `UpdateDirection(Nz::Vector3f const& direction)` |
| `void` | `UpdateInnerAngle(Nz::RadianAnglef innerAngle)` |
| `void` | `UpdateOuterAngle(Nz::RadianAnglef outerAngle)` |
| `void` | `UpdatePosition(Nz::Vector3f const& position)` |
| `void` | `UpdateRadius(float radius)` |
| `void` | `UpdateRotation(Nz::Quaternionf const& rotation)` |
| `void` | `UpdateTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)` |
| SpotLight`&` | `operator=(`SpotLight` const&)` |
| SpotLight`&` | `operator=(`SpotLight`&&)` |
