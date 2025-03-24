---
title: Mesh
description: Nothing
---

# Nz::Mesh

Class description

## Constructors

- `Mesh()`
- `Mesh(`Mesh` const&)`
- `Mesh(`Mesh`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AddSubMesh(std::shared_ptr<`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`> subMesh)` |
| `void` | `AddSubMesh(std::string const& identifier, std::shared_ptr<`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`> subMesh)` |
| `std::shared_ptr<`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`>` | `BuildSubMesh(`[`Primitive`](documentation/generated/Core/Primitive.md)` const& primitive, `[`MeshParams`](documentation/generated/Utility/MeshParams.md)` const& params)` |
| `void` | `BuildSubMeshes(`[`PrimitiveList`](documentation/generated/Core/PrimitiveList.md)` const& primitiveList, `[`MeshParams`](documentation/generated/Utility/MeshParams.md)` const& params)` |
| `bool` | `CreateSkeletal(std::size_t jointCount)` |
| `bool` | `CreateStatic()` |
| `void` | `Destroy()` |
| `void` | `GenerateNormals()` |
| `void` | `GenerateNormalsAndTangents()` |
| `void` | `GenerateTangents()` |
| `Nz::Boxf const&` | `GetAABB()` |
| `std::filesystem::path` | `GetAnimation()` |
| `Nz::AnimationType` | `GetAnimationType()` |
| `std::size_t` | `GetJointCount()` |
| [`ParameterList`](documentation/generated/Core/ParameterList.md)`&` | `GetMaterialData(std::size_t index)` |
| [`ParameterList`](documentation/generated/Core/ParameterList.md)` const&` | `GetMaterialData(std::size_t index)` |
| `std::size_t` | `GetMaterialCount()` |
| [`Skeleton`](documentation/generated/Utility/Skeleton.md)`*` | `GetSkeleton()` |
| [`Skeleton`](documentation/generated/Utility/Skeleton.md)` const*` | `GetSkeleton()` |
| `std::shared_ptr<`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`> const&` | `GetSubMesh(std::string const& identifier)` |
| `std::shared_ptr<`[`SubMesh`](documentation/generated/Utility/SubMesh.md)`> const&` | `GetSubMesh(std::size_t index)` |
| `std::size_t` | `GetSubMeshCount()` |
| `std::size_t` | `GetSubMeshIndex(std::string const& identifier)` |
| `Nz::UInt32` | `GetTriangleCount()` |
| `Nz::UInt32` | `GetVertexCount()` |
| `bool` | `HasSubMesh(std::string const& identifier)` |
| `bool` | `HasSubMesh(std::size_t index)` |
| `void` | `InvalidateAABB()` |
| `bool` | `IsAnimable()` |
| `bool` | `IsValid()` |
| `void` | `Recenter()` |
| `void` | `RemoveSubMesh(std::string const& identifier)` |
| `void` | `RemoveSubMesh(std::size_t index)` |
| `bool` | `SaveToFile(std::filesystem::path const& filePath, `[`MeshParams`](documentation/generated/Utility/MeshParams.md)` const& params)` |
| `bool` | `SaveToStream(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, std::string const& format, `[`MeshParams`](documentation/generated/Utility/MeshParams.md)` const& params)` |
| `void` | `SetAnimation(std::filesystem::path const& animationPath)` |
| `void` | `SetMaterialCount(std::size_t matCount)` |
| `void` | `SetMaterialData(std::size_t matIndex, `[`ParameterList`](documentation/generated/Core/ParameterList.md)` data)` |
| `void` | `Transform(Nz::Matrix4f const& matrix)` |
| Mesh`&` | `operator=(`Mesh` const&)` |
| Mesh`&` | `operator=(`Mesh`&&)` |
