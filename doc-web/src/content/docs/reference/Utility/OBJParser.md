---
title: OBJParser
description: Nothing
---

# Nz::OBJParser

Class description

## Constructors

- `OBJParser()`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `bool` | `Check(`[`Stream`](documentation/generated/Core/Stream.md)`& stream)` |
| `std::string*` | `GetMaterials()` |
| `std::string const*` | `GetMaterials()` |
| `std::size_t` | `GetMaterialCount()` |
| [`OBJParser::Mesh`](documentation/generated/Utility/OBJParser.Mesh.md)`*` | `GetMeshes()` |
| [`OBJParser::Mesh`](documentation/generated/Utility/OBJParser.Mesh.md)` const*` | `GetMeshes()` |
| `std::size_t` | `GetMeshCount()` |
| `std::filesystem::path const&` | `GetMtlLib()` |
| `Nz::Vector3f*` | `GetNormals()` |
| `Nz::Vector3f const*` | `GetNormals()` |
| `std::size_t` | `GetNormalCount()` |
| `Nz::Vector4f*` | `GetPositions()` |
| `Nz::Vector4f const*` | `GetPositions()` |
| `std::size_t` | `GetPositionCount()` |
| `Nz::Vector3f*` | `GetTexCoords()` |
| `Nz::Vector3f const*` | `GetTexCoords()` |
| `std::size_t` | `GetTexCoordCount()` |
| `bool` | `Parse(`[`Stream`](documentation/generated/Core/Stream.md)`& stream, std::size_t reservedVertexCount)` |
| `bool` | `Save(`[`Stream`](documentation/generated/Core/Stream.md)`& stream)` |
| `std::string*` | `SetMaterialCount(std::size_t materialCount)` |
| [`OBJParser::Mesh`](documentation/generated/Utility/OBJParser.Mesh.md)`*` | `SetMeshCount(std::size_t meshCount)` |
| `void` | `SetMtlLib(std::filesystem::path const& mtlLib)` |
| `Nz::Vector3f*` | `SetNormalCount(std::size_t normalCount)` |
| `Nz::Vector4f*` | `SetPositionCount(std::size_t positionCount)` |
| `Nz::Vector3f*` | `SetTexCoordCount(std::size_t texCoordCount)` |
