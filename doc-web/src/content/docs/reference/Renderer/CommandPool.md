---
title: CommandPool
description: Nothing
---

# Nz::CommandPool

Class description

## Constructors

- `CommandPool()`
- `CommandPool(`CommandPool` const&)`
- `CommandPool(`CommandPool`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::CommandBufferPtr` | `BuildCommandBuffer(std::function<void (CommandBufferBuilder &)> const& callback)` |
| `void` | `UpdateDebugName(std::string_view name)` |
| CommandPool`&` | `operator=(`CommandPool` const&)` |
| CommandPool`&` | `operator=(`CommandPool`&&)` |
