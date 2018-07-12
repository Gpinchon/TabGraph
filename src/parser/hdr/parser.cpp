/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/22 01:23:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/07 23:03:05 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/HDR.hpp"
#include "parser/InternalTools.hpp"
#include <stdexcept>
#include <cstddef>

#include <iostream>

typedef unsigned char RGBE[4];
#define R			0
#define G			1
#define B			2
#define E			3

#define  MINELEN	8				// minimum scanline length for encoding
#define  MAXELEN	0x7fff			// maximum scanline length for encoding

static void workOnRGBE(RGBE *scan, int len, float *cols);
static bool decrunch(RGBE *scanline, int len, FILE *file);
static bool oldDecrunch(RGBE *scanline, int len, FILE *file);

Texture *HDR::parse(const std::string &texture_name, const std::string &path)
{
	int i;
	char str[200];
	FILE *file;
	VEC2	size;

	file = fopen(path.c_str(), "rb");
	if (!file)
		throw std::runtime_error("Invalid File");
	fread(str, 10, 1, file);
	if (memcmp(str, "#?RADIANCE", 10)) {
		fclose(file);
		throw std::runtime_error("Invalid Header");
	}
	fseek(file, 1, SEEK_CUR);
	i = 0;
	char c = 0, oldc;
	while (true) {
		oldc = c;
		c = fgetc(file);
		if (c == 0xa && oldc == 0xa)
			break;
	}
	char reso[200];
	i = 0;
	while (true) {
		c = fgetc(file);
		reso[i++] = c;
		if (c == 0xa)
			break;
	}
	long w, h;
	if (!sscanf(reso, "-Y %ld +X %ld", &h, &w)) {
		fclose(file);
		throw std::runtime_error("Invalid Resolution");
	}
	size.x = w;
	size.y = h;
	auto	cols = new float[w * h * 3];
	void	*data = static_cast<void*>(cols);
	RGBE *scanline = new RGBE[w];
	if (!scanline) {
		fclose(file);
		throw std::runtime_error("Invalid Scanline");
	}
	// convert image 
	for (int y = h - 1; y >= 0; y--) {
		if (decrunch(scanline, w, file) == false)
			break;
		workOnRGBE(scanline, w, cols);
		cols += w * 3;
	}

	delete [] scanline;
	fclose(file);

	auto	t = static_cast<HDR*>(Texture::create(texture_name, size,
				GL_TEXTURE_2D, GL_RGB, GL_COMPRESSED_RGB, GL_FLOAT));
	t->_data = static_cast<GLubyte*>(data);
	return (t);
}

float convertComponent(int expo, int val)
{
	float v = val / 256.0f;
	float d = (float) pow(2, expo);
	return v * d;
}

void workOnRGBE(RGBE *scan, int len, float *cols)
{
	while (len-- > 0) {
		int expo = scan[0][E] - 128;
		cols[0] = convertComponent(expo, scan[0][R]);
		cols[1] = convertComponent(expo, scan[0][G]);
		cols[2] = convertComponent(expo, scan[0][B]);
		cols += 3;
		scan++;
	}
}

bool decrunch(RGBE *scanline, int len, FILE *file)
{
	int  i, j;
					
	if (len < MINELEN || len > MAXELEN)
		return oldDecrunch(scanline, len, file);
	i = fgetc(file);
	if (i != 2) {
		fseek(file, -1, SEEK_CUR);
		return oldDecrunch(scanline, len, file);
	}
	scanline[0][G] = fgetc(file);
	scanline[0][B] = fgetc(file);
	i = fgetc(file);

	if (scanline[0][G] != 2 || scanline[0][B] & 128) {
		scanline[0][R] = 2;
		scanline[0][E] = i;
		return oldDecrunch(scanline + 1, len - 1, file);
	}
	// read each component
	for (i = 0; i < 4; i++) {
	    for (j = 0; j < len; ) {
			unsigned char code = fgetc(file);
			if (code > 128) { // run
			    code &= 127;
			    unsigned char val = fgetc(file);
			    while (code--)
					scanline[j++][i] = val;
			}
			else  {	// non-run
			    while(code--)
					scanline[j++][i] = fgetc(file);
			}
		}
    }

	return feof(file) ? false : true;
}

bool oldDecrunch(RGBE *scanline, int len, FILE *file)
{
	int i;
	int rshift = 0;
	
	while (len > 0) {
		scanline[0][R] = fgetc(file);
		scanline[0][G] = fgetc(file);
		scanline[0][B] = fgetc(file);
		scanline[0][E] = fgetc(file);
		if (feof(file))
			return false;
		if (scanline[0][R] == 1 &&
			scanline[0][G] == 1 &&
			scanline[0][B] == 1) {
			for (i = scanline[0][E] << rshift; i > 0; i--) {
				memcpy(&scanline[0][0], &scanline[-1][0], 4);
				scanline++;
				len--;
			}
			rshift += 8;
		}
		else {
			scanline++;
			len--;
			rshift = 0;
		}
	}
	return true;
}


/*struct hdr_parser
{
	char	intro[10];
	VEC2	size;
	std::vector<uint8_t>	rawData;
	std::vector<float>		uncompressedData;
};

//offsets to red, green, and blue components in a data (float) pixel
#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
//number of floats per pixel
#define RGBE_DATA_SIZE   3

void rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
{
	float f;

	if (rgbe[3]) {   //nonzero pixel
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
    //run length encoding is not allowed so read flat
		return RGBE_ReadPixels(fp,data,scanline_width*num_scanlines);
	scanline_buffer = NULL;
  //read in each successive scanline
	while(num_scanlines > 0) {
		if (fread(rgbe,sizeof(rgbe),1,fp) < 1) {
			free(scanline_buffer);
			throw std::runtime_error(NULL);
		}
		if ((rgbe[0] != 2)||(rgbe[1] != 2)||(rgbe[2] & 0x80)) {
      // this file is not run length encoded
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
    // read each of the four channels for the scanline into the buffer
		for(i=0;i<4;i++) {
			ptr_end = &scanline_buffer[(i+1)*scanline_width];
			while(ptr < ptr_end) {
				if (fread(buf,sizeof(buf[0])*2,1,fp) < 1) {
					free(scanline_buffer);
					throw std::runtime_error(NULL);
				}
				if (buf[0] > 128) {
	  // a run of the same value
					count = buf[0]-128;
					if ((count == 0)||(count > ptr_end - ptr)) {
						free(scanline_buffer);
						throw std::runtime_error("bad scanline data");
					}
					while(count-- > 0)
						*ptr++ = buf[1];
				}
				else {
	  // a non-run
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
    //now convert data from buffer into floats
		for(i=0;i<scanline_width;i++) {
			rgbe[0] = scanline_buffer[i];
			rgbe[1] = scanline_buffer[i+scanline_width];
			rgbe[2] = scanline_buffer[i+2*scanline_width];
			rgbe[3] = scanline_buffer[i+3*scanline_width];
			//std::cout << rgbe[0] << " " << rgbe[1] << " " << rgbe[2] << " " << rgbe[3] << std::endl;
			rgbe2float(&data[RGBE_DATA_RED],&data[RGBE_DATA_GREEN],
				&data[RGBE_DATA_BLUE],rgbe);
			//std::cout << data[RGBE_DATA_RED] << " " << data[RGBE_DATA_GREEN] << " " << data[RGBE_DATA_BLUE] << std::endl;
			data += RGBE_DATA_SIZE;
				
		}
		num_scanlines--;
	}
	free(scanline_buffer);
	return (1);
}

Texture	*HDR::parse(const std::string &texture_name, const std::string &path)
{
	hdr_parser	parser;
	//auto		t = static_cast<HDR*>Texture::create(texture_name);

	FILE * stream = fopen(path.c_str(), "rb");
	fread(&parser.intro, sizeof(char), 11, stream);
	if (strncmp(parser.intro, "#?RADIANCE\n", 11)) {
		throw std::runtime_error("Incorrect Header");
	}
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
	parser.rawData = stream_to_vector<uint8_t>(stream, ftell(stream));
	void	*data = new float [size_t(parser.size.x * parser.size.y) * 3];
	RGBE_ReadPixels_RLE(stream, static_cast<float*>(data), parser.size.y, parser.size.x);
	auto		t = static_cast<HDR*>(Texture::create(texture_name, parser.size,
				GL_TEXTURE_2D, GL_RGB, GL_COMPRESSED_RGB, GL_FLOAT));
	t->_data = static_cast<GLubyte*>(data);
	Engine::add(*t);
	return (t);
}
*/