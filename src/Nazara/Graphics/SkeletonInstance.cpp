// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	SkeletonInstance::SkeletonInstance(std::shared_ptr<const Skeleton> skeleton) :
	m_skeleton(std::move(skeleton)),
	m_dataInvalided(true)
	{
		NazaraAssert(m_skeleton, "invalid skeleton");

		m_skeletalDataBuffer = Graphics::Instance()->GetRenderDevice()->InstantiateBuffer(BufferType::Uniform, PredefinedSkeletalOffsets.totalSize, BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
		m_skeletalDataBuffer->UpdateDebugName("Skeletal data");

		m_onSkeletonJointsInvalidated.Connect(m_skeleton->OnSkeletonJointsInvalidated, [this](const Skeleton*)
		{
			m_dataInvalided = true;
			OnTransferRequired(this);
		});
	}

	SkeletonInstance::SkeletonInstance(SkeletonInstance&& skeletonInstance) noexcept :
	m_skeletalDataBuffer(std::move(skeletonInstance.m_skeletalDataBuffer)),
	m_skeleton(std::move(skeletonInstance.m_skeleton)),
	m_dataInvalided(skeletonInstance.m_dataInvalided)
	{
		m_onSkeletonJointsInvalidated.Connect(m_skeleton->OnSkeletonJointsInvalidated, [this](const Skeleton*)
		{
			m_dataInvalided = true;
			OnTransferRequired(this);
		});
	}

	void SkeletonInstance::OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder)
	{
		if (!m_dataInvalided)
			return;

		auto& allocation = renderResources.GetUploadPool().Allocate(m_skeletalDataBuffer->GetSize());
		Matrix4f* matrices = AccessByOffset<Matrix4f*>(allocation.mappedPtr, PredefinedSkeletalOffsets.jointMatricesOffset);

		for (std::size_t i = 0; i < m_skeleton->GetJointCount(); ++i)
			matrices[i] = m_skeleton->GetJoint(i)->GetSkinningMatrix();

		builder.CopyBuffer(allocation, m_skeletalDataBuffer.get());

		m_dataInvalided = false;
	}

	SkeletonInstance& SkeletonInstance::operator=(SkeletonInstance&& skeletonInstance) noexcept
	{
		m_skeletalDataBuffer = std::move(skeletonInstance.m_skeletalDataBuffer);
		m_skeleton = std::move(skeletonInstance.m_skeleton);
		m_dataInvalided = skeletonInstance.m_dataInvalided;

		m_onSkeletonJointsInvalidated.Connect(m_skeleton->OnSkeletonJointsInvalidated, [this](const Skeleton*)
		{
			m_dataInvalided = true;
			OnTransferRequired(this);
		});

		return *this;
	}
}
