#pragma once
#include <vector>
#include <string>

struct Vertex_Input;
struct VertexCheck;
class OBJParser final
{
	public:
		OBJParser() = default;
		~OBJParser() = default;

		OBJParser(const OBJParser&) = delete;
		OBJParser(OBJParser&&) noexcept = delete;
		OBJParser& operator=(const OBJParser&) = delete;
		OBJParser& operator=(OBJParser&&) noexcept = delete;

		//* Function: ..LoadOBJModel.. 
		//* loads in the obj model
		//* When the .obj.vemeta already exists, it will read from that, otherwise it will load in the OBJ and write to a .obj.vemeta file *//
		void LoadOBJModel(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);
		//* Function: ..ConvertToBinFormat.. 
		//* Convert OBJ data to .BIN format *//
		void ConvertToBinFormat(const std::string& filePath, const std::vector<Vertex_Input>& vBuffer, const std::vector<uint32_t>& iBuffer);

	private:
		//* Function: ..ConvertOBJtoBIN.. 
		//* Converts the OBJ to a BIN style format with extension '.obj.vemeta' *//
		void ConvertOBJtoBIN(const std::string& filePath);
		//* Function: ..ReadMeshMetaData.. 
		//* Reads in all the data from the given filepath *//
		void ReadMeshMetaData(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);
		//* Function: ..CheckMeshFileExisting.. 
		//* Checks if the binary file already exists on the drive*//
		bool CheckMeshFileExisting(const std::string& binFilePath) const;

		//* Function: ..LoadModel.. 
		//* Reads in all the OBJ data *//
		void LoadModel(const std::string& givenName, const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);
		//* Function: ..checkVertexExists.. 
		//* Checks if the given vertices already exist in the vertexbuffer *//
		void checkVertexExists(VertexCheck& vCheck1, VertexCheck& vCheck2, VertexCheck& vCheck3,  std::vector<Vertex_Input>& vBuffer);
};