---
title: IndexIterator
description: Nothing
---

# Nz::IndexIterator

Class description

## Constructors

- `IndexIterator()`
- `IndexIterator(`IndexIterator` const& iterator)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `Nz::IndexIterator::Reference` | `operator*()` |
| `Nz::IndexIterator::Reference` | `operator[](Nz::UInt32 index)` |
| IndexIterator`&` | `operator=(`IndexIterator` const& iterator)` |
| `Nz::IndexIterator` | `operator+(Nz::UInt32 indexCount)` |
| `Nz::IndexIterator` | `operator-(Nz::UInt32 indexCount)` |
| IndexIterator`&` | `operator+=(Nz::UInt32 indexCount)` |
| IndexIterator`&` | `operator-=(Nz::UInt32 indexCount)` |
| IndexIterator`&` | `operator++()` |
| `Nz::IndexIterator` | `operator++(int)` |
| IndexIterator`&` | `operator--()` |
| `Nz::IndexIterator` | `operator--(int)` |
