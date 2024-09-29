---
title: AbstractAtlas
description: Nothing
---

# Nz::AbstractAtlas

Class description

## Constructors

- `AbstractAtlas()`
- `AbstractAtlas(`AbstractAtlas` const&)`
- `AbstractAtlas(`AbstractAtlas`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `void` | `Free(SparsePtr<const Nz::Rectui> rects, SparsePtr<unsigned int> layers, unsigned int count)` |
| [`AbstractImage`](documentation/generated/Utility/AbstractImage.md)`*` | `GetLayer(unsigned int layerIndex)` |
| `std::size_t` | `GetLayerCount()` |
| `Nz::DataStoreFlags` | `GetStorage()` |
| `bool` | `Insert(`[`Image`](documentation/generated/Utility/Image.md)` const& image, Nz::Rectui* rect, bool* flipped, unsigned int* layerIndex)` |
| AbstractAtlas`&` | `operator=(`AbstractAtlas` const&)` |
| AbstractAtlas`&` | `operator=(`AbstractAtlas`&&)` |
