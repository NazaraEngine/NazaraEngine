---
title: GraphicsComponent
description: Nothing
---

# Nz::GraphicsComponent

Class description

## Constructors

- `GraphicsComponent(bool initiallyVisible)`
- `GraphicsComponent(std::shared_ptr<`[`InstancedRenderable`](documentation/generated/Graphics/InstancedRenderable.md)`> renderable, Nz::UInt32 renderMask, bool initiallyVisible)`
- `GraphicsComponent(`GraphicsComponent` const&)`
- `GraphicsComponent(`GraphicsComponent`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AttachRenderable(std::shared_ptr<`[`InstancedRenderable`](documentation/generated/Graphics/InstancedRenderable.md)`> renderable, Nz::UInt32 renderMask)` |
| `void` | `Clear()` |
| `void` | `DetachRenderable(std::shared_ptr<`[`InstancedRenderable`](documentation/generated/Graphics/InstancedRenderable.md)`> const& renderable)` |
| `Nz::Boxf` | `GetAABB()` |
| [`GraphicsComponent::Renderable`](documentation/generated/Graphics/GraphicsComponent.Renderable.md)` const&` | `GetRenderableEntry(std::size_t renderableIndex)` |
| `std::array<Renderable, MaxRenderableCount> const&` | `GetRenderables()` |
| `Nz::Recti const&` | `GetScissorBox()` |
| `Nz::WorldInstancePtr const&` | `GetWorldInstance()` |
| `void` | `Hide()` |
| `bool` | `IsVisible()` |
| `void` | `Show(bool show)` |
| `void` | `UpdateScissorBox(Nz::Recti const& scissorBox)` |
| GraphicsComponent`&` | `operator=(`GraphicsComponent` const&)` |
| GraphicsComponent`&` | `operator=(`GraphicsComponent`&&)` |
