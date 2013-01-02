// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Utility/AxisAlignedBox.hpp>
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
	static bool depthTest = true;
	static bool initialized = false;
	static float lineWidth = 1.5f;
	static float pointSize = 3.f;
	static int colorLocation = -1;
}

void NzDebugDrawer::Draw(const NzAxisAlignedBox& aabb)
{
	if (!aabb.IsFinite())
		return;

	Draw(aabb.GetCube());
}

void NzDebugDrawer::Draw(const NzCubei& cube)
{
	Draw(NzCubef(cube));
}

void NzDebugDrawer::Draw(const NzCubef& cube)
{
	if (!initialized)
	{
		NazaraError("Debug drawer is not initialized");
		return;
	}

	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(vertexBuffer->Map(nzBufferAccess_DiscardAndWrite, 0, 24));
	if (!vertex)
	{
		NazaraError("Failed to map buffer");
		return;
	}

	NzVector3f max, min;
	max = cube.GetPosition() + cube.GetSize();
	min = cube.GetPosition();

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

	vertexBuffer->Unmap();

	const NzShader* oldShader = NzRenderer::GetShader();

	if (!NzRenderer::SetShader(shader))
	{
		NazaraError("Failed to set debug shader");
		return;
	}

	bool depthTestActive = NzRenderer::IsEnabled(nzRendererParameter_DepthTest);
	if (depthTestActive != depthTest)
		NzRenderer::Enable(nzRendererParameter_DepthTest, depthTest);

	float oldLineWidth = NzRenderer::GetLineWidth();
	NzRenderer::SetLineWidth(lineWidth);

	NzRenderer::SetVertexBuffer(vertexBuffer);

	shader->SendColor(colorLocation, primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveType_LineList, 0, 24);

	NzRenderer::SetLineWidth(oldLineWidth);

	if (depthTestActive != depthTest)
		NzRenderer::Enable(nzRendererParameter_DepthTest, depthTestActive);

	if (!NzRenderer::SetShader(oldShader))
		NazaraWarning("Failed to reset shader");
}

void NzDebugDrawer::Draw(const NzCubeui& cube)
{
	Draw(NzCubef(cube));
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

	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(vertexBuffer->Map(nzBufferAccess_DiscardAndWrite, 0, jointCount*2));
	if (!vertex)
	{
		NazaraError("Failed to map buffer");
		return;
	}

	unsigned int vertexCount = 0;
	for (unsigned int i = 0; i < jointCount; ++i)
	{
		const NzNode* joint = skeleton->GetJoint(i);
		const NzNode* parent = joint->GetParent();
		if (parent)
		{
			vertex->position = joint->GetDerivedTranslation();
			vertex++;

			vertex->position = parent->GetDerivedTranslation();
			vertex++;

			vertexCount += 2;
		}
	}

	vertexBuffer->Unmap();

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

		float oldPointSize = NzRenderer::GetPointSize();
		NzRenderer::SetPointSize(pointSize);

		shader->SendColor(colorLocation, secondaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveType_PointList, 0, vertexCount);

		NzRenderer::SetLineWidth(oldLineWidth);
		NzRenderer::SetPointSize(oldPointSize);

		if (depthTestActive != depthTest)
			NzRenderer::Enable(nzRendererParameter_DepthTest, depthTestActive);

		if (!NzRenderer::SetShader(oldShader))
			NazaraWarning("Failed to reset shader");
	}
}

bool NzDebugDrawer::Initialize()
{
	if (!initialized)
	{
		// Shader
		{
			shader = NzShaderBuilder::Get(nzShaderBuilder_None);
			if (!shader)
			{
				NazaraError("Failed to build debug shader");
				return false;
			}

			colorLocation = shader->GetUniformLocation("DiffuseColor");
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
			vertexBuffer = new NzVertexBuffer(vertexDeclaration, 256, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);
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

bool NzDebugDrawer::GetDepthTest()
{
	return depthTest;
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

void NzDebugDrawer::SetDepthTest(bool shouldTest)
{
	depthTest = shouldTest;
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
