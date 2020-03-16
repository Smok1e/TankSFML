#pragma once

namespace eu

{

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

	int _forceInit = Init ();

}