// Copyright (C) 2020 Jérôme Leclercq
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
			
			// VertexLayout_XY_UV : VertexStruct_XY_UV
			s_declarations[VertexLayout_XY_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float2,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				},
			});

			NazaraAssert(s_declarations[VertexLayout_XY_UV]->GetStride() == sizeof(VertexStruct_XY_UV), "Invalid stride for declaration VertexLayout_XY_UV");

			// VertexLayout_XYZ : VertexStruct_XYZ
			s_declarations[VertexLayout_XYZ] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ]->GetStride() == sizeof(VertexStruct_XYZ), "Invalid stride for declaration VertexLayout_XYZ");

			// VertexLayout_XYZ_Color : VertexStruct_XYZ_Color
			s_declarations[VertexLayout_XYZ_Color] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Color,
					ComponentType_Color,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Color]->GetStride() == sizeof(VertexStruct_XYZ_Color), "Invalid stride for declaration VertexLayout_XYZ_Color");

			// VertexLayout_XYZ_Color_UV : VertexStruct_XYZ_Color_UV
			s_declarations[VertexLayout_XYZ_Color_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Color,
					ComponentType_Color,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				},
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Color_UV]->GetStride() == sizeof(VertexStruct_XYZ_Color_UV), "Invalid stride for declaration VertexLayout_XYZ_Color_UV");

			// VertexLayout_XYZ_Normal : VertexStruct_XYZ_Normal
			s_declarations[VertexLayout_XYZ_Normal] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Normal,
					ComponentType_Float3,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Normal]->GetStride() == sizeof(VertexStruct_XYZ_Normal), "Invalid stride for declaration VertexLayout_XYZ_Normal");

			// VertexLayout_XYZ_Normal_UV : VertexStruct_XYZ_Normal_UV
			s_declarations[VertexLayout_XYZ_Normal_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Normal,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Normal_UV]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV");

			// VertexLayout_XYZ_Normal_UV_Tangent : VertexStruct_XYZ_Normal_UV_Tangent
			s_declarations[VertexLayout_XYZ_Normal_UV_Tangent] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Normal,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				},
				{
					VertexComponent_Tangent,
					ComponentType_Float3,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Normal_UV_Tangent]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV_Tangent");

			// VertexLayout_XYZ_Normal_UV_Tangent_Skinning : VertexStruct_XYZ_Normal_UV_Tangent_Skinning
			s_declarations[VertexLayout_XYZ_Normal_UV_Tangent_Skinning] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Normal,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				},
				{
					VertexComponent_Tangent,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_Userdata,
					ComponentType_Int1,
					0 // Weight count
				},
				{
					VertexComponent_Userdata,
					ComponentType_Float4,
					1 // Weights
				},
				{
					VertexComponent_Userdata,
					ComponentType_Int4,
					2 // Joint indexes
				},
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_Normal_UV_Tangent_Skinning]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent_Skinning), "Invalid stride for declaration VertexLayout_XYZ_Normal_UV_Tangent_Skinning");

			// VertexLayout_XYZ_UV : VertexStruct_XYZ_UV
			s_declarations[VertexLayout_XYZ_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Position,
					ComponentType_Float3,
					0
				},
				{
					VertexComponent_TexCoord,
					ComponentType_Float2,
					0
				}
			});

			NazaraAssert(s_declarations[VertexLayout_XYZ_UV]->GetStride() == sizeof(VertexStruct_XYZ_UV), "Invalid stride for declaration VertexLayout_XYZ_UV");

			// VertexLayout_Matrix4 : Matrix4f
			s_declarations[VertexLayout_Matrix4] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent_Userdata,
					ComponentType_Float4,
					0
				},
				{
					VertexComponent_Userdata,
					ComponentType_Float4,
					1
				},
				{
					VertexComponent_Userdata,
					ComponentType_Float4,
					2
				},
				{
					VertexComponent_Userdata,
					ComponentType_Float4,
					3
				}
			});

			NazaraAssert(s_declarations[VertexLayout_Matrix4]->GetStride() == sizeof(Matrix4f), "Invalid stride for declaration VertexLayout_Matrix4");
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
