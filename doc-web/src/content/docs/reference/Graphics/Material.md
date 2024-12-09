---
title: Material
description: Nothing
---

# Nz::Material

Class description

## Constructors

- `Material(`[`MaterialSettings`](documentation/generated/Graphics/MaterialSettings.md)` settings, std::string const& referenceModuleName)`
- `Material(`[`MaterialSettings`](documentation/generated/Graphics/MaterialSettings.md)` settings, nzsl::Ast::ModulePtr const& referenceModule)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`>` | `GetDefaultInstance()` |
| `std::size_t` | `FindTextureByTag(std::string const& tag)` |
| `std::size_t` | `FindUniformBlockByTag(std::string const& tag)` |
| `Nz::UInt32` | `GetEngineBindingIndex(Nz::EngineShaderBinding shaderBinding)` |
| `std::shared_ptr<`[`RenderPipelineLayout`](documentation/generated/Renderer/RenderPipelineLayout.md)`> const&` | `GetRenderPipelineLayout()` |
| [`MaterialSettings`](documentation/generated/Graphics/MaterialSettings.md)` const&` | `GetSettings()` |
| [`Material::TextureData`](documentation/generated/Graphics/Material.TextureData.md)` const&` | `GetTextureData(std::size_t textureIndex)` |
| `std::size_t` | `GetTextureCount()` |
| [`Material::UniformBlockData`](documentation/generated/Graphics/Material.UniformBlockData.md)` const&` | `GetUniformBlockData(std::size_t uniformBlockIndex)` |
| `std::size_t` | `GetUniformBlockCount()` |
| `std::shared_ptr<`[`MaterialInstance`](documentation/generated/Graphics/MaterialInstance.md)`>` | `Instantiate()` |
