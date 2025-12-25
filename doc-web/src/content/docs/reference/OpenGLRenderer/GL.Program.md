---
title: GL.Program
description: Nothing
---

# Nz::GL::Program

Class description

## Constructors

- `Program(`GL::Program` const&)`
- `Program(`GL::Program`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `AttachShader(GLuint shader)` |
| `void` | `Get(GLenum pname, GLint* params)` |
| `void` | `GetActiveUniform(GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)` |
| `void` | `GetActiveUniformBlock(GLuint uniformBlockIndex, GLenum pname, GLint* params)` |
| `std::vector<GLint>` | `GetActiveUniformBlockUniformIndices(GLuint uniformBlockIndex)` |
| `void` | `GetActiveUniformBlockName(GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName)` |
| `std::string` | `GetActiveUniformBlockName(GLuint uniformBlockIndex)` |
| `std::string` | `GetActiveUniformName(GLuint index)` |
| `std::vector<GLint>` | `GetActiveUniforms(GLsizei uniformCount, GLuint const* uniformIndices, GLenum pname)` |
| `void` | `GetActiveUniforms(GLsizei uniformCount, GLuint const* uniformIndices, GLenum pname, GLint* params)` |
| `bool` | `GetLinkStatus(std::string* error)` |
| `GLuint` | `GetUniformBlockIndex(char const* uniformBlockName)` |
| `GLuint` | `GetUniformBlockIndex(std::string const& uniformBlockName)` |
| `GLint` | `GetUniformLocation(char const* uniformName)` |
| `GLint` | `GetUniformLocation(std::string const& uniformName)` |
| `void` | `Link()` |
| `void` | `Uniform(GLint uniformLocation, float value)` |
| `void` | `Uniform(GLint uniformLocation, int value)` |
| `void` | `UniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding)` |
| GL::Program`&` | `operator=(`GL::Program` const&)` |
| GL::Program`&` | `operator=(`GL::Program`&&)` |
