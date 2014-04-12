// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

///TODO: Améliorer

namespace
{
	static NzShader* s_shader = nullptr;
	static NzColor s_primaryColor;
	static NzColor s_secondaryColor;
	static NzRenderStates s_renderStates;
	static NzVertexBuffer s_vertexBuffer;
	static bool s_initialized = false;
	static int s_colorLocation = -1;
}

void NzDebugDrawer::Draw(const NzBoundingVolumef& volume)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	if (!volume.IsFinite())
		return;

	NzColor oldPrimaryColor = s_primaryColor;

	Draw(volume.aabb);

	s_primaryColor = s_secondaryColor;
	Draw(volume.obb);

	s_primaryColor = oldPrimaryColor;
}

void NzDebugDrawer::Draw(const NzBoxi& box)
{
	Draw(NzBoxf(box));
}

void NzDebugDrawer::Draw(const NzBoxf& box)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
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

	NzRenderer::SetRenderStates(s_renderStates);
	NzRenderer::SetShader(s_shader);
	NzRenderer::SetVertexBuffer(&s_vertexBuffer);

	s_shader->SendColor(s_colorLocation, s_primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzBoxui& box)
{
	Draw(NzBoxf(box));
}

void NzDebugDrawer::Draw(const NzFrustumf& frustum)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
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

	NzRenderer::SetRenderStates(s_renderStates);
	NzRenderer::SetShader(s_shader);
	NzRenderer::SetVertexBuffer(&s_vertexBuffer);

	s_shader->SendColor(s_colorLocation, s_primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzOrientedBoxf& orientedBox)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 24);
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

	NzRenderer::SetRenderStates(s_renderStates);
	NzRenderer::SetShader(s_shader);
	NzRenderer::SetVertexBuffer(&s_vertexBuffer);

	s_shader->SendColor(s_colorLocation, s_primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 24);
}

void NzDebugDrawer::Draw(const NzSkeleton* skeleton)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	unsigned int jointCount = skeleton->GetJointCount();
	if (s_vertexBuffer.GetVertexCount() < jointCount*2)
	{
		NazaraError("Debug buffer not large enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> mapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, jointCount*2);
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
		NzRenderer::SetRenderStates(s_renderStates);
		NzRenderer::SetShader(s_shader);
		NzRenderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, vertexCount);

		s_shader->SendColor(s_colorLocation, s_secondaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_PointList, 0, vertexCount);
	}
}

void NzDebugDrawer::DrawBinormals(const NzStaticMesh* subMesh)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	unsigned int normalCount = subMesh->GetVertexCount();
	unsigned int vertexCount = normalCount*2;
	if (s_vertexBuffer.GetVertexCount() < vertexCount)
	{
		NazaraError("Debug buffer not large enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> inputMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
	NzBufferMapper<NzVertexBuffer> outputMapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);

	NzMeshVertex* inputVertex = reinterpret_cast<NzMeshVertex*>(inputMapper.GetPointer());
	NzVertexStruct_XYZ* outputVertex = reinterpret_cast<NzVertexStruct_XYZ*>(outputMapper.GetPointer());

	for (unsigned int i = 0; i < normalCount; ++i)
	{
		outputVertex->position = inputVertex->position;
		outputVertex++;

		outputVertex->position = inputVertex->position + NzVector3f::CrossProduct(inputVertex->normal, inputVertex->tangent)*0.01f;
		outputVertex++;

		inputVertex++;
	}

	inputMapper.Unmap();
	outputMapper.Unmap();

	if (vertexCount > 0)
	{
		NzRenderer::SetRenderStates(s_renderStates);
		NzRenderer::SetShader(s_shader);
		NzRenderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, vertexCount);
	}
}

void NzDebugDrawer::DrawCone(const NzVector3f& origin, const NzQuaternionf& rotation, float angle, float length)
{
	if (!Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	NzMatrix4f transformMatrix;
	transformMatrix.MakeIdentity();
	transformMatrix.SetRotation(rotation);
	transformMatrix.SetTranslation(origin);

	NzBufferMapper<NzVertexBuffer> mapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, 16);
	NzVertexStruct_XYZ* vertex = reinterpret_cast<NzVertexStruct_XYZ*>(mapper.GetPointer());

	// On calcule le reste des points
	NzVector3f base(NzVector3f::Forward()*length);

	// Il nous faut maintenant le rayon du cercle projeté à cette distance
	// Tangente = Opposé/Adjaçent <=> Opposé = Adjaçent*Tangente
	float radius = length*std::tan(NzDegreeToRadian(angle));
	NzVector3f lExtend = NzVector3f::Left()*radius;
	NzVector3f uExtend = NzVector3f::Up()*radius;

	vertex->position.Set(transformMatrix * NzVector3f::Zero());
	vertex++;
	vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
	vertex++;
	vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * NzVector3f::Zero());
	vertex++;
	vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
	vertex++;
	vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * NzVector3f::Zero());
	vertex++;
	vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
	vertex++;
	vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * NzVector3f::Zero());
	vertex++;
	vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
	vertex++;

	vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
	vertex++;
	vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
	vertex++;

	mapper.Unmap();

	NzRenderer::SetRenderStates(s_renderStates);
	NzRenderer::SetShader(s_shader);
	NzRenderer::SetVertexBuffer(&s_vertexBuffer);

	s_shader->SendColor(s_colorLocation, s_primaryColor);

	NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, 16);
}

void NzDebugDrawer::DrawNormals(const NzStaticMesh* subMesh)
{
	if (!s_initialized && !Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	unsigned int normalCount = subMesh->GetVertexCount();
	unsigned int vertexCount = normalCount*2;
	if (s_vertexBuffer.GetVertexCount() < vertexCount)
	{
		NazaraError("Debug buffer not large enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> inputMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
	NzBufferMapper<NzVertexBuffer> outputMapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);

	NzMeshVertex* inputVertex = reinterpret_cast<NzMeshVertex*>(inputMapper.GetPointer());
	NzVertexStruct_XYZ* outputVertex = reinterpret_cast<NzVertexStruct_XYZ*>(outputMapper.GetPointer());

	for (unsigned int i = 0; i < normalCount; ++i)
	{
		outputVertex->position = inputVertex->position;
		outputVertex++;

		outputVertex->position = inputVertex->position + inputVertex->normal*0.01f;
		outputVertex++;

		inputVertex++;
	}

	inputMapper.Unmap();
	outputMapper.Unmap();

	if (vertexCount > 0)
	{
		NzRenderer::SetRenderStates(s_renderStates);
		NzRenderer::SetShader(s_shader);
		NzRenderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, vertexCount);
	}
}

void NzDebugDrawer::DrawTangents(const NzStaticMesh* subMesh)
{
	if (!s_initialized && !Initialize())
	{
		NazaraError("Failed to initialize Debug Drawer");
		return;
	}

	unsigned int tangentCount = subMesh->GetVertexCount();
	unsigned int vertexCount = tangentCount*2;
	if (s_vertexBuffer.GetVertexCount() < vertexCount)
	{
		NazaraError("Debug buffer not large enougth to draw object");
		return;
	}

	NzBufferMapper<NzVertexBuffer> inputMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadOnly);
	NzBufferMapper<NzVertexBuffer> outputMapper(s_vertexBuffer, nzBufferAccess_DiscardAndWrite, 0, vertexCount);

	NzMeshVertex* inputVertex = reinterpret_cast<NzMeshVertex*>(inputMapper.GetPointer());
	NzVertexStruct_XYZ* outputVertex = reinterpret_cast<NzVertexStruct_XYZ*>(outputMapper.GetPointer());

	for (unsigned int i = 0; i < tangentCount; ++i)
	{
		outputVertex->position = inputVertex->position;
		outputVertex++;

		outputVertex->position = inputVertex->position + inputVertex->tangent*0.01f;
		outputVertex++;

		inputVertex++;
	}

	inputMapper.Unmap();
	outputMapper.Unmap();

	if (vertexCount > 0)
	{
		NzRenderer::SetRenderStates(s_renderStates);
		NzRenderer::SetShader(s_shader);
		NzRenderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);
		NzRenderer::DrawPrimitives(nzPrimitiveMode_LineList, 0, vertexCount);
	}
}

void NzDebugDrawer::EnableDepthBuffer(bool depthBuffer)
{
	s_renderStates.parameters[nzRendererParameter_DepthBuffer] = depthBuffer;
}

float NzDebugDrawer::GetLineWidth()
{
	return s_renderStates.lineWidth;
}

float NzDebugDrawer::GetPointSize()
{
	return s_renderStates.pointSize;
}

NzColor NzDebugDrawer::GetPrimaryColor()
{
	return s_primaryColor;
}

NzColor NzDebugDrawer::GetSecondaryColor()
{
	return s_secondaryColor;
}

bool NzDebugDrawer::Initialize()
{
	if (!s_initialized)
	{
		// s_shader
		s_shader = NzShaderLibrary::Get("DebugSimple");
		s_colorLocation = s_shader->GetUniformLocation("Color");

		// s_vertexBuffer
		try
		{
			NzErrorFlags flags(nzErrorFlag_ThrowException);
			s_vertexBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XYZ), 65365, nzBufferStorage_Hardware, nzBufferUsage_Dynamic);
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to create buffer: " + NzString(e.what()));

			Uninitialize();
			return false;
		}

		s_primaryColor = NzColor::Red;
		s_renderStates.parameters[nzRendererParameter_DepthBuffer] = true;
		s_secondaryColor = NzColor::Green;

		s_initialized = true;
	}

	return true;
}

bool NzDebugDrawer::IsDepthBufferEnabled()
{
	return s_renderStates.parameters[nzRendererParameter_DepthBuffer];
}

void NzDebugDrawer::SetLineWidth(float width)
{
	s_renderStates.lineWidth = width;
}

void NzDebugDrawer::SetPointSize(float size)
{
	s_renderStates.pointSize = size;
}

void NzDebugDrawer::SetPrimaryColor(const NzColor& color)
{
	s_primaryColor = color;
}

void NzDebugDrawer::SetSecondaryColor(const NzColor& color)
{
	s_secondaryColor = color;
}

void NzDebugDrawer::Uninitialize()
{
	s_shader = nullptr;
	s_vertexBuffer.Reset();
	s_initialized = false;
}
