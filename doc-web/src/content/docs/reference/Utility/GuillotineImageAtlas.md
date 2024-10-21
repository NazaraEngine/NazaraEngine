---
title: GuillotineImageAtlas
description: Nothing
---

# Nz::GuillotineImageAtlas

Class description

## Constructors

- `GuillotineImageAtlas()`
- `GuillotineImageAtlas(`GuillotineImageAtlas` const&)`
- `GuillotineImageAtlas(`GuillotineImageAtlas`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `void` | `Free(SparsePtr<const Nz::Rectui> rects, SparsePtr<unsigned int> layers, unsigned int count)` |
| `unsigned int` | `GetMaxLayerSize()` |
| `GuillotineBinPack::FreeRectChoiceHeuristic` | `GetRectChoiceHeuristic()` |
| `GuillotineBinPack::GuillotineSplitHeuristic` | `GetRectSplitHeuristic()` |
| [`AbstractImage`](documentation/generated/Utility/AbstractImage.md)`*` | `GetLayer(unsigned int layerIndex)` |
| `std::size_t` | `GetLayerCount()` |
| `Nz::DataStoreFlags` | `GetStorage()` |
| `bool` | `Insert(`[`Image`](documentation/generated/Utility/Image.md)` const& image, Nz::Rectui* rect, bool* flipped, unsigned int* layerIndex)` |
| `void` | `SetMaxLayerSize(unsigned int maxLayerSize)` |
| `void` | `SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic)` |
| `void` | `SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic)` |
| GuillotineImageAtlas`&` | `operator=(`GuillotineImageAtlas` const&)` |
| GuillotineImageAtlas`&` | `operator=(`GuillotineImageAtlas`&&)` |
