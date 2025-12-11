---
title: VertexDeclaration
description: Nothing
---

# Nz::VertexDeclaration

Class description

## Constructors

- `VertexDeclaration(Nz::VertexInputRate inputRate, std::initializer_list<ComponentEntry> components)`
- `VertexDeclaration(`VertexDeclaration` const&)`
- `VertexDeclaration(`VertexDeclaration`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| [`VertexDeclaration::Component`](documentation/generated/Utility/VertexDeclaration.Component.md)` const*` | `FindComponent(Nz::VertexComponent vertexComponent, std::size_t componentIndex)` |
| [`VertexDeclaration::Component`](documentation/generated/Utility/VertexDeclaration.Component.md)` const&` | `GetComponent(std::size_t componentIndex)` |
| `std::size_t` | `GetComponentCount()` |
| `std::vector<Component> const&` | `GetComponents()` |
| `Nz::VertexInputRate` | `GetInputRate()` |
| `std::size_t` | `GetStride()` |
| `bool` | `HasComponent(Nz::VertexComponent component, std::size_t componentIndex)` |
| VertexDeclaration`&` | `operator=(`VertexDeclaration` const&)` |
| VertexDeclaration`&` | `operator=(`VertexDeclaration`&&)` |
