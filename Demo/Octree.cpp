#include "Octree.h"

#include <fstream>

Octree::Octree()
{
	m_depth = 0;
	m_root = new OctNode();
}

void Octree::load(const std::string& fname)
{
	std::vector<char>	data;
	std::vector<int>	idata;
	size_t				size;
	std::ifstream		file(fname.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		throw std::string("Could not open octree file: ") + fname;
	}

	file.seekg(0, std::ios::end);
	size = (size_t)file.tellg();
	file.seekg(0, std::ios::beg);

	data.resize(size);
	file.read(&data[0], size);
	file.close();

	idata.resize(size/4);
	for (size_t i = 0; i < size/4; ++i) {
		int	v = (data[i*4] << 24) | (data[i*4+1] << 16) | (data[i*4+2] << 8) | data[i*4+3];
		idata[i] = v;
	}

	load(idata);
}

void Octree::load(const std::vector<int>& data)
{
	m_depth = data[0];
	m_root->load(data, 1);
}

uint32_t* Octree::buffer()
{
	size_t		size;
	uint32_t*	buf;

	size = bufferSize();
	buf = new uint32_t[bufferSize()];
	m_root->dumpData(0, buf);
	return buf;
}

Octree::OctNode::OctNode() : type(-1), children(NULL) { }

Octree::OctNode::~OctNode()
{
	if (children != NULL) {
		for (int i = 0; i < 8; ++i)
			delete children[i];
		delete children;
		children = NULL;
	}
}

size_t Octree::OctNode::bufferSize()
{
	if (type != -1) {
		return 1;
	} else {
		int total = 9;
		for (int i = 0; i < 8; ++i)
			total += children[i]->bufferSize();
		return total;
	}
}

size_t Octree::OctNode::dumpData(size_t index, uint32_t* data)
{
	data[index++] = type;
	if (type == -1) {
		int childIndex = index;
		index += 8;
		for (int i = 0; i < 8; ++i) {
			data[childIndex+i] = index;
			index = children[i]->dumpData(index, data);
		}
	}
	return index;
}

size_t Octree::OctNode::load(const std::vector<int>& data, size_t offset)
{
	type = data[offset++];
	if (type == -1) {
		children = new OctNode*[8];
		for (int i = 0; i < 8; ++i) {
			children[i] = new OctNode();
			offset = children[i]->load(data, offset);
		}
	}
	return offset;
}
