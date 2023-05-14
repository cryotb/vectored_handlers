#include "inc/include.h"

namespace
{
	long __stdcall generic_vh(PEXCEPTION_POINTERS ep)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	int hijacked_invokes = 0;
	long __stdcall hijacked_vh(PEXCEPTION_POINTERS ep)
	{
		if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
			ep->ExceptionRecord->ExceptionInformation[0] == 1 /* WRITE */)
		{
			hijacked_invokes++;
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}
}

namespace tests
{
	/*
	*	Checks if VH::list() works properly.
	*	 It does that by temporarily registering a VEH,
	*    and afterwards checking if it's in there.
	*/
	void enum_vehs(vectored_handlers* vhs)
	{
		bool passed = false;
		printf("------ running ENUM_VEHS ------\n");

		auto handle = AddVectoredExceptionHandler(TRUE, generic_vh);
		if (handle)
		{
			do
			{
				srw_lock_guard _(vhs->plock());

				printf("registered an VEH with handle %p\n", handle);

				for (const auto& rec : vhs->list())
				{
					if (rec.handle == handle && rec.handler == generic_vh)
					{
						printf("  found our own matching handler.\n");
						printf("    handle:%p | handler:%p\n", rec.handle, rec.handler);
						passed = true;
					}
				}
			} while (false);

			RemoveVectoredExceptionHandler(handle);
		}

		printf("------ ( SUCCEEDED:%i ) ------\n\n", passed);
	}

	/*
	*	Does the same check as above, but for VCHs.
	*/
	void enum_vchs(vectored_handlers* vhs)
	{
		bool passed = false;
		printf("------ running ENUM_VCHS ------\n");

		auto handle = AddVectoredContinueHandler(TRUE, generic_vh);
		if (handle)
		{
			do
			{
				srw_lock_guard _(vhs->plock());

				printf("registered an VCH with handle %p\n", handle);

				for (const auto& rec : vhs->list())
				{
					if (rec.handle == handle && rec.handler == generic_vh)
					{
						printf("  found our own matching handler.\n");
						printf("    handle:%p | handler:%p\n", rec.handle, rec.handler);
						passed = true;
					}
				}
			} while (false);

			RemoveVectoredContinueHandler(handle);
		}

		printf("------ ( SUCCEEDED:%i ) ------\n\n", passed);
	}

	/*
	*	Attempts to hijack an existing VEH.
	*	 If this was done correctly, `hijacked_vh` will be invoked eventually.
	*/
	void hijack_veh(vectored_handlers* vhs)
	{
		bool passed = false;
		printf("------ running HIJACK_VEH ------\n");

		auto handle = AddVectoredExceptionHandler(TRUE, generic_vh);
		if (handle)
		{
			printf("registered an VEH with handle %p\n", handle);

			if (vhs->hijack(handle, hijacked_vh))
			{
				printf("  the VEH with handle %p was hijacked (1/2).\n", handle);

				// test our hijacked VEH by forcing an exception:
				for (unsigned char i = 0; i < 12; i++)
				{
					__try
					{
						uintptr_t fake_addr = 0xBEEF21012;
						*(int*)(fake_addr) = 5;
					}
					__except (EXCEPTION_EXECUTE_HANDLER)
					{
						printf("  catched ex (2/2).\n");
					}
				}

				printf("  our hijacked VEH was called %i times.\n", hijacked_invokes);
				passed = (hijacked_invokes == 12);
			}

			RemoveVectoredExceptionHandler(handle);
		}

		printf("------ ( SUCCEEDED:%i ) ------\n\n", passed);
	}
}
