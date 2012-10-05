// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AxisAlignedBox.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAxisAlignedBox::NzAxisAlignedBox() :
m_extend(nzExtend_Null)
{
}

NzAxisAlignedBox::NzAxisAlignedBox(const NzVector3f& vec1, const NzVector3f& vec2) :
m_extend(nzExtend_Finite),
m_cube(vec1, vec2)
{
}

NzAxisAlignedBox::NzAxisAlignedBox(nzExtend extend) :
m_extend(extend)
{
}

bool NzAxisAlignedBox::Contains(const NzAxisAlignedBox& box)
{
	if (m_extend == nzExtend_Null || box.m_extend == nzExtend_Null)
		return false;
	else if (m_extend == nzExtend_Infinite || box.m_extend == nzExtend_Infinite)
		return true;

	return m_cube.Contains(box.m_cube);
}

void NzAxisAlignedBox::ExtendTo(const NzAxisAlignedBox& box)
{
	switch (m_extend)
	{
		case nzExtend_Finite:
		{
			switch (box.m_extend)
			{
				case nzExtend_Finite:
					m_cube.ExtendTo(box.m_cube);
					break;

				case nzExtend_Infinite:
					SetInfinite();
					break;

				case nzExtend_Null:
					break;

				break;
			}

			case nzExtend_Infinite:
				// Rien à faire
				break;

			case nzExtend_Null:
				operator=(box);
				break;
		}
	}
}

void NzAxisAlignedBox::ExtendTo(const NzVector3f& vector)
{
	switch (m_extend)
	{
		case nzExtend_Finite:
			m_cube.ExtendTo(vector);
			break;

		case nzExtend_Infinite:
			// Rien à faire
			break;

		case nzExtend_Null:
			// Nous étendons l'AABB en la construisant de l'origine jusqu'au vecteur
			m_cube.x = 0.f;
			m_cube.y = 0.f;
			m_cube.z = 0.f;
			m_cube.width = std::fabs(vector.x);
			m_cube.height = std::fabs(vector.y);
			m_cube.depth = std::fabs(vector.z);
			break;
	}
}

nzExtend NzAxisAlignedBox::GetExtend() const
{
	return m_extend;
}

NzVector3f NzAxisAlignedBox::GetMaximum() const
{
	return NzVector3f(m_cube.x+m_cube.width, m_cube.y+m_cube.height, m_cube.z+m_cube.depth);
}

NzVector3f NzAxisAlignedBox::GetMinimum() const
{
	return NzVector3f(m_cube.x, m_cube.y, m_cube.z);
}

bool NzAxisAlignedBox::IsFinite() const
{
	return m_extend == nzExtend_Finite;
}

bool NzAxisAlignedBox::IsInfinite() const
{
	return m_extend == nzExtend_Infinite;
}

bool NzAxisAlignedBox::IsNull() const
{
	return m_extend == nzExtend_Null;
}

void NzAxisAlignedBox::SetInfinite()
{
	m_extend = nzExtend_Infinite;
}

void NzAxisAlignedBox::SetExtends(const NzVector3f& vec1, const NzVector3f& vec2)
{
	m_extend = nzExtend_Finite;
	m_cube.Set(vec1, vec2);
}

void NzAxisAlignedBox::SetNull()
{
	m_extend = nzExtend_Null;
}

NzString NzAxisAlignedBox::ToString() const
{
	switch (m_extend)
	{
		case nzExtend_Finite:
			return "NzAxisAlignedBox(min=" + GetMinimum().ToString() + ", max=" + GetMaximum().ToString() + ')';

		case nzExtend_Infinite:
			return "NzAxisAlignedBox(Infinite)";

		case nzExtend_Null:
			return "NzAxisAlignedBox(Null)";
	}

	return "NzAxisAlignedBox(ERROR)";
}

const NzAxisAlignedBox NzAxisAlignedBox::Infinite(nzExtend_Infinite);
const NzAxisAlignedBox NzAxisAlignedBox::Null(nzExtend_Null);

std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox& aabb)
{
	out << aabb.ToString();
	return out;
}
