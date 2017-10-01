// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Renderer/Debug.hpp>

///TODO: Améliorer

namespace Nz
{
	namespace
	{
		static Shader* s_shader = nullptr;
		static Color s_primaryColor;
		static Color s_secondaryColor;
		static RenderStates s_renderStates;
		static VertexBuffer s_vertexBuffer;
		static bool s_initialized = false;
		static int s_colorLocation = -1;
	}

	void DebugDrawer::Draw(const BoundingVolumef& volume)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		if (!volume.IsFinite())
			return;

		Color oldPrimaryColor = s_primaryColor;

		Draw(volume.aabb);

		s_primaryColor = s_secondaryColor;
		Draw(volume.obb);

		s_primaryColor = oldPrimaryColor;
	}

	void DebugDrawer::Draw(const Boxi& box)
	{
		Draw(Boxf(box));
	}

	void DebugDrawer::Draw(const Boxf& box)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		BufferMapper<VertexBuffer> mapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, 24);
		VertexStruct_XYZ* vertex = static_cast<VertexStruct_XYZ*>(mapper.GetPointer());

		Vector3f max, min;
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

		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);

		Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, 24);
	}

	void DebugDrawer::Draw(const Boxui& box)
	{
		Draw(Boxf(box));
	}

	void DebugDrawer::Draw(const Frustumf& frustum)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		BufferMapper<VertexBuffer> mapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, 24);
		VertexStruct_XYZ* vertex = static_cast<VertexStruct_XYZ*>(mapper.GetPointer());

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightBottom));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftTop));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearLeftTop));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(frustum.GetCorner(BoxCorner_NearRightBottom));
		vertex++;
		vertex->position.Set(frustum.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		mapper.Unmap();

		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);

		Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, 24);
	}

	void DebugDrawer::Draw(const OrientedBoxf& orientedBox)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		BufferMapper<VertexBuffer> mapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, 24);
		VertexStruct_XYZ* vertex = static_cast<VertexStruct_XYZ*>(mapper.GetPointer());

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightBottom));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightTop));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftTop));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearLeftTop));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarLeftTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightTop));
		vertex++;

		vertex->position.Set(orientedBox.GetCorner(BoxCorner_NearRightBottom));
		vertex++;
		vertex->position.Set(orientedBox.GetCorner(BoxCorner_FarRightBottom));
		vertex++;

		mapper.Unmap();

		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);

		Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, 24);
	}

	void DebugDrawer::Draw(const Skeleton* skeleton)
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

		BufferMapper<VertexBuffer> mapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, jointCount*2);
		VertexStruct_XYZ* vertex = static_cast<VertexStruct_XYZ*>(mapper.GetPointer());

		unsigned int vertexCount = 0;
		for (unsigned int i = 0; i < jointCount; ++i)
		{
			const Node* joint = skeleton->GetJoint(i);
			const Node* parent = joint->GetParent();
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
			Renderer::SetRenderStates(s_renderStates);
			Renderer::SetShader(s_shader);
			Renderer::SetVertexBuffer(&s_vertexBuffer);

			s_shader->SendColor(s_colorLocation, s_primaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, vertexCount);

			s_shader->SendColor(s_colorLocation, s_secondaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_PointList, 0, vertexCount);
		}
	}

	void DebugDrawer::Draw(const Vector3f& position, float size)
	{
		Draw(Boxf(position.x - size*0.5f, position.y - size*0.5f, position.z - size*0.5f, size, size, size));
	}

	void DebugDrawer::DrawAxes(const Vector3f& position, float size)
	{
		Color oldPrimaryColor = s_primaryColor;
		s_primaryColor = Color::Red;
		DrawLine(position, position + Vector3f::UnitX() * 3.f * size / 4.f);
		s_primaryColor = Color::Green;
		DrawLine(position, position + Vector3f::UnitY() * 3.f * size / 4.f);
		s_primaryColor = Color::Blue;
		DrawLine(position, position + Vector3f::UnitZ() * 3.f * size / 4.f);

		s_primaryColor = Color::Red;
		DrawCone(position + Vector3f::UnitX() * size, EulerAnglesf(0.f, 90.f, 0.f), 15, size / 4.f);
		s_primaryColor = Color::Green;
		DrawCone(position + Vector3f::UnitY() * size, EulerAnglesf(-90.f, 0.f, 0.f), 15, size / 4.f);
		s_primaryColor = Color::Blue;
		DrawCone(position + Vector3f::UnitZ() * size, EulerAnglesf(0.f, 0.f, 0.f), 15, size / 4.f);
		s_primaryColor = oldPrimaryColor;
	}

	void DebugDrawer::DrawBinormals(const StaticMesh* subMesh)
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

		BufferMapper<VertexBuffer> inputMapper(subMesh->GetVertexBuffer(), BufferAccess_ReadOnly);
		BufferMapper<VertexBuffer> outputMapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, vertexCount);

		MeshVertex* inputVertex = static_cast<MeshVertex*>(inputMapper.GetPointer());
		VertexStruct_XYZ* outputVertex = static_cast<VertexStruct_XYZ*>(outputMapper.GetPointer());

		for (unsigned int i = 0; i < normalCount; ++i)
		{
			outputVertex->position = inputVertex->position;
			outputVertex++;

			outputVertex->position = inputVertex->position + Vector3f::CrossProduct(inputVertex->normal, inputVertex->tangent)*0.01f;
			outputVertex++;

			inputVertex++;
		}

		inputMapper.Unmap();
		outputMapper.Unmap();

		if (vertexCount > 0)
		{
			Renderer::SetRenderStates(s_renderStates);
			Renderer::SetShader(s_shader);
			Renderer::SetVertexBuffer(&s_vertexBuffer);

			s_shader->SendColor(s_colorLocation, s_primaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, vertexCount);
		}
	}

	void DebugDrawer::DrawCone(const Vector3f& origin, const Quaternionf& rotation, float angle, float length)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		Matrix4f transformMatrix;
		transformMatrix.MakeIdentity();
		transformMatrix.SetRotation(rotation);
		transformMatrix.SetTranslation(origin);

		BufferMapper<VertexBuffer> mapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, 16);
		VertexStruct_XYZ* vertex = static_cast<VertexStruct_XYZ*>(mapper.GetPointer());

		// On calcule le reste des points
		Vector3f base(Vector3f::Forward()*length);

		// Il nous faut maintenant le rayon du cercle projeté à cette distance
		// Tangente = Opposé/Adjaçent <=> Opposé = Adjaçent*Tangente
		float radius = length*std::tan(DegreeToRadian(angle));
		Vector3f lExtend = Vector3f::Left()*radius;
		Vector3f uExtend = Vector3f::Up()*radius;

		vertex->position.Set(transformMatrix * Vector3f::Zero());
		vertex++;
		vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
		vertex++;
		vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * Vector3f::Zero());
		vertex++;
		vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * (base + lExtend - uExtend));
		vertex++;
		vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * Vector3f::Zero());
		vertex++;
		vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
		vertex++;
		vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * Vector3f::Zero());
		vertex++;
		vertex->position.Set(transformMatrix * (base - lExtend - uExtend));
		vertex++;

		vertex->position.Set(transformMatrix * (base - lExtend + uExtend));
		vertex++;
		vertex->position.Set(transformMatrix * (base + lExtend + uExtend));
		vertex++;

		mapper.Unmap();

		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);

		Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, 16);
	}

	void DebugDrawer::DrawLine(const Vector3f& p1, const Vector3f& p2)
	{
		if (!s_initialized && !Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		VertexStruct_XYZ buffer[2];
		buffer[0].position = p1;
		buffer[1].position = p2;

		s_vertexBuffer.Fill(&buffer[0], 0, 2);

		Renderer::SetRenderStates(s_renderStates);
		Renderer::SetShader(s_shader);
		Renderer::SetVertexBuffer(&s_vertexBuffer);

		s_shader->SendColor(s_colorLocation, s_primaryColor);
		Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, 2);
	}

	void DebugDrawer::DrawPoints(const Vector3f* ptr, unsigned int pointCount)
	{
		static_assert(sizeof(VertexStruct_XYZ) == sizeof(Vector3f), "VertexStruct_XYZ is no longer equal to Vector3f, please rewrite this");

		if (!s_initialized && !Initialize())
		{
			NazaraError("Failed to initialize Debug Drawer");
			return;
		}

		if (pointCount > 0)
		{
			s_vertexBuffer.Fill(ptr, 0, pointCount);

			Renderer::SetRenderStates(s_renderStates);
			Renderer::SetShader(s_shader);
			Renderer::SetVertexBuffer(&s_vertexBuffer);

			s_shader->SendColor(s_colorLocation, s_primaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_PointList, 0, pointCount);
		}
	}

	void DebugDrawer::DrawNormals(const StaticMesh* subMesh)
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

		BufferMapper<VertexBuffer> inputMapper(subMesh->GetVertexBuffer(), BufferAccess_ReadOnly);
		BufferMapper<VertexBuffer> outputMapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, vertexCount);

		MeshVertex* inputVertex = static_cast<MeshVertex*>(inputMapper.GetPointer());
		VertexStruct_XYZ* outputVertex = static_cast<VertexStruct_XYZ*>(outputMapper.GetPointer());

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
			Renderer::SetRenderStates(s_renderStates);
			Renderer::SetShader(s_shader);
			Renderer::SetVertexBuffer(&s_vertexBuffer);

			s_shader->SendColor(s_colorLocation, s_primaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, vertexCount);
		}
	}

	void DebugDrawer::DrawTangents(const StaticMesh* subMesh)
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

		BufferMapper<VertexBuffer> inputMapper(subMesh->GetVertexBuffer(), BufferAccess_ReadOnly);
		BufferMapper<VertexBuffer> outputMapper(s_vertexBuffer, BufferAccess_DiscardAndWrite, 0, vertexCount);

		MeshVertex* inputVertex = static_cast<MeshVertex*>(inputMapper.GetPointer());
		VertexStruct_XYZ* outputVertex = static_cast<VertexStruct_XYZ*>(outputMapper.GetPointer());

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
			Renderer::SetRenderStates(s_renderStates);
			Renderer::SetShader(s_shader);
			Renderer::SetVertexBuffer(&s_vertexBuffer);

			s_shader->SendColor(s_colorLocation, s_primaryColor);
			Renderer::DrawPrimitives(PrimitiveMode_LineList, 0, vertexCount);
		}
	}

	void DebugDrawer::EnableDepthBuffer(bool depthBuffer)
	{
		s_renderStates.depthBuffer = depthBuffer;
	}

	float DebugDrawer::GetLineWidth()
	{
		return s_renderStates.lineWidth;
	}

	float DebugDrawer::GetPointSize()
	{
		return s_renderStates.pointSize;
	}

	Color DebugDrawer::GetPrimaryColor()
	{
		return s_primaryColor;
	}

	Color DebugDrawer::GetSecondaryColor()
	{
		return s_secondaryColor;
	}

	bool DebugDrawer::Initialize()
	{
		if (!s_initialized)
		{
			// s_shader
			s_shader = ShaderLibrary::Get("DebugSimple");
			s_colorLocation = s_shader->GetUniformLocation("Color");

			// s_vertexBuffer
			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);
				s_vertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ), 65365, DataStorage_Hardware, BufferUsage_Dynamic);
			}
			catch (const std::exception& e)
			{
				NazaraError("Failed to create buffer: " + String(e.what()));

				Uninitialize();
				return false;
			}

			s_primaryColor = Color::Red;
			s_renderStates.depthBuffer = true;
			s_secondaryColor = Color::Green;

			s_initialized = true;
		}

		return true;
	}

	bool DebugDrawer::IsDepthBufferEnabled()
	{
		return s_renderStates.depthBuffer;
	}

	void DebugDrawer::SetLineWidth(float width)
	{
		s_renderStates.lineWidth = width;
	}

	void DebugDrawer::SetPointSize(float size)
	{
		s_renderStates.pointSize = size;
	}

	void DebugDrawer::SetPrimaryColor(const Color& color)
	{
		s_primaryColor = color;
	}

	void DebugDrawer::SetSecondaryColor(const Color& color)
	{
		s_secondaryColor = color;
	}

	void DebugDrawer::Uninitialize()
	{
		s_shader = nullptr;
		s_vertexBuffer.Reset();
		s_initialized = false;
	}
}
