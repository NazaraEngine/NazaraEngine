---
title: ElementRendererRegistry
description: Nothing
---

# Nz::ElementRendererRegistry

Class description

## Constructors

- `ElementRendererRegistry()`
- `ElementRendererRegistry(`ElementRendererRegistry` const&)`
- `ElementRendererRegistry(`ElementRendererRegistry`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`ElementRenderer`](documentation/generated/Graphics/ElementRenderer.md)`&` | `GetElementRenderer(std::size_t elementIndex)` |
| `std::size_t` | `GetElementRendererCount()` |
| `void` | `RegisterElementRenderer(std::size_t elementIndex, std::unique_ptr<`[`ElementRenderer`](documentation/generated/Graphics/ElementRenderer.md)`> renderer)` |
| ElementRendererRegistry`&` | `operator=(`ElementRendererRegistry` const&)` |
| ElementRendererRegistry`&` | `operator=(`ElementRendererRegistry`&&)` |
