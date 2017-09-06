// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexDeclaration::VertexDeclaration() :
	m_stride(0)
	{
	}

	VertexDeclaration::VertexDeclaration(const VertexDeclaration& declaration) :
	RefCounted(),
	m_components(declaration.m_components),
	m_stride(declaration.m_stride)
	{
	}

	VertexDeclaration::~VertexDeclaration()
	{
		OnVertexDeclarationRelease(this);
	}

	void VertexDeclaration::DisableComponent(VertexComponent component)
	{
		#ifdef NAZARA_DEBUG
		if (component > VertexComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_UTILITY_SAFE
		if (component == VertexComponent_Unused)
		{
			NazaraError("Cannot disable \"unused\" component");
			return;
		}
		#endif

		Component& vertexComponent = m_components[component];
		if (vertexComponent.enabled)
		{
			vertexComponent.enabled = false;
			m_stride -= Utility::ComponentStride[vertexComponent.type];
		}
	}

	void VertexDeclaration::EnableComponent(VertexComponent component, ComponentType type, std::size_t offset)
	{
		#ifdef NAZARA_DEBUG
		if (component > VertexComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_UTILITY_SAFE
		if (!IsTypeSupported(type))
		{
			NazaraError("Component type 0x" + String::Number(type, 16) + " is not supported by vertex declarations");
			return;
		}
		#endif

		if (component != VertexComponent_Unused)
		{
			Component& vertexComponent = m_components[component];
			if (vertexComponent.enabled)
				m_stride -= Utility::ComponentStride[vertexComponent.type];
			else
				vertexComponent.enabled = true;

			vertexComponent.offset = offset;
			vertexComponent.type = type;
		}

		m_stride += Utility::ComponentStride[type];
	}

	void VertexDeclaration::GetComponent(VertexComponent component, bool* enabled, ComponentType* type, std::size_t* offset) const
	{
		#ifdef NAZARA_DEBUG
		if (component > VertexComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_UTILITY_SAFE
		if (component == VertexComponent_Unused)
		{
			NazaraError("Cannot get \"unused\" component");
			return;
		}
		#endif

		const Component& vertexComponent = m_components[component];

		if (enabled)
			*enabled = vertexComponent.enabled;

		if (type)
			*type = vertexComponent.type;

		if (offset)
			*offset = vertexComponent.offset;
	}

	std::size_t VertexDeclaration::GetStride() const
	{
		return m_stride;
	}

	void VertexDeclaration::SetStride(std::size_t stride)
	{
		m_stride = stride;
	}

	VertexDeclaration& VertexDeclaration::operator=(const VertexDeclaration& declaration)
	{
		m_components = declaration.m_components;
		m_stride = declaration.m_stride;

		return *this;
	}

	VertexDeclaration* VertexDeclaration::Get(VertexLayout layout)
	{
		NazaraAssert(layout <= VertexLayout_Max, "Vertex layout out of enum");

		return &s_declarations[layout];
	}

	bool VertexDeclaration::IsTypeSupported(ComponentType type)
	{
		switch (type)
		{
			case ComponentType_Color:
			case ComponentType_Double1:
			case ComponentType_Double2:
			case ComponentType_Double3:
			case ComponentType_Double4:
			case ComponentType_Float1:
			case ComponentType_Float2:
			case ComponentType_Float3:
			case ComponentType_Float4:
			case ComponentType_Int1:
			case ComponentType_Int2:
			case ComponentType_Int3:
			case ComponentType_Int4:
				return true;

			case ComponentType_Quaternion:
				return false;
		}

		NazaraError("Component type not handled (0x" + String::Number(type, 16) + ')');
		return false;
	}

	bool VertexDeclaration::Initialize()
	{
		if (!VertexDeclarationLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		try
		{
			ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowException);

			// Layout : Type
			VertexDeclaration* declaration;

			// VertexLayout_XY : VertexStruct_XY
			declaration = &s_declarations[VertexLayout_XY];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XY, position));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XY), "Invalid stride for declaration VertexLayout_XY");

			// VertexLayout_XY_Color : VertexStruct_XY_Color
			declaration = &s_declarations[VertexLayout_XY_Color];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XY_Color, position));
			declaration->EnableComponent(VertexComponent_Color,    ComponentType_Color,  NazaraOffsetOf(VertexStruct_XY_Color, color));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XY_Color), "Invalid stride for declaration VertexLayout_XY_Color");

			// VertexLayout_XY_UV : VertexStruct_XY_UV
			declaration = &s_declarations[VertexLayout_XY_UV];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XY_UV, position));
			declaration->EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XY_UV, uv));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XY_UV), "Invalid stride for declaration VertexLayout_XY_UV");

			// VertexLayout_XYZ : VertexStruct_XYZ
			declaration = &s_declarations[VertexLayout_XYZ];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ, position));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ), "Invalid stride for declaration VertexLayout_XYZ");

			// VertexLayout_XYZ_Color : VertexStruct_XYZ_Color
			declaration = &s_declarations[VertexLayout_XYZ_Color];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Color, position));
			declaration->EnableComponent(VertexComponent_Color,    ComponentType_Color,  NazaraOffsetOf(VertexStruct_XYZ_Color, color));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Color), "Invalid stride for declaration VertexLayout_XYZ_Color");

			// VertexLayout_XYZ_Color_UV : VertexStruct_XYZ_Color_UV
			declaration = &s_declarations[VertexLayout_XYZ_Color_UV];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Color_UV, position));
			declaration->EnableComponent(VertexComponent_Color,    ComponentType_Color,  NazaraOffsetOf(VertexStruct_XYZ_Color_UV, color));
			declaration->EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XYZ_Color_UV, uv));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Color_UV), "Invalid stride for declaration VertexLayout_XYZ_Color_UV");

			// VertexLayout_XYZ_Normal : VertexStruct_XYZ_Normal
			declaration = &s_declarations[VertexLayout_XYZ_Normal];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal, position));
			declaration->EnableComponent(VertexComponent_Normal,   ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal, normal));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Normal), "Invalid stride for declaration VertexLayout_XYZ_Normal");

			// VertexLayout_XYZ_Normal_UV : VertexStruct_XYZ_Normal_UV
			declaration = &s_declarations[VertexLayout_XYZ_Normal_UV];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV, position));
			declaration->EnableComponent(VertexComponent_Normal,   ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV, normal));
			declaration->EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV, uv));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV");

			// VertexLayout_XYZ_Normal_UV_Tangent : VertexStruct_XYZ_Normal_UV_Tangent
			declaration = &s_declarations[VertexLayout_XYZ_Normal_UV_Tangent];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent, position));
			declaration->EnableComponent(VertexComponent_Normal,   ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent, normal));
			declaration->EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent, uv));
			declaration->EnableComponent(VertexComponent_Tangent,  ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent, tangent));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV_Tangent");

			// VertexLayout_XYZ_Normal_UV_Tangent_Skinning : VertexStruct_XYZ_Normal_UV_Tangent_Skinning
			declaration = &s_declarations[VertexLayout_XYZ_Normal_UV_Tangent_Skinning];
			declaration->EnableComponent(VertexComponent_Position,  ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, position));
			declaration->EnableComponent(VertexComponent_Normal,    ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, normal));
			declaration->EnableComponent(VertexComponent_TexCoord,  ComponentType_Float2, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, uv));
			declaration->EnableComponent(VertexComponent_Tangent,   ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, tangent));
			declaration->EnableComponent(VertexComponent_Unused,    ComponentType_Int1,   NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, weightCount));
			declaration->EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, weights));
			declaration->EnableComponent(VertexComponent_Userdata1, ComponentType_Int4,   NazaraOffsetOf(VertexStruct_XYZ_Normal_UV_Tangent_Skinning, jointIndexes));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent_Skinning), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV_Tangent_Skinning");

			// VertexLayout_XYZ_UV : VertexStruct_XYZ_UV
			declaration = &s_declarations[VertexLayout_XYZ_UV];
			declaration->EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(VertexStruct_XYZ_UV, position));
			declaration->EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(VertexStruct_XYZ_UV, uv));

			NazaraAssert(declaration->GetStride() == sizeof(VertexStruct_XYZ_UV), "Invalid stride for declaration VertexLayout_XYZ_UV");

			// VertexLayout_Matrix4 : Matrix4f
			declaration = &s_declarations[VertexLayout_Matrix4];
			declaration->EnableComponent(VertexComponent_InstanceData0, ComponentType_Float4, NazaraOffsetOf(Matrix4f, m11));
			declaration->EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(Matrix4f, m21));
			declaration->EnableComponent(VertexComponent_InstanceData2, ComponentType_Float4, NazaraOffsetOf(Matrix4f, m31));
			declaration->EnableComponent(VertexComponent_InstanceData3, ComponentType_Float4, NazaraOffsetOf(Matrix4f, m41));

			NazaraAssert(declaration->GetStride() == sizeof(Matrix4f), "Invalid stride for declaration VertexLayout_Matrix4");
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize vertex declaration: " + String(e.what()));
			return false;
		}

		return true;
	}

	void VertexDeclaration::Uninitialize()
	{
		VertexDeclarationLibrary::Uninitialize();
	}

	std::array<VertexDeclaration, VertexLayout_Max + 1> VertexDeclaration::s_declarations;
	VertexDeclarationLibrary::LibraryMap VertexDeclaration::s_library;
}
