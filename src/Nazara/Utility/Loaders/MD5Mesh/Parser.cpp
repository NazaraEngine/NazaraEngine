// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Mesh/Parser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

NzMD5MeshParser::NzMD5MeshParser(NzInputStream& stream, const NzMeshParams& parameters) :
m_stream(stream),
m_parameters(parameters),
m_keepLastLine(false),
m_lineCount(0),
m_meshIndex(0),
m_streamFlags(stream.GetStreamOptions())
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags | nzStreamOption_Text);
}

NzMD5MeshParser::~NzMD5MeshParser()
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags);
}

nzTernary NzMD5MeshParser::Check()
{
	if (Advance(false))
	{
		unsigned int version;
		if (std::sscanf(&m_currentLine[0], " MD5Version %u", &version) == 1)
		{
			if (version == 10)
				return nzTernary_True;
		}
	}

	return nzTernary_False;
}

bool NzMD5MeshParser::Parse(NzMesh* mesh)
{
	while (Advance(false))
	{
		switch (m_currentLine[0])
		{
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			case 'M': // MD5Version
				if (m_currentLine.GetWord(0) != "MD5Version")
					UnrecognizedLine();
				break;

			case 'c': // commandline
				if (m_currentLine.GetWord(0) != "commandline")
					UnrecognizedLine();
				break;
			#endif

			case 'j': // joints
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (!m_currentLine.StartsWith("joints {"))
				{
					UnrecognizedLine();
					break;
				}
				#endif

				if (!ParseJoints())
				{
					Error("Failed to parse joints");
					return false;
				}
				break;

			case 'm': // mesh
			{
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (m_currentLine != "mesh {")
				{
					UnrecognizedLine();
					break;
				}
				#endif

				if (m_meshIndex >= m_meshes.size())
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					Warning("More meshes than registred");
					#endif

					m_meshes.push_back(Mesh());
				}

				if (!ParseMesh())
				{
					NazaraError("Failed to parse mesh");
					return false;
				}

				m_meshIndex++;
				break;
			}

			case 'n': // num[Frames/Joints]
			{
				unsigned int count;
				if (std::sscanf(&m_currentLine[0], "numJoints %u", &count) == 1)
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (!m_joints.empty())
						Warning("Joint count is already defined");
					#endif

					m_joints.resize(count);
				}
				else if (std::sscanf(&m_currentLine[0], "numMeshes %u", &count) == 1)
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (!m_meshes.empty())
						Warning("Mesh count is already defined");
					#endif

					m_meshes.resize(count);
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
				break;
			}

			default:
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				UnrecognizedLine();
				#endif
				break;
		}
	}

	// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
	NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 180.f, 0.f);
	NzString baseDir = m_stream.GetDirectory();

	// Le hellknight de Doom 3 fait ~120 unités, et il est dit qu'il fait trois mètres
	// Nous réduisons donc sa taille de 1/40
	NzVector3f scale(m_parameters.scale/40.f);

	if (m_parameters.animated)
	{
		if (!mesh->CreateSkeletal(m_joints.size())) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		NzSkeleton* skeleton = mesh->GetSkeleton();
		for (unsigned int i = 0; i < m_joints.size(); ++i)
		{
			NzJoint* joint = skeleton->GetJoint(i);

			int parent = m_joints[i].parent;
			if (parent >= 0)
				joint->SetParent(skeleton->GetJoint(parent));

			joint->SetName(m_joints[i].name);

			NzMatrix4f bindMatrix;

			if (parent >= 0)
				bindMatrix.MakeTransform(m_joints[i].bindPos, m_joints[i].bindOrient);
			else
				bindMatrix.MakeTransform(rotationQuat * m_joints[i].bindPos, rotationQuat * m_joints[i].bindOrient, scale);

			joint->SetInverseBindMatrix(bindMatrix.InverseAffine());
		}

		mesh->SetMaterialCount(m_meshes.size());
		for (unsigned int i = 0; i < m_meshes.size(); ++i)
		{
			const Mesh& md5Mesh = m_meshes[i];

			unsigned int indexCount = md5Mesh.triangles.size()*3;
			unsigned int vertexCount = md5Mesh.vertices.size();
			unsigned int weightCount = md5Mesh.weights.size();

			// Index buffer
			bool largeIndices = (vertexCount > std::numeric_limits<nzUInt16>::max());

			std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(indexCount, largeIndices, m_parameters.storage));
			NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_DiscardAndWrite);

			unsigned int index = 0;
			for (const Mesh::Triangle& triangle : md5Mesh.triangles)
			{
				// On les respécifie dans le bon ordre
				indexMapper.Set(index++, triangle.x);
				indexMapper.Set(index++, triangle.z);
				indexMapper.Set(index++, triangle.y);
			}

			indexMapper.Unmap();

			std::unique_ptr<NzSkeletalMesh> subMesh(new NzSkeletalMesh(mesh));
			if (!subMesh->Create(vertexCount, weightCount))
			{
				NazaraError("Failed to create skeletal mesh");
				continue;
			}

			subMesh->SetIndexBuffer(indexBuffer.get());
			indexBuffer->SetPersistent(false);
			indexBuffer.release();

			NzWeight* weights = subMesh->GetWeight();
			for (unsigned int j = 0; j < weightCount; ++j)
			{
				weights->jointIndex = md5Mesh.weights[j].joint;
				weights->weight = md5Mesh.weights[j].bias;
				weights++;
			}

			NzMeshVertex* bindPosVertex = reinterpret_cast<NzMeshVertex*>(subMesh->GetBindPoseBuffer());
			NzVertexWeight* vertexWeight = subMesh->GetVertexWeight();
			for (const Mesh::Vertex& vertex : md5Mesh.vertices)
			{
				// Skinning MD5 (Formule d'Id Tech)
				NzVector3f finalPos(NzVector3f::Zero());

				vertexWeight->weights.resize(vertex.weightCount);
				for (unsigned int j = 0; j < vertex.weightCount; ++j)
				{
					const Mesh::Weight& weight = md5Mesh.weights[vertex.startWeight + j];
					const Joint& joint = m_joints[weight.joint];

					finalPos += (joint.bindPos + joint.bindOrient*weight.pos) * weight.bias;
					vertexWeight->weights[j] = vertex.startWeight + j;
				}

				bindPosVertex->position = scale * finalPos;
				bindPosVertex->uv.Set(vertex.uv.x, 1.f-vertex.uv.y);
				bindPosVertex++;
				vertexWeight++;
			}

			// Material
			mesh->SetMaterial(i, baseDir + md5Mesh.shader);
			subMesh->SetMaterialIndex(i);

			if (!mesh->AddSubMesh(subMesh.get()))
			{
				NazaraError("Failed to add submesh");
				continue;
			}

			subMesh.release();

			// Animation
			// Il est peut-être éventuellement possible que la probabilité que l'animation ait le même nom soit non-nulle.
			NzString path = m_stream.GetPath();
			if (!path.IsEmpty())
			{
				path.Replace(".md5mesh", ".md5anim", -8, NzString::CaseInsensitive);
				if (NzFile::Exists(path))
					mesh->SetAnimation(path);
			}
		}
	}
	else
	{
		if (!mesh->CreateStatic()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		mesh->SetMaterialCount(m_meshes.size());
		for (unsigned int i = 0; i < m_meshes.size(); ++i)
		{
			const Mesh& md5Mesh = m_meshes[i];
			unsigned int indexCount = md5Mesh.triangles.size()*3;
			unsigned int vertexCount = md5Mesh.vertices.size();

			// Index buffer
			bool largeIndices = (vertexCount > std::numeric_limits<nzUInt16>::max());

			std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(indexCount, largeIndices, m_parameters.storage));
			indexBuffer->SetPersistent(false);

			NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_DiscardAndWrite);
			NzIndexIterator index = indexMapper.begin();

			for (const Mesh::Triangle& triangle : md5Mesh.triangles)
			{
				// On les respécifie dans le bon ordre
				*index++ = triangle.x;
				*index++ = triangle.z;
				*index++ = triangle.y;
			}
			indexMapper.Unmap();

			// Vertex buffer
			std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzMesh::GetDeclaration(), vertexCount, m_parameters.storage));
			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);

			NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(vertexMapper.GetPointer());
			for (const Mesh::Vertex& md5Vertex : md5Mesh.vertices)
			{
				// Skinning MD5 (Formule d'Id Tech)
				NzVector3f finalPos(NzVector3f::Zero());
				for (unsigned int j = 0; j < md5Vertex.weightCount; ++j)
				{
					const Mesh::Weight& weight = md5Mesh.weights[md5Vertex.startWeight + j];
					const Joint& joint = m_joints[weight.joint];

					finalPos += (joint.bindPos + joint.bindOrient*weight.pos) * weight.bias;
				}

				// On retourne le modèle dans le bon sens
				vertex->position = scale * (rotationQuat * finalPos);
				vertex->uv.Set(md5Vertex.uv.x, 1.f - md5Vertex.uv.y);
				vertex++;
			}

			vertexMapper.Unmap();

			// Submesh
			std::unique_ptr<NzStaticMesh> subMesh(new NzStaticMesh(mesh));
			if (!subMesh->Create(vertexBuffer.get()))
			{
				NazaraError("Failed to create static submesh");
				continue;
			}

			vertexBuffer->SetPersistent(false);
			vertexBuffer.release();

			if (m_parameters.optimizeIndexBuffers)
				indexBuffer->Optimize();

			subMesh->SetIndexBuffer(indexBuffer.get());
			indexBuffer.release();

			// Material
			mesh->SetMaterial(i, baseDir + md5Mesh.shader);
			subMesh->GenerateAABB();
			subMesh->GenerateNormalsAndTangents();
			subMesh->SetMaterialIndex(i);

			if (!mesh->AddSubMesh(subMesh.get()))
			{
				NazaraError("Failed to add submesh");
				continue;
			}

			subMesh.release();
		}
	}

	return true;
}

bool NzMD5MeshParser::Advance(bool required)
{
	if (!m_keepLastLine)
	{
		do
		{
			if (m_stream.EndOfStream())
			{
				if (required)
					Error("Incomplete MD5 file");

				return false;
			}

			m_lineCount++;

			m_currentLine = m_stream.ReadLine();
			m_currentLine = m_currentLine.SubstrTo("//"); // On ignore les commentaires
			m_currentLine.Simplify(); // Pour un traitement plus simple
		}
		while (m_currentLine.IsEmpty());
	}
	else
		m_keepLastLine = false;

	return true;
}

void NzMD5MeshParser::Error(const NzString& message)
{
	NazaraError(message + " at line #" + NzString::Number(m_lineCount));
}

bool NzMD5MeshParser::ParseJoints()
{
	unsigned int jointCount = m_joints.size();
	if (jointCount == 0)
	{
		Error("Joint count is invalid or missing");
		return false;
	}

	for (unsigned int i = 0; i < jointCount; ++i)
	{
		if (!Advance())
			return false;

		unsigned int pos = m_currentLine.Find(' ');
		if (pos == NzString::npos)
		{
			UnrecognizedLine(true);
			return false;
		}

		if (pos >= 64)
		{
			NazaraError("Joint name is too long (>= 64 characters)");
			return false;
		}

		char name[64];
		if (std::sscanf(&m_currentLine[0], "%s %d ( %f %f %f ) ( %f %f %f )", &name[0], &m_joints[i].parent,
																			  &m_joints[i].bindPos.x, &m_joints[i].bindPos.y, &m_joints[i].bindPos.z,
																			  &m_joints[i].bindOrient.x, &m_joints[i].bindOrient.y, &m_joints[i].bindOrient.z) != 8)
		{
			UnrecognizedLine(true);
			return false;
		}

		m_joints[i].name = name;
		m_joints[i].name.Trim('"');

		int parent = m_joints[i].parent;
		if (parent >= 0)
		{
			if (static_cast<unsigned int>(parent) >= jointCount)
			{
				Error("Joint's parent is out of bounds (" + NzString::Number(parent) + " >= " + NzString::Number(jointCount) + ')');
				return false;
			}
		}

		m_joints[i].bindOrient.ComputeW(); // On calcule la composante W
	}

	if (!Advance())
		return false;

	if (m_currentLine != '}')
	{
		#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
		Warning("Hierarchy braces closing not found");
		#endif

		// On tente de survivre à l'erreur
		m_keepLastLine = true;
	}

	return true;
}

bool NzMD5MeshParser::ParseMesh()
{
	bool finished = false;
	while (!finished && Advance(false))
	{
		switch (m_currentLine[0])
		{
			case '}':
				finished = true;
				break;

			case 's': // shader
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (!m_currentLine.StartsWith("shader "))
				{
					UnrecognizedLine();
					break;
				}
				#endif

				m_meshes[m_meshIndex].shader = m_currentLine.Substr(7);
				m_meshes[m_meshIndex].shader.Trim('"');
				break;

			case 'n': // num[tris/verts]
			{
				unsigned int count;
				if (std::sscanf(&m_currentLine[0], "numtris %u", &count) == 1)
				{
					m_meshes[m_meshIndex].triangles.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Mesh::Triangle& triangle = m_meshes[m_meshIndex].triangles[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "tri %u %u %u %u", &index, &triangle.x, &triangle.y, &triangle.z) != 4)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected triangle index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
				}
				else if (std::sscanf(&m_currentLine[0], "numverts %u", &count) == 1)
				{
					m_meshes[m_meshIndex].vertices.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Mesh::Vertex& vertex = m_meshes[m_meshIndex].vertices[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "vert %u ( %f %f ) %u %u", &index, &vertex.uv.x, &vertex.uv.y, &vertex.startWeight, &vertex.weightCount) != 5)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected vertex index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
				}
				else if (std::sscanf(&m_currentLine[0], "numweights %u", &count) == 1)
				{
					m_meshes[m_meshIndex].weights.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Mesh::Weight& weight = m_meshes[m_meshIndex].weights[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "weight %u %u %f ( %f %f %f )", &index, &weight.joint, &weight.bias,
																						   &weight.pos.x, &weight.pos.y, &weight.pos.z) != 6)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected weight index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
				break;
			}

			default:
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				UnrecognizedLine();
				#endif
				break;
		}
	}

	if (m_meshes[m_meshIndex].triangles.size() == 0)
	{
		NazaraError("Mesh has no triangles");
		return false;
	}

	if (m_meshes[m_meshIndex].vertices.size() == 0)
	{
		NazaraError("Mesh has no vertices");
		return false;
	}

	if (m_meshes[m_meshIndex].weights.size() == 0)
	{
		NazaraError("Mesh has no weights");
		return false;
	}

	#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
	if (!finished)
		Warning("Mesh braces closing not found");
	#endif

	return true;
}

void NzMD5MeshParser::Warning(const NzString& message)
{
	NazaraWarning(message + " at line #" + NzString::Number(m_lineCount));
}

void NzMD5MeshParser::UnrecognizedLine(bool error)
{
	NzString message = "Unrecognized \"" + m_currentLine + '"';

	if (error)
		Error(message);
	else
		Warning(message);
}
