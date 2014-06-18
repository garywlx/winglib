/*------------------------------------------------------------------
// sq_engine.h
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

class CSqEngine;

/**
	\defgroup SQAPI Squirrel Engine API
*/

//==================================================================
// CSqEngineExport
//
/// This base class is imported in to the squirrel environment
//==================================================================
class CSqEngineExport
{

public:

	SQBIND_CLASS_CTOR_BEGIN( CSqEngineExport )
	SQBIND_CLASS_CTOR_END( CSqEngineExport )

	/** \addtogroup SQAPI
		@{
	*/

		/// Returns non zero if squirrel is a debug build
		int isDebugBuild();

		/// Returns string indicating compiler used to build
		stdString getCompiler();

		/// Returns the cpu size in bits ( 16, 32, 64, etc... )
		int getCpuSize();

		/// Returns the cpu type name
		stdString getCpuType();

		/// Returns the platform name
		stdString getPlatform();

		/// Builds a service name with string
		stdString makeServiceName( const sqbind::stdString &s );

		/// Reboot or restart computer
		/**
			@param [in] nCmd	-	1 = Reboot
									2 = Power Off
									3 = Shutdown
									4 = Log Off
									5 = Restart

			@param [in] nForce	-	0 = No force
									1 = Force
									2 = Force if hung

			@param [in] sMsg	-	Optional message to display to user.

			Executes the specified command

			@return Non-zero if success
		*/
		int ctrl_computer( int nCmd, int nForce, const sqbind::stdString &sMsg );

		/// Reboot computer
		/**
			@param [in] nForce	-	0 = No force
									1 = Force
									2 = Force if hung

			@param [in] sMsg	-	Optional message to display to user.

			Executes the specified command

			@return Non-zero if success
		*/
		int reboot( int nForce, const sqbind::stdString &sMsg );

		/// Shutdown computer
		/**
			@param [in] nForce	-	0 = No force
									1 = Force
									2 = Force if hung

			@param [in] sMsg	-	Optional message to display to user.

			Executes the specified command

			@return Non-zero if success
		*/
		int shutdown( int nForce, const sqbind::stdString &sMsg );

		/// Logoff user
		/**
			@param [in] nForce	-	0 = No force
									1 = Force
									2 = Force if hung

			@param [in] sMsg	-	Optional message to display to user.

			Executes the specified command

			@return Non-zero if success
		*/
		int logoff( int nForce, const sqbind::stdString &sMsg );

		/// Elevates the program to root priviledges
		int set_root();

		/// Returns non-zero if application is running as root
		int is_root();

		/// Returns the buffer for a binary share
		/**
			@param [in] sName	- The share name

			@return A CSqBinary object wrapping the share buffer
		*/
		CSqBinary get_binshare( const stdString &sName );

		/// Shares the binary buffer with the specified name
		/**
			@param [in] sName	- The share name
			@param [in] pBin	- The binary buffe to share

			@return Non-zero on success
		*/
		int set_binshare( const stdString &sName, CSqBinary *pBin );

		/// Return non-zero if the specified share exists
		/**
			@param [in] sName	- The share name

			@return Non-zero if the share exists
		*/
		int is_binshare( const stdString &sName );

		/// Runs garbage collection on binary shares
		int cleanup_binshare();

		/// Returns the module instance handle
		unsigned int get_instance_handle();

		/// Enables capturing console output
		int enable_output_capture( int buffers, int buf_size );

		/// Enables capturing console output to file
		int enable_output_file_capture( const stdString &sFile, unsigned int uTimestampFreq );

		/// Returns the total number of threads that have been created
		int get_total_thread_count();

		/// Returns the number of currently running threads
		int get_running_thread_count();

		/// Sets the thread checkpoint
		int set_thread_checkpoint( int cp );

		/// Gets the thread checkpoint
		int get_thread_checkpoint();

		/// Sets the thread tag
		int set_thread_tag( const stdString &tag );

		/// Gets the thread tag
		stdString get_thread_tag();

		/// Returns console output
		stdString get_output( int max );

		/// Sets the specified property
		void set( const stdString &sPath, const stdString &sKey, const stdString &sVal );

		/// Sets a key timeout
		void tset( const stdString &sPath, const stdString &sKey, unsigned int uTo );

		/// Gets the specified property
		stdString get( const stdString &sPath, const stdString &sKey );

		/// Sets the specified property
		void jset( const stdString &sPath, const stdString &sKey, const stdString &sVal );

		/// Gets the specified property
		stdString jget( const stdString &sPath, const stdString &sKey );

		/// Returns the sizeof the current array
		int asize( const stdString &sPath, const stdString &sKey );

		/// Returns non-zero if the specified value exists
		int isset( const stdString &sPath, const stdString &sKey );

		/// Sets the specified property
		void aset( const stdString &sPath, const stdString &sKey, const stdString &sVal );

		/// Sets the specified property
		void mset( const stdString &sPath, const stdString &sKey, const stdString &sVal );

		/// Gets the specified property
		stdString kget( const stdString &sPath, const stdString &sKey );

		/// Returns a copy of the entire property bag
		CSqMulti pb( const stdString &sPath );

		/// Adds a key to the logger
		int addlog( const stdString &sPath, const stdString &sKey, int nType );

		/// Removes a key from the logger
		int removelog( const stdString &sPath, const stdString &sKey );

		/// Sets the logger output folder
		int setlogroot( const stdString &sPath, const stdString &sRoot );

		/// Returns the root logging folder
		stdString getlogroot( const stdString &sPath );

		/// Sets the logger frequency
		int setlogfreq( const stdString &sPath, int nFreq );

		/// Returns the logger frequency
		int getlogfreq( const stdString &sPath );

		/// Returns a list of keys that are being logged
		stdString getlogkeys( const stdString &sPath, int nTime );

		/// Returns log data for the specified key
		stdString getlog( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, int nDataType, int nMethod );

		/// Returns log data for the specified key in a binary share
		stdString getlogbin( const stdString &sPath, const stdString &sKey, int nStart, int nStop, int nInterval, const stdString &sExt );

		/// Adds binary information to the data log
		int logbin( const stdString &sPath, const stdString &sKey, const stdString &sBin, int nType, int nTime, int nTimeMs, int nBuffering );

		/// Resets the logger
		int resetlog( const stdString &sPath );

		/// Returns the script id
		stdString getid( const stdString &sPath );

		/// Sets the specified timer
		int set_timer( const stdString &sPath, int to, const stdString &sCallback );

		/// Sets the specified timeout
		int set_timeout( const stdString &sPath, int to, const stdString &sCallback );

		/// Kills the specified timer
		int kill_timer( const stdString &sPath, int id );

		/// Requests the appliation exit
		void exit( int nExitCode );

		/// Immediatly stops the application
		void terminate( int nExitCode );

		/// Displays a message box containing string
		int alert( const stdString &sMsg );

		/// Sends data to STDOUT
		int print( const stdString &sMsg );

		/// Sends data to STDOUT
		//  Same as print, but appends new line
		int echo( const stdString &sMsg );

		/// Read data from stdin
		stdString read_stdin( int nMax );

		/// Flushes STDOUT
		int flush();

		/// Seeds the random number generator
		void srand( int nSeed );

		/// Imports the specified class
		int import( const stdString &sClass );

		/// Includes the specified script
		int include( const stdString &sScript );

		/// Includes the specified script the first time,
		/// ignores further calls to include the same script
		int include_once( const stdString &sScript );

		/// Includes the specified inline script
		stdString include_inline( const stdString &sScript, CSqMulti *pParams );

		/// Includes the specified inline script
		stdString run_inline( const stdString &sScript, CSqMulti *pParams );

		/// Returns the result of the last script execution
		stdString get_script_return_value();

		/// Push include stack
		int push_stack( CSqMulti *pParams );

		/// Pop include stack
		stdString pop_stack();

		/// Returns the size of the include stack
		int get_stack_size();

		/// Gets current include params
		CSqMulti get_stack_params();

		/// Sets current include params
		int set_stack_params( CSqMulti *sParams );

		/// Returns current output buffer
		stdString get_stack_output();

		/// Sets the current output buffer
		int set_stack_output( const stdString &sOutput );

		/// Appends the current output buffer
		int append_stack_output( const stdString &sOutput );

		/// Loads the specified module
		int load_module( const stdString &sModule, const stdString &sPath );

		/// Sends the kills message to the specified thread
		int kill( const stdString &sPath );

		/// Sends the kills message to the specified thread and doesn't return until it exits
		int kill_wait( const stdString &sPath, int nTimeout, int bTerminate );

		/// Sends the terminate mesage to the specified thread
		int terminate_thread( const stdString &sPath );

		/// Sets the current threads priority
		int set_thread_priority( int nPriority );

		/// Sets the current threads affinity
		int set_thread_affinity( int nAffinity );

		/// Returns the size of the specified type
		int type_size( int type );

		/// Returns boot time in seconds
		float clock();

		/// Returns boot count
		unsigned int ticks();

		/// Returns the version
		stdString get_version();

		/// Returns the build
		stdString get_build();

		/// Compares two version numbers
		int compare_version( const stdString &sV1, const stdString &sV2, const stdString &sSep );

		/// Returns the application name
		stdString get_app_name();

		/// Returns the application service name
		stdString get_service_name();

		/// Returns the application name
		stdString get_app_long_name();

		/// Returns the application description
		stdString get_app_description();

		/// Returns the script name
		stdString get_name();

		/// Returns the script or script filename
		stdString get_script();

		/// Get child scripts
		stdString get_children( const stdString &sPath );

		/// Decorates the file with appropriate compile time options
		stdString decorate( const stdString &sPath, int bExe, int bLib );

		/// Returns the file path to the current script
		stdString path( const stdString &sPath );

		/// Returns the file path to the current executable
		stdString root( const stdString &sPath );

		/// Returns the specified system folder
		stdString get_sys_folder( int bShared, const stdString &sId );

		/// Returns the specified system and path
		stdString get_sys_path( int bShared, const stdString &sId, const stdString &sPath );

		/// Returns the file path of the current executable
		stdString module_path();

		/// Returns the file name of the current executable
		stdString module_name();

		/// Returns the md5 for the specified string
		stdString md5( const stdString &sStr );

		/// Returns a guid representation of the md5 hash
		stdString guid( const stdString &sStr );

		/// Builds a url from components
		stdString build_url( CSqMulti *pUrl );

		/// Parses a url into its components
		/**
			Parses a url into an associative array.

			This example link...

				http://user:password@server:80/the/path?a=b&c=d#marker

			Parses to this

				[ "scheme" ]	- http
				[ "username" ]	- user
				[ "password" ]	- password
				[ "host" ]		- server
				[ "port" ]		- 80
				[ "path" ]		- /the/path
				[ "extra" ]		- a=b&c=d
				[ "fragment" ]	- marker

		*/
		CSqMulti parse_url( const stdString &sUrl );

		/// Base64 encodes a string
		stdString base64_encode( const stdString &sStr );

		/// Base64 decodes a string
		stdString base64_decode( const stdString &sStr );

		/// Base16 encodes a string
		stdString base16_encode( const stdString &sStr );

		/// Base16 decodes a string
		stdString base16_decode( const stdString &sStr );

		/// Encode array into Registry file
		sqbind::stdString RegEncode( sqbind::CSqMulti *m );

		/// Typed encode array into Registry file
		sqbind::stdString RegEncodeTyped( sqbind::CSqMulti *m, sqbind::CSqMulti *t );

		/// Decode Registry file
		sqbind::CSqMulti RegDecode( const sqbind::stdString &s, int bExpandEmbeddedItems );

		/// Expand embedded registry file items
		int RegExpandEmbedded( sqbind::CSqMulti *m );

		/// Builds a path using the specified elements
		stdString build_path( const stdString &sS1,  const stdString &sS2 );

		/// Builds a file path using the specified elements
		stdString file_path( const stdString &sS1,  const stdString &sS2 );

		/// Returns non-zero if pattern matches string
		int match_file_pattern( const stdString &sPattern, const stdString &sStr, int bIgnoreCase );

		/// Returns an ascii mandelbrot image
		stdString mandelbrot( int w, int h, int x1, int y1, int x2, int y2 );

		/// Logs an error
		void error( int e, const stdString &sStr );

		/// Logs a warning
		void warning( int e, const stdString &sStr );

		/// Logs a message
		void notice( int e, const stdString &sStr );

		/// Returns the value of the last key press, blocks if no key has been pressed
		int get_key();

		/// Returns non-zero if there is a key press
		int is_key();

		/// Trims the specified characters from the beginning of the string
		stdString ltrim( const stdString &sS, const stdString &sChs );

		/// Trims the specified characters from the end of the string
		stdString rtrim( const stdString &sS, const stdString &sChs );

		/// Trims the specified characters from the beginning and end of the string
		stdString trim( const stdString &sS, const stdString &sChs );

		/// Trims the white space from the beginning and end of the string
		stdString trimws( const stdString &sS );

		/// Finds sub string sSub in sS, returns offset or less than zero if not found
		int find( const stdString &sS, const stdString &sSub );

		/// Finds sub string sSub in sS, returns offset or less than zero if not found
		int ifind( const stdString &sS, const stdString &sSub );

		/// Parses sub string sSub in sS, returns parsed string
		stdString parse( const stdString &sS, const stdString &sSub );

		/// Parses sub string sSub in sS, returns parsed string
		stdString iparse( const stdString &sS, const stdString &sSub );

		/// Creates a size string like 1.3MB, 44.5GB, etc...
		/**
			@param [in] d		- Value
			@param [in] dDiv	- Divisor, such as 1024
			@param [in] nDigits	- Number of digits trailing the decimal
			@param [in] sSuffix	- Optional comma sperated suffix array
								  If empty string, defaults are "Bytes,KB,MB,GB,TB,PB,EB,ZB,YB,BB"
		*/
		stdString create_size_string( double d, double dDiv, int nDigits, const stdString &sSuffix );

		/// Splits a string into an array
		CSqMulti splitstr( const stdString &s, const stdString &seps );

		/// Joins array components
		stdString joinstr( CSqMulti *m, const stdString &sep );

		/// Returns a string clipped to the specified size
		stdString str_limit( const stdString &sS, int maxchars, const stdString &sAppend );

		/// Replaces the occurences of the specified string
		stdString replace( const stdString &sS, const stdString &sFind, const stdString &sReplace );

		/// Replaces the case insensitive occurences of the specified string
		stdString ireplace( const stdString &sS, const stdString &sFind, const stdString &sReplace );

		/// Drop specified characters from a string or all but
		stdString drop( const stdString &sS, const stdString &sDrop, int bInclusive );

		/// Drop the specified range of characters from a string or all but
		stdString drop_range( const stdString &sS, const stdString &sBegin, const stdString &sEnd, int bInclusive );

		/// JSON decodes the string
		stdString json_decode( const stdString &sS );

		/// JSON encodes the string
		stdString json_encode( const stdString &sS );

		/// URL encodes the string
		stdString urlencode( const stdString &sS );

		/// URL decodes the string
		stdString urldecode( const stdString &sS );

		/// HTML encodes the string
		stdString htmlencode( const stdString &sS );

		/// HTML encodes the string and limits the length
		stdString htmlencode_limit( const stdString &sS, int nMaxChars, const stdString &sAppend );

		/// HTML decodes the string
		stdString htmldecode( const stdString &sS );

		/// Compresses string using zlib/deflate
		stdString compress( const stdString &sS );

		/// Uncompresses string using zlib/inflate
		stdString uncompress( const stdString &sS );

		/// Returns a unique md5 hash
		stdString unique();

		/// Returns local time stamp
		int local_time();

		/// Returns gmt time stamp
		int gmt_time();

		/// Local timestamp in microseconds
		SQInteger local_time_useconds();

		/// GMT timestamp in microseconds
		SQInteger gmt_time_useconds();

		/// Returns a formated time string
		stdString local_timestr( const stdString &fmt );

		/// Returns a formated time string
		stdString gmt_timestr( const stdString &fmt );

		/// Kills the specified process
		int kill_process( int nPid, int nTimeout, int nExit );

		/// Creates a thread
		int spawn( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile );

		/// Creates a thread
		int spawn2( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScriptName, const stdString &sScript, int bFile );

		/// Executes the specified script
		stdString run( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScript );

		/// Executes the specified script within the current thread
		SquirrelObject run2( const stdString &sName, const stdString &sScript );

		/// Prepares an inline script for processing
		stdString prepare_inline( const stdString &sScript, int bFile );

		/// Checks for path/threads existence
		int is_path( const stdString &sPath );

		/// Processes system messages
		int process_system_messages();

		/// Executes shell command
		int shell( const stdString &sFile, const stdString &sParams, const stdString &sDirectory );

		/// Executes shell command
		int exec( const stdString &sFile, const stdString &sParams, const stdString &sDirectory );

		/// Returns the specified process version
		int get_process_version( int nPid );

		/// Returns the squirrel executable name
		stdString sqexe_name();

		/// Returns the squirrel executable path
		stdString sqexe_path();

		/// Attempts to find the squirrel interpreter and execute the specified params
		int sqexe( const stdString &sParams, const stdString &sDir );

		/// Attempts to find the squirrel interpreter and execute the specified script
		int sqexe_script( const stdString &sScript, const stdString &sParams, const stdString &sDir );

		/// Installs a service
		int service_install( const stdString &sName, const stdString &sDesc, const stdString &sExe, int bAutoRestart );

		/// Removes a service
		int service_remove( const stdString &sName );

		/// Starts a service
		int service_start( const stdString &sName );

		/// Stops a service
		int service_stop( const stdString &sName );

		/// Restarts a service
		int service_restart( const stdString &sName );

		/// Returns the current cpu load
		double get_cpu_load();

		/// Returns information about the cpu
		CSqMulti get_cpu_info();

		/// Returns information about global memory usage
		CSqMulti get_memory_status();

		/// Amount of time in seconds, since the computer started
		double boot_time();

		/// Tick count seconds
		unsigned int get_timer_seconds();

		/// Tick count micro seconds
		SQInteger get_timer_useconds();

		/// Tick count micro seconds
		SQInteger get_total_useconds();

		/// Returns a linked in resource
		stdString get_resource( const stdString &sRes, int bFileOverrideOk );

		/// Returns a linked in resource
		CSqBinary get_resource_bin( const stdString &sRes, int bFileOverrideOk );

		/// Finds resources matching the pattern in sName
		CSqMulti find_resource( const stdString &sName, int bIgnoreCase, int bFileOverrideOk );

		/// Returns information about the specified system drive
		CSqMulti get_system_drive_info( const stdString &sDrive );

		/// Returns the size of the specified file
		SQInteger get_file_size( const stdString &sFile );

		/// Returns information about the specified file
		CSqMulti get_file_info( const stdString &sFile );

		/// Returns the number of screens attached to the system
		int screen_get_num();

		/// Returns information about the system displays
		CSqMulti screen_get_info();

		/// Initializes screen capture resources
		int screen_init_capture( CSqBinary *x_pInf, int screen, int fmt, int w, int h );

		/// Returns information about the screen capture
		CSqMulti screen_get_capture_info( CSqBinary *x_pInf );

		/// Release screen capture resources
		int screen_release_capture( CSqBinary *x_pInf );

		/// Locks screen data
		int screen_lock( CSqBinary *x_pInf, CSqBinary *x_pImg, int x_nScreen );

		/// Unlocks screen data
		int screen_unlock( CSqBinary *x_pInf, CSqBinary *x_pImg );

		/// Returns information about the mouse
		int get_mouse_info( CSqMulti *m );

		/// Returns mouse position as ( ( short(y) << 16 ) | short(x) )
		SQInteger get_mouse_pos();

		/// Sets the mouse position
		int set_mouse_pos( int x, int y );

		/// Queues the specified input
		int queue_input( CSqMulti *m );

		/// Retrieves a list of running processes
		int get_process_list( CSqMulti *m, int bProcessInfo );

		/// Retrieves info about a specific process
		int get_process_info( SQInteger pid, CSqMulti *m );

		/// Returns a list of reg keys
		CSqMulti reg_get_keys( const stdString &sKey, const stdString &sPath, int bValues, int bSubKeys );

		/// Returns non-zero if registry key exists
		int reg_is_key( const stdString &sKey, const stdString &sPath, const stdString &sName );

		/// Returns the string value of the specified key
		stdString reg_get_str( const stdString &sKey, const stdString &sPath, const stdString &sName );

		/// Sets the specified registry key
		int reg_set_str( const stdString &sKey, const stdString &sPath, const stdString &sName, const stdString &sValue );

		/// Deletes the specified registry key
		int reg_del_key( const stdString &sKey, const stdString &sPath, int bSubKeys );

		/// Deletes child keys of the specified key
		int reg_del_sub_keys( const stdString &sKey, const stdString &sPath );

		int tolong( const stdString &s )
		{	return oexStrToLong( s.c_str() ); }

		int toulong( const stdString &s )
		{	return oexStrToULong( s.c_str() ); }

		SQInteger toint64( const stdString &s )
		{	return (SQInteger)oexStrToInt64( s.c_str() ); }

		SQInteger touint64( const stdString &s )
		{	return (SQInteger)oexStrToUInt64( s.c_str() ); }

		int strtol( const stdString &s, int nRadix )
		{	return oexStrToLong( s.c_str(), nRadix ); }

		int strtoul( const stdString &s, int nRadix )
		{	return oexStrToULong( s.c_str(), nRadix ); }

		float tofloat( const stdString &s )
		{	return oexStrToFloat( s.c_str() ); }

		double todouble( const stdString &s )
		{	return oexStrToDouble( s.c_str() ); }

		/// Execute functions
		stdString execute( int nRet, const stdString &sPath, const stdString &sFunction );
		stdString execute1( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1 );
		stdString execute2( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2 );
		stdString execute3( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 );
		stdString execute4( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3, const stdString &sP4 );

		/// Causes the thread to sleep for the specified amount of time in milliseconds
		void sleep( int nMsTime );

		/// Returns a reference to the threads message queue object
		CSqMsgQueue* queue();

	// SQAPI
	/** @} */

public:

	enum
	{
		/// Size of a standard string
		eStrSize = 1024
	};

	struct SScriptData
	{
		// Constructors
		SScriptData() {}
		SScriptData( CSqMulti *m ) { if ( m ) mParams = *m; }

		// Copy
		SScriptData( const SScriptData &r ) { mParams = r.mParams; sOutput = r.sOutput; }
		SScriptData& operator = ( const SScriptData &r ) { mParams = r.mParams; sOutput = r.sOutput; return *this; }

		/// Params passed to script
		CSqMulti mParams;

		/// Script output
		stdString sOutput;
	};

	/// Script data stack
	typedef oexStdList( SScriptData )	t_ScriptData;

public:

	/// Default constructor
	CSqEngineExport();

	/// Copy constructor
	CSqEngineExport( const CSqEngineExport &r ) {}

	/// Copy operator
	CSqEngineExport& operator = ( const CSqEngineExport &r ) { return *this; }

	/// Destructor
	virtual ~CSqEngineExport();

	virtual SquirrelVM* GetVmPtr();

	virtual CSqEngine* GetEnginePtr();

	void SetRoot( const stdString &s ) { m_sRoot = s; }

	stdString& GetRoot() { return m_sRoot; }

	stdString& GetReturnData() { return m_sReturnData; }

protected:

	virtual int OnImport( const stdString &sClass );

	virtual int OnInclude( const stdString &sScript );

	virtual int OnIncludeOnce( const stdString &sScript );

	virtual int OnIncludeInline( const stdString &sScript );

	virtual int OnRunInline( const stdString &sScript );

	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	virtual stdString OnPath( stdString sPath );

	virtual SquirrelObject OnRun( const stdString &sName, const stdString &sScript );

	virtual CSqMsgQueue* OnGetQueue();

protected:

	/// Non-zero if loaded from file
	oex::oexBOOL        m_bFile;

	/// Either a file name or actual script
	stdString        	m_sScript;

	/// Root path
	stdString           m_sRoot;

	/// Name of the script
	stdString			m_sScriptName;

	/// Script include stack data
	t_ScriptData		m_lstScriptData;

	/// Return data from last script execution
	stdString			m_sReturnData;

};

// Declare class functions
}; SQBIND_DECLARE_INSTANCE( sqbind::CSqEngineExport, CSqEngineExport ); namespace sqbind {

#define LogErrorM( r, e )		LogError( r, e, oexNULL, oexTEXT( __FILE__ ), __LINE__ )
#define LogErrorME( r, e, x )	LogError( r, e, x, oexTEXT( __FILE__ ), __LINE__ )

/// Squirrel engine
class CSqEngine : public CSqEngineExport
{
public:

	#define     SQEXE_FN_INIT       oexT( "_init" )
	#define     SQEXE_FN_IDLE       oexT( "_idle" )
	#define     SQEXE_FN_EXIT       oexT( "_exit" )
	#define     SQEXE_SELF          oexT( "_self" )

public:

	/// Contains information about a script error
	struct SScriptErrorInfo
	{
		/// Constructor
		SScriptErrorInfo( stdString x_sDesc, stdString x_sSource, oex::oexUINT x_uLine, oex::oexUINT x_uCol )
		{   sDesc = x_sDesc; sSource = x_sSource; uLine = x_uLine; uCol = x_uCol; }

		/// Description
		stdString    	sDesc;

		/// Source file
		stdString    	sSource;

		/// Line number
		oex::oexUINT    uLine;

		/// Column number
		oex::oexUINT    uCol;
	};

	/// Callback function for custom script source
	typedef int (*t_IncludeScript)( const stdString &sScript, stdString &sData, stdString &sName );

	/// Include script cache type
	typedef oexStdMap( sqbind::stdString, oex::oexUINT ) t_IncludeScriptCache;

public:

	/// Over-ride to perform special registration
	virtual void OnRegisterVariables() {}

public:

	/// Returns non-zero if squirrel has been initialized
	BOOL IsScript();

	/// Returns the last error
	stdString& GetLastError();

	/// Returns the script output
	stdString& GetOutput();

	/// Returns a reference the to the virtual machine
	SquirrelVM& GetVM();

	/// Returns a reference to the virtual machine
	operator SquirrelVM&();

	/// Returns a referenced to the compiled script object
	SquirrelObject& GetScriptObj();

	/// Returns the scripts root table
	const SquirrelObject& GetRootTable();

public:

	/// Intercepts print statements and puts the data in m_sOutput
	static void SqPrint( HSQUIRRELVM v, const SQChar* s, ... );

	/// Intercepts errors
	static SQInteger SqErrorHandler( HSQUIRRELVM v );

	/// Handles a script compiler error
	static void SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column );

	/// Auxiliary error handler
	static SQInteger SqAuxErrorHandler( HSQUIRRELVM v );

public:

	/// Default constructor
	CSqEngine();

	/// Destructor
	virtual ~CSqEngine();

	/// Destroys the virtual machine and prepares the class for re-use
	void Destroy();

	/// Calls script exit function if it hasn't already been called
	/// This only works once
	void Exit();

	/// Initializes the virtual machine
	/**
		\param [in] dwMainThreadId  -   The id of the parent thread

		You don't need to call this directly, it is called by Load().
	*/
	oex::oexBOOL Init();

	/// Sets the name of the script used for debugging
	/**
		\param [in] pScript     -   The script or a file name.

	*/
	void SetScriptName( oex::oexCSTR pScriptName );

	/// Gets the name of the script used for debugging
	stdString GetScriptName();

	/// Loads a script and prepares it for execution
	/**
		\param [in] sScript     -   The script or a file name.
		\param [in] pbScript	-	Binary script object
		\param [in] bFile       -   If non-zero, pScript contains a file name.
		\param [in] bRelative	-	Non-zero if path is relative
		\param [in] bStart		-	Non-zero if script should be initialized
		\param [in] bInline		-	Non-zero if script is inline
	*/
	oex::oexBOOL Load( const stdString &sScript, oex::CStr8 *pbScript, oex::oexBOOL bFile,
					   oex::oexBOOL bRelative = oex::oexFALSE, oex::oexBOOL bStart = oex::oexTRUE,
					   oex::oexBOOL bInline = oex::oexFALSE );

	/// Runs the script an executes the initialization function
	/**
		You can just set the bStart flag in Load() to TRUE to avoid
		calling this function if you have no special registrations.
	*/
	oex::oexBOOL Start();

	/// Destroys the virtual machine and prepares the class for re-use
	oex::oexBOOL Idle();

	/// Executes the specified buffer
	oex::oexBOOL Run( sqbind::stdString *pReply, oex::oexCSTR pName, oex::oexCSTR pScript );

	/// Executes the specified script within the current vm
	virtual SquirrelObject OnRun( const stdString &sName, const stdString &sScript );

	oex::oexINT LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e, oex::oexCSTR x_pExtra = oexNULL );

	oex::oexINT LogError( oex::oexINT x_nReturn, oex::oexCSTR x_pErr, oex::oexCSTR x_pExtra, oex::oexCSTR x_pFile, oex::oexUINT x_uLine );

public:

	/// Binds variables to the root table
	template< typename T >
		void BindRootVariable( T *pVar, oex::oexCSTR pName, SqPlus::VarAccessType access = SqPlus::VAR_ACCESS_READ_WRITE )
		{   SquirrelObject root = m_vm.GetRootTable();
			SqPlus::BindVariable( m_vm, root, pVar, pName, access );
		}

	/// Binds functions to the root table
	template< typename T >
		void BindRootFunction( T pFunction, oex::oexCSTR pName )
		{   SqPlus::RegisterGlobal( m_vm, pFunction, pName );
		}

	/// Binds member functions to the root table
	template< typename T_CLASS, typename T_FUNCTION >
		void BindRootMemberFunction( T_CLASS pClass, T_FUNCTION pFunction, oex::oexCSTR pName )
		{   SqPlus::RegisterGlobal( m_vm, pClass, pFunction, pName );
		}

public:

	/// Executes a specific function within the script
	/**
		\param [in] pFunction   -   Name of the function to execute
	*/
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = sqbind::obj2str( f() );
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_P1 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = sqbind::obj2str( f( p1 ) );
			else
				f( p1 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}


template< typename T_P1, typename T_P2 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = sqbind::obj2str( f( p1, p2 ) );
			else
				f( p1, p2 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_P1, typename T_P2, typename T_P3 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = sqbind::obj2str( f( p1, p2, p3 ) );
			else
				f( p1, p2, p3 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = sqbind::obj2str( f( p1, p2, p3, p4 ) );
			else
				f( p1, p2, p3, p4 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

	/// Executes a specific function within the script
	/**
		\param [in] pFunction   -   Name of the function to execute
	*/
template< typename T_RET >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f();
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{
#if defined( _DEBUG )
			// +++ Startup code needs to check for function existance first
//			return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() );
#else
			return oex::oexFALSE;
#endif
		}

		return oex::oexTRUE;
	}

template< typename T_RET >
	oex::oexBOOL Execute( T_RET &rRet, oex::oexCSTR x_pFunction )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET& > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( &rRet ) )
				rRet = f();
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1 );
			else
				f( p1 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1, p2 );
			else
				f( p1, p2 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2, typename T_P3 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1, p2, p3 );
			else
				f( p1, p2, p3 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1, p2, p3, p4 );
			else
				f( p1, p2, p3, p4 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( "() : " ), e.desc ).Ptr() ); }

		return oex::oexTRUE;
	}

    template< typename T_P1 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = "Invalid function object";
                    return oex::oexFALSE;
                } // end if

                f( p1 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}


    template< typename T_P1, typename T_P2 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1, const T_P2 &p2 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = "Invalid function object";
                    return oex::oexFALSE;
                } // end if

                f( p1, p2 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}

    template< typename T_P1, typename T_P2, typename T_P3 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1, const T_P2 &p2, const T_P3 &p3 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = oexT( "Invalid function object" );
                    return oex::oexFALSE;
                } // end if

                f( p1, p2, p3 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}

public:

	/// Sets pointer to the module manager
	void SetModuleManager( CModuleManager *x_pMm );

	/// Registers library classes
	virtual int OnImport( const stdString &sClass );

	/// Includes the specified script
	virtual int OnInclude( const stdString &sScript );

	/// Includes the specified script
	virtual int OnIncludeOnce( const stdString &sScript );

	/// Includes the specified inline script
	virtual int OnIncludeInline( const stdString &sScript );

	/// Includes the specified inline script
	virtual int OnRunInline( const stdString &sScript );

	/// Loads the specified module
	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	/// Sets the pointer to the thread message queue
	void SetMessageQueue( CSqMsgQueue *pMq );

	/// Return message queue pointer
	virtual CSqMsgQueue* OnGetQueue();

	/// Return vm pointer
	virtual SquirrelVM* GetVmPtr();

	/// Returns the engine pointer
	virtual CSqEngine* GetEnginePtr() { return this; }

	/// Call to set a function that is called when thread starts
	/// to export custom symbols
	void SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa );

	/// Returns pointer to symbol export function
	PFN_SQBIND_Export_Symbols GetExportFunction() { return m_fExportSymbols; }

	/// Returns pointer to memory allocator
	sqbind::SSqAllocator* GetAllocator() { return m_pSqAllocator; }

	/// Custom include function
	void SetIncludeScriptFunction( t_IncludeScript f )
	{	m_fIncludeScript = f; }

	/// Returns custom include function
	t_IncludeScript GetIncludeScriptFunction()
	{	return m_fIncludeScript; }

	/// Returns the compiled script
	oex::CStr8 GetCompiledScript();

	/// Sets a pre compiled script
	oex::oexBOOL SetCompiledScript( oex::CStr8 buf );

private:

	/// Non-zero if a script is loaded
	oex::oexBOOL				m_bLoaded;

	/// Squirrel virtual machine
	SquirrelVM              	m_vm;

	/// ID of the thread that created the vm
	oex::oexUINT				m_uThreadId;

	/// Pointer to module manager object
	CModuleManager          	*m_pModuleManager;

	/// Squirrel script
	SquirrelObject          	m_script;

	/// Last squirrel error
	stdString            		m_sErr;

	/// Script output
	stdString            		m_sOutput;

	/// Thread message queue pointer
	CSqMsgQueue             	*m_pMsgQueue;

	/// Function that is called by thread to export symbols
	PFN_SQBIND_Export_Symbols	m_fExportSymbols;

	/// Custom memory allocator
	sqbind::SSqAllocator		*m_pSqAllocator;

	/// Custom script include function
	t_IncludeScript				m_fIncludeScript;

	/// List of scripts that have already been included
	t_IncludeScriptCache		m_mapIncludeScriptCache;

	/// Non-zero if exit function needs calling
	BOOL						m_bCallExit;

};



