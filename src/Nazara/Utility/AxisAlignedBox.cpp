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

NzAxisAlignedBox::NzAxisAlignedBox(const NzCubef& cube) :
m_extend(nzExtend_Finite),
m_cube(cube)
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

bool NzAxisAlignedBox::Contains(const NzVector3f& vector)
{
	switch (m_extend)
	{
		case nzExtend_Finite:
			return m_cube.Contains(vector);

		case nzExtend_Infinite:
			return true;

		case nzExtend_Null:
			return false;
	}

	NazaraError("Extend type not handled (0x" + NzString::Number(m_extend, 16) + ')');
	return false;
}

void NzAxisAlignedBox::ExtendTo(const NzAxisAlignedBox& box)
{
	switch (m_extend)
	{
		case nzExtend_Finite:
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
			}
			break;

		case nzExtend_Infinite:
			// Rien à faire
			break;

		case nzExtend_Null:
			operator=(box);
			break;
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
			m_extend = nzExtend_Finite;
			m_cube.Set(vector, vector);
			break;
	}
}

NzVector3f NzAxisAlignedBox::GetCorner(nzCorner corner) const
{
	switch (m_extend)
	{
		case nzExtend_Finite:
			return m_cube.GetCorner(corner);

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis dont le signe dépend du coin
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no corner");
			return NzVector3f();

		case nzExtend_Null:
			return NzVector3f::Zero();
	}

	NazaraError("Extend type not handled (0x" + NzString::Number(m_extend, 16) + ')');
	return NzVector3f();
}

NzCubef NzAxisAlignedBox::GetCube() const
{
	return m_cube;
}

nzExtend NzAxisAlignedBox::GetExtend() const
{
	return m_extend;
}

NzVector3f NzAxisAlignedBox::GetMaximum() const
{
	return m_cube.GetPosition() + m_cube.GetSize();
}

NzVector3f NzAxisAlignedBox::GetMinimum() const
{
	return m_cube.GetPosition();
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

void NzAxisAlignedBox::Transform(const NzMatrix4f& matrix, bool applyTranslation)
{
	if (m_extend != nzExtend_Finite)
		return;

	NzVector3f center = matrix.Transform(m_cube.GetCenter(), (applyTranslation) ? 1.f : 0.f); // Valeur multipliant la translation
	NzVector3f halfSize = m_cube.GetSize() * 0.5f;

	halfSize.Set(std::fabs(matrix(0,0))*halfSize.x + std::fabs(matrix(1,0))*halfSize.y + std::fabs(matrix(2,0))*halfSize.z,
	             std::fabs(matrix(0,1))*halfSize.x + std::fabs(matrix(1,1))*halfSize.y + std::fabs(matrix(2,1))*halfSize.z,
	             std::fabs(matrix(0,2))*halfSize.x + std::fabs(matrix(1,2))*halfSize.y + std::fabs(matrix(2,2))*halfSize.z);

	m_cube.Set(center - halfSize, center + halfSize);
}

NzAxisAlignedBox::operator NzString() const
{
	return ToString();
}

NzAxisAlignedBox NzAxisAlignedBox::Lerp(const NzAxisAlignedBox& from, const NzAxisAlignedBox& to, float interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Null;
	}
	#endif

	if (NzNumberEquals(interpolation, 0.f))
		return from;

	if (NzNumberEquals(interpolation, 1.f))
		return to;

	switch (to.m_extend)
	{
		case nzExtend_Finite:
		{
			switch (from.m_extend)
			{
				case nzExtend_Finite:
					return NzCubef::Lerp(from.m_cube, to.m_cube, interpolation);

				case nzExtend_Infinite:
					return Infinite;

				case nzExtend_Null:
					return from.m_cube * interpolation;
			}
		}

		case nzExtend_Infinite:
			return Infinite; // Un petit peu d'infini est infini quand même ;)

		case nzExtend_Null:
		{
			switch (from.m_extend)
			{
				case nzExtend_Finite:
					return from.m_cube * (1.f - interpolation);

				case nzExtend_Infinite:
					return Infinite;

				case nzExtend_Null:
					return Null;
			}
		}
	}

	return Null;
}

const NzAxisAlignedBox NzAxisAlignedBox::Infinite(nzExtend_Infinite);
const NzAxisAlignedBox NzAxisAlignedBox::Null(nzExtend_Null);

std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox& aabb)
{
	out << aabb.ToString();
	return out;
}
