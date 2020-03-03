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
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexDeclaration::VertexDeclaration(VertexInputRate inputRate, std::initializer_list<ComponentEntry> components) :
	m_inputRate(inputRate)
	{
		std::size_t offset = 0;

		m_components.reserve(components.size());
		for (const ComponentEntry& entry : components)
		{
			auto& component = m_components.emplace_back();
			component.component = entry.component;
			component.componentIndex = entry.componentIndex;
			component.offset = offset;
			component.type = entry.type;

			NazaraAssert(IsTypeSupported(component.type), "Component type 0x" + String::Number(component.type, 16) + " is not supported by vertex declarations");
			NazaraAssert(component.componentIndex == 0 || component.component == VertexComponent_Userdata, "Only userdata components can have non-zero component indexes");

			offset += Utility::ComponentStride[component.type];
		}

		m_stride = offset;
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

			auto NewDeclaration = [](VertexInputRate inputRate, std::initializer_list<ComponentEntry> components)
			{
				return New(inputRate, std::move(components));
			};

			// VertexLayout_XY : VertexStruct_XY
			s_declarations[VertexLayout_XY] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float2,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XY]->GetStride() == sizeof(VertexStruct_XY), "Invalid stride for declaration VertexLayout_XY");

			s_declarations[VertexLayout_XY_Color] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float2,
					0
				},
				{
					VertexComponent_Color,
					ComponentType_Color,
					0
				},
			});

			NazaraAssert(s_declarations[VertexLayout_XY_Color]->GetStride() == sizeof(VertexStruct_XY_Color), "Invalid stride for declaration VertexLayout_XY_Color");
			/*
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

			NazaraAssert(declaration->GetStride() == sizeof(Matrix4f), "Invalid stride for declaration VertexLayout_Matrix4");*/
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

		s_declarations.fill(nullptr);
	}

	std::array<VertexDeclarationRef, VertexLayout_Max + 1> VertexDeclaration::s_declarations;
	VertexDeclarationLibrary::LibraryMap VertexDeclaration::s_library;
}
