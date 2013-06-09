// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Renderer/Debug.hpp>

///TODO: Améliorer

namespace
{
	static NzColor primaryColor = NzColor::Red;
	static NzColor secondaryColor = NzColor::Green;
	static const NzShader* shader = nullptr;
	static NzVertexBuffer* vertexBuffer = nullptr;
	static NzVertexDeclaration* vertexDeclaration = nullptr;
	static bool depthBufferEnabled = true;
	static bool initialized = false;
	static float lineWidth = 1.5f;
	static float pointSize = 3.f;
	static int colorLocation = -1;
}

void NzDebugDrawer::Draw(const NzBoundingVolumef& volume)
{
	if (!volume.IsFinite())
		return;

	NzColor oldPrimaryColor = primaryColor;

	Draw(volume.aabb);

	primaryColor = secondaryColor;
	Draw(volume.obb);

	primaryColor = oldPrimaryColor;
}

void NzDebugDrawer::Draw(const NzBoxi& box)
{
	Draw(NzBoxf(box));
}

void NzDebugDrawer::Draw(const NzBoxf& box)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(mapper.GetPointer());

	NzVector3f max, min;
	max = box.GetMaximum();
	min = box.GetMinimum();

	vertex->position.Set(min.x, min.y, min.z);
	vertex++;
	vertex->position.Set(max.x, min.y, min.z);
	vertex++;

	vertex->position.Set(min.x, min.y, min.z);
	vertex++;
	vertex->position.Set(min.x, max.y, min.z);
	vertex++;

	vertex->position.Set(min.x, min.y, min.z);
	vertex++;
	vertex->position.Set(min.x, min.y, max.z);
	vertex++;

	vertex->position.Set(max.x, max.y, max.z);
	vertex++;
	vertex->position.Set(min.x, max.y, max.z);
	vertex++;

	vertex->position.Set(max.x, max.y, max.z);
	vertex++;
	vertex->position.Set(max.x, min.y, max.z);
	vertex++;

	vertex->position.Set(max.x, max.y, max.z);
	vertex++;
	vertex->position.Set(max.x, max.y, min.z);
	vertex++;

	vertex->position.Set(min.x, min.y, max.z);
	vertex++;
	vertex->position.Set(max.x, min.y, max.z);
	vertex++;

	vertex->position.Set(min.x, min.y, max.z);
	vertex++;
	vertex->position.Set(min.x, max.y, max.z);
	vertex++;

	vertex->position.Set(min.x, max.y, min.z);
	vertex++;
	vertex->position.Set(max.x, max.y, min.z);
	vertex++;

	vertex->position.Set(min.x, max.y, min.z);
	vertex++;
	vertex->position.Set(min.x, max.y, max.z);
	vertex++;

	vertex->position.Set(max.x, min.y, min.z);
	vertex++;
	vertex->position.Set(max.x, max.y, min.z);
	vertex++;

	vertex->position.Set(max.x, min.y, min.z);
	vertex++;
	vertex->position.Set(max.x, min.y, max.z);
	vertex++;

	mapper.Unmap();

	NzRenderer::Enable(nzRendererParameter_DepthBuffer, depthBufferEnabled);
	NzRenderer::SetLineWidth(lineWidth);
	NzRenderer::SetShader(shader);
	NzRenderer::SetVertexBuffer(vertexBuffer);

	shader->SendColor(colorLocation, primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzBoxui& box)
{
	Draw(NzBoxf(box));
}

void NzDebugDrawer::Draw(const NzFrustumf& frustum)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(mapper.GetPointer());

	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightBottom));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftBottom));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftTop));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearLeftTop));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(frustum.GetCorner(nzCorner_NearRightBottom));
	vertex++;
	vertex->position.Set(frustum.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	mapper.Unmap();

	NzRenderer::Enable(nzRendererParameter_DepthBuffer, depthBufferEnabled);
	NzRenderer::SetLineWidth(lineWidth);
	NzRenderer::SetShader(shader);
	NzRenderer::SetVertexBuffer(vertexBuffer);

	shader->SendColor(colorLocation, primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzOrientedBoxf& orientedBox)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(mapper.GetPointer());

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightBottom));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftBottom));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightTop));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftTop));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearLeftTop));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarLeftTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightTop));
	vertex++;

	vertex->position.Set(orientedBox.GetCorner(nzCorner_NearRightBottom));
	vertex++;
	vertex->position.Set(orientedBox.GetCorner(nzCorner_FarRightBottom));
	vertex++;

	mapper.Unmap();

	NzRenderer::Enable(nzRendererParameter_DepthBuffer, depthBufferEnabled);
	NzRenderer::SetLineWidth(lineWidth);
	NzRenderer::SetShader(shader);
	NzRenderer::SetVertexBuffer(vertexBuffer);

	shader->SendColor(colorLocation, primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzSkeleton* skeleton)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	unsigned int jointCount = skeleton->GetJointCount();
	if (vertexBuffer->GetVertexCount() < jointCount*2)
	{
		NazaraError("Debug buffer not length enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, jointCount*2);
	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(mapper.GetPointer());

	unsigned int vertexCount = 0;
	for (unsigned int i = 0; i < jointCount; ++i)
	{
		const NzNode* joint = skeleton->GetJoint(i);
		const NzNode* parent = joint->GetParent();
		if (parent)
		{
			vertex->position = joint->GetPosition();
			vertex++;

			vertex->position = parent->GetPosition();
			vertex++;

			vertexCount += 2;
		}
	}

	mapper.Unmap();

	if (vertexCount > 0)
	{
		NzRenderer::Enable(nzRendererParameter_DepthBuffer, depthBufferEnabled);
		NzRenderer::SetLineWidth(lineWidth);
		NzRenderer::SetPointSize(pointSize);
		NzRenderer::SetShader(shader);
		NzRenderer::SetVertexBuffer(vertexBuffer);

		shader->SendColor(colorLocation, primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, vertexCount);

		shader->SendColor(colorLocation, secondaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_PointList, 0, vertexCount);
	}
}
/*
void NzDebugDrawer::DrawNormals(const NzSubMesh* subMesh)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	unsigned int normalCount = subMesh->GetVertexCount();
	unsigned int vertexCount = normalCount*2;
	if (vertexBuffer->GetVertexCount() < vertexCount)
	{
		NazaraError("Debug buffer not length enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> inputMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
	NzBufferMapper<NzVertexBuffer> outputMapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);

	NzMeshVertex* inputVertex = reinterpret_cast<NzMeshVertex*>(inputMapper.GetPointer());
	NzVertexStruct_XYZ* outputVertex = reinterpret_cast<NzVertexStruct_XYZ*>(outputMapper.GetPointer());

	for (unsigned int i = 0; i < normalCount; ++i)
	{
		outputVertex->position = inputVertex->position;
		outputVertex++;

		outputVertex->position = inputVertex->position + inputVertex->normal;
		outputVertex++;

		inputVertex++;
	}

	inputMapper.Unmap();
	outputMapper.Unmap();

	if (vertexCount > 0)
	{
		const NzShader* oldShader = NzRenderer::GetShader();

		if (!NzRenderer::SetShader(shader))
		{
			NazaraError("Failed to set debug shader");
			return;
		}

		bool depthTestActive = NzRenderer::IsEnabled(nzRendererParameter_DepthTest);
		if (depthTestActive != depthTest)
			NzRenderer::Enable(nzRendererParameter_DepthTest, depthTest);

		NzRenderer::SetVertexBuffer(vertexBuffer);

		float oldLineWidth = NzRenderer::GetLineWidth();
		NzRenderer::SetLineWidth(lineWidth);

		shader->SendColor(colorLocation, primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveType_LineList, 0, vertexCount);

		NzRenderer::SetLineWidth(oldLineWidth);

		if (depthTestActive != depthTest)
			NzRenderer::Enable(nzRendererParameter_DepthTest, depthTestActive);

		if (!NzRenderer::SetShader(oldShader))
			NazaraWarning("Failed to reset shader");
	}
}

void NzDebugDrawer::DrawTangents(const NzSubMesh* subMesh)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	unsigned int tangentCount = subMesh->GetVertexCount();
	unsigned int vertexCount = tangentCount*2;
	if (vertexBuffer->GetVertexCount() < vertexCount)
	{
		NazaraError("Debug buffer not length enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> inputMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
	NzBufferMapper<NzVertexBuffer> outputMapper(vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);

	NzMeshVertex* inputVertex = reinterpret_cast<NzMeshVertex*>(inputMapper.GetPointer());
	NzVertexStruct_XYZ* outputVertex = reinterpret_cast<NzVertexStruct_XYZ*>(outputMapper.GetPointer());

	for (unsigned int i = 0; i < tangentCount; ++i)
	{
		outputVertex->position = inputVertex->position;
		outputVertex++;

		outputVertex->position = inputVertex->position + inputVertex->tangent;
		outputVertex++;

		inputVertex++;
	}

	inputMapper.Unmap();
	outputMapper.Unmap();

	if (vertexCount > 0)
	{
		const NzShader* oldShader = NzRenderer::GetShader();

		if (!NzRenderer::SetShader(shader))
		{
			NazaraError("Failed to set debug shader");
			return;
		}

		bool depthTestActive = NzRenderer::IsEnabled(nzRendererParameter_DepthTest);
		if (depthTestActive != depthTest)
			NzRenderer::Enable(nzRendererParameter_DepthTest, depthTest);

		NzRenderer::SetVertexBuffer(vertexBuffer);

		float oldLineWidth = NzRenderer::GetLineWidth();
		NzRenderer::SetLineWidth(lineWidth);

		shader->SendColor(colorLocation, primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveType_LineList, 0, vertexCount);

		NzRenderer::SetLineWidth(oldLineWidth);

		if (depthTestActive != depthTest)
			NzRenderer::Enable(nzRendererParameter_DepthTest, depthTestActive);

		if (!NzRenderer::SetShader(oldShader))
			NazaraWarning("Failed to reset shader");
	}
}
*/

void NzDebugDrawer::EnableDepthBuffer(bool depthBuffer)
{
	depthBufferEnabled = depthBuffer;
}

float NzDebugDrawer::GetLineWidth()
{
	return lineWidth;
}

float NzDebugDrawer::GetPointSize()
{
	return pointSize;
}

NzColor NzDebugDrawer::GetPrimaryColor()
{
	return primaryColor;
}

NzColor NzDebugDrawer::GetSecondaryColor()
{
	return secondaryColor;
}

bool NzDebugDrawer::Initialize()
{
	if (!initialized)
	{
		// Shader
		{
			shader = NzShaderBuilder::Get(nzShaderFlags_None);
			if (!shader)
			{
				NazaraError("Failed to build debug shader");
				return false;
			}

			colorLocation = shader->GetUniformLocation(nzShaderUniform_MaterialDiffuse);
		}

		// VertexDeclaration
		{
			NzVertexElement element;
			element.offset = 0;
			element.type = nzElementType_Float3;
			element.usage = nzElementUsage_Position;

			vertexDeclaration = new NzVertexDeclaration;
			if (!vertexDeclaration->Create(&element, 1))
			{
				NazaraError("Failed to create declaration");
				Uninitialize();

				return false;
			}
		}

		// VertexBuffer (Nécessite la déclaration)
		{
			vertexBuffer = new NzVertexBuffer(vertexDeclaration, 1024, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);
			if (!vertexBuffer->GetBuffer()->IsValid())
			{
				NazaraError("Failed to create buffer");
				Uninitialize();

				return false;
			}
		}

		initialized = true;
	}

	return true;
}

bool NzDebugDrawer::IsDepthBufferEnabled()
{
	return depthBufferEnabled;
}

void NzDebugDrawer::SetLineWidth(float width)
{
	lineWidth = width;
}

void NzDebugDrawer::SetPointSize(float size)
{
	pointSize = size;
}

void NzDebugDrawer::SetPrimaryColor(const NzColor& color)
{
	primaryColor = color;
}

void NzDebugDrawer::SetSecondaryColor(const NzColor& color)
{
	secondaryColor = color;
}

void NzDebugDrawer::Uninitialize()
{
	if (vertexBuffer)
	{
		delete vertexBuffer;
		vertexBuffer = nullptr;
	}

	if (vertexDeclaration)
	{
		delete vertexDeclaration;
		vertexDeclaration = nullptr;
	}

	initialized = false;
}
