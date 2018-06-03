/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/22 01:23:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/06/03 20:15:03 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/HDR.hpp"
#include "parser/InternalTools.hpp"
#include <stdexcept>
#include <cstddef>

struct hdr_parser
{
	char	intro[10];
	VEC2	size;
	std::vector<uint8_t>	rawData;
	std::vector<float>		uncompressedData;
};

#include <iostream>

/* offsets to red, green, and blue components in a data (float) pixel */
#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
/* number of floats per pixel */
#define RGBE_DATA_SIZE   3

void rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
{
	float f;

  if (rgbe[3]) {   /*nonzero pixel*/
	f = ldexp(1.0,rgbe[3]-(int)(128+8));
	*red = rgbe[0] * f;
	*green = rgbe[1] * f;
	*blue = rgbe[2] * f;
}
else
	*red = *green = *blue = 0.0;
}

int RGBE_ReadPixels(FILE *fp, float *data, int numpixels)
{
  unsigned char rgbe[4];

  while(numpixels-- > 0) {
    if (fread(rgbe, sizeof(rgbe), 1, fp) < 1)
      throw std::runtime_error(NULL);
    rgbe2float(&data[RGBE_DATA_RED],&data[RGBE_DATA_GREEN],
	       &data[RGBE_DATA_BLUE],rgbe);
    data += RGBE_DATA_SIZE;
  }
  return 1;
}

int RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width,
	int num_scanlines)
{
	unsigned char rgbe[4], *scanline_buffer, *ptr, *ptr_end;
	int i, count;
	unsigned char buf[2];

	if ((scanline_width < 8)||(scanline_width > 0x7fff))
    /* run length encoding is not allowed so read flat*/
		return RGBE_ReadPixels(fp,data,scanline_width*num_scanlines);
	scanline_buffer = NULL;
  /* read in each successive scanline */
	while(num_scanlines > 0) {
		if (fread(rgbe,sizeof(rgbe),1,fp) < 1) {
			free(scanline_buffer);
			throw std::runtime_error(NULL);
		}
		if ((rgbe[0] != 2)||(rgbe[1] != 2)||(rgbe[2] & 0x80)) {
      /* this file is not run length encoded */
			rgbe2float(&data[0],&data[1],&data[2],rgbe);
			data += RGBE_DATA_SIZE;
			free(scanline_buffer);
			return RGBE_ReadPixels(fp,data,scanline_width*num_scanlines-1);
		}
		if ((((int)rgbe[2])<<8 | rgbe[3]) != scanline_width) {
			free(scanline_buffer);
			throw std::runtime_error("wrong scanline width");
		}
		if (scanline_buffer == NULL)
			scanline_buffer = (unsigned char *)
		malloc(sizeof(unsigned char)*4*scanline_width);
		if (scanline_buffer == NULL) 
			throw std::runtime_error("unable to allocate buffer space");

		ptr = &scanline_buffer[0];
    /* read each of the four channels for the scanline into the buffer */
		for(i=0;i<4;i++) {
			ptr_end = &scanline_buffer[(i+1)*scanline_width];
			while(ptr < ptr_end) {
				if (fread(buf,sizeof(buf[0])*2,1,fp) < 1) {
					free(scanline_buffer);
					throw std::runtime_error(NULL);
				}
				if (buf[0] > 128) {
	  /* a run of the same value */
					count = buf[0]-128;
					if ((count == 0)||(count > ptr_end - ptr)) {
						free(scanline_buffer);
						throw std::runtime_error("bad scanline data");
					}
					while(count-- > 0)
						*ptr++ = buf[1];
				}
				else {
	  /* a non-run */
					count = buf[0];
					if ((count == 0)||(count > ptr_end - ptr)) {
						free(scanline_buffer);
						throw std::runtime_error("bad scanline data");
					}
					*ptr++ = buf[1];
					if (--count > 0) {
						if (fread(ptr,sizeof(*ptr)*count,1,fp) < 1) {
							free(scanline_buffer);
							throw std::runtime_error(NULL);
						}
						ptr += count;
					}
				}
			}
		}
    /* now convert data from buffer into floats */
		for(i=0;i<scanline_width;i++) {
			rgbe[0] = scanline_buffer[i];
			rgbe[1] = scanline_buffer[i+scanline_width];
			rgbe[2] = scanline_buffer[i+2*scanline_width];
			rgbe[3] = scanline_buffer[i+3*scanline_width];
			rgbe2float(&data[RGBE_DATA_RED],&data[RGBE_DATA_GREEN],
				&data[RGBE_DATA_BLUE],rgbe);
			data += RGBE_DATA_SIZE;
		}
		num_scanlines--;
	}
	free(scanline_buffer);
	return (1);
}


Cubemap	*HDR::parse(const std::string &texture_name, const std::string &path)
{
	hdr_parser	parser;
	auto		t = Cubemap::create(texture_name);

	FILE * stream = fopen(path.c_str(), "rb");
	fread(&parser.intro, sizeof(char), 11, stream);
	if (strncmp(parser.intro, "#?RADIANCE\n", 11))
		throw std::runtime_error("Incorrect Header");
	char		 line[4096];
	std::string	lineString;
	do {
		fgets(line, 4096, stream);
		lineString = line;
		std::cout << lineString;
	} while (lineString.length() > 1);
	while (lineString == "\n")
	{
		fgets(line, 4096, stream);
		lineString = line;
	}
	std::cout << lineString;
	auto lineSplit = strsplitwspace(lineString);
	parser.size.x = std::stof(lineSplit.at(1));
	parser.size.y = std::stof(lineSplit.at(3));
	std::cout << parser.size.x << " " << parser.size.y << std::endl;
	//parser.rawData.resize(parser.size.x * parser.size.y * 4);
	parser.rawData = stream_to_vector<uint8_t>(stream, ftell(stream));
	float	*data = new float [size_t(parser.size.x * parser.size.y) * 3];
	RGBE_ReadPixels_RLE(stream, data, parser.size.x, parser.size.y);
	//std::cout << "dataString "  << dataString.size() << std::endl;
	//fread(&parser.rawData[0], sizeof(uint8_t), parser.size.x * parser.size.y * 4, stream);
	Engine::add(*t);
	return (t);
}
