---
title: MaterialSettings
description: Nothing
---

# Nz::MaterialSettings

Class description

## Constructors

- `MaterialSettings()`
- `MaterialSettings(`MaterialSettings` const&)`
- `MaterialSettings(`MaterialSettings`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddPass(std::string_view passName, `[`MaterialPass`](documentation/generated/Graphics/MaterialPass.md)` materialPass)` |
| `void` | `AddPass(std::size_t passIndex, `[`MaterialPass`](documentation/generated/Graphics/MaterialPass.md)` materialPass)` |
| `void` | `AddPropertyHandler(std::unique_ptr<`[`PropertyHandler`](documentation/generated/Graphics/PropertyHandler.md)`> propertyHandler)` |
| `void` | `AddTextureProperty(std::string propertyName, Nz::ImageType propertyType)` |
| `void` | `AddTextureProperty(std::string propertyName, Nz::ImageType propertyType, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> defaultTexture)` |
| `void` | `AddTextureProperty(std::string propertyName, Nz::ImageType propertyType, std::shared_ptr<`[`Texture`](documentation/generated/Renderer/Texture.md)`> defaultTexture, `[`TextureSamplerInfo`](documentation/generated/Renderer/TextureSamplerInfo.md)` const& defaultSamplerInfo)` |
| `void` | `AddValueProperty(std::string propertyName, Nz::MaterialPropertyType propertyType, Nz::MaterialSettings::Value defaultValue)` |
| `std::size_t` | `FindTextureProperty(std::string_view propertyName)` |
| `std::size_t` | `FindValueProperty(std::string_view propertyName)` |
| [`MaterialPass`](documentation/generated/Graphics/MaterialPass.md)` const*` | `GetPass(std::string_view passName)` |
| [`MaterialPass`](documentation/generated/Graphics/MaterialPass.md)` const*` | `GetPass(std::size_t passIndex)` |
| `std::vector<std::optional<`[`MaterialPass`](documentation/generated/Graphics/MaterialPass.md)`>> const&` | `GetPasses()` |
| `std::vector<std::unique_ptr<`[`PropertyHandler`](documentation/generated/Graphics/PropertyHandler.md)`>> const&` | `GetPropertyHandlers()` |
| [`MaterialSettings::TextureProperty`](documentation/generated/Graphics/MaterialSettings.TextureProperty.md)` const&` | `GetTextureProperty(std::size_t texturePropertyIndex)` |
| `std::size_t` | `GetTexturePropertyCount()` |
| [`MaterialSettings::ValueProperty`](documentation/generated/Graphics/MaterialSettings.ValueProperty.md)` const&` | `GetValueProperty(std::size_t valuePropertyIndex)` |
| `std::size_t` | `GetValuePropertyCount()` |
| MaterialSettings`&` | `operator=(`MaterialSettings` const&)` |
| MaterialSettings`&` | `operator=(`MaterialSettings`&&)` |
