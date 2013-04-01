// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_OBJ_MTLPARSER_HPP
#define NAZARA_LOADERS_OBJ_MTLPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <map>

class NzMTLParser
{
	public:
		struct Material
		{
			NzColor ambient = NzColor::White;
			NzColor diffuse = NzColor::White;
			NzColor specular = NzColor::White;
			NzString alphaMap;
			NzString ambientMap;
			NzString bumpMap;
			NzString decalMap;
			NzString diffuseMap;
			NzString displacementMap;
			NzString reflectionMap;
			NzString shininessMap;
			NzString specularMap;
			float alpha = 1.f;
			float refractionIndex = 1.f;
			float shininess = 1.f;
			unsigned int illumModel = 0;
		};

		NzMTLParser(NzInputStream& stream$);
		~NzMTLParser();

		const Material* GetMaterial(const NzString& materialName) const;

		bool Parse();

	private:
		bool Advance(bool required = true);
		void Error(const NzString& message);
		void Warning(const NzString& message);
		void UnrecognizedLine(bool error = false);

		std::map<NzString, Material> m_materials;
		NzInputStream& m_stream;
		NzString m_currentLine;
		bool m_keepLastLine;
		unsigned int m_lineCount;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_OBJ_MTLPARSER_HPP
