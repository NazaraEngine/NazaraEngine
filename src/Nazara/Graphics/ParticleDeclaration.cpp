// Copyright (C) 2017 Jérôme Leclercq
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
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleDeclaration
	* \brief Graphics class that represents the declaration of the particle, works like an ECS
	*/

	/*!
	* \brief Constructs a ParticleDeclaration object by default
	*/

	ParticleDeclaration::ParticleDeclaration() :
	m_stride(0)
	{
	}

	/*!
	* \brief Constructs a ParticleDeclaration object by assignation
	*
	* \param declaration ParticleDeclaration to copy into this
	*/

	ParticleDeclaration::ParticleDeclaration(const ParticleDeclaration& declaration) :
	RefCounted(),
	m_components(declaration.m_components),
	m_stride(declaration.m_stride)
	{
	}

	/*!
	* \brief Destructs the object and calls OnParticleDeclarationRelease
	*
	* \see OnParticleDeclarationRelease
	*/

	ParticleDeclaration::~ParticleDeclaration()
	{
		OnParticleDeclarationRelease(this);
	}

	/*!
	* \brief Disables a component
	*
	* \param component Component to disable in the declaration
	*
	* \remark Produces a NazaraError with NAZARA_DEBUG defined if enumeration is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if enumeration is equal to ParticleComponent_Unused
	*/

	void ParticleDeclaration::DisableComponent(ParticleComponent component)
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (component == ParticleComponent_Unused)
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

	/*!
	* \brief Enables a component
	*
	* \param component Component to enable in the declaration
	* \param type Type of this component
	* \param offset Offset in the declaration
	*
	* \remark Produces a NazaraError with NAZARA_DEBUG defined if enumeration is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if type is not supported
	*/

	void ParticleDeclaration::EnableComponent(ParticleComponent component, ComponentType type, std::size_t offset)
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Vertex component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (!IsTypeSupported(type))
		{
			NazaraError("Component type 0x" + String::Number(type, 16) + " is not supported by particle declarations");
			return;
		}
		#endif

		if (component != ParticleComponent_Unused)
		{
			Component& particleComponent = m_components[component];
			if (particleComponent.enabled)
				m_stride -= Utility::ComponentStride[particleComponent.type];
			else
				particleComponent.enabled = true;

			particleComponent.offset = offset;
			particleComponent.type = type;
		}

		m_stride += Utility::ComponentStride[type];
	}

	/*!
	* \brief Gets a component
	*
	* \param component Component in the declaration
	* \param enabled Optional argument to get if this component is enabled
	* \param type Optional argument to get if the type of the component
	* \param offset Optional argument to get if the offset in the declaration
	*
	* \remark Produces a NazaraError with NAZARA_DEBUG defined if enumeration is invalid
	* \remark Produces a NazaraError with NAZARA_GRAPHICS_SAFE defined if enumeration is equal to ParticleComponent_Unused
	*/

	void ParticleDeclaration::GetComponent(ParticleComponent component, bool* enabled, ComponentType* type, std::size_t* offset) const
	{
		#ifdef NAZARA_DEBUG
		if (component > ParticleComponent_Max)
		{
			NazaraError("Particle component out of enum");
			return;
		}
		#endif

		#if NAZARA_GRAPHICS_SAFE
		if (component == ParticleComponent_Unused)
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

	/*!
	* \brief Gets the stride of the declaration
	* \return Stride of the declaration
	*/

	std::size_t ParticleDeclaration::GetStride() const
	{
		return m_stride;
	}

	/*!
	* \brief Sets the stride of the declaration
	*
	* \param stride Stride of the declaration
	*/

	void ParticleDeclaration::SetStride(unsigned int stride)
	{
		m_stride = stride;
	}

	/*!
	* \brief Sets the current particle declaration with the content of the other one
	* \return A reference to this
	*
	* \param declaration The other ParticleDeclaration
	*/

	ParticleDeclaration& ParticleDeclaration::operator=(const ParticleDeclaration& declaration)
	{
		m_components = declaration.m_components;
		m_stride = declaration.m_stride;

		return *this;
	}

	/*!
	* \brief Gets the particle declaration based on the layout
	* \return Pointer to the declaration
	*
	* \param layout Layout of the particle declaration
	*
	* \remark Produces a NazaraError with NAZARA_DEBUG if enumeration is invalid
	*/

	ParticleDeclaration* ParticleDeclaration::Get(ParticleLayout layout)
	{
		NazaraAssert(layout <= ParticleLayout_Max, "Particle layout out of enum");

		return &s_declarations[layout];
	}

	/*!
	* \brief Checks whether the type is supported
	* \return true If it is the case
	*
	* \param type Type of the component
	*
	* \remark Produces a NazaraError if enumeration is invalid
	*/

	bool ParticleDeclaration::IsTypeSupported(ComponentType type)
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
			case ComponentType_Quaternion:
				return true;
		}

		NazaraError("Component type not handled (0x" + String::Number(type, 16) + ')');
		return false;
	}

	/*!
	* \brief Initializes the particle declaration librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the particle declaration library failed to be initialized
	* \remark Produces a NazaraAssert if memory layout of declaration does not match the corresponding structure
	*/

	bool ParticleDeclaration::Initialize()
	{
		if (!ParticleDeclarationLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		try
		{
			ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowException);

			// Layout : Type
			ParticleDeclaration* declaration;

			// ParticleLayout_Billboard : ParticleStruct_Billboard
			declaration = &s_declarations[ParticleLayout_Billboard];
			declaration->EnableComponent(ParticleComponent_Color,    ComponentType_Color,  NazaraOffsetOf(ParticleStruct_Billboard, color));
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Billboard, life));
			declaration->EnableComponent(ParticleComponent_Normal,   ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, normal));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Billboard, rotation));
			declaration->EnableComponent(ParticleComponent_Size,     ComponentType_Float2, NazaraOffsetOf(ParticleStruct_Billboard, size));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Billboard, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Billboard), "Invalid stride for declaration ParticleLayout_Billboard");

			// ParticleLayout_Model : ParticleStruct_Model
			declaration = &s_declarations[ParticleLayout_Model];
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Float1,     NazaraOffsetOf(ParticleStruct_Model, life));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float3,     NazaraOffsetOf(ParticleStruct_Model, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Quaternion, NazaraOffsetOf(ParticleStruct_Model, rotation));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float3,     NazaraOffsetOf(ParticleStruct_Model, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Model), "Invalid stride for declaration ParticleLayout_Model");

			// ParticleLayout_Sprite : ParticleStruct_Sprite
			declaration = &s_declarations[ParticleLayout_Sprite];
			declaration->EnableComponent(ParticleComponent_Color,    ComponentType_Color,  NazaraOffsetOf(ParticleStruct_Sprite, color));
			declaration->EnableComponent(ParticleComponent_Life,     ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Sprite, life));
			declaration->EnableComponent(ParticleComponent_Position, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Sprite, position));
			declaration->EnableComponent(ParticleComponent_Rotation, ComponentType_Float1, NazaraOffsetOf(ParticleStruct_Sprite, rotation));
			declaration->EnableComponent(ParticleComponent_Velocity, ComponentType_Float3, NazaraOffsetOf(ParticleStruct_Sprite, velocity));

			NazaraAssert(declaration->GetStride() == sizeof(ParticleStruct_Sprite), "Invalid stride for declaration ParticleLayout_Sprite");
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize particle declarations: " + String(e.what()));
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the particle declaration librairies
	*/

	void ParticleDeclaration::Uninitialize()
	{
		ParticleDeclarationLibrary::Uninitialize();
	}

	std::array<ParticleDeclaration, ParticleLayout_Max + 1> ParticleDeclaration::s_declarations;
	ParticleDeclarationLibrary::LibraryMap ParticleDeclaration::s_library;
}
