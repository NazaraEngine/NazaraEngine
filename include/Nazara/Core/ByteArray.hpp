// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTEARRAY_HPP
#define NAZARA_BYTEARRAY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <atomic>

class NzAbstractHash;
class NzHashDigest;

class NAZARA_API NzByteArray : public NzHashable
{
	public:
		struct SharedArray;

		NzByteArray();
		NzByteArray(const nzUInt8* buffer, unsigned int length);
		NzByteArray(const NzByteArray& buffer);
		NzByteArray(NzByteArray&& buffer) noexcept;
		NzByteArray(SharedArray* sharedArray);
		~NzByteArray();

		NzByteArray& Append(nzUInt8 byte);
		NzByteArray& Append(const nzUInt8* buffer, unsigned int length);
		NzByteArray& Append(const NzByteArray& array);

		void Clear(bool keepBuffer = false);

		nzUInt8* GetBuffer();
		unsigned int GetCapacity() const;
		const nzUInt8* GetConstBuffer() const;
		unsigned int GetSize() const;

		NzByteArray& Insert(int pos, nzUInt8 byte);
		NzByteArray& Insert(int pos, const nzUInt8* buffer, unsigned int length);
		NzByteArray& Insert(int pos, const NzByteArray& array);

		bool IsEmpty() const;

		NzByteArray& Prepend(nzUInt8 byte);
		NzByteArray& Prepend(const nzUInt8* buffer, unsigned int length);
		NzByteArray& Prepend(const NzByteArray& array);

		void Reserve(unsigned int bufferSize);

		NzByteArray& Resize(int size, nzUInt8 byte = 0);
		NzByteArray Resized(int size, nzUInt8 byte = 0) const;

		NzByteArray SubArray(int startPos, int endPos = -1) const;

		void Swap(NzByteArray& array);

		// Méthodes compatibles STD
		nzUInt8* begin();
		const nzUInt8* begin() const;
		nzUInt8* end();
		const nzUInt8* end() const;
		void push_front(nzUInt8 byte);
		void push_back(nzUInt8 byte);
		/*nzUInt8* rbegin();
		const nzUInt8* rbegin() const;
		nzUInt8* rend();
		const nzUInt8* rend() const;*/

		typedef const nzUInt8& const_reference;
		typedef nzUInt8* iterator;
		//typedef nzUInt8* reverse_iterator;
		typedef nzUInt8 value_type;
		// Méthodes compatibles STD

		nzUInt8& operator[](unsigned int pos);
		nzUInt8 operator[](unsigned int pos) const;

		NzByteArray& operator=(const NzByteArray& array);
		NzByteArray& operator=(NzByteArray&& array) noexcept;

		NzByteArray operator+(nzUInt8 byte) const;
		NzByteArray operator+(const NzByteArray& array) const;
		NzByteArray& operator+=(nzUInt8 byte);
		NzByteArray& operator+=(const NzByteArray& array);

		static int Compare(const NzByteArray& first, const NzByteArray& second);

		struct NAZARA_API SharedArray
		{
			SharedArray() :
			refCount(1)
			{
			}

			SharedArray(unsigned short referenceCount, unsigned int bufferSize, unsigned int arraySize, nzUInt8* ptr) :
			capacity(bufferSize),
			size(arraySize),
			buffer(ptr),
			refCount(referenceCount)
			{
			}

			unsigned int capacity;
			unsigned int size;
			nzUInt8* buffer;

			std::atomic_ushort refCount;
		};

		static SharedArray emptyArray;
		static unsigned int npos;

	private:
		void EnsureOwnership();
		bool FillHash(NzAbstractHash* hash) const;
		void ReleaseArray();

		SharedArray* m_sharedArray;
};

namespace std
{
	NAZARA_API void swap(NzByteArray& lhs, NzByteArray& rhs);
}

#endif // NAZARA_BYTEARRAY_HPP
