---
title: ViewerInstance
description: Nothing
---

# Nz::ViewerInstance

Class description

## Constructors

- `ViewerInstance()`
- `ViewerInstance(`ViewerInstance` const&)`
- `ViewerInstance(`ViewerInstance`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::Vector3f const&` | `GetEyePosition()` |
| `Nz::Matrix4f const&` | `GetInvProjectionMatrix()` |
| `Nz::Matrix4f const&` | `GetInvViewMatrix()` |
| `Nz::Matrix4f const&` | `GetInvViewProjMatrix()` |
| `Nz::Matrix4f const&` | `GetProjectionMatrix()` |
| `Nz::Vector2f const&` | `GetTargetSize()` |
| `Nz::Matrix4f const&` | `GetViewMatrix()` |
| `Nz::Matrix4f const&` | `GetViewProjMatrix()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`>&` | `GetViewerBuffer()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetViewerBuffer()` |
| `void` | `OnTransfer(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& builder)` |
| `void` | `UpdateEyePosition(Nz::Vector3f const& eyePosition)` |
| `void` | `UpdateProjectionMatrix(Nz::Matrix4f const& projectionMatrix)` |
| `void` | `UpdateProjectionMatrix(Nz::Matrix4f const& projectionMatrix, Nz::Matrix4f const& invProjectionMatrix)` |
| `void` | `UpdateProjViewMatrices(Nz::Matrix4f const& projectionMatrix, Nz::Matrix4f const& viewMatrix)` |
| `void` | `UpdateProjViewMatrices(Nz::Matrix4f const& projectionMatrix, Nz::Matrix4f const& invProjectionMatrix, Nz::Matrix4f const& viewMatrix, Nz::Matrix4f const& invViewMatrix)` |
| `void` | `UpdateProjViewMatrices(Nz::Matrix4f const& projectionMatrix, Nz::Matrix4f const& invProjectionMatrix, Nz::Matrix4f const& viewMatrix, Nz::Matrix4f const& invViewMatrix, Nz::Matrix4f const& viewProjMatrix, Nz::Matrix4f const& invViewProjMatrix)` |
| `void` | `UpdateTargetSize(Nz::Vector2f const& targetSize)` |
| `void` | `UpdateViewMatrix(Nz::Matrix4f const& viewMatrix)` |
| `void` | `UpdateViewMatrix(Nz::Matrix4f const& viewMatrix, Nz::Matrix4f const& invViewMatrix)` |
| ViewerInstance`&` | `operator=(`ViewerInstance` const&)` |
| ViewerInstance`&` | `operator=(`ViewerInstance`&&)` |
