// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Graphics/Debug.hpp>

bool NzRenderQueue::MaterialComparator::operator()(const NzMaterial* mat1, const NzMaterial* mat2)
{
	const NzShader* shader1 = mat1->GetCustomShader();
	const NzShader* shader2 = mat2->GetCustomShader();

	if (shader1)
	{
		if (shader2)
			return shader1 < shader2;
		else
			return true;
	}
	else if (shader2)
		return false;
	else
	{
		nzUInt32 shaderFlags1 = mat1->GetShaderFlags();
		nzUInt32 shaderFlags2 = mat2->GetShaderFlags();

		if (shaderFlags1 == shaderFlags2)
			return mat1 < mat2;
		else
			return shaderFlags1 < shaderFlags2;
	}
}

bool NzRenderQueue::SkeletalMeshComparator::operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2)
{
	const NzBuffer* buffer1;
	const NzIndexBuffer* iBuffer1 = subMesh1->GetIndexBuffer();
	if (iBuffer1)
		buffer1 = iBuffer1->GetBuffer();
	else
		buffer1 = nullptr;

	const NzBuffer* buffer2;
	const NzIndexBuffer* iBuffer2 = subMesh1->GetIndexBuffer();
	if (iBuffer2)
		buffer2 = iBuffer1->GetBuffer();
	else
		buffer2 = nullptr;

	if (buffer1 == buffer2)
		return subMesh1 < subMesh2;
	else
		return buffer2 < buffer2;
}

bool NzRenderQueue::StaticMeshComparator::operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2)
{
	const NzBuffer* buffer1;
	const NzIndexBuffer* iBuffer1 = subMesh1->GetIndexBuffer();
	if (iBuffer1)
		buffer1 = iBuffer1->GetBuffer();
	else
		buffer1 = nullptr;

	const NzBuffer* buffer2;
	const NzIndexBuffer* iBuffer2 = subMesh1->GetIndexBuffer();
	if (iBuffer2)
		buffer2 = iBuffer1->GetBuffer();
	else
		buffer2 = nullptr;

	if (iBuffer1 == iBuffer2)
	{
		buffer1 = subMesh1->GetVertexBuffer()->GetBuffer();
		buffer2 = subMesh2->GetVertexBuffer()->GetBuffer();

		if (buffer1 == buffer2)
			return subMesh1 < subMesh2;
		else
			return buffer1 < buffer2;
	}
	else
		return iBuffer1 < iBuffer2;
}

void NzRenderQueue::Clear()
{
	directionnalLights.clear();
	otherDrawables.clear();
	visibleLights.clear();
	visibleSkeletalModels.clear();
	visibleStaticModels.clear();
}
