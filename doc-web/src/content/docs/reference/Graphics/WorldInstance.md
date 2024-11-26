---
title: WorldInstance
description: Nothing
---

# Nz::WorldInstance

Class description

## Constructors

- `WorldInstance()`
- `WorldInstance(`WorldInstance` const&)`
- `WorldInstance(`WorldInstance`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`>&` | `GetInstanceBuffer()` |
| `std::shared_ptr<`[`RenderBuffer`](documentation/generated/Renderer/RenderBuffer.md)`> const&` | `GetInstanceBuffer()` |
| `Nz::Matrix4f const&` | `GetInvWorldMatrix()` |
| `Nz::Matrix4f const&` | `GetWorldMatrix()` |
| `void` | `OnTransfer(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& builder)` |
| `void` | `UpdateWorldMatrix(Nz::Matrix4f const& worldMatrix)` |
| `void` | `UpdateWorldMatrix(Nz::Matrix4f const& worldMatrix, Nz::Matrix4f const& invWorldMatrix)` |
| WorldInstance`&` | `operator=(`WorldInstance` const&)` |
| WorldInstance`&` | `operator=(`WorldInstance`&&)` |
