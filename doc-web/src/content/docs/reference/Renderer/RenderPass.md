---
title: RenderPass
description: Nothing
---

# Nz::RenderPass

Class description

## Constructors

- `RenderPass(std::vector<Attachment> attachments, std::vector<SubpassDescription> subpassDescriptions, std::vector<SubpassDependency> subpassDependencies)`
- `RenderPass(`RenderPass` const&)`
- `RenderPass(`RenderPass`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RenderPass::Attachment`](documentation/generated/Renderer/RenderPass.Attachment.md)` const&` | `GetAttachment(std::size_t attachmentIndex)` |
| `std::size_t` | `GetAttachmentCount()` |
| `std::vector<Attachment> const&` | `GetAttachments()` |
| `std::vector<SubpassDescription> const&` | `GetSubpassDescriptions()` |
| `std::vector<SubpassDependency> const&` | `GetSubpassDependencies()` |
| `void` | `UpdateDebugName(std::string_view name)` |
| RenderPass`&` | `operator=(`RenderPass` const&)` |
| RenderPass`&` | `operator=(`RenderPass`&&)` |
