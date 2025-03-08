---
title: Node
description: Nothing
---

# Nz::Node

Class description

## Constructors

- `Node(Nz::Vector3f const& translation, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale)`
- `Node(`Node` const& node)`
- `Node(`Node`&& node)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnsureDerivedUpdate()` |
| `void` | `EnsureTransformMatrixUpdate()` |
| `Nz::Vector3f` | `GetBackward()` |
| `std::vector<`Node` *> const&` | `GetChilds()` |
| `Nz::Vector3f` | `GetDown()` |
| `Nz::Vector3f` | `GetForward()` |
| `bool` | `GetInheritPosition()` |
| `bool` | `GetInheritRotation()` |
| `bool` | `GetInheritScale()` |
| `Nz::Vector3f` | `GetInitialPosition()` |
| `Nz::Quaternionf` | `GetInitialRotation()` |
| `Nz::Vector3f` | `GetInitialScale()` |
| `Nz::Vector3f` | `GetLeft()` |
| `Nz::NodeType` | `GetNodeType()` |
| Node` const*` | `GetParent()` |
| `Nz::Vector3f` | `GetPosition(Nz::CoordSys coordSys)` |
| `Nz::Vector3f` | `GetRight()` |
| `Nz::Quaternionf` | `GetRotation(Nz::CoordSys coordSys)` |
| `Nz::Vector3f` | `GetScale(Nz::CoordSys coordSys)` |
| `Nz::Matrix4f const&` | `GetTransformMatrix()` |
| `Nz::Vector3f` | `GetUp()` |
| `bool` | `HasChilds()` |
| `void` | `Invalidate(Nz::Node::Invalidation invalidation)` |
| Node`&` | `Interpolate(`Node` const& nodeA, `Node` const& nodeB, float interpolation, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Move(Nz::Vector3f const& movement, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Move(float movementX, float movementY, float movementZ, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Rotate(Nz::Quaternionf const& rotation, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Scale(Nz::Vector3f const& scale, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Scale(float scale, Nz::Node::Invalidation invalidation)` |
| Node`&` | `Scale(float scaleX, float scaleY, float scaleZ, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInheritPosition(bool inheritPosition, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInheritRotation(bool inheritRotation, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInheritScale(bool inheritScale, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialPosition(Nz::Vector3f const& translation, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialPosition(float translationX, float translationXY, float translationZ, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialRotation(Nz::Quaternionf const& quat, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialScale(Nz::Vector3f const& scale, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialScale(float scale, Nz::Node::Invalidation invalidation)` |
| `void` | `SetInitialScale(float scaleX, float scaleY, float scaleZ, Nz::Node::Invalidation invalidation)` |
| `void` | `SetParent(`Node` const* node, bool keepDerived, Nz::Node::Invalidation invalidation)` |
| `void` | `SetParent(`Node` const& node, bool keepDerived, Nz::Node::Invalidation invalidation)` |
| `void` | `SetPosition(Nz::Vector3f const& translation, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetPosition(float translationX, float translationY, float translationZ, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetRotation(Nz::Quaternionf const& rotation, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetScale(Nz::Vector2f const& scale, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetScale(Nz::Vector3f const& scale, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetScale(float scale, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetScale(float scaleX, float scaleY, float scaleZ, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetTransform(Nz::Vector3f const& position, Nz::Quaternionf const& rotation, Nz::Vector3f const& scale, Nz::CoordSys coordSys, Nz::Node::Invalidation invalidation)` |
| `void` | `SetTransformMatrix(Nz::Matrix4f const& matrix, Nz::Node::Invalidation invalidation)` |
| `Nz::Vector3f` | `ToGlobalPosition(Nz::Vector3f const& localPosition)` |
| `Nz::Quaternionf` | `ToGlobalRotation(Nz::Quaternionf const& localRotation)` |
| `Nz::Vector3f` | `ToGlobalScale(Nz::Vector3f const& localScale)` |
| `Nz::Vector3f` | `ToLocalPosition(Nz::Vector3f const& globalPosition)` |
| `Nz::Quaternionf` | `ToLocalRotation(Nz::Quaternionf const& globalRotation)` |
| `Nz::Vector3f` | `ToLocalScale(Nz::Vector3f const& globalScale)` |
| Node`&` | `operator=(`Node` const& node)` |
| Node`&` | `operator=(`Node`&& node)` |
