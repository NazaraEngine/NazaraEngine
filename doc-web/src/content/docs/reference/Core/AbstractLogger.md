---
title: AbstractLogger
description: Nothing
---

# Nz::AbstractLogger

Class description

## Constructors

- `AbstractLogger()`
- `AbstractLogger(`AbstractLogger` const&)`
- `AbstractLogger(`AbstractLogger`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableStdReplication(bool enable)` |
| `bool` | `IsStdReplicationEnabled()` |
| `void` | `Write(std::string_view string)` |
| `void` | `WriteError(Nz::ErrorType type, std::string_view error, unsigned int line, char const* file, char const* function)` |
| AbstractLogger`&` | `operator=(`AbstractLogger` const&)` |
| AbstractLogger`&` | `operator=(`AbstractLogger`&&)` |
