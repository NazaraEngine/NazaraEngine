---
title: DebugDrawer
description: Nothing
---

# Nz::DebugDrawer

Class description

## Constructors

- `DebugDrawer(`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`& renderDevice, std::size_t maxVertexPerDraw)`
- `DebugDrawer(`DebugDrawer` const&)`
- `DebugDrawer(`DebugDrawer`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Draw(`[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& builder)` |
| `void` | `DrawBox(Nz::Boxf const& box, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `DrawFrustum(Nz::Frustumf const& frustum, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `DrawLine(Nz::Vector3f const& start, Nz::Vector3f const& end, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `DrawLine(Nz::Vector3f const& start, Nz::Vector3f const& end, `[`Color`](documentation/generated/Core/Color.md)` const& startColor, `[`Color`](documentation/generated/Core/Color.md)` const& endColor)` |
| `void` | `DrawSkeleton(`[`Skeleton`](documentation/generated/Utility/Skeleton.md)` const& skeleton, `[`Color`](documentation/generated/Core/Color.md)` const& color)` |
| `void` | `Prepare(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `void` | `Reset(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame)` |
| `void` | `SetViewerData(Nz::Matrix4f const& viewProjMatrix)` |
| DebugDrawer`&` | `operator=(`DebugDrawer` const&)` |
| DebugDrawer`&` | `operator=(`DebugDrawer`&&)` |
