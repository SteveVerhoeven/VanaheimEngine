#include "VanaheimPCH.h"
#include "OBJParser.h"
#include <filesystem>

struct VertexCheck
{
	bool found{ false };
	int foundIndex{ -1 };
	Vertex_Input vertexToCheck;
};

void OBJParser::ConvertOBJtoBIN(const std::string& filePath)
{
	std::string filePathCopy{ filePath };
	const std::string binFilePath{ filePathCopy.append(".vemeta")};
	if (!std::filesystem::exists(binFilePath))
	{
		std::cout << "Does not exists \n";

		// Load OBJ into buffers (Vertex & Index)
		std::vector<Vertex_Input> vBuffer{};
		std::vector<uint32_t> iBuffer{};
		LoadModel("name", filePath, vBuffer, iBuffer);
		
		// Convert OBJ to BIN format
		ConvertToBinFormat(binFilePath, vBuffer, iBuffer);
	}	
	else
	{
		std::cout << "Exists \n";
	}
}

void OBJParser::ReadMeshMetaData(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer)
{

}

bool OBJParser::CheckMeshFileExisting(const std::string& filePath) const
{
	std::string filePathCopy{ filePath };
	return std::filesystem::exists(filePathCopy.append(".vemeta"));
}

void OBJParser::LoadModel(const std::string& givenName, const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer)
{
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> UVs;

	std::string name = givenName;

	std::ifstream in(filePath, std::ios::in);
	if (!in)
	{
		std::cout << ("Model \"" + filePath + "\" does not exist.");
		return;
	}

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 1) == "#" || line.substr(0, 1) == "")
			continue;

		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s{ line.substr(2) };
			DirectX::XMFLOAT3 v{};
			s >> v.x >> v.y >> v.z;
			positions.push_back(DirectX::XMFLOAT3{ v.x, v.y, -v.z });
		}
		else if (line.substr(0, 2) == "vt")
		{
			std::istringstream s(line.substr(2));
			DirectX::XMFLOAT2 v{};
			s >> v.x >> v.y;
			UVs.push_back(DirectX::XMFLOAT2(v.x, 1 - v.y));

			/* Calculation is happening with the Y component on the bottom
			*  So "1 - uv.y" is to convert from OBJ parser uv to the UV used in the rasterizer
			* --------------------------------------------------------------------------------
			*	             UV space rasterizer	   --		UV space OBJ-parser
			*                 [0, 1]---[1, 1]                     [0, 0]---[1, 0]
			*                  ||       ||	                       ||       ||
			*                  ||       ||	                       ||       ||
			*                 [0, 0]---[1, 0]                     [0, 1]---[1, 1]
			* --------------------------------------------------------------------------------
			*/
		}
		else if (line.substr(0, 2) == "vn")
		{
			std::istringstream s(line.substr(2));
			DirectX::XMFLOAT3 v{};
			s >> v.x >> v.y >> v.z;
			normals.push_back(DirectX::XMFLOAT3{ v.x, v.y, -v.z });
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream s(line.substr(2));
			unsigned short v1, v2, v3;
			unsigned short vt1, vt2, vt3;
			unsigned short vn1, vn2, vn3;
			char slash{};
			s >> v1 >> slash >> vt1 >> slash >> vn1
			  >> v2 >> slash >> vt2 >> slash >> vn2
			  >> v3 >> slash >> vt3 >> slash >> vn3;

			Vertex_Input vert0{};
			vert0.Position	= DirectX::XMFLOAT3{ positions[v1] };
			vert0.Color		= DirectX::XMFLOAT3{ 1.f, 0.f, 0.f };

			Vertex_Input vert1{};
			vert1.Position	= DirectX::XMFLOAT3{ positions[v2] };
			vert1.Color		= DirectX::XMFLOAT3{ 0.f, 0.f, 1.f };

			Vertex_Input vert2{};
			vert2.Position	= DirectX::XMFLOAT3{ positions[v3] };
			vert2.Color		= DirectX::XMFLOAT3{ 0.f, 1.f, 0.f };

			VertexCheck vCheck1{};	vCheck1.vertexToCheck = vert0;
			VertexCheck vCheck2{};	vCheck2.vertexToCheck = vert1;
			VertexCheck vCheck3{};	vCheck3.vertexToCheck = vert2;

			checkVertexExists(vCheck1, vCheck2, vCheck3, vBuffer);
			{
				if (!vCheck1.found)
				{
					vBuffer.push_back(vert0);
					iBuffer.push_back((uint32_t)vBuffer.size() - 1);
				}
				else
				{
					iBuffer.push_back((uint32_t)vCheck1.foundIndex);
				}
				if (!vCheck2.found)
				{
					vBuffer.push_back(vert1);
					iBuffer.push_back((uint32_t)vBuffer.size() - 1);
				}
				else
				{
					iBuffer.push_back((uint32_t)vCheck2.foundIndex);
				}
				if (!vCheck3.found)
				{
					vBuffer.push_back(vert2);
					iBuffer.push_back((uint32_t)vBuffer.size() - 1);
				}
				else
				{
					iBuffer.push_back((uint32_t)vCheck3.foundIndex);
				}
			}
		}
	}

	std::cout << ("Model \"" + name + "\" parsed & loaded in vBuffer, iBuffer. \n");
}

void OBJParser::checkVertexExists(VertexCheck& vCheck0, VertexCheck& vCheck1, VertexCheck& vCheck2, std::vector<Vertex_Input>& vBuffer)
{
	for (size_t i{}; i < vBuffer.size(); ++i)
	{
		if (!vCheck0.found && vBuffer[i] == vCheck0.vertexToCheck)
		{
			vCheck0.found = true;
			vCheck0.foundIndex = (int)i;
		}
		if (!vCheck1.found && vBuffer[i] == vCheck1.vertexToCheck)
		{
			vCheck1.found = true;
			vCheck1.foundIndex = (int)i;
		}
		if (!vCheck2.found && vBuffer[i] == vCheck2.vertexToCheck)
		{
			vCheck2.found = true;
			vCheck2.foundIndex = (int)i;
		}

		if (vCheck0.found && vCheck1.found && vCheck2.found)
			break;
	}
}

void OBJParser::ConvertToBinFormat(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer)
{
	std::ofstream wf(filePath, std::ios::binary);
	if (!wf)
	{
		std::cout << "Cannot open file!" << std::endl;
		return;
	}

	const size_t vBufferSize{ vBuffer.size() };
	wf.write((char*)&vBufferSize, sizeof(size_t));
	for (size_t vertex = 0; vertex < vBufferSize; ++vertex)
	{
		wf.write((char*)&vBuffer[vertex], sizeof(Vertex_Input));
	}

	const size_t iBufferSize{ iBuffer.size() };
	wf.write((char*)&iBufferSize, sizeof(size_t));
	for (size_t index = 0; index < iBufferSize; ++index)
	{
		wf.write((char*)&iBuffer[index], sizeof(uint32_t));
	}
}
