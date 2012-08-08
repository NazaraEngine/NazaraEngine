// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTEARRAY_HPP
#define NAZARA_BYTEARRAY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Hashable.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_BYTEARRAY
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NzAbstractHash;
class NzHashDigest;

class NAZARA_API NzByteArray : public NzHashable
{
	public:
		struct SharedArray;

		NzByteArray();
		NzByteArray(const nzUInt8* buffer, unsigned int bufferLength);
		NzByteArray(const NzByteArray& buffer);
		NzByteArray(NzByteArray&& buffer) noexcept;
		NzByteArray(SharedArray* sharedArray);
		~NzByteArray();

		NzByteArray& Append(const NzByteArray& byteArray);

		void Clear();

		nzUInt8* GetBuffer();
		unsigned int GetCapacity() const;
		const nzUInt8* GetConstBuffer() const;
		unsigned int GetSize() const;

		NzByteArray& Insert(int pos, const nzUInt8* buffer, unsigned int bufferLength);
		NzByteArray& Insert(int pos, const NzByteArray& byteArray);

		bool IsEmpty() const;

		void Reserve(unsigned int bufferSize);

		NzByteArray& Resize(int size, nzUInt8 byte = 0);
		NzByteArray Resized(int size, nzUInt8 byte = 0) const;

		NzByteArray Subarray(int startPos, int endPos = -1) const;

		void Swap(NzByteArray& byteArray);

		NzByteArray& Trim(nzUInt8 byte = '\0');
		NzByteArray Trimmed(nzUInt8 byte = '\0') const;

		// Méthodes compatibles STD
		nzUInt8* begin();
		const nzUInt8* begin() const;
		nzUInt8* end();
		const nzUInt8* end() const;
		void push_front(nzUInt8 c);
		void push_back(nzUInt8 c);
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

		NzByteArray& operator=(const NzByteArray& byteArray);
		NzByteArray& operator=(NzByteArray&& byteArray) noexcept;

		NzByteArray operator+(const NzByteArray& byteArray) const;
		NzByteArray& operator+=(const NzByteArray& byteArray);

		static int Compare(const NzByteArray& first, const NzByteArray& second);

		struct NAZARA_API SharedArray
		{
			SharedArray() = default;

			SharedArray(unsigned short referenceCount, unsigned int bufferSize, unsigned int arraySize, nzUInt8* ptr) :
			capacity(bufferSize),
			size(arraySize),
			refCount(referenceCount),
			buffer(ptr)
			{
			}

			unsigned int capacity;
			unsigned int size;
			unsigned short refCount = 1;
			nzUInt8* buffer;

			NazaraMutex(mutex)
		};

		static SharedArray emptyArray;
		static unsigned int npos;

	private:
		void EnsureOwnership();
		bool FillHash(NzHashImpl* hash) const;
		void ReleaseArray();

		SharedArray* m_sharedArray;
};

namespace std
{
	NAZARA_API void swap(NzByteArray& lhs, NzByteArray& rhs);
}

#endif // NAZARA_BYTEARRAY_HPP
