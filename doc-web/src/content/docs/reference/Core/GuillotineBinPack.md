---
title: GuillotineBinPack
description: Nothing
---

# Nz::GuillotineBinPack

Class description

## Constructors

- `GuillotineBinPack()`
- `GuillotineBinPack(unsigned int width, unsigned int height)`
- `GuillotineBinPack(Nz::Vector2ui const& size)`
- `GuillotineBinPack(`GuillotineBinPack` const&)`
- `GuillotineBinPack(`GuillotineBinPack`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `void` | `Expand(unsigned int newWidth, unsigned int newHeight)` |
| `void` | `Expand(Nz::Vector2ui const& newSize)` |
| `void` | `FreeRectangle(Nz::Rectui const& rect)` |
| `unsigned int` | `GetHeight()` |
| `float` | `GetOccupancy()` |
| `Nz::Vector2ui` | `GetSize()` |
| `unsigned int` | `GetWidth()` |
| `bool` | `Insert(Nz::Rectui* rects, unsigned int count, bool merge, Nz::GuillotineBinPack::FreeRectChoiceHeuristic rectChoice, Nz::GuillotineBinPack::GuillotineSplitHeuristic splitMethod)` |
| `bool` | `Insert(Nz::Rectui* rects, bool* flipped, unsigned int count, bool merge, Nz::GuillotineBinPack::FreeRectChoiceHeuristic rectChoice, Nz::GuillotineBinPack::GuillotineSplitHeuristic splitMethod)` |
| `bool` | `Insert(Nz::Rectui* rects, bool* flipped, bool* inserted, unsigned int count, bool merge, Nz::GuillotineBinPack::FreeRectChoiceHeuristic rectChoice, Nz::GuillotineBinPack::GuillotineSplitHeuristic splitMethod)` |
| `bool` | `MergeFreeRectangles()` |
| `void` | `Reset()` |
| `void` | `Reset(unsigned int width, unsigned int height)` |
| `void` | `Reset(Nz::Vector2ui const& size)` |
| GuillotineBinPack`&` | `operator=(`GuillotineBinPack` const&)` |
| GuillotineBinPack`&` | `operator=(`GuillotineBinPack`&&)` |
