#pragma once

#include <string>

#include "ModelData.h"

namespace ModelWriter
{

void write_obj(const std::string& file, const ModelData& data);

}