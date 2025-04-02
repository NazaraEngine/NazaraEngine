---
title: ShaderReflection
description: Nothing
---

# Nz::ShaderReflection

Class description

## Constructors

- `ShaderReflection()`
- `ShaderReflection(`ShaderReflection` const&)`
- `ShaderReflection(`ShaderReflection`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`RenderPipelineLayoutInfo`](documentation/generated/Renderer/RenderPipelineLayoutInfo.md)` const&` | `GetPipelineLayoutInfo()` |
| [`ShaderReflection::ExternalBlockData`](documentation/generated/Graphics/ShaderReflection.ExternalBlockData.md)` const*` | `GetExternalBlockByTag(std::string const& tag)` |
| [`ShaderReflection::OptionData`](documentation/generated/Graphics/ShaderReflection.OptionData.md)` const*` | `GetOptionByName(std::string const& optionName)` |
| [`ShaderReflection::StructData`](documentation/generated/Graphics/ShaderReflection.StructData.md)` const*` | `GetStructByIndex(std::size_t structIndex)` |
| `void` | `Reflect(nzsl::Ast::Module& module)` |
| ShaderReflection`&` | `operator=(`ShaderReflection` const&)` |
| ShaderReflection`&` | `operator=(`ShaderReflection`&&)` |
