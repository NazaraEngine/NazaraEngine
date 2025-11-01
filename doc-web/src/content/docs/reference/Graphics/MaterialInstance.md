---
title: MaterialInstance
description: Nothing
---

# Nz::MaterialInstance

Class description

## Constructors

- `MaterialInstance(std::shared_ptr<const `[`Material`](documentation/generated/Graphics/Material.md)`> parent)`
- `MaterialInstance(`MaterialInstance` const&)`
- `MaterialInstance(`MaterialInstance` const& material, `[`MaterialInstance::CopyToken`](documentation/generated/Graphics/MaterialInstance.CopyToken.md)`)`
- `MaterialInstance(`MaterialInstance`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `std::shared_ptr<`MaterialInstance`>` | `Clone()` |
| `void` | `DisablePass(std::string_view passName)` |
| `void` | `DisablePass(std::size_t passIndex)` |
| `void` | `EnablePass(std::string_view passName, bool enable)` |
| `void` | `EnablePass(std::size_t passIndex, bool enable)` |
| `void` | `FillShaderBinding(std::vector<`[`ShaderBinding::Binding`](documentation/generated/Renderer/ShaderBinding.Binding.md)`>& bindings)` |
| `std::size_t` | `FindTextureProperty(std::string_view propertyName)` |
| `std::size_t` | `FindValueProperty(std::string_view propertyName)` |
| `std::shared_ptr<const `[`Material`](documentation/generated/Graphics/Material.md)`> const&` | `GetParentMaterial()` |
| `Nz::MaterialPassFlags` | `GetPassFlags(std::size_t passIndex)` |
| `std::shared_ptr<`[`MaterialPipeline`](documentation/generated/Graphics/MaterialPipeline.md)`> const&` | `GetPipeline(std::size_t passIndex)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> const*` | `GetTextureProperty(std::string_view propertyName)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> const&` | `GetTextureProperty(std::size_t textureIndex)` |
| `std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> const&` | `GetTexturePropertyOverride(std::size_t textureIndex)` |
| [`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const*` | `GetTextureSamplerProperty(std::string_view propertyName)` |
| [`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const&` | `GetTextureSamplerProperty(std::size_t textureIndex)` |
| `MaterialSettings::Value const*` | `GetValueProperty(std::string_view propertyName)` |
| `MaterialSettings::Value const&` | `GetValueProperty(std::size_t valueIndex)` |
| `MaterialSettings::Value const&` | `GetValuePropertyOverride(std::size_t valueIndex)` |
| `bool` | `HasPass(std::string_view passName)` |
| `bool` | `HasPass(std::size_t passIndex)` |
| `void` | `OnTransfer(`[`RenderFrame`](documentation/generated/Renderer/RenderFrame.md)`& renderFrame, `[`CommandBufferBuilder`](documentation/generated/Renderer/CommandBufferBuilder.md)`& builder)` |
| `void` | `SetTextureProperty(std::string_view propertyName, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> texture)` |
| `void` | `SetTextureProperty(std::string_view propertyName, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> texture, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& samplerInfo)` |
| `void` | `SetTextureProperty(std::size_t textureIndex, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> texture)` |
| `void` | `SetTextureProperty(std::size_t textureIndex, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> texture, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& samplerInfo)` |
| `void` | `SetTextureSamplerProperty(std::string_view propertyName, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& samplerInfo)` |
| `void` | `SetTextureSamplerProperty(std::size_t textureIndex, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& samplerInfo)` |
| `void` | `SetValueProperty(std::string_view propertyName, MaterialSettings::Value const& value)` |
| `void` | `SetValueProperty(std::size_t valueIndex, MaterialSettings::Value const& value)` |
| `void` | `UpdateOptionValue(Nz::UInt32 optionHash, nzsl::Ast::ConstantSingleValue const& value)` |
| `void` | `UpdatePassFlags(std::string_view passName, Nz::MaterialPassFlags materialFlags)` |
| `void` | `UpdatePassFlags(std::size_t passName, Nz::MaterialPassFlags materialFlags)` |
| `void` | `UpdatePassStates(std::string_view passName, FunctionRef<bool (Nz::RenderStates &)> stateUpdater)` |
| `void` | `UpdatePassesStates(std::initializer_list<std::string_view> passesName, FunctionRef<bool (Nz::RenderStates &)> stateUpdater)` |
| `void` | `UpdateTextureBinding(std::size_t textureBinding, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> texture, std::shared_ptr<`[`TextureSampler`](documentation/generated/Renderer/TextureSampler.md)`> textureSampler)` |
| `void` | `UpdateUniformBufferData(std::size_t uniformBufferIndex, std::size_t offset, std::size_t size, void const* data)` |
| MaterialInstance`&` | `operator=(`MaterialInstance` const&)` |
| MaterialInstance`&` | `operator=(`MaterialInstance`&&)` |
