// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	unsigned int attributeSize[nzAttributeType_Max+1] =
	{
		4, // nzAttributeType_Color
		1, // nzAttributeType_Double1
		2, // nzAttributeType_Double2
		3, // nzAttributeType_Double3
		4, // nzAttributeType_Double4
		1, // nzAttributeType_Float1
		2, // nzAttributeType_Float2
		3, // nzAttributeType_Float3
		4  // nzAttributeType_Float4
	};

	unsigned int attributeStride[nzAttributeType_Max+1] =
	{
		4*sizeof(nzUInt8), // nzAttributeType_Color
		1*sizeof(double),  // nzAttributeType_Double1
		2*sizeof(double),  // nzAttributeType_Double2
		3*sizeof(double),  // nzAttributeType_Double3
		4*sizeof(double),  // nzAttributeType_Double4
		1*sizeof(float),   // nzAttributeType_Float1
		2*sizeof(float),   // nzAttributeType_Float2
		3*sizeof(float),   // nzAttributeType_Float3
		4*sizeof(float)    // nzAttributeType_Float4
	};
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

void NzVertexDeclaration::DisableAttribute(nzAttributeUsage usage)
{
	#ifdef NAZARA_DEBUG
	if (usage > nzAttributeUsage_Max)
	{
		NazaraError("Attribute usage out of enum");
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

	Attribute& attribute = m_attributes[usage];

	if (attribute.enabled)
		m_stride -= attributeStride[attribute.type];
	else
		attribute.enabled = true;

	attribute.offset = offset;
	attribute.type = type;

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
	s_declarations[nzVertexLayout_XY].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float2, 0);

	s_declarations[nzVertexLayout_XY_UV].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float2, 0);
	s_declarations[nzVertexLayout_XY_UV].EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, 2*sizeof(float));

	s_declarations[nzVertexLayout_XYZ].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, 0);

	s_declarations[nzVertexLayout_XYZ_Normal].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, 0);
	s_declarations[nzVertexLayout_XYZ_Normal].EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, 3*sizeof(float));

	s_declarations[nzVertexLayout_XYZ_Normal_UV].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, 0);
	s_declarations[nzVertexLayout_XYZ_Normal_UV].EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, 3*sizeof(float));
	s_declarations[nzVertexLayout_XYZ_Normal_UV].EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, (3+3)*sizeof(float));

	s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, 0);
	s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent].EnableAttribute(nzAttributeUsage_Normal, nzAttributeType_Float3, 3*sizeof(float));
	s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent].EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, (3+3)*sizeof(float));
	s_declarations[nzVertexLayout_XYZ_Normal_UV_Tangent].EnableAttribute(nzAttributeUsage_Tangent, nzAttributeType_Float3, (3+3+2)*sizeof(float));

	s_declarations[nzVertexLayout_XYZ_UV].EnableAttribute(nzAttributeUsage_Position, nzAttributeType_Float3, 0);
	s_declarations[nzVertexLayout_XYZ_UV].EnableAttribute(nzAttributeUsage_TexCoord, nzAttributeType_Float2, 3*sizeof(float));

	s_declarations[nzVertexLayout_Matrix4].EnableAttribute(nzAttributeUsage_InstanceData0, nzAttributeType_Float4, 0*4*sizeof(float));
	s_declarations[nzVertexLayout_Matrix4].EnableAttribute(nzAttributeUsage_InstanceData1, nzAttributeType_Float4, 1*4*sizeof(float));
	s_declarations[nzVertexLayout_Matrix4].EnableAttribute(nzAttributeUsage_InstanceData2, nzAttributeType_Float4, 2*4*sizeof(float));
	s_declarations[nzVertexLayout_Matrix4].EnableAttribute(nzAttributeUsage_InstanceData3, nzAttributeType_Float4, 3*4*sizeof(float));

	return true;
}

void NzVertexDeclaration::Uninitialize()
{
}

NzVertexDeclaration NzVertexDeclaration::s_declarations[nzVertexLayout_Max+1];
