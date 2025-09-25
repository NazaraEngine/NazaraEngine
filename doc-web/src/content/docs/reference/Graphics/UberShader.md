---
title: UberShader
description: Nothing
---

# Nz::UberShader

Class description

## Constructors

- `UberShader(nzsl::ShaderStageTypeFlags shaderStages, std::string moduleName)`
- `UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::ModuleResolver& moduleResolver, std::string moduleName)`
- `UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::ModulePtr shaderModule)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `nzsl::ShaderStageTypeFlags` | `GetSupportedStages()` |
| `std::shared_ptr<`[`ShaderModule`](documentation/generated/Renderer/ShaderModule.md)`> const&` | `Get(`[`UberShader::Config`](documentation/generated/Graphics/UberShader.Config.md)` const& config)` |
| `bool` | `HasOption(std::string const& optionName, Pointer<const `[`UberShader::Option`](documentation/generated/Graphics/UberShader.Option.md)`>* option)` |
| `void` | `UpdateConfig(`[`UberShader::Config`](documentation/generated/Graphics/UberShader.Config.md)`& config, std::vector<`[`RenderPipelineInfo::VertexBufferData`](documentation/generated/Renderer/RenderPipelineInfo.VertexBufferData.md)`> const& vertexBuffers)` |
| `void` | `UpdateConfigCallback(Nz::UberShader::ConfigCallback callback)` |
