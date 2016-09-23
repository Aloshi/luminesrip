#include "ModelWriter.h"

#include <assert.h>
#include <fstream>

#include <iostream>

namespace ModelWriter
{

void write_obj(const std::string& file, const ModelData& data)
{
	std::ofstream f(file, std::ios::trunc);
	f << "# OBJ file\n";

	for (unsigned int i = 0; i < data.n_vertices; i++)
	{
		const float* vert = data.vert_data.data() + (data.vert_stride / sizeof(float) * i);
		f << "v " << vert[0] << " " << vert[1] << " " << vert[2] << "\n";
	}

	/*for (unsigned int i = 0; i < data.vert_indices.size(); i++)
	{
		std::cout << data.vert_indices[i] << "\n";;
	}*/

	if (data.index_type == ModelData::INDEX_TRIANGLES)
	{
		//assert(data.vert_indices.size() % 3 == 0);
		unsigned int end = data.vert_indices.size();
		std::cout << "  (ignoring extra " << (end % 3) << " indices)\n";
		end -= (end % 3);
		std::vector<bool> seen(data.vert_indices.size(), false);
		for (unsigned int i = 0; i < end; i++)
		{
			seen.assign(data.vert_indices[i], true);
			if (i % 3 == 0)
				f << "f";
			f << " " << (data.vert_indices[i] + 1);
			if (i % 3 == 2)
				f << "\n";
		}

		unsigned int n_unused_vertices = 0;
		for (unsigned int i = 0; i < seen.size(); i++)
		{
			if (seen.at(i) == false)
				n_unused_vertices++;
		}
		assert(n_unused_vertices == 0);

	} else if (data.index_type == ModelData::INDEX_TRIANGLE_STRIP)
	{
		const auto& indices = data.vert_indices;
		/*for (unsigned int v = 0; v < indices.size() - 2; v++)
		{
			if (v & 1) {
				f << "f " << (data.vert_indices[v] + 1) << " " << (indices[v + 1] + 1) << " " << (indices[v + 2] + 1) << "\n";
			} else {
				f << "f " << (data.vert_indices[v] + 1) << " " << (indices[v + 2] + 1) << " " << (indices[v + 1] + 1) << "\n";
			}
		}*/

		bool flipWindingOrder = false;

		uint32_t numIndices = indices.size() - 2;
		uint16_t a = indices[0];
		uint16_t b = indices[1];

		unsigned int idx = 2;
		while (numIndices > 0)
		{
			uint32_t c = indices[idx++];

			if ((a != b) && (b != c) && (c != a))
			{
				if (!flipWindingOrder) {
					f << "f " << (a + 1) << " " << (b + 1) << " " << (c + 1) << "\n";
				} else {
					f << "f " << (a + 1) << " " << (c + 1) << " " << (b + 1) << "\n";
				}
			} else
			{
				flipWindingOrder = !flipWindingOrder;
			}

			a = b;
			b = c;
			--numIndices;
		}
	} else {
		throw std::exception("Unknown ModelData::index_type");
	}

	f.close();
}

}