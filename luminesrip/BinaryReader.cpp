#include "BinaryReader.h"

#include <sstream>
#include <assert.h>

BinaryReader::~BinaryReader()
{
	close();
}

void BinaryReader::open(const std::string& path)
{
	file_.exceptions(file_.exceptions() | std::ios::failbit);
	close();
	file_.open(path, std::ios::binary | std::ios::in);

	// find length
	file_.seekg(0, std::ios_base::end);
	length_ = file_.tellg();
	file_.seekg(0, std::ios_base::beg);
	file_.clear();
}

void BinaryReader::close()
{
	if (file_.is_open())
		file_.close();
}

void BinaryReader::read(void* buff, std::streamsize n_bytes)
{
	file_.read((char*)buff, n_bytes);
}

int32_t BinaryReader::read_int32_LE()
{
	int32_t data;
	file_.read((char*) &data, 4);
	return data;
}

uint32_t BinaryReader::read_uint32_LE()
{
	uint32_t data;
	file_.read((char*)&data, 4);
	return data;
}

uint64_t BinaryReader::read_uint64_LE()
{
	uint64_t data;
	file_.read((char*) &data, 8);
	return data;
}

char BinaryReader::read_byte()
{
	char data = file_.get();
	return data;
}

std::string BinaryReader::read_string()
{
	std::stringstream ss;

	while (true) {
		char data = file_.get();
		if (data == '\0')
			break;
		ss << data;
	}

	return ss.str();
}

std::string BinaryReader::read_chars(int n)
{
	char* buff = new char[n+1];
	file_.read(buff, n);
	buff[n] = '\0';

	std::string str = buff;
	delete buff;
	return str;
}

float BinaryReader::read_float_LE()
{
	float val;
	file_.read((char*) &val, 4);
	return val;
}

void BinaryReader::seek(std::streamoff pos, std::ios::seekdir way)
{
	file_.seekg(pos, way);
}
