#include "VanaheimPCH.h"
#include "ImageGenerator.h"

#include "Image.h"

void ImageGenerator::GenerateImage(const std::vector<std::vector<DirectX::XMFLOAT3>>& map, const std::string& filePath, const DirectX::XMFLOAT2& mapSize)
{
	Image image({ mapSize.x, mapSize.y });
	image.SetColor(map);
	image.ExportImage(filePath);
}
void ImageGenerator::GenerateImage(const std::vector<std::vector<float>>& map, const std::string& filePath, const DirectX::XMFLOAT2& mapSize)
{
	Image image({ mapSize.x, mapSize.y });
	image.SetColor(map);
	image.ExportImage(filePath);
}