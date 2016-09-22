#pragma once

#include <stdint.h>
#include <vector>

#pragma pack(push, 1)
struct VertHeader
{
	int32_t unknown[3];
	uint32_t vertex_data_stride;
	uint32_t n_vertices;
	uint32_t n_indices;
	int32_t unknown3;  // maybe data type - 2 for triangles, 1 for tri strips?
	uint32_t vertex_data_start_offset;
	uint32_t index_data_start_offset;
	int32_t unknown4;
};
#pragma pack(pop)

inline std::ostream& operator<<(std::ostream& stream, const VertHeader& header) {
	stream << "unknown0:      " << header.unknown[0] << "\n"
           << "unknown1:      " << header.unknown[1] << "\n"
		   << "unknown2:      " << header.unknown[2] << "\n"
		   << "vert_stride:   " << header.vertex_data_stride << "\n"
		   << "n_vertices:    " << header.n_vertices << "\n"
		   << "n_indices:     " << header.n_indices << "\n"
		   << "unknown3:      " << header.unknown3 << "\n"
		   << "verts_offset:  " << header.vertex_data_start_offset << "\n"
		   << "index_offset:  " << header.index_data_start_offset << "\n"
		   << "unknown4:      " << header.unknown4 << "";
	return stream;
}

struct ModelData
{
	enum IndexType : int32_t {
		INDEX_TRIANGLE_STRIP = 1,
		INDEX_TRIANGLES = 2
	};

	uint32_t n_vertices;
	uint32_t vert_stride;
	IndexType index_type;
	std::vector<float> vert_data;
	std::vector<int16_t> vert_indices;
};