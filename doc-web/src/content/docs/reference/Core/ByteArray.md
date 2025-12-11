---
title: ByteArray
description: Nothing
---

# Nz::ByteArray

Class description

## Constructors

- `ByteArray()`
- `ByteArray(Nz::ByteArray::size_type n)`
- `ByteArray(void const* buffer, Nz::ByteArray::size_type n)`
- `ByteArray(Nz::ByteArray::size_type n, Nz::ByteArray::value_type value)`
- `ByteArray(Nz::ByteArray::Container container)`
- `ByteArray(`ByteArray` const& other)`
- `ByteArray(`ByteArray`&& other)`

## Methods

| Return type | Signature |
| ----------- | --------- |
| `iterator` | `Append(void const* buffer, Nz::ByteArray::size_type size)` |
| `iterator` | `Append(`ByteArray` const& other)` |
| `void` | `Assign(Nz::ByteArray::size_type n, Nz::ByteArray::value_type value)` |
| `reference` | `Back()` |
| `const_reference` | `Back()` |
| `void` | `Clear(bool keepBuffer)` |
| `iterator` | `Erase(Nz::ByteArray::const_iterator pos)` |
| `iterator` | `Erase(Nz::ByteArray::const_iterator first, Nz::ByteArray::const_iterator last)` |
| `reference` | `Front()` |
| `const_reference` | `Front()` |
| `allocator_type` | `GetAllocator()` |
| `pointer` | `GetBuffer()` |
| `size_type` | `GetCapacity()` |
| `const_pointer` | `GetConstBuffer()` |
| `size_type` | `GetMaxSize()` |
| `size_type` | `GetSize()` |
| `Nz::ByteArray` | `GetSubArray(Nz::ByteArray::const_iterator startPos, Nz::ByteArray::const_iterator endPos)` |
| `iterator` | `Insert(Nz::ByteArray::const_iterator pos, void const* buffer, Nz::ByteArray::size_type n)` |
| `iterator` | `Insert(Nz::ByteArray::const_iterator pos, `ByteArray` const& other)` |
| `iterator` | `Insert(Nz::ByteArray::const_iterator pos, Nz::ByteArray::size_type n, Nz::ByteArray::value_type byte)` |
| `bool` | `IsEmpty()` |
| `void` | `PopBack()` |
| `void` | `PopFront()` |
| `iterator` | `Prepend(void const* buffer, Nz::ByteArray::size_type size)` |
| `iterator` | `Prepend(`ByteArray` const& other)` |
| `void` | `PushBack(Nz::ByteArray::value_type byte)` |
| `void` | `PushFront(Nz::ByteArray::value_type byte)` |
| `void` | `Reserve(Nz::ByteArray::size_type bufferSize)` |
| `void` | `Resize(Nz::ByteArray::size_type newSize)` |
| `void` | `Resize(Nz::ByteArray::size_type newSize, Nz::ByteArray::value_type byte)` |
| `void` | `ShrinkToFit()` |
| `void` | `Swap(`ByteArray`& other)` |
| `std::string` | `ToHex()` |
| `std::string` | `ToString()` |
| `iterator` | `begin()` |
| `const_iterator` | `begin()` |
| `bool` | `empty()` |
| `iterator` | `end()` |
| `const_iterator` | `end()` |
| `reverse_iterator` | `rbegin()` |
| `const_reverse_iterator` | `rbegin()` |
| `reverse_iterator` | `rend()` |
| `const_reverse_iterator` | `rend()` |
| `const_iterator` | `cbegin()` |
| `const_iterator` | `cend()` |
| `const_reverse_iterator` | `crbegin()` |
| `const_reverse_iterator` | `crend()` |
| `size_type` | `size()` |
| `reference` | `operator[](Nz::ByteArray::size_type pos)` |
| `const_reference` | `operator[](Nz::ByteArray::size_type pos)` |
| ByteArray`&` | `operator=(`ByteArray` const& array)` |
| ByteArray`&` | `operator=(`ByteArray`&& array)` |
| `Nz::ByteArray` | `operator+(`ByteArray` const& array)` |
| ByteArray`&` | `operator+=(`ByteArray` const& array)` |
| `bool` | `operator==(`ByteArray` const& rhs)` |
| `bool` | `operator!=(`ByteArray` const& rhs)` |
| `bool` | `operator<(`ByteArray` const& rhs)` |
| `bool` | `operator<=(`ByteArray` const& rhs)` |
| `bool` | `operator>(`ByteArray` const& rhs)` |
| `bool` | `operator>=(`ByteArray` const& rhs)` |
