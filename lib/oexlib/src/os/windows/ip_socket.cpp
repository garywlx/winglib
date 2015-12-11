/*------------------------------------------------------------------
// ip_socket.cpp
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

#include "oexlib.h"
#include "std_os.h"

#include <ws2tcpip.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// A few verifications
oexSTATIC_ASSERT( sizeof( CIpSocket::t_SOCKET ) == sizeof( SOCKET ) );

#if !defined( OEX_NOSOCKET2 )
oexSTATIC_ASSERT( sizeof( CIpSocket::t_SOCKETEVENT ) == sizeof( WSAEVENT ) );
#endif

// Socket version we will use
oexCONST WORD c_MinSocketVersion = MAKEWORD( 2, 2 );

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

//#if !defined( OEX_NOSOCKET2 )

//==============================================================
// CIpSocket_GetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address structure to be filled in.
	\param [in] x_pSai 	-	Address information
*/
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, SOCKADDR_IN *x_pSai )
{_STT();
	// Verify pointers
	if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
		return oexFALSE;

	// Sets the raw address value
	x_pIa->SetRawAddress( ntohl( *(DWORD*)&x_pSai->sin_addr.S_un.S_addr ), ntohs( x_pSai->sin_port ) );

	return oexTRUE;
}

//==============================================================
// CIpSocket_GetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address structure to be filled in.
	\param [in] x_pSa 	-	Address information
*/
static oexBOOL CIpSocket_GetAddressInfo( CIpAddress *x_pIa, SOCKADDR *x_pSa )
{_STT();
	return CIpSocket_GetAddressInfo( x_pIa, (SOCKADDR_IN*)x_pSa );
}

//==============================================================
// CIpSocket_SetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address information
	\param [in] x_pSai 	-	Structure to be filled in
*/
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, SOCKADDR_IN *x_pSai )
{_STT();
	// Verify pointers
	if ( !oexVERIFY_PTR( x_pSai ) || !oexVERIFY_PTR( x_pIa ) )
		return oexFALSE;

	// Set the ip address
	*(DWORD*)&x_pSai->sin_addr.s_addr = htonl( (DWORD)x_pIa->GetIpv4() );

	// Set the port
	x_pSai->sin_port = htons( (short)x_pIa->GetPort() );

	return oexTRUE;
}

//==============================================================
// CIpSocket_SetAddressInfo()
//==============================================================
/// Copies the specified address
/**
	\param [in] x_pIa	-	Address information
	\param [in] x_pSai 	-	Structure to be filled in
* /
/ *
static oexBOOL CIpSocket_SetAddressInfo( CIpAddress *x_pIa, SOCKADDR *x_pSa )
{_STT();
	return CIpSocket_GetAddressInfo( x_pIa, (SOCKADDR_IN*)x_pSa );
}
*/

//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexLONG CIpSocket::m_lInit = -1;
static oexLONG g_CIpSocket_lInitCount = 0;

oexCONST CIpSocket::t_SOCKET CIpSocket::c_InvalidSocket = (CIpSocket::t_SOCKET)INVALID_SOCKET;
#if !defined( OEX_NOSOCKET2 )
oexCONST CIpSocket::t_SOCKETEVENT CIpSocket::c_InvalidEvent = (CIpSocket::t_SOCKETEVENT)WSA_INVALID_EVENT;
#else
oexCONST CIpSocket::t_SOCKETEVENT CIpSocket::c_InvalidEvent = (CIpSocket::t_SOCKETEVENT)SOCKET_ERROR;
#endif
oexCONST CIpSocket::t_SOCKET CIpSocket::c_SocketError = (CIpSocket::t_SOCKET)SOCKET_ERROR;


void CIpSocket::Construct()
{
	// Initialize socket library
	InitSockets();

	m_uActivityTimeout = eActivityTimeout;

	m_hSocket = c_InvalidSocket;
	
	m_hSocketEvent = c_InvalidEvent;

	m_uLastError = 0;

	m_uConnectState = 0;

	m_uEventState = 0;
	os::CSys::Zero( &m_uEventStatus, sizeof( m_uEventStatus ) );

	m_uSocketFamily = 0;
	m_uSocketType = 0;
	m_uSocketProtocol = 0;

	m_uReads = 0;
	m_uWrites = 0;
	m_uAccepts = 0;

	m_uFlags = 0;

	m_bFree = oexTRUE;

	m_bHookEvents = oexTRUE;
}

CIpSocket::CIpSocket()
{_STT();

	// Construct class
	Construct();
}

CIpSocket::CIpSocket( t_SOCKET hSocket, oexBOOL x_bFree )
{_STT();

	// Construct class
	Construct();

	// Attach to specified socket
	Attach( hSocket, x_bFree );
}

CIpSocket::~CIpSocket()
{_STT();

	// Lose the current socket
	Destroy();

	// Uninitialize socket library
	UninitSockets();
}

oexBOOL CIpSocket::InitSockets()
{_STT();
	// Add ref
	if ( 1 == oexIncrement( &g_CIpSocket_lInitCount ) )
	{
		// Quit if already initialized
		if ( m_lInit == 0 )
			return oexTRUE;

		WSADATA wd;

		// Attempt to initialize the Socket library
		m_lInit = WSAStartup( c_MinSocketVersion, &wd );

	} // end if

	return IsInitialized();
}

void CIpSocket::UninitSockets()
{_STT();
	// Deref
	if ( oexDecrement( &g_CIpSocket_lInitCount ) )
		return;

	// Punt if not initialized
	if ( !IsInitialized() )
		return;

	// Not initialized
	m_lInit	= -1;

#if !defined( OEX_NOSOCKET2 )
	// Clean up socket lib
	WSACleanup();
#endif
}

oexLONG CIpSocket::GetInitCount()
{_STT();
	return 0;
}

CIpSocket::t_SOCKET CIpSocket::Detach()
{   
	// Save away the socket handle
	t_SOCKET hSocket = m_hSocket;

	// Ditch the event handle
	CloseEventHandle();

	// We won't be freeing the socket
	m_hSocket = c_InvalidSocket;

	// Free whatever else is left
	Destroy();

	// It's the callers problem now
	return hSocket;
}


void CIpSocket::Destroy()
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return;

	// Let everyone know we're closing
	if ( c_InvalidSocket != m_hSocket )
		OnClose();

	// Ditch the event handle
	CloseEventHandle();

	// Save socket pointer to socket
	SOCKET hSocket = m_bFree ? (SOCKET)m_hSocket : INVALID_SOCKET;

	// Ditch member variable
	m_hSocket = c_InvalidSocket;

	// Invalidate member variables
	m_sConnect.Destroy();
	m_addrPeer.Destroy();
	m_addrLocal.Destroy();

	m_uConnectState = 0;
	m_uEventState = 0;
	oexZero( m_uEventStatus );

	m_uSocketFamily = 0;
	m_uSocketType = 0;
	m_uSocketProtocol = 0;

	m_uReads = 0;
	m_uWrites = 0;
	m_uAccepts = 0;
	m_uFlags = 0;

	m_bFree = oexTRUE;

	// Reset activity timeout
	m_toActivity.Clear();

	// Ensure valid socket handle
	if ( INVALID_SOCKET == hSocket )
		return;
	
	if ( IsInitialized() )
	{
		// Turn off non-blocking
//		int flags = fcntl( oexPtrToInt( hSocket ), F_GETFL, 0 );
//		fcntl( oexPtrToInt( hSocket ), F_SETFL, flags & ~O_NONBLOCK );

		struct linger lopt;

//		lopt.l_onoff = 0; lopt.l_linger = 0;
//		if ( SOCKET_ERROR == setsockopt( hSocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&lopt, sizeof( lopt ) ) )

//		lopt.l_onoff = 1; lopt.l_linger = 60;
//		if ( SOCKET_ERROR == setsockopt( hSocket, SOL_SOCKET, SO_LINGER, (const char*)&lopt, sizeof( lopt ) ) )

		lopt.l_onoff = 0; lopt.l_linger = 0;
		if ( SOCKET_ERROR == setsockopt( hSocket, SOL_SOCKET, SO_LINGER, (const char*)&lopt, sizeof( lopt ) ) )
		{	m_uLastError = WSAGetLastError();
//			oexERROR( m_uLastError, oexT( "setsockopt() failed" ) );
		} // end if

		// Shutdown the socket
//		if ( SOCKET_ERROR == shutdown( hSocket, SD_BOTH ) )
//		{	m_uLastError = WSAGetLastError();
//			oexERROR( m_uLastError, oexT( "shutdown() failed" ) );
//		} // end if

		// Close the socket
		if ( SOCKET_ERROR == closesocket( hSocket ) )
		{	m_uLastError = WSAGetLastError();
//			oexERROR( m_uLastError, oexT( "close() failed" ) );
		} // end if

	} // end if
/*

	// Start socket shutdown
	shutdown( hSocket, SD_SEND );

	// Wait on graceful shutdown
	int ret; char buf[ 1024 ];
	do { ret = recv( hSocket, buf, sizeof( buf ), 0 );
	} while ( 0 < ret );

	// Close the socket
	closesocket( hSocket );

	// Save the last error code
	m_uLastError = WSAGetLastError();
*/
}

oexBOOL CIpSocket::Shutdown()
{_STT();
	if ( c_InvalidSocket == m_hSocket )
		return oexFALSE;

	// Shut down the socket
	shutdown( (SOCKET)m_hSocket, SD_SEND );

	return oexTRUE;
}


oexBOOL CIpSocket::Create( oexINT x_af, oexINT x_type, oexINT x_protocol )
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return FALSE;

	// Close any open socket
	Destroy();

	// Clear errors
	m_uLastError = 0;

	// Create a scocket
	m_hSocket = (t_SOCKET)socket( (int)x_af, (int)x_type, (int)x_protocol );

	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0;

	// Was there an error?
	if ( c_InvalidSocket == m_hSocket )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	// Setup socket timeout defaults

	// Set the recv and send timeouts
	int nMs = oexDEFAULT_WAIT_TIMEOUT;
	setsockopt( (SOCKET)m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nMs, sizeof( nMs ) );
	setsockopt( (SOCKET)m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&nMs, sizeof( nMs ) );

	// Process socket creation
	if ( !OnAttach() )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Save socket connect params
	m_uSocketFamily = x_af;
	m_uSocketType = x_type;
	m_uSocketProtocol = x_protocol;

	// Capture all events
	if ( m_bHookEvents )
		EventSelect();

	return IsSocket();
}

int CIpSocket::setsockint( int optname, int opt )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Create socket if there is none
	if ( !IsSocket() )
		return oexFALSE;
	
	// Set socket int
	return setsockopt( (SOCKET)m_hSocket, SOL_SOCKET, optname, (const char *)&opt, sizeof( opt ) );
}

oexBOOL CIpSocket::Bind( oexUINT x_uPort )
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return FALSE;

	// Create socket if there is none
	if ( !IsSocket() && !Create() )
	{	Destroy();
		m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	sockaddr_in sai;
	ZeroMemory( &sai, sizeof( sai ) );
	sai.sin_family = PF_INET;
	sai.sin_port = htons( (WORD)x_uPort );

	// Attempt to bind the socket
	int nRet = bind( (SOCKET)m_hSocket, (sockaddr*)&sai, sizeof( sockaddr_in ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0, nRet = 0;

	// Grab the address
	CIpSocket_GetAddressInfo( &m_addrLocal, &sai );

	if ( nRet )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CIpSocket::BindTo( oexCSTR x_pAddress, oexUINT x_uPort )
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Create socket if there is none
	if ( !IsSocket() && !Create() )
	{	Destroy();
		m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if
	
	// Save address type
	if ( x_pAddress && *x_pAddress )
		m_addrPeer.SetDotAddress( x_pAddress, x_uPort );
	
	sockaddr_in sai;
	ZeroMemory( &sai, sizeof( sai ) );
	sai.sin_family = PF_INET;
	sai.sin_port = htons( (WORD)x_uPort );
	
	if ( x_pAddress && *x_pAddress )
		sai.sin_addr.s_addr = inet_addr( oexStrToMbPtr( x_pAddress ) );
	else
		sai.sin_addr.s_addr = htonl( INADDR_ANY );

	// Attempt to bind the socket
	int nRet = bind( (SOCKET)m_hSocket, (sockaddr*)&sai, sizeof( sockaddr_in ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0, nRet = 0;

	// Grab the address
	CIpSocket_GetAddressInfo( &m_addrLocal, &sai );

	if ( nRet )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	return oexTRUE;
}

/*
	struct ip_mreq_source imr;
	ZeroMemory( &imr, sizeof( imr ) );	
	imr.imr_interface.s_addr = htonl( INADDR_ANY );
	
    setsockopt( (SOCKET)m_hSocket, IPPROTO_IP, IP_MULTICAST_IF,
				(char *)&imr.imr_interface.s_addr,
				sizeof( imr.imr_interface.s_addr ) );	

*/


// +++ Add IP_DROP_MEMBERSHIP

oexBOOL CIpSocket::AddMulticastAddr( oexCSTR x_pAddress, oexCSTR x_pAdapter )
{
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Create socket if there is none
	if ( !IsSocket())
		return oexFALSE;

	struct ip_mreq_source imr;
	ZeroMemory( &imr, sizeof( imr ) );	

	// Set address
	if ( x_pAddress && *x_pAddress )
		imr.imr_multiaddr.s_addr = inet_addr( oexStrToMbPtr( x_pAddress ) );
	else
		imr.imr_multiaddr.s_addr = htonl( INADDR_ANY );
		
	// Set adapter
	if ( x_pAdapter && *x_pAdapter )
		imr.imr_interface.s_addr = inet_addr( oexStrToMbPtr( x_pAdapter ) );
	else
		imr.imr_interface.s_addr = htonl( INADDR_ANY );

	int nRet = setsockopt( (SOCKET)m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&imr, sizeof( imr ) );
	
	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0, nRet = 0;

	if ( nRet )
		return oexFALSE;
	
	return oexTRUE;
}

oexBOOL CIpSocket::Listen( oexUINT x_uMaxConnections )
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return FALSE;

	// Must have socket
	if ( !IsSocket() )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	// Valid number of connections?
	if ( x_uMaxConnections == 0 )
		x_uMaxConnections = 16;
//		x_uMaxConnections = SOMAXCONN;

	// Start the socket listening
	int nRet = listen( (SOCKET)m_hSocket, (int)( x_uMaxConnections ? x_uMaxConnections : SOMAXCONN ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0, nRet = 0;

	// Error?
	if ( c_SocketError == (t_SOCKET)nRet )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	// We're trying to connect
	m_toActivity.SetMs( m_uActivityTimeout );
	m_uConnectState |= eCsConnecting;

	// Return the result
	return oexTRUE;
}

oexBOOL CIpSocket::Connect( CIpAddress &x_rIpAddress )
{_STT();
	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	// Ensure we were passed a valid address
	if ( !x_rIpAddress.ValidateAddress() )
	{	Destroy(); return oexFALSE; }

	// Create socket if there is none
	if ( !IsSocket() && !Create() )
	{	m_uConnectState |= eCsError;
		Destroy();
		return oexFALSE;
	} // end if

	// Save the address
	m_addrPeer = x_rIpAddress;

	SOCKADDR_IN si;
	os::CSys::Zero( &si, sizeof( si ) );
	si.sin_family = m_uSocketFamily;
	CIpSocket_SetAddressInfo( &m_addrPeer, &si );

	// Attempt to connect
	int nRet = connect( (SOCKET)m_hSocket, (PSOCKADDR)&si, sizeof( si ) );

	// Save the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0, nRet = 0;

	// Check for error
	if ( nRet )
	{	m_uConnectState |= eCsError;
		return oexFALSE;
	} // end if

	// We're trying to connect
	m_toActivity.SetMs( m_uActivityTimeout );
	m_uConnectState |= eCsConnecting;

	// Return the result
	return oexTRUE;
}

oexBOOL CIpSocket::Connect( oexCSTR x_pAddress, oexUINT x_uPort)
{_STT();
	if ( !oexVERIFY_PTR( x_pAddress ) )
		return oexFALSE;

	// Punt if not initialized
	if ( !IsInitialized() )
		return oexFALSE;

	CIpAddress addr;

	// Were we passed a URL?
	if ( !x_uPort && !addr.LookupUrl( x_pAddress ) )
		return oexFALSE;

	// Lookup the host address
	else if ( !addr.LookupHost( x_pAddress, x_uPort ) )
		return oexFALSE;

	return Connect( addr );
}

oexBOOL CIpSocket::Attach( t_SOCKET x_hSocket, oexBOOL x_bFree )
{_STT();
	// Lose the old socket
    Destroy();

	// Save socket handle
    m_hSocket = x_hSocket;
	
	// Should we free the socket?
	m_bFree = x_bFree;

	// Call on attach
	if ( !OnAttach() )
		m_hSocket = c_InvalidSocket;

	// How'd it go?
    return IsSocket();
}

oexBOOL CIpSocket::Accept( CIpSocket &x_is )
{_STT();

	// Punt if no socket
	if ( !IsSocket() )
		return FALSE;

	// Lose any current connection there might be
	x_is.Destroy();

	// Accept the connection
	SOCKADDR saAddr = { 0 }; int iAddr = sizeof( saAddr );

	// Accept and encapsulate the socket
	BOOL bSuccess = x_is.Attach( (t_SOCKET)accept( (SOCKET)m_hSocket, &saAddr, &iAddr ) );

	// Check for error
	if ( !bSuccess )
	{   m_uLastError = WSAGetLastError();
		return FALSE;
	} // end if

	// Grab the address
	CIpSocket_GetAddressInfo( &m_addrPeer, &saAddr );

	// Capture all events
	if ( x_is.m_bHookEvents )
		x_is.EventSelect();

	m_uAccepts++;
	m_toActivity.SetMs( m_uActivityTimeout );

	// Child is connecting
	x_is.m_toActivity.SetMs( m_uActivityTimeout );
	x_is.m_uConnectState |= eCsConnecting;

	return oexTRUE;
}

oexBOOL CIpSocket::CreateEventHandle()
{_STT();
#if defined( OEX_NOSOCKET2 )
	return oexFALSE;
#else
	// Check for event handle
	if ( IsEventHandle() )
		return oexTRUE;

	// Create socket event
	m_hSocketEvent = WSACreateEvent();

	// How did it turn out?
	return c_InvalidEvent != m_hSocketEvent;
#endif
}

void CIpSocket::CloseEventHandle()
{_STT();
#if defined( OEX_NOSOCKET2 )
	return;
#else

	// Do we have a valid handle?
	if ( c_InvalidEvent != m_hSocketEvent )
	{
		// Blocking socket
		if ( c_InvalidSocket != m_hSocket )
		{
			WSAEventSelect( (SOCKET)m_hSocket, (WSAEVENT)m_hSocketEvent, 0 );
			WSAEventSelect( (SOCKET)m_hSocket, (WSAEVENT)0, 0 );
			unsigned long l = 0; ioctlsocket( (SOCKET)m_hSocket, FIONBIO, &l );

			// Set the recv and send timeouts
			int nMs = oexDEFAULT_WAIT_TIMEOUT;
			setsockopt( (SOCKET)m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nMs, sizeof( nMs ) );
			setsockopt( (SOCKET)m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&nMs, sizeof( nMs ) );

		} // end if

		// Close the event handle
		WSACloseEvent( m_hSocketEvent );

		// Invalid handle value
		m_hSocketEvent = c_InvalidEvent;

	} // end if
#endif
}

oexBOOL CIpSocket::EventSelect( oexLONG x_lEvents )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return oexFALSE;
#else
	// Punt if no socket
	if ( !IsSocket() )
		return FALSE;

	// Must have event handle
	if ( !IsEventHandle() )
		CreateEventHandle();

	return ( WSAEventSelect( (SOCKET)m_hSocket, (WSAEVENT)m_hSocketEvent, x_lEvents ) == 0 );
#endif
}

oexUINT CIpSocket::WaitEvent( oexLONG x_lEventId, oexUINT x_uTimeout )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return 0;
#else
	// Must have a socket handle
	if ( !IsSocket() )
		return 0;

	// Must have event handle
	if ( !IsEventHandle() )
	{   if ( !CreateEventHandle() || !EventSelect() )
			return 0;
	} // end if

	// Save start time
	UINT uEnd = GetTickCount() + x_uTimeout;
	for( ; ; )
	{
		// What's the event state
		if ( 0 == ( m_uEventState & x_lEventId ) )
		{
			if ( x_uTimeout )
			{
				// Wait for event
//				UINT uRet =
					WaitForSingleObject( m_hSocketEvent, x_uTimeout );

				// Check for timeout or error
//		        if ( uRet != WAIT_OBJECT_0 )
//                   return 0;

			} // end if

			// Reset the network event
			ResetEvent( m_hSocketEvent );

			// Get network events
			WSANETWORKEVENTS wne; os::CSys::Zero( &wne, sizeof( wne ) );
			if ( SOCKET_ERROR == WSAEnumNetworkEvents( (SOCKET)m_hSocket, m_hSocketEvent, &wne ) )
			{	oexERROR( WSAGetLastError(), oexT( "WSAEnumNetworkEvents() failed" ) );
				return 0;
			} // end if

			// Save the status of all events
			for ( oexUINT uMask = 1; uMask && uMask <= eAllEvents; uMask <<= 1 )
				if ( wne.lNetworkEvents & uMask )
				{
					// Get bit offset
					oexUINT uOffset = GetEventBit( uMask );

					// Save the event info
					m_uEventState |= uMask;
					m_uEventStatus[ uOffset ] = wne.iErrorCode[ uOffset ];

					// Signal activity
					m_uConnectState |= eCsActivity;

					// Signal when we get a connect message
					if ( 0 != ( FD_CONNECT & wne.lNetworkEvents ) )
					{
						// Set connected status
						if ( !m_uEventStatus[ FD_CONNECT_BIT ] )
							m_uConnectState |= eCsConnected;

						// Handle connect error
						else
							m_uLastError = m_uEventStatus[ FD_CONNECT_BIT ],
							m_uConnectState &= ~( eCsConnected | eCsActivity );

						// Connecting process is over
						m_uConnectState &= ~eCsConnecting;

					} // end if

					// Check for socket close
					if ( 0 != ( FD_CLOSE & wne.lNetworkEvents ) )
						m_uConnectState &= ~( eCsConnected | eCsActivity | eCsConnecting );

				} // end if

			// !!!  Kludge around missing connect message
			//      If we're accepting a connection, we don't
			//      get a connect message.  ( At least on Windows )
			if ( !( m_uConnectState & 2 ) && ( m_uConnectState & 1 ) )
			{	m_uConnectState |= eCsConnected;
				m_uConnectState &= ~eCsConnecting;
				wne.lNetworkEvents |= FD_CONNECT;
				m_uEventState |= FD_CONNECT;
				m_uEventStatus[ FD_CONNECT_BIT ] = 0;
			} // end if

		} // end if

		// Did our event go off?
		if ( 0 != ( m_uEventState & x_lEventId ) )
		{
			// Get the first event
			oexUINT uBit = GetEventBit( x_lEventId & m_uEventState );
			oexUINT uMask = 1 << uBit;

			// Acknowledge this event
			m_uEventState &= ~uMask;

			// Save the error code
			m_uLastError = m_uEventStatus[ uBit ];

			// Something is going on
			m_toActivity.SetMs( m_uActivityTimeout );

			// We received the event
			return uMask;

		} // end if

		// Have we timed out?
		UINT uTick = GetTickCount();
		if ( uEnd <= uTick )
			return 0;

		// Adjust timeout
		x_uTimeout = uEnd - uTick;

	} // end if

	// Can't get here...
	return 0;
#endif
}

oexUINT CIpSocket::GetEventBit( oexLONG x_lEventMask )
{_STT();
#if defined( OEX_NOSOCKET2 )
	return 0;
#else
	// !!!  Events will be returned by WaitEvent() in the order
	//      they appear below.

	if ( 0 != ( FD_CONNECT & x_lEventMask ) )
		return FD_CONNECT_BIT;

	if ( 0 != ( FD_ACCEPT & x_lEventMask ) )
		return FD_ACCEPT_BIT;

	if ( 0 != ( FD_WRITE & x_lEventMask ) )
		return FD_WRITE_BIT;

	if ( 0 != ( FD_READ & x_lEventMask ) )
		return FD_READ_BIT;

	if ( 0 != ( FD_OOB & x_lEventMask ) )
		return FD_OOB_BIT;

	if ( 0 != ( FD_QOS & x_lEventMask ) )
		return FD_QOS_BIT;

	if ( 0 != ( FD_GROUP_QOS & x_lEventMask ) )
		return FD_GROUP_QOS_BIT;

	if ( 0 != ( FD_ROUTING_INTERFACE_CHANGE & x_lEventMask ) )
		return FD_ROUTING_INTERFACE_CHANGE_BIT;

	if ( 0 != ( FD_CLOSE & x_lEventMask ) )
		return FD_CLOSE_BIT;

	return 0;
#endif
}

oexCSTR CIpSocket::GetErrorMsg( oexUINT x_uErr )
{_STT();
	oexCSTR ptr = oexT( "Unknown Winsock error" );

	switch( x_uErr )
	{
		case WSAEACCES:
			ptr = oexT( "Access Denied" );
			break;
		case WSAEADDRINUSE:
			ptr = oexT( "Address already in use" );
			break;
		case WSAEADDRNOTAVAIL:
			ptr = oexT( "Cannot assign requested address" );
			break;
		case WSAEAFNOSUPPORT:
			ptr = oexT( "Address family not supported by protocol family" );
			break;
		case WSAEALREADY:
			ptr = oexT( "Operation already in progress" );
			break;
		case WSAECONNABORTED:
			ptr = oexT( "Software caused connection abort" );
			break;
		case WSAECONNREFUSED:
			ptr = oexT( "Connection refused" );
			break;
		case WSAECONNRESET:
			ptr =oexT( "Connection reset by peer" );
			break;
		case WSAEDESTADDRREQ:
			ptr =oexT( "Destination addres required" );
			break;
		case WSAEFAULT:
			ptr =oexT( "Bad Address" );
			break;
		case WSAEHOSTDOWN:
			ptr =oexT( "Host is down" );
			break;
		case WSAEHOSTUNREACH:
			ptr =oexT( "Host is unreachable" );
			break;
		case WSAEINPROGRESS:
			ptr =oexT( "Operation is now in progress" );
			break;
		case WSAEINTR:
			ptr =oexT( "Interrupted function call" );
			break;
		case WSAEINVAL:
			ptr =oexT( "Invalid argument" );
			break;
		case WSAEISCONN:
			ptr =oexT( "Socket is already connected" );
			break;
		case WSAEMFILE:
			ptr =oexT( "Too many open files" );
			break;
		case WSAEMSGSIZE:
			ptr =oexT( "Message is too long" );
			break;
		case WSAENETDOWN:
			ptr =oexT( "Network is down" );
			break;
		case WSAENETRESET:
			ptr =oexT( "Network dropped connection on reset" );
			break;
		case WSAENETUNREACH:
			ptr =oexT( "Network is unreachable" );
			break;
		case WSAENOBUFS:
			ptr =oexT( "Insufficient buffer space is available" );
			break;
		case WSAENOPROTOOPT:
			ptr =oexT( "Bad protocol option" );
			break;
		case WSAENOTCONN:
			ptr =oexT( "Socket is not connected" );
			break;
		case WSAENOTSOCK:
			ptr =oexT( "Socket operation on non-socket" );
			break;
		case WSAEOPNOTSUPP:
			ptr =oexT( "Operation not supported" );
			break;
		case WSAEPFNOSUPPORT:
			ptr =oexT( "Protocol family not supported" );
			break;
		case WSAEPROCLIM:
			ptr =oexT( "Too many processes" );
			break;
		case WSAEPROTONOSUPPORT:
			ptr =oexT( "Protocol not supported" );
			break;
		case WSAEPROTOTYPE:
			ptr =oexT( "Protocol wrong type for socket" );
			break;
		case WSAESHUTDOWN:
			ptr =oexT( "Cannot send after socket shutdown" );
			break;
		case WSAESOCKTNOSUPPORT:
			ptr =oexT( "Socket type not supported" );
			break;
		case WSAETIMEDOUT:
			ptr =oexT( "Connection timed out" );
			break;
		case WSATYPE_NOT_FOUND:
			ptr =oexT( "Class type not found" );
			break;
		case WSAEWOULDBLOCK:
			ptr =oexT( "Resource temporarily unavailable (Would block)" );
			break;
		case WSAHOST_NOT_FOUND:
			ptr =oexT( "Host not found" );
			break;

		case WSANOTINITIALISED:
			ptr =oexT( "Successful WSAStartup not yet performed" );
			break;
		case WSANO_DATA:
			ptr =oexT( "Valid name, no data record of requested type" );
			break;
		case WSANO_RECOVERY:
			ptr =oexT( "Non-recoverable error has occured" );
			break;
//		case WSAPROVIDERFAILEDINIT:
//			ptr =oexT( "Unable to initialize a service provider" );
//			break;
		case WSASYSCALLFAILURE:
			ptr =oexT( "System call failure" );
			break;
		case WSASYSNOTREADY:
			ptr =oexT( "Network subsystem is unavailable" );
			break;
		case WSATRY_AGAIN:
			ptr =oexT( "Non-authoritative host not found" );
			break;
		case WSAVERNOTSUPPORTED:
			ptr =oexT( "WINSOCK.DLL version not supported" );
			break;
		case WSAEDISCON:
			ptr =oexT( "Graceful shutdown in progress" );
			break;

#if !defined( OEX_NOSOCKET2 )
		case WSA_INVALID_HANDLE:
			ptr =oexT( "Specified event object handle is invalid" );
			break;
		case WSA_INVALID_PARAMETER:
			ptr =oexT( "One or mor parameters are invalid" );
			break;
//		case WSAINVALIDPROCTABLE;
//			ptr =oexT( "Invalid procedure table from service provider" );
//			break;
//		case WSAINVALIDPROVIDER:
//			ptr =oexT( "Invalid service provider version number" );
//			break;
		case WSA_IO_INCOMPLETE:
			ptr =oexT( "Overlapped I/O event object not in signaled state" );
			break;
		case WSA_IO_PENDING:
			ptr =oexT( "Overlapped I/O operations will complete later" );
			break;
		case WSA_NOT_ENOUGH_MEMORY:
			ptr =oexT( "Insufficient memory available" );
			break;
		case WSA_OPERATION_ABORTED:
			ptr =oexT( "Overlapped I/O operation has been aborted" );
			break;
#endif

	} // end switch

	return ptr;
}

int CIpSocket::v_recvfrom( int socket, void *buffer, int length, int flags )
{
	SOCKADDR_IN si;
	int nSize = sizeof( si );
	os::CSys::Zero( &si, sizeof( si ) );
	si.sin_family = m_uSocketFamily;
	
	// Receive data from socket
	int res = recvfrom( socket, (char*)buffer, length, flags, (LPSOCKADDR)&si, &nSize );

    // Save the address
    CIpSocket_GetAddressInfo( &m_addrPeer, &si );

	return res;
}

oexUINT CIpSocket::RecvFrom( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT x_uFlags )
{_STT();
	// Initialize bytes read
	if ( x_puRead )
		*x_puRead = 0;

	// Must have a socket handle
	if ( !IsSocket() )
		return 0;

	// Receive data from socket
	int nRet = v_recvfrom( (SOCKET)m_hSocket, x_pData, (int)x_uSize, (int)x_uFlags );

	// Grab the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0;

	m_uReads++;
	m_toActivity.SetMs( m_uActivityTimeout );

	// Check for closed socket
	if ( !nRet )
	{	m_uConnectState |= eCsError;
		return 0;
	} // end if

	// Check for socket error
	if ( SOCKET_ERROR == nRet || x_uSize < (oexUINT)nRet  || 0 > nRet )
	{
		// Is the socket blocking?
		if ( !getWouldBlock() && m_uLastError )
		{	m_uConnectState |= eCsError;
			return 0;
		} // end if

		// Nothing read
		if ( x_puRead )
			*x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead )
		*x_puRead = nRet;

	return nRet;
}

CStr8 CIpSocket::RecvFrom( oexUINT x_uMax, oexUINT x_uFlags )
{_STT();
	// Do we have a size limit?
	if ( x_uMax )
	{
		// Allocate buffer
		CStr8 sBuf;
		if ( sBuf.OexAllocate( x_uMax ) )
		{
			// Attempt to read data
			oexUINT uRead = RecvFrom( sBuf._Ptr(), x_uMax, oexNULL, x_uFlags );

			// Accept as the length
			sBuf.SetLength( uRead );

		} // end if

		return sBuf;

	} // end if

	// Buffer size needs to be large for UDP packets
	x_uMax = 16 * 1024;
	
	// Allocate buffer
	CStr8 sBuf;
	oexUINT uRead = 0, uOffset = 0;
	
	// Allocate space
	if ( !sBuf.OexAllocate( x_uMax ) )
		return sBuf;

	// +++ Perhaps this shouldn't loop for UDP?
	
	// Read all available data
	while ( 0 < ( uRead = RecvFrom( sBuf._Ptr( uOffset ), x_uMax, oexNULL, x_uFlags ) )
			&& uRead >= (oexUINT)x_uMax )
	{
		if ( uRead > (oexUINT)x_uMax )
			uRead = x_uMax;
		
		// Allocate more space
		uOffset += uRead;
		if ( !sBuf.OexAllocate( uOffset + x_uMax ) )
			return sBuf;

	} // end while

	// Set the length
	sBuf.SetLength( uOffset + uRead );

	return sBuf;
}

int CIpSocket::v_recv( int socket, void *buffer, int length, int flags )
{
	return recv( socket, (char*)buffer, length, flags );
}

oexUINT CIpSocket::Recv( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT x_uFlags )
{_STT();
	// Initialize bytes read
	if ( x_puRead )
		*x_puRead = 0;

	// Must have a socket handle
	if ( !IsSocket() )
		return oexFALSE;

	// Receive data from socket
	int nRet = v_recv( (SOCKET)m_hSocket, x_pData, (int)x_uSize, (int)x_uFlags );

	// Grab the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0;

	m_uReads++;
	m_toActivity.SetMs( m_uActivityTimeout );

	// Check for closed socket
	if ( !nRet )
	{	m_uConnectState |= eCsError;
		return 0;
	} // end if

	// Check for socket error
	if ( SOCKET_ERROR == nRet || x_uSize < (oexUINT)nRet  || 0 > nRet )
	{
		// Is the socket blocking?
		if ( !getWouldBlock() && m_uLastError )
		{	m_uConnectState |= eCsError;
			return 0;
		} // end if

		// Nothing read
		if ( x_puRead )
			*x_puRead = 0;

		return 0;

	} // end if

	// Save the number of bytes read
	if ( x_puRead )
		*x_puRead = nRet;

	return nRet;
}

CStr8 CIpSocket::Recv( oexUINT x_uMax, oexUINT x_uFlags )
{_STT();

	// Do we have a size limit?
	if ( x_uMax )
	{
		// Allocate buffer
		CStr8 sBuf;
		if ( sBuf.OexAllocate( x_uMax ) )
		{
			// Attempt to read data
			oexUINT uRead = Recv( sBuf._Ptr(), x_uMax, oexNULL, x_uFlags );

			// Accept as the length
			sBuf.SetLength( uRead );

		} // end if

		return sBuf;

	} // end if

	// Buffer size needs to be large for UDP packets
	x_uMax = 16 * 1024;
	
	// Allocate buffer
	CStr8 sBuf;
	oexUINT uRead = 0, uOffset = 0;

	// Allocate space
	if ( !sBuf.OexAllocate( x_uMax ) )
		return sBuf;

	// +++ Perhaps this shouldn't loop for UDP
	
	// Read all available data
	while ( 0 < ( uRead = Recv( sBuf._Ptr( uOffset ), x_uMax, oexNULL, x_uFlags ) )
			&& uRead >= (oexUINT)x_uMax )
	{
		if ( uRead > (oexUINT)x_uMax )
			uRead = x_uMax;

		// Allocate more space
		uOffset += uRead;
		if ( !sBuf.OexAllocate( uOffset + x_uMax ) )
			return sBuf;

	} // end while

	// Set the length
	sBuf.SetLength( uOffset + uRead );

	return sBuf;
}


int CIpSocket::v_sendto(int socket, const void *message, int length, int flags )
{
	// Use the peer address
	struct sockaddr_in si;
	oexZero( si );
	si.sin_family = m_uSocketFamily;
	CIpSocket_SetAddressInfo( &m_addrPeer, &si );

	// Send the data
	return sendto( socket, (const char*)message, length, flags, (struct sockaddr*)&si, sizeof( si ) );
}


oexUINT CIpSocket::SendTo( oexCONST oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent, oexUINT x_uFlags )
{_STT();
	// Initialize bytes sent
	if ( x_puSent )
		*x_puSent = 0;

	// Must have a socket handle
	if ( !IsSocket() )
		return 0;

	// Send the data
	int nRet = v_sendto( (SOCKET)m_hSocket, x_pData, (int)x_uSize, (int)x_uFlags );

	// Get the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0;

	m_uWrites++;
	m_toActivity.SetMs( m_uActivityTimeout );

	// Check for error
	if ( SOCKET_ERROR == nRet )
	{
		// Is the socket blocking?
		if ( !getWouldBlock() && m_uLastError )
		{	m_uConnectState |= eCsError;
			return 0;
		} // end if

		// Number of bytes sent
		if ( x_puSent )
			*x_puSent = 0;

		// Error
		return 0;

	} // end if

	// Save the number of bytes sent
	if ( x_puSent )
		*x_puSent = nRet;

	return nRet;
}

int CIpSocket::v_send( int socket, const void *buffer, int length, int flags )
{
	return send( socket, (const char*)buffer, length, flags );
}

oexUINT CIpSocket::Send( oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent, oexUINT x_uFlags )
{_STT();
	// Initialize bytes sent
	if ( x_puSent )
		*x_puSent = 0;

	// Must have a socket handle
	if ( !IsSocket() )
		return 0;

	// Attempt to send the data
	setWouldBlock( 0 );
	int nRet = v_send( (SOCKET)m_hSocket, x_pData, (int)x_uSize, (int)x_uFlags );

	// Get the last error code
	m_uLastError = WSAGetLastError();
	if ( WSAEWOULDBLOCK == m_uLastError )
		m_uLastError = 0;

	m_uWrites++;
	m_toActivity.SetMs( m_uActivityTimeout );

	// Check for error
	if ( SOCKET_ERROR == nRet || 0 > nRet )
	{
		// Is the socket blocking?
		if ( !getWouldBlock() && m_uLastError )
		{	m_uConnectState |= eCsError;
			return 0;
		} // end if

		// Not an error
		m_uConnectState &= ~eCsError;

		// Number of bytes sent
		if ( x_puSent )
			*x_puSent = 0;

		// no bytes sent
		return 0;

	} // end if

	// Save the number of bytes sent
	if ( x_puSent )
		*x_puSent = nRet;

	return nRet;
}

oexBOOL CIpSocket::GetPeerAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa )
{_STT();
	if ( !oexVERIFY_PTR( x_pIa ) )
		return oexFALSE;

	// Reset address information
	x_pIa->Destroy();

	// Ensure socket
	if ( c_InvalidSocket == x_hSocket )
		return oexFALSE;

	// Init socket structure
	SOCKADDR_IN sai;
	os::CSys::Zero( &sai, sizeof( sai ) );
	int len = sizeof( sai );

	// Get the socket info
	if ( getpeername( (SOCKET)x_hSocket, (sockaddr*)&sai, &len ) )
		return FALSE;

	// Format the info
	return CIpSocket_GetAddressInfo( x_pIa, &sai );
}

oexBOOL CIpSocket::GetLocalAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa )
{_STT();
	if ( !oexVERIFY_PTR( x_pIa ) )
		return oexFALSE;

	// Reset address information
	x_pIa->Destroy();

	// Ensure socket
	if ( c_InvalidSocket == x_hSocket )
		return oexFALSE;

	// Init socket structure
	SOCKADDR_IN sai;
	os::CSys::Zero( &sai, sizeof( sai ) );
	int len = sizeof( sai );

	// Get the socket info
	if ( getsockname( (SOCKET)x_hSocket, (sockaddr*)&sai, &len ) )
		return FALSE;

	// Format the info
	return CIpSocket_GetAddressInfo( x_pIa, &sai );
}

oexUINT32 CIpSocket::hton_l( oexUINT32 v ) { return ::htonl( v ); }
oexUINT32 CIpSocket::ntoh_l( oexUINT32 v ) { return ::ntohl( v ); }
oexUINT16 CIpSocket::hton_s( oexUINT16 v ) { return ::htons( v ); }
oexUINT16 CIpSocket::ntoh_s( oexUINT16 v ) { return ::ntohs( v ); }
