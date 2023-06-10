#pragma once
#include <vector>
#include <string>

class Camera;
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

		//* Converts the OBJ to a BIN style format with extension '.obj.vemeta' *//
		void ConvertOBJtoBIN(const std::string& filePath);
		//*  *//
		void ReadMeshMetaData(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);

		bool CheckMeshFileExisting(const std::string& filePath) const;

	private:
		// Reading .OBJ file
		void LoadModel(const std::string& givenName, const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);
		void checkVertexExists(VertexCheck& vCheck1, VertexCheck& vCheck2, VertexCheck& vCheck3,  std::vector<Vertex_Input>& vBuffer);

		// Convert obj data to .BIN format
		void ConvertToBinFormat(const std::string& filePath, std::vector<Vertex_Input>& vBuffer, std::vector<uint32_t>& iBuffer);
};