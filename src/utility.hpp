#pragma once

#include <ios>
#include <array>

namespace sp
{
	class ctype : public std::ctype<char>
	{
		using base = std::ctype<char>;
		std::array<std::ctype_base::mask, base::table_size> mTable;

	public:
		ctype(size_t refs = 0);
	};

}