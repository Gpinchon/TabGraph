#include "Texture.hpp"
#include "Debug.hpp"
#include "TextureParser.hpp"
#include "parser/TerrainData.hpp"
#include <gdal_priv.h>

//TextureParser __tifParser("tif", TerrainData::parse);
//TextureParser __gtifParser("gtif", TerrainData::parse);
//TextureParser __tiffParser("tiff", TerrainData::parse);


std::shared_ptr<Texture>  TerrainData::parse(const std::string& texture_name, const std::string& imagepath)
{
	GDALAllRegister();
	auto data = (GDALDataset*)GDALOpen(imagepath.c_str(), GA_ReadOnly );
	if (data == nullptr)
		debugLog("Could not open " + imagepath);
	auto	band = data->GetRasterBand( 1 );
	VEC2	size {float(band->GetXSize()), float(band->GetYSize())};
	float	*scanLine = (float *) CPLMalloc(sizeof(float) * size.x * size.y);
	auto	err = band->RasterIO(GF_Read, 0, 0, size.x, size.y,
	                  scanLine, size.x, size.y, GDT_Float32,
	                  0, 0);
	if (err != CE_None)
	{
		debugLog("Something went wrong with : " + imagepath);
		return TextureParser::parse(texture_name, imagepath);
	}
	std::string proj = data->GetProjectionRef();
	debugLog(proj);
	GDALClose(data);
	auto t = Texture::create(texture_name, size, GL_TEXTURE_2D,
    GL_RED, GL_COMPRESSED_RED, GL_FLOAT, scanLine);
	CPLFree(scanLine);
	return t;
}
