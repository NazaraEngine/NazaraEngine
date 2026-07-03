// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void FramePipeline::UpdateInstanceData(UInt32 instanceIndex, const Matrix4f& worldMatrix)
	{
		Matrix4f inverseWorldMatrix;
		worldMatrix.GetInverseTransform(&inverseWorldMatrix);

		UpdateInstanceData(instanceIndex, worldMatrix, inverseWorldMatrix);
	}
}
