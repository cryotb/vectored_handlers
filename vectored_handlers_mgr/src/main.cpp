#include "inc/include.h"

int main()
{
	if (auto vehs = new vectored_handlers(VH_MODE_EXCEPTION))
	{
		tests::enum_vehs(vehs);
		tests::hijack_veh(vehs);

		delete vehs;
	}

	if (auto vchs = new vectored_handlers(VH_MODE_CONTINUE))
	{
		tests::enum_vchs(vchs);

		delete vchs;
	}

	return getchar();
}
