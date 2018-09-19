
#include <memory>
#include <istream>

#include "StringFormat.h"
#include "Yggdrasil.h"

void create_object(str, st_type&, str);

void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                 std::unique_ptr<AudioObject>&, uint);

void make_patch(st_type&, std::istream&);
