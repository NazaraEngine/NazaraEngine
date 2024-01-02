// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <memory>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JPH::ShapeSettings* JoltCollider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	template<typename T>
	const T* JoltCollider3D::GetShapeSettingsAs() const
	{
		return SafeCast<T*>(m_shapeSettings.get());
	}


	inline JoltTranslatedRotatedCollider3D::JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Vector3f& translation) :
	JoltTranslatedRotatedCollider3D(std::move(collider), translation, Quaternionf::Identity())
	{
	}

	inline JoltTranslatedRotatedCollider3D::JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Quaternionf& rotation) :
	JoltTranslatedRotatedCollider3D(std::move(collider), Vector3f::Zero(), rotation)
	{
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
