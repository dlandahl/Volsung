
#include <fstream>
#include <vector>

#include "../StringFormat.h"
#include "~fileout.h"

void FileoutObject::run(buf &in, buf &out, int index)
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
	AudioObject(1, 0), filename(sf::split_by(filename, ' ')[3])
{ }
