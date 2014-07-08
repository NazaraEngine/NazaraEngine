// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	unsigned int attributeSize[] =
	{
		4, // nzAttributeType_Color
		1, // nzAttributeType_Double1
		2, // nzAttributeType_Double2
		3, // nzAttributeType_Double3
		4, // nzAttributeType_Double4
		1, // nzAttributeType_Float1
		2, // nzAttributeType_Float2
		3, // nzAttributeType_Float3
		4, // nzAttributeType_Float4
		1, // nzAttributeType_Int1
		2, // nzAttributeType_Int2
		3, // nzAttributeType_Int3
		4  // nzAttributeType_Int4
	};

	static_assert(sizeof(attributeSize)/sizeof(unsigned int) == nzAttributeType_Max+1, "Attribute size array is incomplete");

	unsigned int attributeStride[] =
	{
		4*sizeof(nzUInt8),  // nzAttributeType_Color
		1*sizeof(double),   // nzAttributeType_Double1
		2*sizeof(double),   // nzAttributeType_Double2
		3*sizeof(double),   // nzAttributeType_Double3
		4*sizeof(double),   // nzAttributeType_Double4
		1*sizeof(float),    // nzAttributeType_Float1
		2*sizeof(float),    // nzAttributeType_Float2
		3*sizeof(float),    // nzAttributeType_Float3
		4*sizeof(float),    // nzAttributeType_Float4
		1*sizeof(nzUInt32), // nzAttributeType_Int1
		2*sizeof(nzUInt32), // nzAttributeType_Int2
		3*sizeof(nzUInt32), // nzAttributeType_Int3
		4*sizeof(nzUInt32)  // nzAttributeType_Int4
	};

	static_assert(sizeof(attributeStride)/sizeof(unsigned int) == nzAttributeType_Max+1, "Attribute stride array is incomplete");
}

NzVertexDeclaration::NzVertexDeclaration() :
m_stride(0)
{
}

NzVertexDeclaration::NzVertexDeclaration(NzVertexDeclaration& declaration) :
NzResource(),
m_stride(declaration.m_stride)
{
	std::memcpy(m_attributes, declaration.m_attributes, sizeof(Attribute)*(nzAttributeUsage_Max+1));
}

NzVertexDeclaration::~NzVertexDeclaration()
{
	NotifyDestroy();
}

void NzVertexDeclaration::DisableAttribute(nzAttributeUsage usage)
{
	#ifdef NAZARA_DEBUG
	if (usage > nzAttributeUsage_Max)
	{
		NazaraError("Attribute usage out of enum");
		return;
	}
	#endif

	#if NAZARA_UTILITY_SAFE
	if (usage == nzAttributeUsage_Unused)
	{
		NazaraError("Cannot disable \"unused\" attribute");
		return;
	}
	#endif

	Attribute& attribute = m_attributes[usage];
	if (attribute.enabled)
	{
		attribute.enabled = false;
		m_stride -= attributeStride[attribute.type];
	}
}

void NzVertexDeclaration::EnableAttribute(nzAttributeUsage usage, nzAttributeType type, unsigned int offset)
{
	#ifdef NAZARA_DEBUG
	if (usage > nzAttributeUsage_Max)
	{
		NazaraError("Attribute usage out of enum");
		return;
	}
	#endif

	if (usage != nzAttributeUsage_Unused)
	{
		Attribute& attribute = m_attributes[usage];
		if (attribute.enabled)
			m_stride -= attributeStride[attribute.type];
		else
			attribute.enabled = true;

		attribute.offset = offset;
		attribute.type = type;
	}

	m_stride += attributeStride[type];
}

void NzVertexDeclaration::GetAttribute(nzAttributeUsage usage, bool* enabled, nzAttributeType* type, unsigned int* offset) const
{
	#ifdef NAZARA_DEBUG
	if (usage > nzAttributeUsage_Max)
	{
		NazaraError("Attribute usage out of enum");
		return;
	}
	#endif

	#if NAZARA_UTILITY_SAFE
	if (usage == nzAttributeUsage_Unused)
	{
		NazaraError("Cannot get \"unused\" attribute");
		return;
	}
	#endif

	const Attribute& attribute = m_attributes[usage];

	if (enabled)
		*enabled = attribute.enabled;

	if (type)
		*type = attribute.type;

	if (offset)
		*offset = attribute.offset;
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
	std::memcpy(m_attributes, declaration.m_attributes, sizeof(Attribute)*(nzAttributeUsage_Max+1));
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

unsigned int NzVertexDeclaration::GetAttributeSize(nzAttributeType type)
{
	#ifdef NAZARA_DEBUG
	if (type > nzAttributeType_Max)
	{
		NazaraError("Attribute type out of enum");
		return 0;
	}
	#endif

	return attributeSize[type];
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
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XY, position));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XY), "Invalid stride for declaration NzVertexStruct_XY");

		// nzVertexLayout_XY_UV : NzVertexStruct_XY_UV
		declaration = &s_declarations[nzVertexLayout_XY_UV];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XY_UV, position));
		declaration->EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XY_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XY_UV), "Invalid stride for declaration nzVertexLayout_XY_UV");

		// nzVertexLayout_XYZ : NzVertexStruct_XYZ
		declaration = &s_declarations[nzVertexLayout_XYZ];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ, position));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ), "Invalid stride for declaration nzVertexLayout_XYZ");

		// nzVertexLayout_XYZ_Normal : NzVertexStruct_XYZ_Normal
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal, position));
		declaration->EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal, normal));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal), "Invalid stride for declaration nzVertexLayout_XYZ_Normal");

		// nzVertexLayout_XYZ_Normal_UV : NzVertexStruct_XYZ_Normal_UV
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal_UV];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, position));
		declaration->EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, normal));
		declaration->EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal_UV), "Invalid stride for declaration nzVertexLayout_XYZ_Normal_UV");

		// nzVertexLayout_XYZ_Normal_UV_Tangent : NzVertexStruct_XYZ_Normal_UV_Tangent
		declaration = &s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, position));
		declaration->EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, normal));
		declaration->EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, uv));
		declaration->EnableAttribute(nzAttributeUsage_Tangent, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_Normal_UV_Tangent, tangent));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_Normal_UV_Tangent), "Invalid stride for declaration nzVertexLayout_XYZ_Normal_UV_Tangent");

		// nzVertexLayout_XYZ_UV : NzVertexStruct_XYZ_UV
		declaration = &s_declarations[nzVertexLayout_XYZ_UV];
		declaration->EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, NzOffsetOf(NzVertexStruct_XYZ_UV, position));
		declaration->EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, NzOffsetOf(NzVertexStruct_XYZ_UV, uv));

		NazaraAssert(declaration->GetStride() == sizeof(NzVertexStruct_XYZ_UV), "Invalid stride for declaration nzVertexLayout_XYZ_UV");

		// nzVertexLayout_Matrix4 : NzMatrix4f
		declaration = &s_declarations[nzVertexLayout_Matrix4];
		declaration->EnableAttribute(nzAttributeUsage_InstanceData0, nzAttributeType_Float4, NzOffsetOf(NzMatrix4f, m11));
		declaration->EnableAttribute(nzAttributeUsage_InstanceData1, nzAttributeType_Float4, NzOffsetOf(NzMatrix4f, m21));
		declaration->EnableAttribute(nzAttributeUsage_InstanceData2, nzAttributeType_Float4, NzOffsetOf(NzMatrix4f, m31));
		declaration->EnableAttribute(nzAttributeUsage_InstanceData3, nzAttributeType_Float4, NzOffsetOf(NzMatrix4f, m41));

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
