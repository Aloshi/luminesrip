#include <iostream>
#include <vector>
#include <assert.h>
#include <sstream>
#include <Windows.h>

#include "BinaryReader.h"
#include "ModelData.h"
#include "ModelWriter.h"

#define CHECK(expr) { bool v = expr; assert(v); }


template <typename T>
std::ostream& operator<<(std::ostream& lhs, const std::vector<T>& rhs)
{
	std::cout << rhs.size() << " items:" << std::endl;

	for (unsigned int i = 0; i < rhs.size(); i++)
		lhs << "  " << i << ": " << rhs.at(i) << std::endl;

	return lhs;
}

std::vector<std::string> seek_and_read_string_list(BinaryReader& r)
{
	r.seek(0x4);
	const uint32_t string_list_pos = r.read_uint32_LE();
	r.seek(string_list_pos);

	std::vector<std::string> strings;
	while (r.good() && r.cur_pos() < r.length())
	{
		std::cout << "  " << std::hex << (r.cur_pos() - string_list_pos) << std::dec << ": ";
		strings.push_back(r.read_string());
		std::cout << strings.back() << "\n";
	}
	return strings;
}

std::vector<uint32_t> seek_and_read_section_list(BinaryReader& r)
{
	r.seek(0x24);
	uint32_t n_sections = r.read_uint32_LE();
	uint32_t sections_start = r.read_uint32_LE();
	r.seek(sections_start);

	std::vector<uint32_t> positions;
	for (unsigned int i = 0; i < n_sections; i++)
	{
		positions.push_back(r.read_uint32_LE());
	}

	return positions;
}

// assumes reader is at the start of the header
ModelData read_verts(BinaryReader& r)
{
	uint64_t header_start = r.cur_pos();
	
	VertHeader header;
	r.read(&header, sizeof(header));
	//CHECK(header.unknown3 == 1 || header.unknown3 == 2);
	CHECK(header.vertex_data_start_offset == 0x28);
	CHECK((header.unknown[1] == 3 || header.unknown[1] == 4));
	std::cout << "data at 0x" << std::hex << header_start << std::dec << "\n";
	std::cout << header << "\n";

	ModelData data;
	data.n_vertices = header.n_vertices;
	data.vert_stride = header.vertex_data_stride;
	data.index_type = (ModelData::IndexType) header.unknown[1];

	// rip the verts
	data.vert_data.resize(header.n_vertices * header.vertex_data_stride / sizeof(float));
	r.seek(header_start + header.vertex_data_start_offset);
	r.read(data.vert_data.data(), header.n_vertices * header.vertex_data_stride);

	// rip indices
	data.vert_indices.resize(header.n_indices);
	r.seek(header_start + header.index_data_start_offset);
	r.read(data.vert_indices.data(), header.n_indices * sizeof(int16_t));

	// read that unknown extra data
	r.seek(header_start + header.unknown4);
	std::cout << std::hex;
	std::cout << "End of Data 0x00: " << r.read_uint32_LE() << "\n";
	std::cout << "            0x04: " << r.read_uint32_LE() << "\n";
	std::cout << std::dec;

	return data;
}

void process_file(const std::string& path)
{
	BinaryReader reader;
	reader.open(path);
	CHECK(reader.read_chars(4) == "MDLT");

	seek_and_read_string_list(reader);
	const std::vector<uint32_t> sections = seek_and_read_section_list(reader);
	std::cout << std::hex << sections << std::dec << "\n";

	for (unsigned int i = 0; i < sections.size(); i++)
	{
		reader.seek(sections[i]);
		ModelData model = read_verts(reader);

		std::stringstream ss;
		ss << path << "-" << (i) << ".obj";

		std::cout << "Exporting model " << i << " as " << ss.str() << "...\n";
		ModelWriter::write_obj(ss.str(), model);
	}
}

int main()
{
	process_file("blk_red.model");

	/*BinaryReader reader;
	reader.open("blk_red.model");

	CHECK(reader.read_chars(4) == "MDLT");

	const uint32_t string_list_pos = reader.read_uint32_LE();*/

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

	// 0xA0 starts referencing the textures (*.gxt) in a list:
	// texture path string list offsets: 0x14, 0x1F, 0x28, 0x34, 0x40

	/* 
	        Strings
	0:   www.rocketstd.co.jp
	14:  window.gxt
	1f:  noiz.gxt
	28:  cube_02.gxt
	34:  ref_913.gxt
	40:  spblk_dummy.gxt
	50:  frame
	56:  CgFX/blk_red{frame}.cgfx
	6f:  default
	77:  blk_red{frame}_cgfx_VS.gxp
	92:  blk_red{frame}_cgfx_PS.gxp
	ad:  g_Opacity
	b7: 
	b8:  g_diffuseColorSelect
	cd:  k_d
	d1:  g_emissionColor
	e1:  g_diffuseAttenuation
	f6:  g_ambientLightColor
	10a: g_colorRimSpecular
	11d: g_rimSpecularPower
	130: g_rimSpecularSelfLuminous
	14a: g_glossColorSelect
	15d: g_MorphMaskTexturemapChannel
	17a: LightD0_En
	185: LightD1_En
	190: front_ATTR_ADD_
	1a0: CgFX/blk_red{front_ATTR_ADD_}.cgfx
	1c3: blk_red{front_ATTR_ADD_}_cgfx_VS.gxp
	1e8: blk_red{front_ATTR_ADD_}_cgfx_PS.gxp
	20d: g_BottomUVScroll
	21e: g_MidUVScroll
	22c: g_ReflectColor
	23b: gray
	240: CgFX/blk_red{gray}.cgfx
	258: blk_red{gray}_cgfx_VS.gxp
	272: blk_red{gray}_cgfx_PS.gxp
	28c: inner
	292: CgFX/blk_red{inner}.cgfx
	2ab: blk_red{inner}_cgfx_VS.gxp
	2c6: blk_red{inner}_cgfx_PS.gxp
	2e1: inner_block
	2ed: CgFX/blk_red{inner_block}.cgfx
	30c: blk_red{inner_block}_cgfx_VS.gxp
	32d: blk_red{inner_block}_cgfx_PS.gxp
	34e: k_s
	352: g_SpecularPower
	362: blk_SP_ATTR_BLE_
	373: CgFX/blk_red{blk_SP_ATTR_BLE_}.cgfx
	397: blk_red{blk_SP_ATTR_BLE_}_cgfx_VS.gxp
	3bd: blk_red{blk_SP_ATTR_BLE_}_cgfx_PS.gxp
	3e3: _4_sp02
	3eb: _2_innerbox01
	3f9: _1_inner02
	404: _3_box02
	40d: rkt_root
	416: none
	41b: block001_red
	428: block001_red-node
	43a: _1_inner02-node
	44a: _2_innerbox01-node
	45d: _3_box02-node
	46b: _4_sp02-node
	*/

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
	/*reader.seek(0x3558);  // 03000000 00007800 = (0x78 = 128)
	ModelData data = read_verts(reader);
	ModelWriter::write_obj("blk_red4.obj", data);*/

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
	while (true) {
		Sleep(500);
	}
	return 0;
}

// MODEL LIST:
// **0x28 == first header
// (**0x28) + 4 == second header

// blk_del.model
// header starts: 0x494
// 0x00000014  = 20
// 0x00000003  = 3 - seems to switch tri/tri strip...
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