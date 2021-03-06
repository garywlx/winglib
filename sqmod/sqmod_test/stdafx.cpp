// stdafx.cpp

#include "stdafx.h"
#include "test_class.h"

// Export classes
static void SQBIND_Export_test( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	// Register class instances
	CTestClass::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "test_class.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_test( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

