// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_AXISALIGNEDBOX_HPP
#define NAZARA_AXISALIGNEDBOX_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>

class NAZARA_API NzAxisAlignedBox
{
	public:
		NzAxisAlignedBox();
		NzAxisAlignedBox(const NzCubef& cube);
		NzAxisAlignedBox(const NzVector3f& vec1, const NzVector3f& vec2);
		NzAxisAlignedBox(nzExtend extend);

		bool Contains(const NzAxisAlignedBox& box);
		bool Contains(const NzVector3f& vector);

		void ExtendTo(const NzAxisAlignedBox& box);
		void ExtendTo(const NzVector3f& vector);

		NzVector3f GetCorner(nzCorner corner) const;
		NzCubef GetCube() const;
		nzExtend GetExtend() const;
		NzVector3f GetMaximum() const;
		NzVector3f GetMinimum() const;

		bool IsFinite() const;
		bool IsInfinite() const;
		bool IsNull() const;

		void SetInfinite();
		void SetExtends(const NzVector3f& vec1, const NzVector3f& vec2);
		void SetNull();

		NzString ToString() const;

		void Transform(const NzMatrix4f& matrix, bool applyTranslation = true);

		operator NzString() const;

		static NzAxisAlignedBox Lerp(const NzAxisAlignedBox& from, const NzAxisAlignedBox& to, float interpolation);

		static const NzAxisAlignedBox Infinite;
		static const NzAxisAlignedBox Null;

	private:
		nzExtend m_extend;
		NzCubef m_cube;
};

NAZARA_API std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox& aabb);

#endif // NAZARA_AXISALIGNEDBOX_HPP
