#pragma once

#include <stdint.h>
#include <fstream>
#include <string>

class BinaryReader
{
public:
	virtual ~BinaryReader();

	void open(const std::string& path);
	void close();

	void read(void* buff, std::streamsize n_bytes);
	int32_t read_int32_LE();
	uint32_t read_uint32_LE();
	uint64_t read_uint64_LE();
	float read_float_LE();
	char read_byte();
	std::string read_chars(int n);
	std::string read_string();  // reads a null-terminated string

	void seek(std::streamoff pos, std::ios::seekdir way = std::ios::beg);

	inline std::streamoff cur_pos() { return file_.tellg(); }
	inline bool good() { return file_.good(); }
	inline bool bad() { return file_.bad(); }
	inline bool eof() { return file_.eof(); }
	inline std::streamsize length() const { return length_; }
private:
	std::ifstream file_;
	std::streamsize length_;
};