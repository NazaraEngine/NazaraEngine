// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/VertexDeclaration.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <NazaraUtils/OffsetOf.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr EnumArray<ComponentType, std::size_t> s_componentStride = {
			1 * sizeof(UInt8),    // ComponentType::Byte1
			2 * sizeof(UInt8),    // ComponentType::Byte2
			3 * sizeof(UInt8),    // ComponentType::Byte3
			4 * sizeof(UInt8),    // ComponentType::Byte4
			1 * sizeof(double),   // ComponentType::Double1
			2 * sizeof(double),   // ComponentType::Double2
			3 * sizeof(double),   // ComponentType::Double3
			4 * sizeof(double),   // ComponentType::Double4
			1 * sizeof(float),    // ComponentType::Float1
			2 * sizeof(float),    // ComponentType::Float2
			3 * sizeof(float),    // ComponentType::Float3
			4 * sizeof(float),    // ComponentType::Float4
			1 * sizeof(Int32),    // ComponentType::Int1
			2 * sizeof(Int32),    // ComponentType::Int2
			3 * sizeof(Int32),    // ComponentType::Int3
			4 * sizeof(Int32),    // ComponentType::Int4
			1 * sizeof(UInt32),   // ComponentType::UInt1
			2 * sizeof(UInt32),   // ComponentType::UInt2
			3 * sizeof(UInt32),   // ComponentType::UInt3
			4 * sizeof(UInt32),   // ComponentType::UInt4
		};
	}

	VertexDeclaration::VertexDeclaration(VertexInputRate inputRate, std::initializer_list<ComponentEntry> componentEntries) :
	m_inputRate(inputRate)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorFlags errFlags(ErrorMode::ThrowException);
		std::size_t offset = 0;

		m_components.reserve(componentEntries.size());
		for (const ComponentEntry& entry : componentEntries)
		{
			NazaraAssertMsg(entry.componentIndex == 0 || entry.component == VertexComponent::Userdata, "only userdata components can have non-zero component indexes");

			if (entry.component != VertexComponent::Unused)
			{
				// Check for duplicates
				for (const Component& component : m_components)
				{
					if (component.component == entry.component && component.componentIndex == entry.componentIndex)
						NazaraError("duplicate component type found");
				}
			}

			auto& component = m_components.emplace_back();
			component.component = entry.component;
			component.componentIndex = entry.componentIndex;
			component.offset = offset;
			component.type = entry.type;

			offset += s_componentStride[component.type];
		}

		m_stride = offset;
	}

	VertexDeclaration::VertexDeclaration(VertexInputRate inputRate, std::size_t stride, std::initializer_list<Component> components) :
	m_stride(stride),
	m_inputRate(inputRate)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorFlags errFlags(ErrorMode::ThrowException);

		m_components.reserve(components.size());
		for (const Component& entry : components)
		{
			NazaraAssertMsg(entry.componentIndex == 0 || entry.component == VertexComponent::Userdata, "only userdata components can have non-zero component indexes");
			NazaraAssertMsg(entry.offset + s_componentStride[entry.type], "component offset + size exceeds stride");

			if (entry.component != VertexComponent::Unused)
			{
				// Check for duplicates
				for (const Component& component : m_components)
				{
					if (component.component == entry.component && component.componentIndex == entry.componentIndex)
						NazaraError("duplicate component type found");
				}
			}

			m_components.push_back(entry);
		}
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
			s_declarations[VertexLayout::XY] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				}
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XY]->GetStride() == sizeof(VertexStruct_XY), "Invalid stride for declaration VertexLayout::XY");

			s_declarations[VertexLayout::XY_Color] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Float4,
					0
				},
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XY_Color]->GetStride() == sizeof(VertexStruct_XY_Color), "Invalid stride for declaration VertexLayout::XY_Color");

			// VertexLayout::XY_Color_UV : VertexStruct_XY_Color_UV
			s_declarations[VertexLayout::XY_Color_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Float4,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XY_Color_UV]->GetStride() == sizeof(VertexStruct_XY_Color_UV), "Invalid stride for declaration VertexLayout::XY_Color_UV");

			// VertexLayout::XY_UV : VertexStruct_XY_UV
			s_declarations[VertexLayout::XY_UV] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::XY_UV]->GetStride() == sizeof(VertexStruct_XY_UV), "Invalid stride for declaration VertexLayout::XY_UV");

			// VertexLayout::XYZ : VertexStruct_XYZ
			s_declarations[VertexLayout::XYZ] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ]->GetStride() == sizeof(VertexStruct_XYZ), "Invalid stride for declaration VertexLayout::XYZ");

			// VertexLayout::XYZ_Color : VertexStruct_XYZ_Color
			s_declarations[VertexLayout::XYZ_Color] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Float4,
					0
				}
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Color]->GetStride() == sizeof(VertexStruct_XYZ_Color), "Invalid stride for declaration VertexLayout::XYZ_Color");

			// VertexLayout::XYZ_Color_UV : VertexStruct_XYZ_Color_UV
			s_declarations[VertexLayout::XYZ_Color_UV] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::Position,
					ComponentType::Float3,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Float4,
					0
				},
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Color_UV]->GetStride() == sizeof(VertexStruct_XYZ_Color_UV), "Invalid stride for declaration VertexLayout::XYZ_Color_UV");

			// VertexLayout::XYZ_Normal : VertexStruct_XYZ_Normal
			s_declarations[VertexLayout::XYZ_Normal] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Normal]->GetStride() == sizeof(VertexStruct_XYZ_Normal), "Invalid stride for declaration VertexLayout::XYZ_Normal");

			// VertexLayout::XYZ_Normal_UV : VertexStruct_XYZ_Normal_UV
			s_declarations[VertexLayout::XYZ_Normal_UV] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Normal_UV]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV");

			// VertexLayout::XYZ_Normal_UV_Tangent : VertexStruct_XYZ_Normal_UV_Tangent
			s_declarations[VertexLayout::XYZ_Normal_UV_Tangent] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Normal_UV_Tangent]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV_Tangent");

			// VertexLayout::XYZ_Normal_UV_Tangent_Skinning : VertexStruct_XYZ_Normal_UV_Tangent_Skinning
			s_declarations[VertexLayout::XYZ_Normal_UV_Tangent_Skinning] = NewDeclaration(VertexInputRate::Vertex, {
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
					VertexComponent::JointWeights,
					ComponentType::Float4,
					0
				},
				{
					VertexComponent::JointIndices,
					ComponentType::Int4,
					0
				},
			});

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_Normal_UV_Tangent_Skinning]->GetStride() == sizeof(VertexStruct_XYZ_Normal_UV_Tangent_Skinning), "Invalid stride for declaration VertexLayout::XYZ_Normal_UV_Tangent_Skinning");

			// VertexLayout::XYZ_SizeRot : VertexStruct_XYZ_SizeRot
			s_declarations[VertexLayout::UV_SizeSinCos] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::SizeSinCos,
					ComponentType::Float4,
					0
				}
			});

			NazaraAssertMsg(s_declarations[VertexLayout::UV_SizeSinCos]->GetStride() == sizeof(VertexStruct_UV_SizeSinCos), "Invalid stride for declaration VertexLayout::UV_SizeSinCos");

			// VertexLayout::XYZ_SizeRot_Color : VertexStruct_XYZ_SizeRot_Color
			s_declarations[VertexLayout::UV_SizeSinCos_Color] = NewDeclaration(VertexInputRate::Vertex, {
				{
					VertexComponent::TexCoord,
					ComponentType::Float2,
					0
				},
				{
					VertexComponent::SizeSinCos,
					ComponentType::Float4,
					0
				},
				{
					VertexComponent::Color,
					ComponentType::Float4,
					0
				}
			});

			NazaraAssertMsg(s_declarations[VertexLayout::UV_SizeSinCos_Color]->GetStride() == sizeof(VertexStruct_UV_SizeSinCos_Color), "Invalid stride for declaration VertexLayout::UV_SizeSinCos_Color");

			// VertexLayout::XYZ_UV : VertexStruct_XYZ_UV
			s_declarations[VertexLayout::XYZ_UV] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::XYZ_UV]->GetStride() == sizeof(VertexStruct_XYZ_UV), "Invalid stride for declaration VertexLayout::XYZ_UV");

			// VertexLayout::Matrix4 : Matrix4f
			s_declarations[VertexLayout::Matrix4] = NewDeclaration(VertexInputRate::Vertex, {
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

			NazaraAssertMsg(s_declarations[VertexLayout::Matrix4]->GetStride() == sizeof(Matrix4f), "Invalid stride for declaration VertexLayout::Matrix4");
		}
		catch (const std::exception& e)
		{
			NazaraError("failed to initialize vertex declaration: {0}", e.what());
			return false;
		}

		return true;
	}

	void VertexDeclaration::Uninitialize()
	{
		s_declarations.fill(nullptr);
	}

	EnumArray<VertexLayout, std::shared_ptr<VertexDeclaration>> VertexDeclaration::s_declarations;
}
