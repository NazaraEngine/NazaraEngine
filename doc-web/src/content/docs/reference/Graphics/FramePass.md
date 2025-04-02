---
title: FramePass
description: Nothing
---

# Nz::FramePass

Class description

## Constructors

- `FramePass(`[`FrameGraph`](documentation/generated/Graphics/FrameGraph.md)`& owner, std::size_t passId, std::string name)`
- `FramePass(`FramePass` const&)`
- `FramePass(`FramePass`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::size_t` | `AddInput(std::size_t attachmentId)` |
| `std::size_t` | `AddOutput(std::size_t attachmentId)` |
| `CommandCallback&` | `GetCommandCallback()` |
| `std::optional<DepthStencilClear> const&` | `GetDepthStencilClear()` |
| `std::size_t` | `GetDepthStencilInput()` |
| `std::size_t` | `GetDepthStencilOutput()` |
| `ExecutionCallback&` | `GetExecutionCallback()` |
| `std::vector<Input> const&` | `GetInputs()` |
| `std::string const&` | `GetName()` |
| `std::vector<Output> const&` | `GetOutputs()` |
| `std::size_t` | `GetPassId()` |
| `void` | `SetCommandCallback(Nz::FramePass::CommandCallback callback)` |
| `void` | `SetClearColor(std::size_t outputIndex, std::optional<`[`Color`](documentation/generated/Core/Color.md)`> const& color)` |
| `void` | `SetDepthStencilClear(float depth, Nz::UInt32 stencil)` |
| `void` | `SetDepthStencilInput(std::size_t attachmentId)` |
| `void` | `SetDepthStencilOutput(std::size_t attachmentId)` |
| `void` | `SetExecutionCallback(Nz::FramePass::ExecutionCallback callback)` |
| `void` | `SetInputLayout(std::size_t inputIndex, Nz::TextureLayout layout)` |
| `void` | `SetReadInput(std::size_t inputIndex, bool doesRead)` |
| FramePass`&` | `operator=(`FramePass` const&)` |
| FramePass`&` | `operator=(`FramePass`&&)` |
