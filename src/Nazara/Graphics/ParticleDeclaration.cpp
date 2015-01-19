// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleDeclaration.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/ParticleStruct.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

NzParticleDeclaration::NzParticleDeclaration() :
m_stride(0)
{
}

NzParticleDeclaration::NzParticleDeclaration(const NzParticleDeclaration& declaration) :
NzResource(),
m_stride(declaration.m_stride)
{
	std::memcpy(m_components, declaration.m_components, sizeof(Component)*(nzParticleComponent_Max+1));
}

NzParticleDeclaration::~NzParticleDeclaration()
{
	NotifyDestroy();
}

void NzParticleDeclaration::DisableComponent(nzParticleComponent component)
{
	#ifdef NAZARA_DEBUG
	if (component > nzParticleComponent_Max)
	{
		NazaraError("Vertex component out of enum");
		return;
	}
	#endif

	#if NAZARA_GRAPHICS_SAFE
	if (component == nzParticleComponent_Unused)
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

void NzParticleDeclaration::EnableComponent(nzParticleComponent component, nzComponentType type, unsigned int offset)
{
	#ifdef NAZARA_DEBUG
	if (component > nzParticleComponent_Max)
	{
		NazaraError("Vertex component out of enum");
		return;
	}
	#endif

	#if NAZARA_GRAPHICS_SAFE
	if (!IsTypeSupported(type))
	{
		NazaraError("Component type 0x" + NzString::Number(type, 16) + " is not supported by particle declarations");
		return;
	}
	#endif

	if (component != nzParticleComponent_Unused)
	{
		Component& particleComponent = m_components[component];
		if (particleComponent.enabled)
			m_stride -= NzUtility::ComponentStride[particleComponent.type];
		else
			particleComponent.enabled = true;

		particleComponent.offset = offset;
		particleComponent.type = type;
	}

	m_stride += NzUtility::ComponentStride[type];
}

void NzParticleDeclaration::GetComponent(nzParticleComponent component, bool* enabled, nzComponentType* type, unsigned int* offset) const
{
	#ifdef NAZARA_DEBUG
	if (component > nzParticleComponent_Max)
	{
		NazaraError("Particle component out of enum");
		return;
	}
	#endif

	#if NAZARA_GRAPHICS_SAFE
	if (component == nzParticleComponent_Unused)
	{
		NazaraError("Cannot get \"unused\" component");
		return;
	}
	#endif

	const Component& particleComponent = m_components[component];

	if (enabled)
		*enabled = particleComponent.enabled;

	if (type)
		*type = particleComponent.type;

	if (offset)
		*offset = particleComponent.offset;
}

unsigned int NzParticleDeclaration::GetStride() const
{
	return m_stride;
}

void NzParticleDeclaration::SetStride(unsigned int stride)
{
	m_stride = stride;
}

NzParticleDeclaration& NzParticleDeclaration::operator=(const NzParticleDeclaration& declaration)
{
	std::memcpy(m_components, declaration.m_components, sizeof(Component)*(nzParticleComponent_Max+1));
	m_stride = declaration.m_stride;

	return *this;
}

NzParticleDeclaration* NzParticleDeclaration::Get(nzParticleLayout layout)
{
	#ifdef NAZARA_DEBUG
	if (layout > nzParticleLayout_Max)
	{
		NazaraError("Particle layout out of enum");
		return nullptr;
	}
	#endif

	return &s_declarations[layout];
}

bool NzParticleDeclaration::IsTypeSupported(nzComponentType type)
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
		case nzComponentType_Quaternion:
			return true;
	}

	NazaraError("Component type not handled (0x" + NzString::Number(type, 16) + ')');
	return false;
}

bool NzParticleDeclaration::Initialize()
{
	try
	{
		NzErrorFlags flags(nzErrorFlag_Silent | nzErrorFlag_ThrowException);

		// Layout : Type
		NzParticleDeclaration* declaration;

		// nzParticleLayout_Billboard : NzParticleStruct_Billboard
		declaration = &s_declarations[nzParticleLayout_Billboard];
		declaration->EnableComponent(nzParticleComponent_Color,    nzComponentType_Color,  NzOffsetOf(NzParticleStruct_Billboard, color));
		declaration->EnableComponent(nzParticleComponent_Life,     nzComponentType_Int1,   NzOffsetOf(NzParticleStruct_Billboard, life));
		declaration->EnableComponent(nzParticleComponent_Normal,   nzComponentType_Float3, NzOffsetOf(NzParticleStruct_Billboard, normal));
		declaration->EnableComponent(nzParticleComponent_Position, nzComponentType_Float3, NzOffsetOf(NzParticleStruct_Billboard, position));
		declaration->EnableComponent(nzParticleComponent_Rotation, nzComponentType_Float1, NzOffsetOf(NzParticleStruct_Billboard, rotation));
		declaration->EnableComponent(nzParticleComponent_Velocity, nzComponentType_Float3, NzOffsetOf(NzParticleStruct_Billboard, velocity));

		NazaraAssert(declaration->GetStride() == sizeof(NzParticleStruct_Billboard), "Invalid stride for declaration nzParticleLayout_Billboard");

		// nzParticleLayout_Model : NzParticleStruct_Model
		declaration = &s_declarations[nzParticleLayout_Model];
		declaration->EnableComponent(nzParticleComponent_Life,     nzComponentType_Int1,       NzOffsetOf(NzParticleStruct_Model, life));
		declaration->EnableComponent(nzParticleComponent_Position, nzComponentType_Float3,     NzOffsetOf(NzParticleStruct_Model, position));
		declaration->EnableComponent(nzParticleComponent_Rotation, nzComponentType_Quaternion, NzOffsetOf(NzParticleStruct_Model, rotation));
		declaration->EnableComponent(nzParticleComponent_Velocity, nzComponentType_Float3,     NzOffsetOf(NzParticleStruct_Model, velocity));

		NazaraAssert(declaration->GetStride() == sizeof(NzParticleStruct_Model), "Invalid stride for declaration nzParticleLayout_Model");

		// nzParticleLayout_Sprite : NzParticleStruct_Sprite
		declaration = &s_declarations[nzParticleLayout_Sprite];
		declaration->EnableComponent(nzParticleComponent_Color,    nzComponentType_Color,  NzOffsetOf(NzParticleStruct_Sprite, color));
		declaration->EnableComponent(nzParticleComponent_Life,     nzComponentType_Int1,   NzOffsetOf(NzParticleStruct_Sprite, life));
		declaration->EnableComponent(nzParticleComponent_Position, nzComponentType_Float2, NzOffsetOf(NzParticleStruct_Sprite, position));
		declaration->EnableComponent(nzParticleComponent_Rotation, nzComponentType_Float1, NzOffsetOf(NzParticleStruct_Sprite, rotation));
		declaration->EnableComponent(nzParticleComponent_Velocity, nzComponentType_Float2, NzOffsetOf(NzParticleStruct_Sprite, velocity));

		NazaraAssert(declaration->GetStride() == sizeof(NzParticleStruct_Sprite), "Invalid stride for declaration nzParticleLayout_Sprite");
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to initialize particle declarations: " + NzString(e.what()));
		return false;
	}

	return true;
}

void NzParticleDeclaration::Uninitialize()
{
	// Rien à faire
}

NzParticleDeclaration NzParticleDeclaration::s_declarations[nzParticleLayout_Max+1];
