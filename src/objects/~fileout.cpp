
#include <fstream>
#include <vector>

#include "../StringFormat.h"
#include "~fileout.h"

namespace Yggdrasil {

void FileoutObject::run(buf &in, buf &, int index)
{
	data.push_back(in[0][index]);
}

void FileoutObject::finish()
{
	std::fstream file(filename, std::fstream::out | std::fstream::binary);

	for (uint i = 0; i < data.size(); i++)
	{
		file.write((const char*)& data[i], sizeof(float));
	}

	file.close();
}

FileoutObject::FileoutObject(std::string filename) :
	filename(split_by(filename, ' ')[3])
{ set_io(1, 0); }

}