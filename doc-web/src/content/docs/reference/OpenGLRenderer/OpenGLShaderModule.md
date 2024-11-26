---
title: OpenGLShaderModule
description: Nothing
---

# Nz::OpenGLShaderModule

Class description

## Constructors

- `OpenGLShaderModule(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::Module const& shaderModule, nzsl::ShaderWriter::States const& states)`
- `OpenGLShaderModule(`[`OpenGLDevice`](documentation/generated/OpenGLRenderer/OpenGLDevice.md)`& device, nzsl::ShaderStageTypeFlags shaderStages, Nz::ShaderLanguage lang, void const* source, std::size_t sourceSize, nzsl::ShaderWriter::States const& states)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `nzsl::ShaderStageTypeFlags` | `Attach(`[`GL::Program`](documentation/generated/OpenGLRenderer/GL.Program.md)`& program, nzsl::GlslWriter::BindingMapping const& bindingMapping, std::vector<ExplicitBinding>* explicitBindings)` |
| `std::vector<ExplicitBinding> const&` | `GetExplicitBindings()` |
| `void` | `UpdateDebugName(std::string_view name)` |
