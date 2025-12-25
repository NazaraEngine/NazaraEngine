---
title: ParameterList
description: Nothing
---

# Nz::ParameterList

Class description

## Constructors

- `ParameterList()`
- `ParameterList(`ParameterList` const& list)`
- `ParameterList(`ParameterList`&&)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `void` | `Clear()` |
| `void` | `ForEach(std::function<bool (const `ParameterList` &, const std::string &)> const& callback)` |
| `void` | `ForEach(std::function<void (const `ParameterList` &, const std::string &)> const& callback)` |
| `Result<bool, Nz::ParameterList::Error>` | `GetBooleanParameter(std::string const& name, bool strict)` |
| `Result<Nz::Color, Nz::ParameterList::Error>` | `GetColorParameter(std::string const& name, bool strict)` |
| `Result<double, Nz::ParameterList::Error>` | `GetDoubleParameter(std::string const& name, bool strict)` |
| `Result<long long, Nz::ParameterList::Error>` | `GetIntegerParameter(std::string const& name, bool strict)` |
| `Result<Nz::ParameterType, Nz::ParameterList::Error>` | `GetParameterType(std::string const& name)` |
| `Result<void *, Nz::ParameterList::Error>` | `GetPointerParameter(std::string const& name, bool strict)` |
| `Result<std::string, Nz::ParameterList::Error>` | `GetStringParameter(std::string const& name, bool strict)` |
| `Result<std::string_view, Nz::ParameterList::Error>` | `GetStringViewParameter(std::string const& name, bool strict)` |
| `Result<void *, Nz::ParameterList::Error>` | `GetUserdataParameter(std::string const& name, bool strict)` |
| `bool` | `HasParameter(std::string const& name)` |
| `void` | `RemoveParameter(std::string const& name)` |
| `void` | `SetParameter(std::string const& name)` |
| `void` | `SetParameter(std::string const& name, `[`Color`](documentation/generated/Core/Color.md)` const& value)` |
| `void` | `SetParameter(std::string const& name, std::string const& value)` |
| `void` | `SetParameter(std::string const& name, char const* value)` |
| `void` | `SetParameter(std::string const& name, bool value)` |
| `void` | `SetParameter(std::string const& name, double value)` |
| `void` | `SetParameter(std::string const& name, long long value)` |
| `void` | `SetParameter(std::string const& name, void* value)` |
| `void` | `SetParameter(std::string const& name, void* value, Nz::ParameterList::Destructor destructor)` |
| `std::string` | `ToString()` |
| ParameterList`&` | `operator=(`ParameterList` const& list)` |
| ParameterList`&` | `operator=(`ParameterList`&&)` |
