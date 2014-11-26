/*------------------------------------------------------------------
// http_server.h
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

//==================================================================
// CHttpServer
//
/// Provides basic HTTP server
/**

  \code


  \endcode

*/
//==================================================================

template < typename T_SPORT, typename T_SESSION, typename T_CPORT = T_SPORT > class THttpServer : public CThread
{
public:

	enum
	{
		/// Invalid event
		eSeNone = 0,

		/// Server connected
		eSeConnect = 1,

		/// Server disconnected
		eSeDisconnect = 2,

		/// Server accepted new connection
		eSeAccept = 3,

		/// Cleanup interval in seconds
		eCleanupInterval = 10,

		/// Maximum number of sessions to keep
		///  After this amount is reached, oldest sessions will be dropped
		eMaxSessions = 64,

		/// Maximum socket queue size
		eMaxQueue = 16

	};

	/// Server callback
	typedef oexINT (*PFN_OnServerEvent)( oexPVOID x_pData, oexINT x_nEvent, oexINT x_nErr, THttpServer< T_SPORT, T_SESSION, T_CPORT > *x_pServer );

public:

	/// Holds information about a session
	class CSessionInfo
	{
	public:

		/// Constructor
		CSessionInfo()
		{
			pPortFactory = oexNULL;
			port = oexNULL;
		}

		/// Destructor
		~CSessionInfo()
		{
			if ( pPortFactory && port )
				pPortFactory->Free( port );
			pPortFactory = oexNULL;
			port = oexNULL;
		}

		/// Returns non-zero if we need to keep the connection running
		oex::oexBOOL IsValid()
		{
			// Does it need updating?
			if ( !session.KeepAlive() || !port->IsRunning( 1 ) )
				return oexFALSE;

			return oexTRUE;
		}

		// Updates the connection
		oex::oexRESULT Update( oexUINT x_uTimeout )
		{
			if ( !port )
				return -1;

			// Any data waiting?
			if ( !port->WaitEvent( oex::os::CIpSocket::eReadEvent, x_uTimeout ) )
				return 0;

			// Go process it
			if ( 0 > session.OnRead( 0 ) )
				return -2;

			return 1;
		}

		/// Sets port and free function
		void SetPort( T_CPORT *p, CFactory *f )
		{	port = p; pPortFactory = f; }

		/// Session object
		T_SESSION			session;

		/// Session port
		T_CPORT				*port;

		/// Function that frees the port
		CFactory			*pPortFactory;

	};

	/// Holds information about a session
	class CSessionThread :
		public CSessionInfo,
		public CThread
	{
	public:

		virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData )
		{
			_STT_SET_NAME( oexT( "CSessionThread" ) );
			return oexTRUE;
		}

		virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
		{
			// While thread is running and no transactions
			while ( GetStopEvent().Wait( 0 ) && CSessionInfo::IsValid() )
				if ( 0 > CSessionInfo::Update( 100 ) )
					return oex::oexFALSE;

			return oex::oexFALSE;
		}

	};

	/// Session list type
	typedef TList< CSessionThread > t_LstSessionThread;

	/// Session list type
	typedef TList< CSessionInfo > t_LstSessionInfo;

	/// Processes multiple sessions
	class CSingleSessionThread : public CThread
	{
	public:

		/// Constructor
		CSingleSessionThread()
		{
			m_pSessionInfo = oexNULL;
			m_pSessionLock = oexNULL;
		}

		virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData )
		{
			_STT_SET_NAME( oexT( "CSingleSessionThread" ) );
			return oexTRUE;
		}

		/// Do the work
		virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
		{
			if ( !m_pSessionInfo || !m_pSessionLock )
				return oexFALSE;

			oexUINT uWait = 0;
			while ( GetStopEvent().Wait( 0 ) )
			{
				oexBOOL bUpdate = oexFALSE;

				// +++ Make this a little safer / faster
				/*
					!!! WARNING : Be very careful with this code.

					Since we're in charge of deleting items, and
					the server thread always appends to the end,
					we can safely read up to Size() items without
					locking.  Important to keep things straight here.
				*/

				// Get current size of the list
				oexUINT uSize = m_pSessionInfo->Size();
				oexUINT uProcessed = 0;

				// Anything to do?
				if ( !uSize )
					oexSleep( 15 );

				else
					for ( typename t_LstSessionInfo::iterator it; uSize-- && m_pSessionInfo->Next( it ); )
						if ( it->port->GetConnectState() )
						{	uProcessed++;
							if ( it->IsValid() )
							{
								oexRESULT r = it->Update( uWait );

								if ( 0 > r )
								{	if ( it->port )
										it->port->Destroy();
								} // end if
								else if ( r )
									bUpdate = oexTRUE;

								uWait = 0;

							} // end if
							else if ( it->port )
							{
								if ( 3 > uSize )
								{
									// Must lock if we're close to the end
									oexAutoLock ll( m_pSessionLock );
									if ( !ll.IsLocked() )
										return oexFALSE;

									// Erase item
									it = m_pSessionInfo->Erase( it );

								} // end if

								else
									// Ok to modify this one without locking
									it = m_pSessionInfo->Erase( it );

							} // end if
						} // end if

				// Maybe we had sockets that weren't ready?
				if ( uSize && !uProcessed )
					oexSleep( 15 );

				// Don't hog the processor
				if ( !bUpdate )
					uWait = 15;
				else
					uWait = 0;

			} // end while

			return oex::oexFALSE;
		}

		/// Session information
		t_LstSessionInfo 	*m_pSessionInfo;

		/// Session information lock
		oexLock				*m_pSessionLock;

	};

public:

	/// Default constructor
	THttpServer()
	{
		m_pData = oexNULL;
		m_fnOnServerEvent = oexNULL;
		m_nTransactions = 0;
		m_pSessionData = oexNULL;
		m_pSessionCallback = oexNULL;
		m_pAuthData = oexNULL;
		m_pAuthCallback = oexNULL;
		m_bEnableSessions = oexFALSE;
		m_bLocalOnly = oexFALSE;
		m_uSessionTimeout = 60 * 60;
		m_uCleanup = 0;
		m_bMultiThreaded = oexTRUE;
		m_pPortFactory = oexNULL;
		m_uMaxConnections = eMaxSessions;
		m_uMaxQueue = eMaxQueue;
	}

	~THttpServer()
	{
		// Stop the server
		Stop();

		m_pData = oexNULL;
		m_fnOnServerEvent = oexNULL;
		m_pSessionData = oexNULL;
		m_pSessionCallback = oexNULL;
		m_pAuthData = oexNULL;
		m_pAuthCallback = oexNULL;
		m_bEnableSessions = oexFALSE;
		m_bLocalOnly = oexFALSE;
		m_pPortFactory = oexNULL;
	}

	oexBOOL StartServer( oexINT x_nPort, PFN_OnServerEvent fnOnServerEvent = oexNULL, oexPVOID x_pData = oexNULL )
	{
		Destroy();

		m_nPort = x_nPort;
		m_pData = x_pData;
		m_fnOnServerEvent = fnOnServerEvent;

		// Ensure we have a server id
		if ( !m_sServerId.Length() )
			m_sServerId = oexStrToMb( oexGuidToString() );

		return 0 == CThread::Start();
	}

	void SetServerId( oexCSTR8 x_pServerId )
	{
		if ( x_pServerId )
			m_sServerId = x_pServerId;
		else
			m_sServerId.Destroy();
	}

	CStr8& GetServerId() { return m_sServerId; }

	void SetPortFactory( CFactory *p )
	{	m_pPortFactory = p; }

	CFactory* GetPortFactory()
	{ return m_pPortFactory; }

protected:

	oexBOOL ThreadStartServer()
	{
		// Bind to port
		if ( !m_server.Bind( m_nPort ) )
		{	if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeConnect, -1, this );
			return oexFALSE;
		} // end if

		// Listen
		if ( !m_server.Listen( m_uMaxQueue ) )
		{	if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeConnect, -2, this );
			return oexFALSE;
		} // end if

		// Notify that server is running
		if ( m_fnOnServerEvent )
			m_fnOnServerEvent( m_pData, eSeConnect, 0, this );

		return oexTRUE;
	}

	virtual oexBOOL InitThread( oex::oexPVOID x_pData )
	{
		_STT_SET_NAME( oexT( "THttpServer" ) );

		// Attempt to start the server
		if ( !ThreadStartServer() )
			return oexFALSE;

		return oexTRUE;
	}

	/// Authenticate the connection
	virtual oexBOOL OnAuthenticate( T_CPORT &port )
	{
		// Accepting anyone?
		if ( !m_bLocalOnly )
			return oexTRUE;

		// Get local and remote address
		CStr8 sLocal = oexStrToMb( port.LocalAddress().GetDotAddress() );
		CStr8 sRemote = oexStrToMb( port.PeerAddress().GetDotAddress() );

		// Verify it is a local address
		if ( sLocal != sRemote && sRemote != "127.0.0.1" )
			return oexFALSE;

		return oexTRUE;
	}

	oexBOOL InformSession( T_SESSION &session, T_CPORT &port )
	{
		// Initialize the session
		session.Init();

		// Count a transaction
		session.SetTransactionId( m_nTransactions++ );

		// Set the log file name
		session.SetLogFile( m_sLog.Ptr() );

		// Set the callback function for the data
		session.SetCallback( m_pSessionCallback, m_pSessionData );

		// Set the authentication callback
		session.SetAuthCallback( m_pAuthCallback, m_pAuthData );

		// Connect the port
		session.SetPort( &port );

		// Set default session timeout
		session.SetSessionTimeout( m_uSessionTimeout );

		// Let the session know the server id
		session.SetServerId( m_sServerId );

		// Mapped folders
		session.SetMappedFoldersList( &m_pbMappedFolders, &m_lockMappedFolders );

		// Copy the default headers to the session
		session.DefHeaders() = m_pbDefHeaders.Copy();
		
		// Enable sessions?
		if ( m_bEnableSessions )
			session.SetSessionObject( &m_pbSession, &m_lockSession );

		return oexTRUE;
	}

	/// Accepts multi-threaded connection
	oexBOOL MultiAccept()
	{
		// Allocate a new port object
		T_CPORT *port = oexNULL;
		if ( m_pPortFactory )
			port = (T_CPORT*)m_pPortFactory->Create();
		else
			port = (T_CPORT*)m_cDefaultPortFactory.Create();
		if ( !port )
			return oexFALSE;

		// Add a new session
		typename THttpServer::t_LstSessionThread::iterator it = m_lstSessionThread.Append();

		// Set the port
		it->SetPort( port, m_pPortFactory ? m_pPortFactory : &m_cDefaultPortFactory );

		// Attempt to connect session
		if ( !m_server.Accept( *it->port ) )
		{
			// Erase session
			m_lstSessionThread.Erase( it );

			// Let user in on the error
			if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

		} // end if

		else
		{
			// Erase session
			if ( m_lstSessionThread.Size() >= m_uMaxConnections )
				m_lstSessionThread.Erase( it );

			// Authenticate the connection
			else if ( !OnAuthenticate( *it->port ) )
			{
				m_lstSessionThread.Erase( it );

				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

			} // end if

			else
			{
				// Fill in session information
				InformSession( it->session, *it->port );

				// Notify of new connection
				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

				// Start the thread
				it->Start();

			} // end if

		} // end else

		return oexTRUE;
	}

	/// Accepts single-threaded connection
	oexBOOL SingleAccept()
	{
		// Allocate a new port object
		T_CPORT *port = oexNULL;
		if ( m_pPortFactory )
			port = (T_CPORT*)m_pPortFactory->Create();
		else
			port = (T_CPORT*)m_cDefaultPortFactory.Create();
		if ( !port )
			return oexFALSE;

		// Must lock to make changes
		oexAutoLock ll( m_lockSessionInfo );
		if ( !ll.IsLocked() )
			return oexFALSE;

		// Add a new session
		typename THttpServer::t_LstSessionInfo::iterator it = m_lstSessionInfo.Append();

		// Set the port
		it->SetPort( port, m_pPortFactory ? m_pPortFactory : &m_cDefaultPortFactory );

		// Attempt to connect session
		if ( !m_server.Accept( *it->port ) )
		{
			// Erase session
			m_lstSessionInfo.Erase( it );

			// Let user in on the error
			if ( m_fnOnServerEvent )
				m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

		} // end if

		else
		{
			// Erase session
			if ( m_lstSessionInfo.Size() >= m_uMaxConnections )
				m_lstSessionInfo.Erase( it );

			// Authenticate the connection
			else if ( !OnAuthenticate( *it->port ) )
			{
				m_lstSessionInfo.Erase( it );

				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, -1, this );

			} // end if

			else
			{
				// Fill in session information
				InformSession( it->session, *it->port );

				// Notify of new connection
				if ( m_fnOnServerEvent )
					m_fnOnServerEvent( m_pData, eSeAccept, 0, this );

			} // end if

		} // end else

		// Start the processing thread if needed
		if ( !m_cSingleSessionThread.IsRunning() )
		{
			// Some info the thread will need
			m_cSingleSessionThread.m_pSessionInfo = &m_lstSessionInfo;
			m_cSingleSessionThread.m_pSessionLock = &m_lockSession;

			// Start the thread
			m_cSingleSessionThread.Start();

		} // end if

		return oexTRUE;
	}

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		// Wait for stop event
		while ( GetStopEvent().Wait( 0 ) )
		{
			// Wait for connect event
			if ( m_server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, 100 ) )
			{
				if ( m_bMultiThreaded )
					MultiAccept();
				else
					SingleAccept();

			} // end if

			// Did we lose the connection?
			else if ( m_server.IsError() )
			{
				// Drop old socket
				m_server.Destroy();

				// Wait a bit
				oexSleep( 3000 );

				// Restart the server
				ThreadStartServer();

			} // end else

			// Clean up expired connections
			CleanupConnections();

			// Is it time to cleanup sessions?
			if ( m_uCleanup )
				m_uCleanup--;
			else
				CleanupSessions();

		} // end while

		return oexFALSE;
	}

	oexBOOL CleanupConnections()
	{
		// Check for expired connections
		if ( m_lstSessionThread.Size() )
			for ( typename t_LstSessionThread::iterator it; m_lstSessionThread.Next( it ); )
				if ( !it->IsRunning() )
					it = m_lstSessionThread.Erase( it );

		return oexTRUE;
	}

	oexBOOL CleanupSessions()
	{
//		oexEcho( "Cleaning up..." );
		m_uCleanup = eCleanupInterval * 10;

		// Attempt to cleanup session data
		oexAutoLock ll( m_lockSession );
		if ( ll.IsLocked() )
		{
			// +++ This gets us by, but it would be nice to drop the oldest
			//     connections based on _ts.  Currently just dropping anything
			//     that hasn't communicated in 30 seconds when we're over the limit.

			// Do we need to drop sessions?
			oexBOOL bDrop = eMaxSessions < m_pbSession.Size();

			// Remove timed out sessions
			oexUINT ts = (oexUINT)oexGetUnixTime();
			for ( CPropertyBag8::iterator it; m_pbSession.List().Next( it ); )
				if ( !it->IsKey( "_ts" )
					 || ( it.Obj()[ "_ts" ].ToULong() + m_uSessionTimeout ) < ts
					 || ( bDrop && ( it.Obj()[ "_ts" ].ToULong() + 30 ) < ts )
				   )
				{
//					oexEcho( oexMks( oexT( "Erasing session : " ),
//									 ts, oexT( " : " ),
//									 m_uSessionTimeout, oexNL,
//									 it->PrintR() ).Ptr() );
					it = m_pbSession.List().Erase( it );

				} // end if

		} // end if

		return oexTRUE;
	}

	virtual oexINT EndThread( oex::oexPVOID x_pData )
	{
		// Ensure the session thread has stopped
		m_cSingleSessionThread.Stop();

		// Stop the server
		m_server.Destroy();

		// Lose all sessions
		m_lstSessionThread.Destroy();

		// Lose session info objects
		m_lstSessionInfo.Destroy();

		// Notify that server is running
		if ( m_fnOnServerEvent )
			m_fnOnServerEvent( m_pData, eSeDisconnect, 0, this );

		return oexTRUE;
	}

public:

	/// Returns the number of client transactions serviced
	oexINT GetNumTransactions()
	{	return m_nTransactions; }

	/// Sets the session callback function
	void SetSessionCallback( oexPVOID x_pCallback, oexPVOID x_pData )
	{	m_pSessionCallback = x_pCallback; m_pSessionData = x_pData; }

	/// Sets the session callback function
	void SetAuthCallback( oexPVOID x_pCallback, oexPVOID x_pData )
	{	m_pAuthCallback = x_pCallback; m_pAuthData = x_pData; }

	/// Returns the number of active sessions
	oexINT GetNumActiveClients()
	{	return m_lstSessionThread.Size() + m_lstSessionInfo.Size(); }

	/// Sets the log file name
	oexBOOL SetLogFile( oexCSTR x_pLog )
	{	m_sLog = x_pLog;
		return oexTRUE;
	}

	/// Returns reference to port object
	T_SPORT& Port() { return m_server; }

	/// Enable / disable sessions
	void EnableSessions( oexBOOL b ) { m_bEnableSessions = b; }

	/// Enable / disable remote connections
	void EnableRemoteConnections( oexBOOL b ) { m_bLocalOnly = !b; }

	/// Sets the length of time that session data is to be valid
	void SetSessionTimeout( oexUINT uTo )
	{	m_uSessionTimeout = uTo; }

	/// Enable / disable multi threading
	//  For some reason, I enable doing this
	//  while the server is running ;)
	void EnableMultiThreading( oexBOOL b )
	{	m_bMultiThreaded = b; }

	/// Maps / unmaps a folder
	oexBOOL MapFolder( oexCSTR pName, oexCSTR pFolder, oexBOOL bMap )
	{
		if ( !pName || !*pName )
			return oexFALSE;

		if ( !pFolder || !*pFolder )
			return oexFALSE;

		oexAutoLock ll( m_lockMappedFolders );
		if ( !ll.IsLocked() )
			return oexFALSE;

		if ( bMap )
			m_pbMappedFolders[ pName ][ pFolder ] = 1;
		else
		{	m_pbMappedFolders[ pName ].Unset( pFolder );
			if ( !m_pbMappedFolders[ pName ].Size() )
				m_pbMappedFolders = 0;
		} // end else

		return oexTRUE;
	}

	/// Sets the maximum number of connections
	void setMaxConn( oexUINT m ) { m_uMaxConnections = m; }

	/// Gets the maximum number of connections
	oexUINT getMaxConn() { return m_uMaxConnections; }

	/// Sets the maximum connection queue size
	void setMaxQueue( oexUINT m ) { m_uMaxQueue = m; }

	/// Gets the maximum connection queue size
	oexUINT getMaxQueue() { return m_uMaxQueue; }

	/// Returns a reference to the default server headers
	CPropertyBag8& DefHeaders() { return m_pbDefHeaders; }
	
	/// Enables / disables compression
	int EnableCompression( oexBOOL b )
	{
#ifdef OEX_ENABLE_ZIP
		m_bEnableCompression = b;
#else
		m_bEnableCompression = oexFALSE;
#endif
		return m_bEnableCompression;
	}

	/// Returns non-zero if compression is enabled
	oexBOOL IsCompressionEnabled()
	{	return m_bEnableCompression; }
	
private:

	/// The TCP port to listen
	oexINT						m_nPort;

	/// Server port
	T_SPORT						m_server;

	/// Max connections
	oexUINT						m_uMaxConnections;

	/// Max connection queue
	oexUINT						m_uMaxQueue;

	/// Transactions
	oexLONG						m_nTransactions;

	/// List of session thread objects (multi-threaded)
	t_LstSessionThread			m_lstSessionThread;

	/// List of session info objects (single-threaded)
	t_LstSessionInfo			m_lstSessionInfo;

	/// Runs single threaded sessions
	CSingleSessionThread		m_cSingleSessionThread;

	/// Data passed to m_fnOnServerEvent
	oexPVOID					m_pData;

	/// Function to call on server event
	PFN_OnServerEvent			m_fnOnServerEvent;

	/// Data passed to session callback
	oexPVOID					m_pSessionData;

	/// Pointer to session callback function
	oexPVOID					m_pSessionCallback;

	/// Data passed to session callback
	oexPVOID					m_pAuthData;

	/// Pointer to session callback function
	oexPVOID					m_pAuthCallback;

	/// Log file name
	CStr						m_sLog;

	/// Enable sessions
	oexBOOL						m_bEnableSessions;
	
	/// Non-zero to enable compression
	oexBOOL						m_bEnableCompression;

	/// Unique server id
	CStr8						m_sServerId;

	/// Stores session data
	CPropertyBag8				m_pbSession;

	/// Lock for session data access
	CLock						m_lockSession;

	/// Non-zero to accept local connections only
	oexBOOL						m_bLocalOnly;

	/// Length of time in seconds that session data is to be valid
	oexUINT						m_uSessionTimeout;

	/// Time to cleanup sessions;
	oexUINT						m_uCleanup;

	/// Non-zero to enable multi-threading
	oexBOOL						m_bMultiThreaded;

	/// Session info lock
	oexLock						m_lockSessionInfo;

	/// Lock for the mapped folders list
	oexLock						m_lockMappedFolders;

	/// List of mapped folders
	CPropertyBag				m_pbMappedFolders;

    /// Default outgoing HTTP headers
    CPropertyBag8     		  	m_pbDefHeaders;

	/// Manages port creation / destruction
	CFactory					*m_pPortFactory;

	/// Default port factory
	TFactory< T_CPORT >			m_cDefaultPortFactory;
};
