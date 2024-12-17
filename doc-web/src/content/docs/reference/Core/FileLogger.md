---
title: FileLogger
description: Nothing
---

# Nz::FileLogger

Class description

## Constructors

- `FileLogger(std::filesystem::path logPath)`
- `FileLogger(`FileLogger` const&)`
- `FileLogger(`FileLogger`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `EnableTimeLogging(bool enable)` |
| `void` | `EnableStdReplication(bool enable)` |
| `bool` | `IsStdReplicationEnabled()` |
| `bool` | `IsTimeLoggingEnabled()` |
| `void` | `Write(std::string_view string)` |
| `void` | `WriteError(Nz::ErrorType type, std::string_view error, unsigned int line, char const* file, char const* function)` |
| FileLogger`&` | `operator=(`FileLogger` const&)` |
| FileLogger`&` | `operator=(`FileLogger`&&)` |
