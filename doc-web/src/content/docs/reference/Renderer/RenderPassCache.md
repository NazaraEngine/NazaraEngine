---
title: RenderPassCache
description: Nothing
---

# Nz::RenderPassCache

Class description

## Constructors

- `RenderPassCache(`[`RenderDevice`](documentation/generated/Renderer/RenderDevice.md)`& device)`
- `RenderPassCache(`RenderPassCache` const&)`
- `RenderPassCache(`RenderPassCache`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`RenderPass`](documentation/generated/Renderer/RenderPass.md)`> const&` | `Get(std::vector<`[`RenderPass::Attachment`](documentation/generated/Renderer/RenderPass.Attachment.md)`> const& attachments, std::vector<`[`RenderPass::SubpassDescription`](documentation/generated/Renderer/RenderPass.SubpassDescription.md)`> const& subpassDescriptions, std::vector<`[`RenderPass::SubpassDependency`](documentation/generated/Renderer/RenderPass.SubpassDependency.md)`> const& subpassDependencies)` |
| RenderPassCache`&` | `operator=(`RenderPassCache` const&)` |
| RenderPassCache`&` | `operator=(`RenderPassCache`&&)` |
