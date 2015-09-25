// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void PrimitiveList::AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Box(lengths, subdivision, transformMatrix));
	}

	void PrimitiveList::AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Box(lengths, subdivision, position, rotation));
	}

	void PrimitiveList::AddCone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Cone(length, radius, subdivision, transformMatrix));
	}

	void PrimitiveList::AddCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Cone(length, radius, subdivision, position, rotation));
	}

	void PrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::CubicSphere(size, subdivision, transformMatrix));
	}

	void PrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::CubicSphere(size, subdivision, position, rotation));
	}

	void PrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::IcoSphere(size, recursionLevel, transformMatrix));
	}

	void PrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::IcoSphere(size, recursionLevel, position, rotation));
	}

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, transformMatrix));
	}

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, planeInfo));
	}

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, position, rotation));
	}

	void PrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::UVSphere(size, sliceCount, stackCount, transformMatrix));
	}

	void PrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::UVSphere(size, sliceCount, stackCount, position, rotation));
	}

	Primitive& PrimitiveList::GetPrimitive(unsigned int i)
	{
		#if NAZARA_CORE_SAFE
		if (i >= m_primitives.size())
		{
			NazaraError("Primitive index out of range (" + String::Number(i) + " >= " + String::Number(m_primitives.size()) + ')');

			static Primitive dummy;
			return dummy;
		}
		#endif

		return m_primitives[i];
	}

	const Primitive& PrimitiveList::GetPrimitive(unsigned int i) const
	{
		#if NAZARA_CORE_SAFE
		if (i >= m_primitives.size())
		{
			NazaraError("Primitive index out of range (" + String::Number(i) + " >= " + String::Number(m_primitives.size()) + ')');

			static Primitive dummy;
			return dummy;
		}
		#endif

		return m_primitives[i];
	}

	unsigned int PrimitiveList::GetSize() const
	{
		return m_primitives.size();
	}

	Primitive& PrimitiveList::operator()(unsigned int i)
	{
		return GetPrimitive(i);
	}

	const Primitive& PrimitiveList::operator()(unsigned int i) const
	{
		return GetPrimitive(i);
	}
}
