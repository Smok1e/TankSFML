#pragma once
#include <io.h>

namespace eu

{

	//----------------------------------------------------------------

	int Init ();

	double rnd (double from, double to);

	bool fileExists (const char * filename);

	//----------------------------------------------------------------

	int Init ()

	{

		#if defined _EU_ALLOC_TRACE

			_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);

		#endif

		#if defined _EU_ALLOC_BREAK

			_CrtSetBreakAlloc (_EU_ALLOC_BREAK);

		#endif

		return 0;

	}

	//----------------------------------------------------------------

	double rnd (double from, double to)

	{

		return from + 1.0 * rand () / RAND_MAX * (to - from);

	}

	//----------------------------------------------------------------

    bool fileExists (const char * filename)

    {

        return _access (filename, 0) != -1;

    }

	//Initializing----------------------------------------------------

	int _forceInit = Init ();

}
