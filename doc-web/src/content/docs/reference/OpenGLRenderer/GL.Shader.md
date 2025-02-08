---
title: GL.Shader
description: Nothing
---

# Nz::GL::Shader

Class description

## Constructors

- `Shader(`GL::Shader` const&)`
- `Shader(`GL::Shader`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Compile()` |
| `bool` | `GetCompilationStatus(std::string* error)` |
| `std::string` | `GetSource()` |
| `void` | `SetBinarySource(GLenum binaryFormat, void const* binary, GLsizei length)` |
| `void` | `SetSource(char const* source, GLint length)` |
| `void` | `SetSource(std::string_view const& source)` |
| `void` | `SpecializeShader(GLchar const* pEntryPoint, GLuint numSpecializationConstants, GLuint const* pConstantIndex, GLuint const* pConstantValue)` |
| GL::Shader`&` | `operator=(`GL::Shader` const&)` |
| GL::Shader`&` | `operator=(`GL::Shader`&&)` |
