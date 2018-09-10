
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>

#include "Yggdrasil.h"
#include "Patch.h"
#include "StringFormat.h"

#include "./objects/~add.h"
#include "./objects/~mult.h"
#include "./objects/~noise.h"
#include "./objects/~filter.h"
#include "./objects/~oscillator.h"
#include "./objects/~square.h"
#include "./objects/~fileout.h"
#include "./objects/~crush.h"

template<class obj>
void create_object(str name, st_type &symbols, str args = "")
{
	static_assert(std::is_base_of<AudioObject, obj>::value, "no");
	symbols[name] = std::make_unique<obj>(args);
}

void connect_objects(std::unique_ptr<AudioObject> &a, uint out,
	                 std::unique_ptr<AudioObject> &b, uint in)
{
	a->outputs[out].connect(b->inputs[in]);
}

void make_patch(st_type &st, std::istream &in_stream)
{
	std::string cmd;

	while (cmd != "done")
	{
		std::getline(in_stream, cmd);

		if (sf::starts_with(cmd, "mk "))
		{
			auto mk_cmd = sf::split_by(cmd, ' ');

			if (mk_cmd[1] == "osc")
			  { create_object<OscillatorObject>(mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "out")
				{ create_object<FileoutObject> (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "add")
				{ create_object<AddObject>     (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "mult")
				{ create_object<MultObject>    (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "noise")
				{ create_object<NoiseObject>   (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "filter")
				{ create_object<FilterObject>  (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "square")
				{ create_object<SquareObject>  (mk_cmd[2], st, cmd); }

			if (mk_cmd[1] == "crush")
				{ create_object<CrushObject>   (mk_cmd[2], st, cmd); }
		}

		if (sf::starts_with(cmd, "ct "))
		{
			cmd        = sf::split_by(cmd, ' ')[1];
			auto c_cmd = sf::split_by(cmd, '>');

			int in, out;

			out = c_cmd[0][c_cmd[0].size() - 1] - '0'; c_cmd[0].pop_back();
			in  = c_cmd[1][c_cmd[1].size() - 1] - '0'; c_cmd[1].pop_back();
			connect_objects(st[c_cmd[0]], out, st[c_cmd[1]], in);
		}
	}
}
