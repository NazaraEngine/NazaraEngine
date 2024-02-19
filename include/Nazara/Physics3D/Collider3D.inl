// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <memory>

namespace Nz
{
	inline JPH::ShapeSettings* Collider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	template<typename T>
	const T* Collider3D::GetShapeSettingsAs() const
	{
		return SafeCast<T*>(m_shapeSettings.get());
	}


	inline TranslatedRotatedCollider3D::TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Vector3f& translation) :
	TranslatedRotatedCollider3D(std::move(collider), translation, Quaternionf::Identity())
	{
	}

	inline TranslatedRotatedCollider3D::TranslatedRotatedCollider3D(std::shared_ptr<Collider3D> collider, const Quaternionf& rotation) :
	TranslatedRotatedCollider3D(std::move(collider), Vector3f::Zero(), rotation)
	{
	}
}

