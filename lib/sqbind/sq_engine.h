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

// namespace
namespace sqbind
{
	//==================================================================
	// CSqEngineExport
	//
	/// This base class is imported in to the squirrel environment
	//==================================================================
	class CSqEngineExport 
	{            
	public:

		enum
		{
			/// Size of a standard string
			eStrSize = 1024
		};

	public:

		// Simple alert function
		int alert( const std::tstring &sMsg )
		{	return oex::os::CSys::ShowMessageBox( sMsg.c_str(), _T( "Notice" ) ); 
		}

		int import( const std::tstring &sClass )
		{   return OnImport( sClass ); }

		int load_module( const std::tstring &sModule, oex::oexBOOL bRelative )
		{   return OnLoadModule( sModule, bRelative ); }

		void quit( int nExitCode )
		{   oex::os::CSys::Exit( nExitCode );
		}

		std::tstring path( const std::tstring &sPath )
		{   return OnPath( sPath ); }

		SquirrelObject spawn( const std::tstring &sName, const std::tstring &sScript, int bFile )
		{   return OnSpawn( sName, sScript, bFile ); }

		SquirrelObject execute( const std::tstring &sName, const std::tstring &sFunction )
		{   return OnExecute( sName, sFunction ); }

		SquirrelObject execute1( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
		{   return OnExecute1( sName, sFunction, sP1 ); }

		SquirrelObject execute2( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2 )
		{   return OnExecute2( sName, sFunction, sP1, sP2 ); }

		SquirrelObject execute3( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2, const std::tstring &sP3 )
		{   return OnExecute3( sName, sFunction, sP1, sP2, sP3 ); }

		void sleep( int nMsTime )
		{   oex::os::CSys::Sleep( nMsTime ); }

		// _self.queue()
		CMsgQueue* queue()
		{   return OnGetQueue(); }

	protected:

		virtual int OnImport( const std::tstring &sClass ) { return 0; }

		virtual int OnLoadModule( const std::tstring &sModule, BOOL bRelative  ) { return 0; }

		virtual std::tstring OnPath( std::tstring sPath ) { return std::tstring(); }

		virtual SquirrelObject OnSpawn( const std::tstring &sName, const std::tstring &sScript, int bFile )
		{   return SquirrelObject( NULL ); }
	    
		virtual SquirrelObject OnExecute( const std::tstring &sName, const std::tstring &sFunction )
		{   return SquirrelObject( NULL ); }
		virtual SquirrelObject OnExecute1( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
		{   return SquirrelObject( NULL ); }
		virtual SquirrelObject OnExecute2( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2 )
		{   return SquirrelObject( NULL ); }
		virtual SquirrelObject OnExecute3( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2, const std::tstring &sP3 )
		{   return SquirrelObject( NULL ); }

		virtual CMsgQueue* OnGetQueue() { return NULL; }

	protected:

		/// Non-zero if loaded from file
		oex::oexBOOL        m_bFile;

		/// Either a file name or actual script
		std::tstring        m_sScript;
	    
	};

};
	// Declare class functions
	DECLARE_INSTANCE_TYPE( sqbind::CSqEngineExport );

namespace sqbind
{
	/// Squirrel engine
    class CSqEngine : public CSqEngineExport
    {
	public:

		#define     SQEXE_FN_INIT       oexT( "_init" )
		#define     SQEXE_FN_END        oexT( "_end" )
		#define     SQEXE_SELF          oexT( "_self" )

	public:

		/// Contains information about a script error
		struct SScriptErrorInfo
		{
			/// Constructor
			SScriptErrorInfo( std::tstring x_sDesc, std::tstring x_sSource, oex::oexUINT x_uLine, oex::oexUINT x_uCol )
			{   sDesc = x_sDesc; sSource = x_sSource; uLine = x_uLine; uCol = x_uCol; }

			/// Description
			std::tstring    sDesc;

			/// Source file
			std::tstring    sSource;

			/// Line number
			oex::oexUINT    uLine;

			/// Column number
			oex::oexUINT    uCol;
		};

	public:

		/// Over-ride to perform special registration
		virtual void OnRegisterVariables() {}

	public:

		/// Returns non-zero if squirrel has been initialized
		BOOL IsScript() { return m_bLoaded; }

		/// Returns the last error
		std::tstring& GetLastError() { return m_sErr; }

		/// Returns the script output
		std::tstring& GetOutput() { return m_sOutput; }

		/// Returns a reference the to the virtual machine
		SquirrelVM& GetVM() { return m_vm; }

		/// Returns a reference to the virtual machine
		operator SquirrelVM&() { return m_vm; }

		/// Returns a referenced to the compiled script object
		SquirrelObject& GetScriptObj() { return m_script; }

		/// Returns the scripts root table
		const SquirrelObject& GetRootTable() { return m_vm.GetRootTable(); }


	public:

		/// Intercepts print statements and puts the data in m_sOutput
		static void SqPrint( HSQUIRRELVM v, const SQChar* s, ... )
		{
			StackHandler sa( v );
			SquirrelObject root( v, sa.GetObjectHandle( 1 ) );

			// Retrieve our class pointer
			SquirrelObject ptr = root.GetValue( SQEXE_SELF );
			CSqEngine *pSqEngine = (CSqEngine*)(CSqEngineExport*)ptr.GetInstanceUP( SqPlus::ClassType< CSqEngineExport >::type() );
			if ( !pSqEngine ) return;

			// Grab string from the command line
			oex::CStr sErr;
			oexVaList ap; oexVaStart( ap, s );
			sErr.vFmt( s, ap );
			oexVaEnd( ap );

			// Save the output string
			pSqEngine->m_sOutput += sErr.Ptr();
		}

		/// Intercepts errors
		static SQInteger CSqEngine::SqErrorHandler( HSQUIRRELVM v )
		{
			if( 0 >= sq_gettop( v ) ) 
				return 0;

			const SQChar *sErr = 0;
			if( SQ_SUCCEEDED( sq_getstring( v, -1, &sErr ) ) ) 
				throw SScriptErrorInfo( sErr, _T( "" ), 0, 0 );

			else
				throw SScriptErrorInfo( _T( "Unknown error" ), _T( "" ), 0, 0 );

			return 0;
		}

		/// Handles a script compiler error
		static void CSqEngine::SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column )
		{
			throw SScriptErrorInfo( sErr, sSource, line, column );
		}

		/// Auxiliary error handler
		static SQInteger CSqEngine::SqAuxErrorHandler( HSQUIRRELVM v )
		{
			if( 0 >= sq_gettop( v ) ) 
				return 0;

			std::tstring sErr;
			const SQChar *pErr = 0;
			if( SQ_SUCCEEDED( sq_getstring( v, -1, &pErr ) ) ) 
				sErr = pErr;
			else
				sErr = _T( "Unknown Error" );

			// Get stack trace
			SQStackInfos si;
			if ( SQ_SUCCEEDED( sq_stackinfos( v, 1, &si ) ) )
			{   std::tstring sMsg;
				if ( si.funcname ) 
				{   sMsg += si.funcname; 
					sMsg += _T( "() : " ); 
				} // end if
				sMsg += sErr;
				throw SScriptErrorInfo( sMsg.c_str(), si.source ? si.source : _T( "" ), si.line, 0 );
			} // end if

			else 
				throw SScriptErrorInfo( sErr.c_str(), _T( "" ), 0, 0 );

			return 0;
		}

	public:

		/// Default constructor
		CSqEngine() :
		    m_vm( SquirrelVM::StdLib_All ), 
			m_script( m_vm.GetVMHandle() ) 
		{
			m_bLoaded = oex::oexFALSE;
		}

		/// Destructor
		virtual ~CSqEngine() { Destroy(); }

		/// Destroys the virtual machine and prepares the class for re-use
		void Destroy()
		{
//			Execute( SQEXE_FN_END );

			m_script.Reset();

			m_vm.Shutdown();

			m_sErr = oexT( "" );
			m_sOutput = oexT( "" );
			m_sRoot = oexT( "" );

			m_bLoaded = oex::oexFALSE;
		}

		/// Initializes the virtual machine
		/**
			\param [in] dwMainThreadId  -   The id of the parent thread

			You don't need to call this directly, it is called by Load().
		*/
		oex::oexBOOL CSqEngine::Init()
		{
			Destroy();

			try
			{
				// Initialize the virtual machine
				m_vm.Init();

				// Set compiler error handler
				sq_setcompilererrorhandler( m_vm.GetVMHandle(), &CSqEngine::SqCompilerErrorHandler );

				// Set print function
				sq_setprintfunc( m_vm.GetVMHandle(), CSqEngine::SqPrint );

				// Set run time error handler
		//        sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqErrorHandler, 0 );

				// Set auxiliary error handler
    			sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqAuxErrorHandler, 0 );

				sq_seterrorhandler( m_vm.GetVMHandle() );

				// Bind Squirrel variables
				sqbind::SqBindAll( m_vm );

				// Squirrel must understand CMsgQueue
				SqPlus::SQClassDef< CMsgQueue > ( m_vm, _T( "CMsgQueue" ) )												  ;

				// Define our base class
				SqPlus::SQClassDef< CSqEngineExport > ( m_vm, _T( "CSqEngineExport" ) )
													.func( &CSqEngineExport::alert,             _T( "alert" ) )
													.func( &CSqEngineExport::import,            _T( "import" ) )
													.func( &CSqEngineExport::load_module,       _T( "load_module" ) )
													.func( &CSqEngineExport::sleep,             _T( "sleep" ) )
													.func( &CSqEngineExport::spawn,             _T( "spawn" ) )
													.func( &CSqEngineExport::execute,           _T( "execute" ) )
													.func( &CSqEngineExport::execute1,          _T( "execute1" ) )
													.func( &CSqEngineExport::execute2,          _T( "execute2" ) )
													.func( &CSqEngineExport::execute3,          _T( "execute3" ) )
													.func( &CSqEngineExport::quit,              _T( "quit" ) )
													.func( &CSqEngineExport::queue,             _T( "queue" ) )
													.func( &CSqEngineExport::path,              _T( "path" ) )
												  ;

				// Set base class pointer
				BindRootVariable( (CSqEngineExport*)this, SQEXE_SELF );

				// Allow derived class to register it's stuff
				OnRegisterVariables();

			} // end try

			catch( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			catch( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			m_bLoaded = TRUE;

			return oex::oexTRUE;
		}

		/// Loads a script and prepares it for execution
		/**
			\param [in] pScript     -   The script or a file name.
			\param [in] bFile       -   If non-zero, pScript contains a file name.

		*/
		BOOL Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative = FALSE, oex::oexBOOL bStart = TRUE )
		{
			if ( !oexCHECK_PTR( pScript ) || !*pScript ) 
				return oex::oexFALSE;

			if ( !Init() )
				return oex::oexFALSE;

			try
			{
				// Save away root path
				m_sRoot = oexGetModulePath().Ptr();

				oex::CStr sFull = oexGetModulePath();
				if ( bFile && bRelative )
				{
					// Append path
					//_tcscat( szFull, pScript );
					sFull.BuildPath( pScript );

					// Point to new path
					pScript = sFull.Ptr();

				} // end if

				// Load the script
				m_script = bFile ? m_vm.CompileScript( pScript ) 
	 							 : m_vm.CompileBuffer( pScript );

				if ( bStart )
				{
					// Initialize the script
					m_vm.RunScript( m_script ); 

					// Execute init function
					Execute( SQEXE_FN_INIT );

				} // end if

			} // end try

			catch( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			catch( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			// Save script source information
			m_bFile = bFile;
			m_sScript = pScript;

			return oex::oexTRUE;
		}

		/// Runs the script an executes the initialization function
		/**
			You can just set the bStart flag in Load() to TRUE to avoid
			calling this function if you have no special registrations.
		*/
		BOOL Start()
		{
			if ( !IsScript() )
				return oex::oexFALSE;

			try
			{
				// Initialize the script
				m_vm.RunScript( m_script ); 

				// Execute init function
				Execute( SQEXE_FN_INIT );

			} // end try
			catch( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			catch( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

	    /// Executes the specified buffer
		oex::oexBOOL Run( oex::oexCSTR pScript )
		{
			if ( !oexCHECK_PTR( pScript ) || !*pScript ) 
				return oex::oexFALSE;

			try
			{
				m_vm.RunScript( m_vm.CompileBuffer( pScript ) );

			} // end try

			catch( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			catch( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

		oex::oexINT LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e )
		{	m_sErr = oex::CStr().Fmt( oexT( "%s(%lu)\r\n   %s" ), x_e.sSource.c_str(), x_e.uLine, x_e.sDesc.c_str() ).Ptr();
			return x_nReturn;
		}

	public:

		/// Binds variables to the root table
		template< typename T >
			void BindRootVariable( T *pVar, oex::oexCSTR pName, oex::oexCSTR pImport = NULL, SqPlus::VarAccessType access = SqPlus::VAR_ACCESS_READ_WRITE )
			{   if ( pImport && *pImport ) import( pImport );
				SquirrelObject root = m_vm.GetRootTable();
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
		BOOL Execute( oex::oexCSTR pFunction )
		{
			if ( !IsScript() )
				return oex::oexFALSE;

			if ( !pFunction )
				return oex::oexFALSE;;

			try
			{
				SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), pFunction );            

				if ( f.func.IsNull() )
				{   m_sErr = _T( "Function not found : " ); m_sErr += pFunction;
					return oex::oexFALSE;
				} // end if

				// Call the function
				f();

			} // end try

			catch( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			catch( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;

		}

	private:

		/// Non-zero if a script is loaded
		oex::oexBOOL			m_bLoaded;

		/// Squirrel virtual machine
		SquirrelVM              m_vm;

		/// Squirrel script
		SquirrelObject          m_script;

		/// Last squirrel error
		std::tstring            m_sErr;

		/// Script output
		std::tstring            m_sOutput;

		/// Root path
		std::tstring            m_sRoot;

	};

}; // end namespace
