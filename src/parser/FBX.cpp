/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FBX.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/11 17:34:53 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/11 20:18:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/FBX.hpp"
#include "parser/InternalTools.hpp"
#include <iostream>

#pragma pack(1)
struct FBXArray
{
	unsigned		arrayLength;
	unsigned		encoding;
	unsigned		compressedEncoding;
	void			*data;
};

struct FBXRawData
{
	unsigned		length;
	unsigned char	*data;
};

struct FBXProperty
{
	unsigned char	typeCode;
	void			*data;
};

struct FBXNode
{
	unsigned					endOffset;
	unsigned					numProperties;
	unsigned					propertyListLen;
	unsigned char				nameLen;
	unsigned char				*name;
	std::vector<FBXProperty *>	properties;
	std::vector<FBXNode *>		nodes;
};

struct FBXHeader
{
	unsigned char	fileMagic[21];
	unsigned char	hex[2];
	unsigned		version;
};
#pragma pack()

FBXArray		*parseArray(unsigned char typeCode, FILE *fd)
{
	auto	array = new FBXArray;

	fread(array, 12, 1, fd);
	switch (typeCode) {
		case ('f') :
			array->data = new float[array->arrayLength];
			fread(array, array->arrayLength, sizeof(float), fd);
			break ;
		case ('d') :
			array->data = new double[array->arrayLength];
			fread(array, array->arrayLength, sizeof(double), fd);
			break ;
		case ('l') :
			array->data = new long long[array->arrayLength];
			fread(array, array->arrayLength, sizeof(long long), fd);
			break ;
		case ('i') :
			array->data = new int[array->arrayLength];
			fread(array, array->arrayLength, sizeof(int), fd);
			break ;
		case ('b') :
			array->data = new char[array->arrayLength];
			fread(array, array->arrayLength, sizeof(char), fd);
			break ;
	}
	return (array);
}

FBXRawData	*parseRawData(FILE *fd)
{
	auto	rawData = new FBXRawData;
	fread(&rawData->length, sizeof(unsigned), 1, fd);
	rawData->data = new unsigned char[rawData->length];
	fread(rawData->data, rawData->length, 1, fd);
	return (rawData);
}

FBXProperty	*parseProperty(FILE *fd)
{
	auto	property = new FBXProperty;

	fread(property, 1, 1, fd);
	std::cout <<
		"		Property :\n" <<
		"				typeCode : " << property->typeCode << std::endl;
	switch (property->typeCode) {
		case ('Y') :
			property->data = new short;
			fread(property->data, sizeof(short), 1, fd);
			break ;
		case ('C') :
			property->data = new char;
			fread(property->data, sizeof(char), 1, fd);
			break ;
		case ('I') :
			property->data = new int;
			fread(property->data, sizeof(int), 1, fd);
			break ;
		case ('F') :
			property->data = new float;
			fread(property->data, sizeof(float), 1, fd);
			break ;
		case ('D') :
			property->data = new double;
			fread(property->data, sizeof(double), 1, fd);
			break ;
		case ('L') :
			property->data = new long long;
			fread(property->data, sizeof(long long), 1, fd);
			break ;
		case ('R') :
		case ('S') :
			property->data = parseRawData(fd);
			break ;
		case ('f') :
		case ('d') :
		case ('l') :
		case ('i') :
		case ('b') :
			property->data = parseArray(property->typeCode, fd);
			break ;
	}
	return (property);
}

FBXNode	*parseNode(FILE *fd)
{
	FBXNode	n;

	fread(&n, 1, 13, fd);
	if (n.endOffset == 0) {
		std::cout << ftell(fd) << " " << n.endOffset << std::endl;
		return (nullptr);
	}
	auto	*node = new FBXNode(n);
	node->name = new unsigned char[int(node->nameLen + 1)];
	memset(node->name, 0, node->nameLen + 1);
	fread(node->name, 1, node->nameLen, fd);

	std::cout <<
		"	Node :\n" <<
		"		endOffset : " << node->endOffset << "\n" <<
		"		numProperties : " << node->numProperties << "\n" <<
		"		propertyListLen : " << node->propertyListLen << "\n" <<
		"		nameLen : " << int(node->nameLen) << "\n" <<
		"		name : " << node->name << std::endl;
	
	for (unsigned i = 0; i < node->numProperties; i++) {
		node->properties.push_back(parseProperty(fd));
	}
	while (ftell(fd) != long(n.endOffset)) {
		FBXNode *subNode = parseNode(fd);
		if (subNode != nullptr)
			node->nodes.push_back(subNode);
	}
	std::cout << ftell(fd) << " " << n.endOffset << std::endl;
	//fseek(fd, node->endOffset, SEEK_SET);
	return (node);
}

void FBX::parseBin(const std::string &path) {
	FILE	*fd;
	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	if ((fd = fopen(path.c_str(), "rb")) == nullptr) {
		throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
	}
	FBXHeader	header;
	fread(&header, sizeof(FBXHeader), 1, fd);
	std::cout <<
		"Header :\n" <<
		"	fileMagic : " << header.fileMagic << "\n" <<
		"	hex : " << header.hex << "\n" <<
		"	version : " << header.version << std::endl;
	std::vector<FBXNode *>	nodes;
	for (FBXNode *node = nullptr; (node = parseNode(fd)) != nullptr;)
		nodes.push_back(node);
}