/*------------------------------------------------------------------
// sys.h
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/
#pragma once

#define asmOP1( p, i, o1 ) 				p[ i++ ] = (o1)
#define asmOP2( p, i, o1, o2 ) 			p[ i++ ] = (o1), p[ i++ ] = (o2)
#define asmOP3( p, i, o1, o2, o3 )		p[ i++ ] = (o1), p[ i++ ] = (o2), p[ i++ ] = (o3)
#define asmOP4( p, i, o1, o2, o3, o4 )	p[ i++ ] = (o1), p[ i++ ] = (o2), p[ i++ ] = (o3), p[ i++ ] = (o4)
#define asmOSHORT( p, i, n ) 			*((unsigned short*)&p[ i ]) = (n), i += 2
#define asmOINT( p, i, n ) 				*((unsigned int*)&p[ i ]) = (n), i += 4

class CSys
{
public:

	/// Exception class
	class CException
	{
	public:

		// Default constructor
		CException() { m_error = 0; }

		// Initializing constructor
		CException( oexINT e ) { m_error = 0; }

		// Error code
		oexINT getError() { return m_error; }

		// Error code
		oexINT		m_error;
	};

#if !defined( OEX_CPU_64 )

#	define OEX_CDECL_RETADDR void *_oex_ret_addr,
#	define OEX_CDECL_CALL 0,

	class CThunk
	{
		/// Bruteforce cast
		template< typename T > struct SCast { union { T o; void* p; }; };

	public:

		/// Constructor
		CThunk() : m_p( 0 ) {}

		/// Destructor
		virtual ~CThunk() { Release(); }

		/// Bind to __stdcall
		template< typename TO, typename TI > TO Stdcall2This( const void *pThis, TI pFun )
		{	SCast< TI > in; SCast< TO > out; in.o = pFun; out.p = Stdcall2This( pThis, in.p ); return out.o; }
		void* Stdcall2This( const void *pThis, void *pFun );

		/// Bind to __cdecl
		template< typename TO, typename TI > TO CDecl2This( const void *pThis, TI pFun, long lCleanup )
		{	SCast< TI > in; SCast< TO > out; in.o = pFun; out.p = CDecl2This( pThis, in.p, lCleanup ); return out.o; }
		void* CDecl2This( const void *pThis, void *pFun, long lCleanup );

		/// Release function memory
		void Release();

		/// Return function pointer
		void* Ptr() { return m_p; }

	private:
		void *m_p;
	};

#endif

	// Last error code
	volatile static oexINT s_last_error;

	/// Throws an exception of type CSys::CException
	oexNORETURN static void ThrowException();

	/// Injects an exception into the specified thread
	static oexUINT InjectException( oexPVOID hThread, oexINT nError );

	/// Must be called to catch exception
	static void InitException();

private:

    /// Constructor
	CSys() {}

    /// Destructor
	virtual ~CSys() {}

public:

	//==============================================================
	// GetInstanceHandle()
	//==============================================================
	/// Returns the module instance handle
	/**
		In Windows, this is the application instance handle.
		In Linux, this is the process ID

		\return Instance handle

		\see
	*/
	static oexCPVOID GetInstanceHandle();

    /// Initializes os system stuff
    static oexBOOL Init();

    /// Cleans up os system stuff
    static oexBOOL Uninit();

    /// Call to terminate the application
	static void Exit( oexINT nRet );

	/// Displays a system message to the user
	static oexINT ShowMessageBox( oexCSTR x_pTitle, oexCSTR x_pStr );

	/// Indicates that the application should exit
	static oexINT Quit( oexINT x_nReturnCode );

	/// Flushes the standard output buffer
	static oexINT Flush_stdout();

	/// Returns last key press value, blocks until a key press occurs
	static int GetKey();

	/// Returns non-zero if a key has been pressed
	static int IsKey();

	// String conversion
public:

	static oexCSTR SetLocale( oexINT nCategory, oexCSTR pLocal );

    // Multi-byte
public:

	static oexCSTR8 StrFmt( oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, ... );

	static oexCSTR8 vStrFmt( oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, oexVaList pArgs );

	static oexCSTR8 StrFmt( oexRESULT *pRes, oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, ... );

	static oexCSTR8 vStrFmt( oexRESULT *pRes, oexSTR8 pDst, oexUINT uMax, oexCSTR8 pFmt, oexVaList pArgs );

    /// Converts to int64
    static oexINT64 StrToInt64( oexCSTR8 x_pStr, oexUINT x_uRadix = 10 );

    /// Converts to uint64
    static oexUINT64 StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix = 10 );

	/// Converts to long
	static oexLONG StrToLong( oexCSTR8 pStr, oexUINT uRadix = 10 );

	/// Converts to unsigned long
	static oexULONG StrToULong( oexCSTR8 pStr, oexUINT uRadix = 10 );

	/// Converts to double
	static oexFLOAT StrToFloat( oexCSTR8 pStr );

	/// Converts to double
	static oexDOUBLE StrToDouble( oexCSTR8 pStr );

	/// Format the string and pring to console
	static int Print( oexCSTR8 x_pFmt, ... );

	/// Format string and print to console
	static int vPrint( oexCSTR8 x_pFmt, oexVaList pArgs );	
	
	/// printf function
	static int Printf( oexCSTR8 x_pFmt, ... );

	/// vprintf
	static int vPrintf( oexCSTR8 x_pFmt, oexVaList pArgs );

	/// Writes data on stdout
	static int Echo( oexCSTR8 x_pFmt );

	/// Writes data to stdout with length
	static int Echo( oexCSTR8 x_pFmt, oexLONG x_lLen );

	/// Read data from stdin
	static oexLONG Read_stdin( oexSTR8 x_pBuf, oexLONG x_lMax );

    // Unicode
#if !defined( OEX_NOWCHAR )
public:

	static oexCSTRW StrFmt( oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, ... );

	static oexCSTRW vStrFmt( oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, oexVaList pArgs );

	static oexCSTRW StrFmt( oexRESULT *pRes, oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, ... );

	static oexCSTRW vStrFmt( oexRESULT *pRes, oexSTRW pDst, oexUINT uMax, oexCSTRW pFmt, oexVaList pArgs );

	/// Converts to int64
	static oexINT64 StrToInt64( oexCSTRW x_pStr, oexUINT x_uRadix = 10 );

	/// Converts to uint64
	static oexUINT64 StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix = 10 );

	/// Converts to long
	static oexLONG StrToLong( oexCSTRW pStr, oexUINT uRadix = 10 );

	/// Converts to unsigned long
	static oexULONG StrToULong( oexCSTRW pStr, oexUINT uRadix = 10 );

	/// Converts to double
	static oexFLOAT StrToFloat( oexCSTRW pStr );

	/// Converts to double
	static oexDOUBLE StrToDouble( oexCSTRW pStr );

	// +++ Hmmm, probably shouldn't overload Printf(), could cause nasty errors

	/// Format the string and pring to console
	static int Print( oexCSTRW x_pFmt, ... );

	/// Format string and print to console
	static int vPrint( oexCSTRW x_pFmt, oexVaList pArgs );	

	/// printf function
	static int Printf( oexCSTRW x_pFmt, ... );

	/// vprintf
	static int vPrintf( oexCSTRW x_pFmt, oexVaList pArgs );

	/// Writes data on stdout
	static int Echo( oexCSTRW x_pFmt );

	/// Writes data to stdout with length
	static int Echo( oexCSTRW x_pFmt, oexLONG x_lLen );

	static oexUINT WcsToMbs( oexSTR8 pDst, oexUINT uMax, oexCSTRW pSrc, oexUINT uLen );

	static oexUINT MbsToWcs( oexSTRW pDst, oexUINT uMax, oexCSTR8 pSrc, oexUINT uLen );

#endif // OEX_NOWCHAR

public:

	static oexPVOID MemCpy( oexPVOID pDst, oexCPVOID pSrc, oexUINT uSize );

	static oexINT MemCmp( oexCPVOID pDst, oexCPVOID pSrc, oexUINT uSize );

	static oexPVOID MemSet( oexPVOID pDst, oexINT nCh, oexUINT uSize );

	static oexPVOID Zero( oexPVOID pDst, oexUINT uSize )
	{	return MemSet( pDst, 0, uSize ); }

	/// Creates a guid
	static oexGUID * CreateGuid( oexGUID *pGuid );

//------------------------------------------------------------------
// Sleep functions
//------------------------------------------------------------------
public:

	/// Sleep in milliseconds
	static void Sleep( oexUINT uMilliseconds, oexUINT uSeconds = 0 );

	/// Sleep in microseconds
	static oexBOOL MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds = 0 );

	/// Strings constants
	static const oexUINT		c_StrErr_OK;
	static const oexUINT		c_StrErr_INVALID_PARAMETER;
	static const oexUINT		c_StrErr_INSUFFICIENT_BUFFER;

//------------------------------------------------------------------
// Wait functions
//------------------------------------------------------------------
public:

	enum etWait
	{
		/// Value indicating wait succeeded
		waitSuccess = 0,

		/// Value indicating wait timed out
		waitTimeout = -1,

		/// Value indicating wait expired
		waitFailed = -2
	};

	enum
	{
        /// Maximum number of handles that can be passed to WaitForMultipleObjects()
        eMaximumWaitObjects = 64
	};

	/// Waitable handle type
	typedef oexCPVOID t_WAITABLE;

	//==============================================================
	// CloseHandle()
	//==============================================================
	/// Free a event object created by osCreateEvent()
	/**
		\param [in] x_hHandle	-	Handle to close

		\see
	*/
	static void CloseHandle( t_WAITABLE x_pHandle );

	//==============================================================
	// osDuplicateHandle()
	//==============================================================
	/// Creates a event object
	/**
		\param [in] x_hEvent		 -	Handle to duplicate

		\return Duplicate handle to object

		\see osCreateEvent()
	*/
	static t_WAITABLE DuplicateHandle( t_WAITABLE x_pHandle );

	//==============================================================
	// WaitForSingleObject()
	//==============================================================
	/// Waits for a waitable object to become signaled
	/**
		\param [in] x_pHandle	-	Handle to waitable object
		\param [in] x_uTimeout	-	Maximum amount of time, in
									milli-seconds, to wait.

		\return Zero if success, otherwise waitTimeout or waitFailed

		\see
	*/
	static oexINT WaitForSingleObject( t_WAITABLE x_pHandle, oexUINT x_uTimeout );

	//==============================================================
	// WaitForMultipleObjects()
	//==============================================================
	/// Waits for multiple waitable objects
	/**
		\param [in] x_uObjects	-	Number of waitable objects
		\param [in] x_pHandle	-	Array of waitable objects
		\param [in] x_bWaitAll	-	Non-zero if you wish to wait for
									all objects to become signaled.
									Otherwise this function returns
									as soon as any objects are in
									the signaled state.
		\param [in] x_uTimeout	-	Maximum amount of time, in
									milli-seconds, to wait.

		\return Zero based index of object that is signaled if success,
				otherwise waitTimeout or waitFailed

		\see
	*/
	static oexINT WaitForMultipleObjects( oexUINT x_uObjects, t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout );

	//==============================================================
	// InterlockedIncrement()
	//==============================================================
	/// Thread safe value increment.
	/**
		\param [in] x_puVal		-	Pointer to value to increment.

		\return Value *before* incrementing.

		\see
	*/
	static oexLONG increment( oexLONG *x_puVal );

	//==============================================================
	// InterlockedDecrement()
	//==============================================================
	/// Thread safe value increment.
	/**
		\param [in] x_puVal		-	Pointer to value to decrement

		\return Value *before* decrementing.

		\see
	*/
	static oexLONG decrement( oexLONG *x_puVal );

	//==============================================================
	// vInfinite()
	//==============================================================
	/// Value representing infinite timeout.
	/**
		Use with wait functions
	*/
	static oexCONST oexUINT vInfinite()
    {   return c_Infinite; }

	//==============================================================
	// vInfinite()
	//==============================================================
    /// Maximum number of objects that can be passed to WaitForMultipleObjects()
	/**
		Use with wait functions
	*/
	static oexCONST oexUINT vMaximumWaitObjects()
    {   return c_uMaximumWaitObjects; }

	//==============================================================
	// GetCurThreadId()
	//==============================================================
    /// Returns the Id of the current thread
	/**

	*/
	static oexUINT GetCurThreadId();

    /// Sets the thread priority level
	/**

	*/
	static oexBOOL SetThreadPriority( oexINT x_nPriority );

    /// Sets the thread affinity
	/**

	*/
	static oexUINT SetThreadAffinity( oexUINT x_nAffinity );

private:

	/// Infinite timeout value
	static oexCONST oexUINT		c_Infinite;

    /// Maximum number of objects that can be passed to WaitForMultipleObjects()
	static oexCONST oexUINT		c_uMaximumWaitObjects;

public:

    /// Contains information on the current time
    struct STime
    {
        oexUINT         uYear;
        oexUINT         uMonth;
        oexUINT         uDayOfWeek;
        oexUINT         uDay;
        oexUINT         uHour;
        oexUINT         uMinute;
        oexUINT         uSecond;
        oexUINT         uMillisecond;
        oexUINT			uMicrosecond;
        oexUINT			uNanosecond;
        oexINT          nTzBias;
    };

	/// Returns the current unix time stamp in GMT time
	static oexUINT GetUnixTime();

    /// Local time
    static oexBOOL GetLocalTime( STime &x_st );

    /// Local time
    static oexBOOL SetLocalTime( STime &x_st );

    /// System time
    static oexBOOL GetSystemTime( STime &x_st );

    /// System time
    static oexBOOL SetSystemTime( STime &x_st );

    /// Converts an STime structure to file time
    static oexINT64 SystemTimeToFileTime( STime &x_st );

    /// Converts a file time to an STime structure
    static void FileTimeToSystemTime( STime &x_st, oexINT64 x_ft );

    /// Returns the local timezone bias in seconds
    static oexINT GetLocalTzBias();

public:

public:

	/// Pumps thread messages / only applies to Windows
	static oexBOOL PumpThreadMessages();

	//==============================================================
	// Fork()
	//==============================================================
	/// Fork current process
	/**
		\param [in] x_pWorkingDirectory	- Working directory for the
									      newly forked process.
		\param [in] x_pLogFile			- token used in creating the
										  new log file.

		iii Linux only

		\return Less than zero if failure, zero if child, greater
				than zero on return from parent.
	*/
	static oexINT Fork( oexCSTR x_pWorkingDirectory = oexNULL, oexCSTR x_pLogFile = oexNULL );


	//==============================================================
	// Shell()
	//==============================================================
	/// Executes the instruction on the shell
	/**
		\param [in] x_pFile			- File to execute
		\param [in] x_pParams		- Parmeters to pass to file
		\param [in] x_pDirectory	- Directory to start in

		\return Non-zero if success
	*/
	static oexBOOL Shell( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory );

	//==============================================================
	// StartProcess()
	//==============================================================
	/// Creates the specified process
	/**
		\param [in] x_pFile			- File to execute
		\param [in] x_pParams		- Parmeters to pass to file
		\param [in] x_pDirectory	- Directory to start in

		\return Process ID or zero if failure
	*/
	static oexUINT StartProcess( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory );

	/// Returns the specified process version
	static oexUINT GetProcessVersion( oexUINT uPid );
	
	enum
	{
		/// Reboots the computer
		eReboot = 1,

		/// Powers off the computer
		ePowerOff,

		/// Shuts down the OS, but does not power off
		eShutdown,

		/// Logs off the current user
		eLogOff,

		/// Restarts the os
		eRestart

	};

	//==============================================================
	// CtrlComputer()
	//==============================================================
	/// Reboot, Restart, Logoff computer
	/**
		@param [in] nCmd	-	eReboot, ePowerOff, eShutdown,
								eLogOff, eRestart
		@param [in] nForce	-	0 = Do not force
								1 = Force apps to shutdown
								2 = Force apps that are hung
		@param [in] pMsg	-	If not null, message to display to
								user before shutdown starts.

		@return Non-zero if success
	*/
	static oexINT CtrlComputer( int nCmd, int nForce, oexCSTR pMsg );

	//==============================================================
	// KillProcess()
	//==============================================================
	/// Kill the specified process
	/**
		@param [in] nPid		-	Process ID
		@param [in] uTimeout	-	If not zero, the time in milliseconds
									to wait for the process to exit.
		@param [in] uExit		-	Exit code for the killed process

		@return Non-zero if success
	*/
	static oexINT KillProcess( oexINT nPid, oexUINT uTimeout, oexUINT uExit );
	
	//==============================================================
	// SetRoot()
	//==============================================================
	/// Switch to root or administrator
	static oexINT SetRoot();

	//==============================================================
	// IsRoot()
	//==============================================================
	/// Switch to root or administrator
	static oexINT IsRoot();

	//==============================================================
	// GetCpuLoad()
	//==============================================================
	/// Returns a value between 0 and 100 representing the current
	/// cpu load.
	static oexDOUBLE GetCpuLoad();

	//==============================================================
	// GetCpuLoad()
	//==============================================================
	/// Returns a value between 0 and 100 representing the current
	/// cpu load of the specified process
	/**
		\param [in] x_pProcessName	- Process for which the cpu
									  load is relevant
	*/
	static oexDOUBLE GetCpuLoad( oexCSTR x_pProcessName );

	//==============================================================
	// Rand()
	//==============================================================
	/// Returns a random number between 0 and nMax
	/**
		\param [in] nMax	- Maximum number to return.  If nMax is
							  zero, the entire int will be random
							  bits.
	*/
	static oexUINT Rand( oexUINT uMax = 0 );

	//==============================================================
	// Rand()
	//==============================================================
	/// Randomizes the specified array
	/**
		\param [in] p	-	Pointer to the buffer to ramdomize
		\param [in] sz	-	Size of the buffer in p

		\return Returns the number of bytes processed
	*/
	static oexUINT Rand( oexPVOID p, oexUINT sz );

	//==============================================================
	// RandSeed()
	//==============================================================
	/// Seeds the random number generator
	static void RandSeed( oexUINT uSeed );
};

//==================================================================
/// SResInfo
//==================================================================
/// Holds information about resources
/**
	The idea here is to unify resource handles and functions so
	we can more closely emulate Windows.
*/
typedef struct tagSResInfo
{
	// Resource type
	oexUINT					uType;

	// Pointer to resource
	oexPVOID				pRes;

	// CStr object containing name
	oexPVOID				pName;

	// Resource state
	oexINT					nState;

public:

	enum
	{
		eInvalid = 0,
		eNamedEvent
	};

	static tagSResInfo* Create( oexCSTR x_pName );

	static void Release( tagSResInfo *x_pRi );

} SResInfo;


/*
	static float TriangleArea( float a, float b, float c )
	{	float s = ( a + b + c ) / (float)2;
		return sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );
	}

	static float TriangleArea( float x1, float y1, float x2, float y2, float x3, float y3 )
	{
		float ax = x1 - x2;
		float ay = y1 - y2;
		float al = sqrt( ( ax * ax ) + ( ay * ay ) );

		float bx = x2 - x3;
		float by = y2 - y3;
		float bl = sqrt( ( bx * bx ) + ( by * by ) );

		float cx = x3 - x1;
		float cy = y3 - y1;
		float cl = sqrt( ( cx * cx ) + ( cy * cy ) );

		return TriangleArea( al, bl, cl );
	}


*/
