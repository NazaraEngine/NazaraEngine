// Copyright (C) 2014 Jérôme Leclercq
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

NzVertexDeclaration::NzVertexDeclaration() :
m_stride(0)
{
}

NzVertexDeclaration::NzVertexDeclaration(NzVertexDeclaration& declaration) :
NzResource(),
m_stride(declaration.m_stride)
{
	std::memcpy(m_components, declaration.m_components, sizeof(Component)*(nzVertexComponent_Max+1));
}

NzVertexDeclaration::~NzVertexDeclaration()
{
	NotifyDestroy();
}

void NzVertexDeclaration::DisableComponent(nzVertexComponent component)
{
	#ifdef NAZARA_DEBUG
	if (component > nzVertexComponent_Max)
	{
		NazaraError("Vertex component out of enum");
		return;
	}
	#endif

	#if NAZARA_UTILITY_SAFE
	if (component == nzVertexComponent_Unused)
	{
		NazaraError("Cannot disable \"unused\" component");
		return;
	}
	#endif

	Component& vertexComponent = m_components[component];
	if (vertexComponent.enabled)
	{
		vertexComponent.enabled = false;
		m_stride -= NzUtility::ComponentStride[vertexComponent.type];
	}
}

void NzVertexDeclaration::EnableComponent(nzVertexComponent component, nzComponentType type, unsigned int offset)
{
	#ifdef NAZARA_DEBUG
	if (component > nzVertexComponent_Max)
	{
		NazaraError("Vertex component out of enum");
		return;
	}
	#endif

	#if NAZARA_UTILITY_SAFE
	if (!IsTypeSupported(type))
	{
		NazaraError("Component type 0x" + NzString::Number(type, 16) + " is not supported by vertex declarations");
		return;
	}
	#endif

	if (component != nzVertexComponent_Unused)
	{
		Component& vertexComponent = m_components[component];
		if (vertexComponent.enabled)
			m_stride -= NzUtility::ComponentStride[vertexComponent.type];
		else
			vertexComponent.enabled = true;

		vertexComponent.offset = offset;
		vertexComponent.type = type;
	}

	m_stride += NzUtility::ComponentStride[type];
}

void NzVertexDeclaration::GetComponent(nzVertexComponent component, bool* enabled, nzComponentType* type, unsigned int* offset) const
{
	#ifdef NAZARA_DEBUG
	if (component > nzVertexComponent_Max)
	{
		NazaraError("Vertex component out of enum");
		return;
	}
	#endif

	#if NAZARA_UTILITY_SAFE
	if (component == nzVertexComponent_Unused)
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

unsigned int NzVertexDeclaration::GetStride() const
{
	return m_stride;
}

void NzVertexDeclaration::SetStride(unsigned int stride)
{
	m_stride = stride;
}

NzVertexDeclaration& NzVertexDeclaration::operator=(const NzVertexDeclaration& declaration)
{
	std::memcpy(m_components, declaration.m_components, sizeof(Component)*(nzVertexComponent_Max+1));
	m_stride = declaration.m_stride;

	return *this;
}

NzVertexDeclaration* NzVertexDeclaration::Get(nzVertexLayout layout)
{
	#ifdef NAZARA_DEBUG
	if (layout > nzVertexLayout_Max)
	{
		NazaraError("Vertex layout out of enum");
		return nullptr;
	}
	#endif

	return &s_declarations[layout];
}

bool NzVertexDeclaration::IsTypeSupported(nzComponentType type)
{
	switch (type)
	{
		case nzComponentType_Color:
		case nzComponentType_Double1:
		case nzComponentType_Double2:
		case nzComponentType_Double3:
		case nzComponentType_Double4:
		case nzComponentType_Float1:
		case nzComponentType_Float2:
		case nzComponentType_Float3:
		case nzComponentType_Float4:
		case nzComponentType_Int1:
		case nzComponentType_Int2:
		case nzComponentType_Int3:
		case nzComponentType_Int4:
			return true;
	}

	NazaraError("Component type not handled (0x" + NzString::Number(type, 16) + ')');
	return false;
}

bool NzVertexDeclaration::Initialize()
{
	try
	{
		NzErrorFlags flags(nzErrorFlag_Silent | nzErrorFlag_ThrowException);

		// Layout : Type
		NzVertexDeclaration* declaration;

		// nzVertexLayout_XY : NzVertexStruct_XY
		declaration = &s_declarations[nzVertexLayout_XY];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XY, position));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XY), "Invalid stride for declaration NzVertexStruct_XY");

		// nzVertexLayout_XY_UV : NzVertexStruct_XY_UV
		declaration = &s_declarations[nzVertexLayout_XY_UV];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XY_UV, position));
		declaration->EnableComponent(nzVertexComponent_TexCoord, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XY_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XY_UV), "Invalid stride for declaration nzVertexLayout_XY_UV");

		// nzVertexLayout_XYZ : NzVertexStruct_XYZ
		declaration = &s_declarations[nzVertexLayout_XYZ];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ, position));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ), "Invalid stride for declaration nzVertexLayout_XYZ");

		// nzVertexLayout_XYZ_Normal : NzVertexStruct_XYZ_Normal
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal, position));
		declaration->EnableComponent(nzVertexComponent_Normal,   nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal, normal));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal), "Invalid stride for declaration nzVertexLayout_XYZ_Normal");

		// nzVertexLayout_XYZ_Normal_UV : NzVertexStruct_XYZ_Normal_UV
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal_UV];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, position));
		declaration->EnableComponent(nzVertexComponent_Normal,   nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, normal));
		declaration->EnableComponent(nzVertexComponent_TexCoord, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal_UV), "Invalid stride for declaration nzVertexLayout_XYZ_Normal_UV");

		// nzVertexLayout_XYZ_Normal_UV_Tangent : NzVertexStruct_XYZ_Normal_UV_Tangent
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, position));
		declaration->EnableComponent(nzVertexComponent_Normal,   nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, normal));
		declaration->EnableComponent(nzVertexComponent_TexCoord, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, uv));
		declaration->EnableComponent(nzVertexComponent_Tangent,  nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, tangent));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration nzVertexLayout_XYZ_Normal_UV_Tangent");

		// nzVertexLayout_XYZ_Normal_UV_Tangent_Skinning : NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent_Skinning];
		declaration->EnableComponent(nzVertexComponent_Position,  nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, position));
		declaration->EnableComponent(nzVertexComponent_Normal,    nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, normal));
		declaration->EnableComponent(nzVertexComponent_TexCoord,  nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, uv));
		declaration->EnableComponent(nzVertexComponent_Tangent,   nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, tangent));
		declaration->EnableComponent(nzVertexComponent_Unused,    nzComponentType_Int1,   NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, weightCount));
		declaration->EnableComponent(nzVertexComponent_Userdata0, nzComponentType_Float4, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, weights));
		declaration->EnableComponent(nzVertexComponent_Userdata1, nzComponentType_Int4,   NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning, jointIndexes));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning), "Invalid stride for declaration nzVertexLayout_XYZ_Normal_UV_Tangent_Skinning");

		// nzVertexLayout_XYZ_UV : NzVertexStruct_XYZ_UV
		declaration = &s_declarations[nzVertexLayout_XYZ_UV];
		declaration->EnableComponent(nzVertexComponent_Position, nzComponentType_Float3, NzOffsetOf(NzVertexStruct_XYZ_UV, position));
		declaration->EnableComponent(nzVertexComponent_TexCoord, nzComponentType_Float2, NzOffsetOf(NzVertexStruct_XYZ_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_UV), "Invalid stride for declaration nzVertexLayout_XYZ_UV");

		// nzVertexLayout_Matrix4 : NzMatrix4f
		declaration = &s_declarations[nzVertexLayout_Matrix4];
		declaration->EnableComponent(nzVertexComponent_InstanceData0, nzComponentType_Float4, NzOffsetOf(NzMatrix4f, m11));
		declaration->EnableComponent(nzVertexComponent_InstanceData1, nzComponentType_Float4, NzOffsetOf(NzMatrix4f, m21));
		declaration->EnableComponent(nzVertexComponent_InstanceData2, nzComponentType_Float4, NzOffsetOf(NzMatrix4f, m31));
		declaration->EnableComponent(nzVertexComponent_InstanceData3, nzComponentType_Float4, NzOffsetOf(NzMatrix4f, m41));

		NazaraAssert(declaration->GetStride() == sizeof(NzMatrix4f), "Invalid stride for declaration nzVertexLayout_Matrix4");
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to initialize vertex declaration: " + NzString(e.what()));
		return false;
	}

	return true;
}

void NzVertexDeclaration::Uninitialize()
{
	// Rien à faire
}

NzVertexDeclaration NzVertexDeclaration::s_declarations[nzVertexLayout_Max+1];
