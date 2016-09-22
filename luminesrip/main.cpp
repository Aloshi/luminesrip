#include <iostream>
#include <vector>
#include <assert.h>

#include "BinaryReader.h"
#include "ModelData.h"
#include "ModelWriter.h"

#define CHECK(expr) { bool v = expr; assert(v); }

std::vector<std::string> read_string_list(BinaryReader& r)
{
	std::vector<std::string> strings;
	std::cout << r.length() << std::endl;
	while (r.good() && r.cur_pos() < r.length())
	{
		strings.push_back(r.read_string());
		std::cout << strings.back() << std::endl;
	}
	return strings;
}

// assumes reader is at the start of the header
ModelData read_verts(BinaryReader& r)
{
	uint64_t header_start = r.cur_pos();
	
	VertHeader header;
	r.read(&header, sizeof(header));
	//CHECK(header.unknown3 == 1 || header.unknown3 == 2);
	CHECK(header.vertex_data_start_offset == 0x28);
	std::cout << "data at 0x" << std::hex << header_start << std::dec << "\n";
	std::cout << header << "\n";

	// rip the verts
	ModelData data;
	data.n_vertices = header.n_vertices;
	data.vert_stride = header.vertex_data_stride;
	//data.index_type = (ModelData::IndexType) header.unknown3;
	//data.index_type = ModelData::INDEX_TRIANGLES;
	data.index_type = ModelData::INDEX_TRIANGLE_STRIP;

	data.vert_data.resize(header.n_vertices * header.vertex_data_stride / sizeof(float));
	r.seek(header_start + header.vertex_data_start_offset);
	r.read(data.vert_data.data(), header.n_vertices * header.vertex_data_stride);

	data.vert_indices.resize(header.n_indices);
	r.seek(header_start + header.index_data_start_offset);
	r.read(data.vert_indices.data(), header.n_indices * sizeof(int16_t));

	return data;
}

template <typename T>
std::ostream& operator<<(std::ostream& lhs, const std::vector<T>& rhs)
{
	std::cout << rhs.size() << " items:" << std::endl;

	for (unsigned int i = 0; i < rhs.size(); i++)
		lhs << "  " << i << ": " << rhs.at(i) << std::endl;

	return lhs;
}

int main()
{
	BinaryReader reader;
	reader.open("blk_red.model");

	CHECK(reader.read_chars(4) == "MDLT");

	const uint32_t string_list_pos = reader.read_uint32_LE();

	// print string list
	/*reader.seek(string_list_pos);
	std::vector<std::string> strings = read_string_list(reader);
	std::cout << strings << std::endl;*/

	// blk_del.model
	/*reader.seek(0x494);  // 03000000 00005100
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_del.obj", data);*/

	// box_red.model (coords/indices identical to blk_del, colors might be diff)
	/*reader.seek(0x430);  // 03000000 00005100
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("box_red.obj", data);*/
	
	// blk_red.model
	// some sort of list of headers at 0x78
	// 0x78 is referenced at 0x28
	// B80B0000 EC130000 242A0000 58350000

	// blk_red.model pt1
	/*reader.seek(0xBB8);  //  0x8 of extra data - 04000000 00008200 (0x82 = 130)
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_red.obj", data);*/

	// blk_red.model pt2
	/*reader.seek(0x13EC);  // 0x8 of extra data - 03000000 00008400 (0x84 = 132)
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_red2.obj", data);*/

	// blk_red.model pt3 (looks weird...)
	/*reader.seek(0x2A24);  // 0x8 of extra data - 03000000 00005A00 (0x5A = 90)
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_red3.obj", data);*/

	// blk_red.model pt4
	reader.seek(0x3558);  // 03000000 00007800 = (0x78 = 128)
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_red4.obj", data);

	/*
	00000404 = 1028
	00000003 = 3
	0000031B = 795
	00000040 - stride
	000000CA - n vertices
	0000017E - n indices
	00000003 - ???
	00000028 - offset to vert data
	000032A8 - offset to index data
	000035A4 - offset to ??? data?
	*/

	std::cout << "DONE\n";
	while (true);
	return 0;
}

// MODEL LIST:
// **0x28 == first header
// (**0x28) + 4 == second header

// blk_del.model
// header starts: 0x494
// 0x00000014  = 20
// 0x00000003  = 3
// 0x0000011B  = 283
// 0x00000038  - vertex data stride
// 0x0000008C  - number of vertices
// 0x000000B4  - number of indices
// 0x00000002  - index stride? (short)
// 0x00000028  - offset from header start to vertex data start
// 0x00001EC8  - offset from header start to index data start
// 0x00002030  - end of index data...

// vertices start: 0x4BC
//   - stride: 0x38 = 56
//   - floats per vertex: 0xE = 14
//   - vertices: 0x8C = 140
//   - length: 0x1EA0 = 7840
// indices start: 0x235C
//   - stored as shorts (2 bytes)
//   - number of indices: 0xB4C = 180
//   - length: 0x168 = 360

// box_red.model
// header starts: 0x430
// 0x00000021  = 33   !!! only difference !!!
// 0x00000003  = 3
// 0x0000011B  = 283
// 0x00000038 - vertex data stride
// 0x0000008C - number of vertices
// 0x000000B4 - number of indices
// 0x00000002 - index stride? (short)
// 0x00000028  = 40
// 0x00001EC8  = 7880
// 0x00002030  = 8240
// 
// vertices start: 0x458
//   - header to vertices: 0x28 = 40
//   - length: 0x1EA0 = 7840
// indices start: 0x22F8