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
	namespace
	{
		std::size_t s_componentStride[ComponentTypeCount] =
		{
			4 * sizeof(UInt8),    // ComponentType::Color
			1 * sizeof(double),   // ComponentType::Double1
			2 * sizeof(double),   // ComponentType::Double2
			3 * sizeof(double),   // ComponentType::Double3
			4 * sizeof(double),   // ComponentType::Double4
			1 * sizeof(float),    // ComponentType::Float1
			2 * sizeof(float),    // ComponentType::Float2
			3 * sizeof(float),    // ComponentType::Float3
			4 * sizeof(float),    // ComponentType::Float4
			1 * sizeof(UInt32),   // ComponentType::Int1
			2 * sizeof(UInt32),   // ComponentType::Int2
			3 * sizeof(UInt32),   // ComponentType::Int3
			4 * sizeof(UInt32),   // ComponentType::Int4
			4 * sizeof(float)     // ComponentType::Quaternion
		};
	}
	VertexDeclaration::VertexDeclaration(VertexInputRate inputRate, std::initializer_list<ComponentEntry> components) :
	m_inputRate(inputRate)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException);
		std::size_t offset = 0;

		m_components.reserve(components.size());
		for (const ComponentEntry& entry : components)
		{
			NazaraAssert(IsTypeSupported(entry.type), "Component type 0x" + NumberToString(UnderlyingCast(entry.type), 16) + " is not supported by vertex declarations");
			NazaraAssert(entry.componentIndex == 0 || entry.component == VertexComponent::Userdata, "Only userdata components can have non-zero component indexes");

			if (entry.component != VertexComponent::Unused)
			{
				// Check for duplicates
				for (const Component& component : m_components)
				{
					if (component.component == entry.component && component.componentIndex == entry.componentIndex)
						NazaraError("Duplicate component type found");
				}
			}

			auto& component = m_components.emplace_back();
			component.component = entry.component;
			component.componentIndex = entry.componentIndex;
			component.offset = offset;
			component.type = entry.type;

			offset += s_componentStride[UnderlyingCast(component.type)];
		}

		m_stride = offset;
	}

	bool VertexDeclaration::IsTypeSupported(ComponentType type)
	{
		switch (type)
		{
			case ComponentType::Color:
			case ComponentType::Double1:
			case ComponentType::Double2:
			case ComponentType::Double3:
			case ComponentType::Double4:
			case ComponentType::Float1:
			case ComponentType::Float2:
			case ComponentType::Float3:
			case ComponentType::Float4:
			case ComponentType::Int1:
			case ComponentType::Int2:
			case ComponentType::Int3:
			case ComponentType::Int4:
				return true;

			case ComponentType::Quaternion:
				return false;
		}

		NazaraError("Component type not handled (0x" + NumberToString(UnderlyingCast(type), 16) + ')');
		return false;
	}

	bool VertexDeclaration::Initialize()
	{
		try
		{
			ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowException);

			auto NewDeclaration = [](VertexInputRate inputRate, std::initializer_list<ComponentEntry> components)
			{
				return std::make_shared<VertexDeclaration>(inputRate, std::move(components));
			};

			// VertexLayout::XY : VertexStruct_XY
			s_declarations[UnderlyingCast(VertexLayout::XY)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XY)]->GetStride() == sizeof(VertexStruct_XY), "Invalid stride for declaration VertexLayout::XY");

			s_declarations[UnderlyingCast(VertexLayout::XY_Color)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Color,
					0
				},
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XY_Color)]->GetStride() == sizeof(VertexStruct_XY_Color), "Invalid stride for declaration VertexLayout::XY_Color");
			
			// VertexLayout::XY_UV : VertexStruct_XY_UV
			s_declarations[UnderlyingCast(VertexLayout::XY_UV)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XY_UV)]->GetStride() == sizeof(VertexStruct_XY_UV), "Invalid stride for declaration VertexLayout::XY_UV");

			// VertexLayout::XYZ : VertexStruct_XYZ
			s_declarations[UnderlyingCast(VertexLayout::XYZ)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ)]->GetStride() == sizeof(VertexStruct_XYZ), "Invalid stride for declaration VertexLayout::XYZ");

			// VertexLayout::XYZ_Color : VertexStruct_XYZ_Color
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Color)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Color,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Color)]->GetStride() == sizeof(VertexStruct_XYZ_Color), "Invalid stride for declaration VertexLayout::XYZ_Color");

			// VertexLayout::XYZ_Color_UV : VertexStruct_XYZ_Color_UV
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Color_UV)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Color,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Color_UV)]->GetStride() == sizeof(VertexStruct_XYZ_Color_UV), "Invalid stride for declaration VertexLayout::XYZ_Color_UV");

			// VertexLayout::XYZ_Normal : VertexStruct_XYZ_Normal
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Normal,
					ComponentType::Float3,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal)]->GetStride() == sizeof(VertexStruct_XYZ_Normal), "Invalid stride for declaration VertexLayout::XYZ_Normal");

			// VertexLayout::XYZ_Normal_UV : VertexStruct_XYZ_Normal_UV
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Normal,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV)]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV");

			// VertexLayout::XYZ_Normal_UV_Tangent : VertexStruct_XYZ_Normal_UV_Tangent
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV_Tangent)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Normal,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::Tangent,
					ComponentType::Float3,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV_Tangent)]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV_Tangent");

			// VertexLayout::XYZ_Normal_UV_Tangent_Skinning : VertexStruct_XYZ_Normal_UV_Tangent_Skinning
			s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV_Tangent_Skinning)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Normal,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::Tangent,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Userdata,
					ComponentType::Int1,
					0 // Weight count
				},
				{
					VertexComponent::Userdata,
					ComponentType::Float4,
					1 // Weights
				},
				{
					VertexComponent::Userdata,
					ComponentType::Int4,
					2 // Joint indexes
				},
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_Normal_UV_Tangent_Skinning)]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent_Skinning), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV_Tangent_Skinning");

			// VertexLayout::XYZ_UV : VertexStruct_XYZ_UV
			s_declarations[UnderlyingCast(VertexLayout::XYZ_UV)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::XYZ_UV)]->GetStride() == sizeof(VertexStruct_XYZ_UV), "Invalid stride for declaration VertexLayout::XYZ_UV");

			// VertexLayout::Matrix4 : Matrix4f
			s_declarations[UnderlyingCast(VertexLayout::Matrix4)] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Userdata,
					ComponentType::Float4,
					0
				},
				{
					VertexComponent::Userdata,
					ComponentType::Float4,
					1
				},
				{
					VertexComponent::Userdata,
					ComponentType::Float4,
					2
				},
				{
					VertexComponent::Userdata,
					ComponentType::Float4,
					3
				}
			});

			NazaraAssert(s_declarations[UnderlyingCast(VertexLayout::Matrix4)]->GetStride() == sizeof(Matrix4f), "Invalid stride for declaration VertexLayout::Matrix4");
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize vertex declaration: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	void VertexDeclaration::Uninitialize()
	{
		s_declarations.fill(nullptr);
	}

	std::array<std::shared_ptr<VertexDeclaration>, VertexLayoutCount> VertexDeclaration::s_declarations;
}
