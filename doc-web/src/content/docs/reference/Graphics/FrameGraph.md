---
title: FrameGraph
description: Nothing
---

# Nz::FrameGraph

Class description

## Constructors

- `FrameGraph()`
- `FrameGraph(`FrameGraph` const&)`
- `FrameGraph(`FrameGraph`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `AddAttachment(`[`FramePassAttachment`](documentation/generated/Graphics/FramePassAttachment.md)` attachment)` |
| `std::size_t` | `AddAttachmentCube(`[`FramePassAttachment`](documentation/generated/Graphics/FramePassAttachment.md)` attachment)` |
| `std::size_t` | `AddAttachmentCubeFace(std::size_t attachmentId, Nz::CubemapFace face)` |
| `std::size_t` | `AddAttachmentProxy(std::string name, std::size_t attachmentId)` |
| `void` | `AddBackbufferOutput(std::size_t backbufferOutput)` |
| [`FramePass`](documentation/generated/Graphics/FramePass.md)`&` | `AddPass(std::string name)` |
| `Nz::BakedFrameGraph` | `Bake()` |
| FrameGraph`&` | `operator=(`FrameGraph` const&)` |
| FrameGraph`&` | `operator=(`FrameGraph`&&)` |
