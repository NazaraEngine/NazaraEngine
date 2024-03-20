// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <limits>
#include <utility>

namespace Nz
{
	/*!
	* \ingroup core
	* \brief Computes the hash of a hashable object
	* \return A bytearray which represents the hash
	*
	* \param hash Enumeration of type HashType
	* \param v Object to hash
	*
	* \remark a HashAppend specialization for type T is required
	*
	* \see ComputeHash
	*/
	template<typename T>
	ByteArray ComputeHash(HashType hash, T&& v)
	{
		return ComputeHash(*AbstractHash::Get(hash), std::forward<T>(v));
	}

	/*!
	* \ingroup core
	* \brief Computes the hash of a hashable object
	* \return A bytearray which represents the hash
	*
	* \param hash Pointer to abstract hash
	* \param v Object to hash
	*
	* \remark Produce a NazaraAssert if pointer to Abstracthash is invalid
	* \remark a HashAppend specialization for type T is required
	*
	* \see ComputeHash
	*/
	template<typename T>
	ByteArray ComputeHash(AbstractHash& hash, T&& v)
	{
		hash.Begin();

		HashAppend(hash, std::forward<T>(v));

		return hash.End();
	}

	inline bool HashAppend(AbstractHash& hash, std::string_view v)
	{
		hash.Append(reinterpret_cast<const UInt8*>(v.data()), v.size());
		return true;
	}

	inline Vector3f TransformPositionSRT(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& position)
	{
		return transformRotation * (transformScale * position) + transformTranslation;
	}

	Vector3f TransformNormalSRT(const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& normal)
	{
		return Quaternionf::Mirror(transformRotation, transformScale) * normal;
	}

	inline Quaternionf TransformRotationSRT(const Quaternionf& transformRotation, const Vector3f& transformScale, const Quaternionf& rotation)
	{
		return Quaternionf::Mirror(transformRotation, transformScale) * rotation;
	}

	inline Vector3f TransformScaleSRT(const Vector3f& transformScale, const Vector3f& scale)
	{
		return transformScale * scale;
	}

	inline void TransformSRT(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, Vector3f& position, Quaternionf& rotation, Vector3f& scale)
	{
		position = TransformPositionSRT(transformTranslation, transformRotation, transformScale, position);
		rotation = TransformRotationSRT(transformRotation, transformScale, rotation);
		scale    = TransformScaleSRT(transformScale, scale);
	}

	inline void TransformVertices(VertexPointers vertexPointers, UInt32 vertexCount, const Matrix4f& matrix)
	{
		if (vertexPointers.positionPtr)
		{
			for (UInt32 i = 0; i < vertexCount; ++i)
				*vertexPointers.positionPtr++ = matrix.Transform(*vertexPointers.positionPtr);
		}

		if (vertexPointers.normalPtr || vertexPointers.tangentPtr)
		{
			Vector3f scale = matrix.GetScale();

			if (vertexPointers.normalPtr)
			{
				for (UInt64 i = 0; i < vertexCount; ++i)
					*vertexPointers.normalPtr++ = matrix.Transform(*vertexPointers.normalPtr, 0.f) / scale;
			}

			if (vertexPointers.tangentPtr)
			{
				for (UInt64 i = 0; i < vertexCount; ++i)
					*vertexPointers.tangentPtr++ = matrix.Transform(*vertexPointers.tangentPtr, 0.f) / scale;
			}
		}
	}

	template<typename T> constexpr ComponentType ComponentTypeId()
	{
		static_assert(AlwaysFalse<T>::value, "This type cannot be used as a component.");
		return ComponentType{};
	}

	template<> constexpr ComponentType ComponentTypeId<Color>()       { return ComponentType::Color; }
	template<> constexpr ComponentType ComponentTypeId<double>()      { return ComponentType::Double1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2d>()    { return ComponentType::Double2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3d>()    { return ComponentType::Double3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4d>()    { return ComponentType::Double4; }
	template<> constexpr ComponentType ComponentTypeId<float>()       { return ComponentType::Float1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2f>()    { return ComponentType::Float2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3f>()    { return ComponentType::Float3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4f>()    { return ComponentType::Float4; }
	template<> constexpr ComponentType ComponentTypeId<int>()         { return ComponentType::Int1; }
	template<> constexpr ComponentType ComponentTypeId<Vector2i>()    { return ComponentType::Int2; }
	template<> constexpr ComponentType ComponentTypeId<Vector3i>()    { return ComponentType::Int3; }
	template<> constexpr ComponentType ComponentTypeId<Vector4i>()    { return ComponentType::Int4; }

	template<typename T>
	constexpr ComponentType GetComponentTypeOf()
	{
		return ComponentTypeId<std::decay_t<T>>();
	}
}

