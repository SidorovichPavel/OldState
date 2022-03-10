#include "utility.hpp"

namespace sp
{
	ctype::ctype(size_t refs)
		:
		base(mTable.data(), false, refs)
	{
		std::copy_n(classic_table(), base::table_size, mTable.data());
		mTable[' '] = base::alnum;
	}


}