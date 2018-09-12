/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FBX.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/11 17:34:53 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/12 10:59:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/FBX.hpp"
#include "parser/InternalTools.hpp"
#include <iostream>

#pragma pack(1)
struct FBXArray
{
	unsigned		length;
	unsigned		encoding;
	unsigned		compressedLength;
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
	std::cout << "			ArrayLength : " << array->length << "\n";
	std::cout << "			ArrayEncoding : " << array->encoding << "\n";
	std::cout << "			CompressedLength : " << array->compressedLength << "\n";
	if (array->encoding == 0)
	{
		switch (typeCode) {
			case ('f') :
				array->data = new float[array->length];
				fread(array->data, array->length, sizeof(float), fd);
				break ;
			case ('d') :
				array->data = new double[array->length];
				fread(array->data, array->length, sizeof(double), fd);
				break ;
			case ('l') :
				array->data = new long long[array->length];
				fread(array->data, array->length, sizeof(long long), fd);
				break ;
			case ('i') :
				array->data = new int[array->length];
				fread(array->data, array->length, sizeof(int), fd);
				break ;
			case ('b') :
				array->data = new char[array->length];
				fread(array->data, array->length, sizeof(char), fd);
				break ;
		}
	}
	else {
		array->data = new unsigned char[array->compressedLength];
		fread(array->data, array->compressedLength, 1, fd);
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

FBXRawData	*parseString(FILE *fd)
{
	auto	rawString = new FBXRawData;
	fread(&rawString->length, sizeof(unsigned), 1, fd);
	rawString->data = new unsigned char[rawString->length + 1];
	memset(rawString->data, 0, rawString->length + 1);
	fread(rawString->data, rawString->length, 1, fd);
	return (rawString);
}

FBXProperty	*parseProperty(FILE *fd)
{
	auto	property = new FBXProperty;

	fread(property, 1, 1, fd);
	std::cout <<
		"	Property :\n" <<
		"			typeCode : " << property->typeCode << std::endl;
	switch (property->typeCode) {
		case ('Y') :
			property->data = new short;
			fread(property->data, sizeof(short), 1, fd);
			std::cout << "			value : " << *(short*)(property->data) << std::endl;
			break ;
		case ('C') :
			property->data = new char;
			fread(property->data, sizeof(char), 1, fd);
			std::cout << "			value : " << *(char*)(property->data) << std::endl;
			break ;
		case ('I') :
			property->data = new int;
			fread(property->data, sizeof(int), 1, fd);
			std::cout << "			value : " << *(int*)(property->data) << std::endl;
			break ;
		case ('F') :
			property->data = new float;
			fread(property->data, sizeof(float), 1, fd);
			std::cout << "			value : " << *(float*)(property->data) << std::endl;
			break ;
		case ('D') :
			property->data = new double;
			fread(property->data, sizeof(double), 1, fd);
			std::cout << "			value : " << *(double*)(property->data) << std::endl;
			break ;
		case ('L') :
			property->data = new long long;
			fread(property->data, sizeof(long long), 1, fd);
			std::cout << "			value : " << *(long long*)(property->data) << std::endl;
			break ;
		case ('R') :
			property->data = parseRawData(fd);
			break ;
		case ('S') :
			property->data = parseString(fd);
			break ;
		case ('f') :
		case ('d') :
		case ('l') :
		case ('i') :
		case ('b') :
			property->data = parseArray(property->typeCode, fd);
			break ;
	}
	if (property->typeCode == 'S') {
		auto	rawData = static_cast<FBXRawData*>(property->data);
		std::cout << "			string : " << (char*)(rawData->data) << std::endl;
	}
	return (property);
}

FBXNode	*parseNode(FILE *fd)
{
	FBXNode	n;

	fread(&n, 1, 13, fd);
	if (n.endOffset == 0 &&
		n.numProperties == 0 &&
		n.propertyListLen == 0 &&
		n.nameLen == 0) {
		return (nullptr);
	}
	std::cout <<
		"Node :\n" <<
		"	endOffset : " << n.endOffset << "\n" <<
		"	numProperties : " << n.numProperties << "\n" <<
		"	propertyListLen : " << n.propertyListLen << "\n" <<
		"	nameLen : " << int(n.nameLen) << std::endl;
	if (n.nameLen == 0) //this is top node, ignore it
	{
		fseek(fd, n.endOffset, SEEK_SET);
		return (parseNode(fd));
	}
	auto	*node = new FBXNode(n);
	node->name = new unsigned char[int(node->nameLen + 1)];
	memset(node->name, 0, node->nameLen + 1);
	fread(node->name, 1, node->nameLen, fd);
	std::cout <<
		"	name : " << node->name << "\n{" << std::endl;
	for (unsigned i = 0; i < node->numProperties; i++) {
		node->properties.push_back(parseProperty(fd));
	}
	while (ftell(fd) != long(n.endOffset)) {
		FBXNode *subNode = parseNode(fd);
		if (subNode == nullptr)
			break ;
		node->nodes.push_back(subNode);
	}
	std::cout << "} " << node->name << std::endl;
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