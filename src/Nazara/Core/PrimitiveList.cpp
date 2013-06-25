// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

void NzPrimitiveList::AddBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& transformMatrix)
{
	m_primitives.push_back(NzPrimitive::Box(lengths, subdivision, transformMatrix));
}

void NzPrimitiveList::AddBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	m_primitives.push_back(NzPrimitive::Box(lengths, subdivision, position, rotation));
}

void NzPrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& transformMatrix)
{
	m_primitives.push_back(NzPrimitive::CubicSphere(size, subdivision, transformMatrix));
}

void NzPrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	m_primitives.push_back(NzPrimitive::CubicSphere(size, subdivision, position, rotation));
}

void NzPrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& transformMatrix)
{
	m_primitives.push_back(NzPrimitive::IcoSphere(size, recursionLevel, transformMatrix));
}

void NzPrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation)
{
	m_primitives.push_back(NzPrimitive::IcoSphere(size, recursionLevel, position, rotation));
}

void NzPrimitiveList::AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix)
{
	m_primitives.push_back(NzPrimitive::Plane(size, subdivision, transformMatrix));
}

void NzPrimitiveList::AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& planeInfo)
{
	m_primitives.push_back(NzPrimitive::Plane(size, subdivision, planeInfo));
}

void NzPrimitiveList::AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	m_primitives.push_back(NzPrimitive::Plane(size, subdivision, position, rotation));
}

void NzPrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& transformMatrix)
{
	m_primitives.push_back(NzPrimitive::UVSphere(size, sliceCount, stackCount, transformMatrix));
}

void NzPrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation)
{
	m_primitives.push_back(NzPrimitive::UVSphere(size, sliceCount, stackCount, position, rotation));
}

NzPrimitive& NzPrimitiveList::GetPrimitive(unsigned int i)
{
	#if NAZARA_CORE_SAFE
	if (i >= m_primitives.size())
	{
		NazaraError("Primitive index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_primitives.size()) + ')');

		static NzPrimitive dummy;
		return dummy;
	}
	#endif

	return m_primitives[i];
}

const NzPrimitive& NzPrimitiveList::GetPrimitive(unsigned int i) const
{
	#if NAZARA_CORE_SAFE
	if (i >= m_primitives.size())
	{
		NazaraError("Primitive index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_primitives.size()) + ')');

		static NzPrimitive dummy;
		return dummy;
	}
	#endif

	return m_primitives[i];
}

unsigned int NzPrimitiveList::GetSize() const
{
	return m_primitives.size();
}

NzPrimitive& NzPrimitiveList::operator()(unsigned int i)
{
	return GetPrimitive(i);
}

const NzPrimitive& NzPrimitiveList::operator()(unsigned int i) const
{
	return GetPrimitive(i);
}
