// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://stackoverflow.com/questions/687490/c0x-how-do-i-expand-a-tuple-into-variadic-template-function-arguments
// Merci à Ryan "FullMetal Alchemist" Lahfa
// Merci aussi à Freedom de siteduzero.com

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		// http://www.cppsamples.com/common-tasks/apply-tuple-to-function.html
		template<typename F, typename Tuple, size_t... S>
		auto ApplyImplFunc(F&& fn, Tuple&& t, std::index_sequence<S...>)
		{
			return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
		}

		template<typename O, typename F, typename Tuple, size_t... S>
		auto ApplyImplMethod(O& object, F&& fn, Tuple&& t, std::index_sequence<S...>)
		{
			return (object .* std::forward<F>(fn))(std::get<S>(std::forward<Tuple>(t))...);
		}
	}

	template<typename F, typename Tuple>
	auto Apply(F&& fn, Tuple&& t)
	{
		constexpr std::size_t tSize = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;

		return Detail::ApplyImplFunc(std::forward<F>(fn), std::forward<Tuple>(t), std::make_index_sequence<tSize>());
	}

	template<typename O, typename F, typename Tuple>
	auto Apply(O& object, F&& fn, Tuple&& t)
	{
		constexpr std::size_t tSize = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;

		return Detail::ApplyImplMethod(object, std::forward<F>(fn), std::forward<Tuple>(t), std::make_index_sequence<tSize>());
	}

	template<typename T> 
	ByteArray ComputeHash(HashType hash, const T& v)
	{
		return ComputeHash(AbstractHash::Get(hash).get(), v);
	}

	template<typename T> 
	ByteArray ComputeHash(AbstractHash* hash, const T& v)
	{
		hash->Begin();
		
		HashAppend(hash, v);

		return hash->End();
	}

	// Algorithme venant de CityHash par Google
	// http://stackoverflow.com/questions/8513911/how-to-create-a-good-hash-combine-with-64-bit-output-inspired-by-boosthash-co
	template<typename T>
	void HashCombine(std::size_t& seed, const T& v)
	{
		const UInt64 kMul = 0x9ddfea08eb382d69ULL;

		std::hash<T> hasher;
		UInt64 a = (hasher(v) ^ seed) * kMul;
		a ^= (a >> 47);

		UInt64 b = (seed ^ a) * kMul;
		b ^= (b >> 47);

		seed = static_cast<std::size_t>(b * kMul);
	}

	inline bool Serialize(SerializationContext& context, bool value)
	{
		if (context.currentBitPos == 8)
		{
			context.currentBitPos = 0;
			context.currentByte = 0;
		}

		if (value)
			context.currentByte |= 1 << context.currentBitPos;

		if (++context.currentBitPos >= 8)
			return Serialize<UInt8>(context, context.currentByte);
		else
			return true;
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(SerializationContext& context, T value)
	{
		// Flush bits if a writing is in progress
		if (context.currentBitPos != 8)
		{
			context.currentBitPos = 8;

			if (!Serialize<UInt8>(context, context.currentByte))
				NazaraWarning("Failed to flush bits");
		}

		if (context.endianness != Endianness_Unknown && context.endianness != GetPlatformEndianness())
			SwapBytes(&value, sizeof(T));

		return context.stream->Write(&value, sizeof(T)) == sizeof(T);
	}

	inline bool Unserialize(UnserializationContext& context, bool* value)
	{
		NazaraAssert(value, "Invalid data pointer");

		if (context.currentBitPos == 8)
		{
			if (!Unserialize(context, &context.currentByte))
				return false;

			context.currentBitPos = 0;
		}

		if (value)
			*value = (context.currentByte & (1 << context.currentBitPos)) != 0;

		context.currentBitPos++;

		return true;
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(UnserializationContext& context, T* value)
	{
		NazaraAssert(value, "Invalid data pointer");

		// Reset bit position
		context.currentBitPos = 8;

		if (context.stream->Read(value, sizeof(T)) == sizeof(T))
		{
			if (context.endianness != Endianness_Unknown && context.endianness != GetPlatformEndianness())
				SwapBytes(value, sizeof(T));

			return true;
		}
		else
			return false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
