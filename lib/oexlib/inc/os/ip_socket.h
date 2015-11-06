/*------------------------------------------------------------------
// ip_socket.h
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

/*
 _____    _   __
|  _  |  | | / /
| | | |  | |/ /
| |_| |  | |\ \
|_____|  |_| \_\

*/

//==================================================================
// CIpSocket
//
/// Socket API wrapper class
/**
	Provides wrapper for os specific socket support

	Example:
	\code

    if ( !oexVERIFY( CIpSocket::InitSockets() ) )
        return -1;

    CIpSocket is;

    if ( !is.Connect( "google.com", 80 )
         || !is.WaitEvent( CIpSocket::eConnectEvent, oexDEFAULT_WAIT_TIMEOUT ) )
    {   oexTRACE( "CIpSocket::Connect() : %s\n", is.GetLastErrorMsg().Ptr() );
        return -2;
    } // end if

    if ( !is.Send( "GET / HTTP/1.0\r\n\r\n" ) )
    {   oexTRACE( "CIpSocket::Send() : %s\n", is.GetLastErrorMsg().Ptr() );
        return -3;
    } // end if

	// Wait for data
    if ( is.WaitEvent( CIpSocket::eReadEvent, oexDEFAULT_WAIT_TIMEOUT ) )
        oexTRACE( "%s\n", is.Recv().Ptr() );

    oexTRACE( "Peer: %s\n", is.PeerAddress().GetDotAddress().Ptr() );
    oexTRACE( "Local: %s\n", is.LocalAddress().GetDotAddress().Ptr() );

    CIpSocket::UninitSockets();

	\endcode

	\see CIpAsyncSocket, CIpSocketProtocol, CIpSocket
*/
//==================================================================
class CIpSocket
{
public:

	/// Address Family
	enum
	{
		/// Unspecified
		eAfUnspec		= 0,

		/// Unix sockets
		eAfUnix			= 1,

		/// Internet IP sockets
		eAfInet			= 2
	};

	/// Socket Type
	enum
	{
		/// TCP / Stream
		eTypeStream		= 1,

		/// UDP / Datagram
		eTypeDgram		= 2,

        /// Raw interface
        eTypeRaw        = 3,

        /// Reliably delivered message
        eTypeRdm        = 4,

        /// Sequenced packet stream
        eTypeSeqPacket  = 5

	};

    /// Protocol type
    enum
    {
        /// IP
        eProtoIp                = 0,
        eProtoHopOpts           = 0,
        eProtoIcmp              = 1,
        eProtoIgmp              = 2,
        eProtoGgp               = 3,
        eProtoIpv4              = 4,
        eProtoTcp               = 6,
        eProtoPup               = 12,
        eProtoUdp               = 17,
        eProtoIdp               = 22,
        eProtoIpv6              = 41,
        eProtoRouting           = 43,
        eProtoFragment          = 44,
        eProtoEsp               = 50,
        eProtoAh                = 51,
        eProtoIcmpv6            = 58,
        eProtoIpv6None          = 59,
        eProtoIpv6DstOpts       = 60,
        eProtoNetDisk           = 61,

        eProtoRaw               = 255,
        eProtoMax               = 256

    };

	/// Socket 2 values
	enum
	{
		eReadBit						= 0,
		eReadEvent						= ( 1 << eReadBit ),

		eWriteBit						= 1,
		eWriteEvent						= ( 1 << eWriteBit ),

		eOobBit							= 2,
		eOobEvent						= ( 1 << eOobBit ),

		eAcceptBit						= 3,
		eAcceptEvent					= ( 1 << eAcceptBit ),

		eConnectBit						= 4,
		eConnectEvent					= ( 1 << eConnectBit ),

		eCloseBit						= 5,
		eCloseEvent						= ( 1 << eCloseBit ),

		eQosBit							= 6,
		eQosEvent						= ( 1 << eQosBit ),

		eGroupQosBit					= 7,
		eGroupQosEvent					= ( 1 << eGroupQosBit ),

		eRoutingInterfaceChangeBit		= 8,
		eRoutingInterfaceChangeEvent	= ( 1 << eRoutingInterfaceChangeBit ),

		eAddressListChangeBit			= 9,
		eAddressListChangeEvent			= ( 1 << eAddressListChangeBit ),

		eNumEvents						= 10,
		eAllEvents						= ( ( 1 << eNumEvents ) - 1 )
	};

	enum
	{
		/// Maximum number of events handled on a single call to
		/// epoll_wait() (linux only)
		eMaxEvents						= 1000
	};

	enum
	{
		/// Activity timeout
		eActivityTimeout				= 30 * 1000
	};

	enum
	{
		// Activity
		eCsActivity						= 0x00000001,

		// Socket has received connected signal
		eCsConnected					= 0x00000002,

		// Socket is connecting
		eCsConnecting					= 0x00000004,

		// Error on socket
		eCsError						= 0x00010000
	};

	enum
	{
		// Operation would have blocked
		eFlagWouldBlock					= 0x00000001
	};

    /// Socket handle type
    typedef oexPVOID t_SOCKET;

    /// Socket event type
    typedef oexPVOID t_SOCKETEVENT;

	/// Invalid socket value
	static oexCONST t_SOCKET c_InvalidSocket;

	/// Failure value
	static oexCONST t_SOCKET c_SocketError;

	/// Invalid socket event value
	static oexCONST t_SOCKETEVENT c_InvalidEvent;

protected:

	/// Constructs the class
	virtual void Construct();

public:

	/// Default Constructor
	CIpSocket();

	/// Construct from socket handle
	CIpSocket( t_SOCKET hSocket, oexBOOL x_bFree = oexTRUE );

	/// Destructor
	virtual ~CIpSocket();

public:

	//==============================================================
	// Destroy()
	//==============================================================
	/// Closes the socket and releases related resources
	void Destroy();

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when closing a socket
	/**
		\return Returns non-zero if success
	*/
	virtual oexBOOL OnClose() { return oexTRUE; }

	//==============================================================
	// InitSockets()
	//==============================================================
	/// Initializes the Socket API
	/**
		\warning	You must call this function before any other
					socket functions!  Best if called from your
					application startup code.  Call UninitSockets()
					before your application shuts down and after
					all instances of this class have been closed.

		\return Returns non-zero if success.
	*/
	static oexBOOL InitSockets();

	//==============================================================
	// UninitSockets()
	//==============================================================
	/// Uninitializes the Windows Socket API
	static void UninitSockets();

	//==============================================================
	// GetInitCount()
	//==============================================================
	/// Returns the number of outstanding calls to InitSockets()
	static oexLONG GetInitCount();

	//==============================================================
	// IsInitialized()
	//==============================================================
	/// Returns non-zero if the Socket API was successfully initialized.
	static oexBOOL IsInitialized()
    {   return ( m_lInit == 0 ); }

public:

	/// Override to provide custom read
	virtual int v_recv( int socket, void *buffer, int length, int flags );
	virtual int v_recvfrom( int socket, void *buffer, int length, int flags );

	/// Override to provide custom write
	virtual int v_send( int socket, const void *buffer, int length, int flags );
	virtual int v_sendto(int socket, const void *message, int length, int flags );

	/// Transport type
	virtual oexCSTR v_get_transport_type() { return oexT( "raw" ); }
	virtual oexCSTR v_get_transport_name() { return oexT( "socket" ); }
	virtual CStr v_get_transport_properties() { return CStr(); }

public:

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to existing socket handle
	/**
		\param [in] x_hSocket		-	Existing socket handle
		\param [in] x_bFree			-	Non-zero if this class should free the socket

		\return Returns non-zero if success
	*/
	oexBOOL Attach( t_SOCKET x_hSocket, oexBOOL x_bFree = oexTRUE );

	//==============================================================
	// OnAttach()
	//==============================================================
	/// Called when attaching to a new socket
	/**

		\return Returns non-zero if success
	*/
	virtual oexBOOL OnAttach() { return oexTRUE; }

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from existing socket handle without releasing it.
	t_SOCKET Detach();

	//==============================================================
	// IsSocket()
	//==============================================================
	/// Returns non-zero if the class contains a valid socket handle
	oexBOOL IsSocket()
	{   return ( m_hSocket != c_InvalidSocket ); }

	//==============================================================
	// GetConnectState()
	//==============================================================
	/// Returns value representing connect status
	/**
			bit 1	-	non zero if activity
			bit 2	-	non zero if connected
			bit 3	-	non zero if connecting
	*/
	oexINT GetConnectState()
	{	return m_uConnectState; }

	//==============================================================
	// IsConnected()
	//==============================================================
	/// Returns non-zero if th socket is connected
	oexBOOL IsConnected()
	{	if ( m_uLastError )
			return oexFALSE;
		if ( m_uConnectState & ( eCsActivity | eCsConnected ) )
			return oexTRUE;
		if ( !WaitEvent( eConnectEvent, 0 ) )
			return oexFALSE;
		return m_uLastError ? oexFALSE : oexTRUE;
	}

	//==============================================================
	// IsConnecting()
	//==============================================================
	/// Returns non-zero if the socket is in the process of connecting
	oexBOOL IsConnecting()
	{	WaitEvent( eConnectEvent, 0 );
		return ( m_uConnectState & eCsConnecting )
				? oexTRUE : oexFALSE;
	}

	//==============================================================
	// IsError()
	//==============================================================
	/// Returns
	oexBOOL IsError()
	{	return ( m_uConnectState & eCsError )
				? oexTRUE : oexFALSE;
	}

	//==============================================================
	// IsActivity()
	//==============================================================
	/// Returns
	oexBOOL IsActivity()
	{	return m_uActivityTimeout ? m_toActivity.IsValid() : oexTRUE;
	}

	//==============================================================
	// GetActivityTimeout()
	//==============================================================
	/// Returns
	oexINT GetActivityTimeout()
	{	return (oexINT)m_toActivity.Remaining();
	}

	//==============================================================
	// SetActivityTimeout()
	//==============================================================
	/// Sets the activity timeout
	void SetActivityTimeout( oexINT to )
	{	m_uActivityTimeout = to;
	}

	//==============================================================
	// IsRunning()
	//==============================================================
	/// Returns non-zero if the socket is working
	oexBOOL IsRunning( oexBOOL bCheckActivity )
	{
		// Is there a socket?
		if ( !IsSocket() )
			return oexFALSE;

		// Has it erred out?
		if ( IsError() )
			return oexFALSE;

		// Ensure it's connected or connecting
		if ( !IsConnected() && !IsConnecting() )
			return oexFALSE;

		// Does the caller want an activity check?
		if ( !bCheckActivity )
			return oexTRUE;

		// Is the connection active?
		return IsActivity();
	}

	//==============================================================
	// GetSocketHandle()
	//==============================================================
	/// Returns a handle to the socket
	t_SOCKET GetSocketHandle()
	{	return m_hSocket; }

	//==============================================================
	// getWouldBlock()
	//==============================================================
	/// Returns non-zero if the socket is in the process of connecting
	oexBOOL getWouldBlock()
	{	return ( m_uFlags & eFlagWouldBlock ) ? oexTRUE : oexFALSE;
	}

	//==============================================================
	// setWouldBlock()
	//==============================================================
	/// Returns non-zero if the socket is in the process of connecting
	void setWouldBlock( oexBOOL b )
	{	m_uFlags = b ? ( m_uFlags | eFlagWouldBlock ) : ( m_uFlags & ~eFlagWouldBlock );
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a new socket handle.
	/**
		\param [in] x_af		-	Address family specification.
		\param [in] x_type		-	The type specification.
		\param [in] x_protocol	-	The protocol to be used with the socket.

		\return Returns non-zero if success.

		\see Bind(), Listen(), Connect()
	*/
	virtual oexBOOL Create( oexINT x_af = eAfInet, oexINT x_type = eTypeStream, oexINT x_protocol = 0 );

	//==============================================================
	// GetLastError()
	//==============================================================
	/// Returns the most recent error code
	oexUINT GetLastError()
    {   return m_uLastError; }

	//==============================================================
	// GetErrorMsg()
	//==============================================================
	/// Returns the human readable error description
    oexCSTR GetErrorMsg( oexUINT x_uErr );

	//==============================================================
	// GetErrorMsg()
	//==============================================================
	/// Returns a human readable description of the last error
    CStr GetLastErrorMsg()
    {   return GetErrorMsg( GetLastError() ); }

	//==============================================================
	// Accept()
	//==============================================================
	/// Accepts an incomming connection on the specified socket
    /*
        \param [in] x_is    -   Socket that accepts the incomming
                                connection.
    */
    oexBOOL Accept( CIpSocket &x_is );

	//==============================================================
	// GetPeerInfo()
	//==============================================================
	/// Returns address information about the connected peer
    /*
        \param [in] x_hSocket   -   Socket handle
        \param [in] x_pIa        -   Receives address information.
    */
    static oexBOOL GetPeerAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa );

	//==============================================================
	// GetLocalAddress()
	//==============================================================
	/// Returns address information about the local connection
    /*
        \param [in] x_hSocket   -   Socket handle
        \param [in] x_pIa        -   Receives address information.
    */
    static oexBOOL GetLocalAddress( t_SOCKET x_hSocket, CIpAddress *x_pIa );

    /// Returns peer address information
    CIpAddress& PeerAddress()
    {   if ( !m_addrPeer.GetIpv4() )
            GetPeerAddress( m_hSocket, &m_addrPeer );
        return m_addrPeer;
    }

    /// Returns local address information
    CIpAddress& LocalAddress()
    {   if ( !m_addrLocal.GetIpv4() )
            GetLocalAddress( m_hSocket, &m_addrLocal );
        return m_addrLocal;
    }

	/// Set peer address object
	void SetPeerAddress( CIpAddress *pAddr )
	{	if ( pAddr )
			m_addrPeer = *pAddr;
	}
	
	/// Set local address object
	void SetLocalAddress( CIpAddress *pAddr )
	{	if ( pAddr )
			m_addrLocal = *pAddr;
	}
	
public:

	//==============================================================
	// Bind()
	//==============================================================
	/// Binds the open socket to the specified Port
	/**
		\param [in] x_uPort	-	Port to bind to.

		\return Returns non-zero if success.

		\see Create(), Listen(), Connect()
	*/
	oexBOOL Bind( oexUINT x_uPort );

	//==============================================================
	// BindTo()
	//==============================================================
	/// Binds the open socket to the specified address and Port
	/**
		\param [in] x_pAddress	-	Address to bind to.
		\param [in] x_uPort		-	Port to bind to.

		\return Returns non-zero if success.

		\see Create(), Listen(), Connect()
	*/
	oexBOOL BindTo( oexCSTR x_pAddress, oexUINT x_uPort );

	//==============================================================
	// Listen()
	//==============================================================
	/// Creates a socket listening on the bound port.
	/**
		\param [in] uMaxConnections	-	The maximum number of connections allowed.

		\return Returns non-zero if success, otherwise zero.

		\see Create(), Bind(), Connect()
	*/
	oexBOOL Listen( oexUINT x_uMaxConnections = 0 );

	//==============================================================
	// Connect()
	//==============================================================
	/// Address of remote peer.
	/**
		\param [in] x_pAddress	-	URL formed address of remote peer.
		\param [in] x_uPort		-	Port of remote peer.

		\return Returns non-zero if success.
	*/
	oexBOOL Connect( oexCSTR x_pAddress, oexUINT x_uPort );

	//==============================================================
	// Connect()
	//==============================================================
	/// Address of remote peer.
	/**
		\param [in] x_rIpAddress -	Address of remote peer

		\return Returns non-zero if success.
	*/
	oexBOOL Connect( CIpAddress &x_rIpAddress );

	/// Add multicast address to socket
	/**
		\param [in] x_pAddress	-	Address to add
		\param [in] x_pAdapter	-	Addapter
	
	*/
	oexBOOL AddMulticastAddr( oexCSTR x_pAddress, oexCSTR x_pAdapter );
	
	/// Set socket options that take an integer parameter
	int setsockint( int optname, int opt );
	
public:

	//==============================================================
	// EventSelect()
	//==============================================================
	/// Selects which events will generate callbacks
	/**
		\param [in] x_lEvents	-	The events to hook.

		\return Returns non-zero if success.
	*/
	oexBOOL EventSelect( oexLONG x_lEvents =	eReadEvent | eWriteEvent
								    			| eAcceptEvent | eConnectEvent
									    		| eCloseEvent );

	//==============================================================
	// GetEventHandle()
	//==============================================================
	/// Retuns the current event handle
	t_SOCKETEVENT GetEventHandle()
    {   return m_hSocketEvent; }

	//==============================================================
	// IsEventHandle()
	//==============================================================
	/// Returns non-zero if there is a valid event handle
    oexBOOL IsEventHandle()
    {   return vInvalidEvent() != GetEventHandle() ? oexTRUE : oexFALSE; }

	//==============================================================
	// CreateEventHandle()
	//==============================================================
	/// Creates a network event handle
	oexBOOL CreateEventHandle();

	//==============================================================
	// CloseEventHandle()
	//==============================================================
	/// Closes the event handle
	void CloseEventHandle();

	//==============================================================
	// WaitEvent()
	//==============================================================
	/// Waits for a socket event to occur
	/**
		\param [in] x_lEventId	-	Mask identifying event(s) to wait
									for.
		\param [in] x_uTimeout	-	Maximum time to wait in milli-
									seconds.

		\return The mask of the event that triggered the return. Zero
                if timed out waiting for event.

		\see
	*/
	oexUINT WaitEvent( oexLONG x_lEventId = ~0, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT );

	//==============================================================
	// GetEventBit()
	//==============================================================
	/// Returns the bit offset for the specified event
	/**
		\param [in] x_lEventMask	-	Event mask

		\return Bit offset for specified event

		\see
	*/
	static oexUINT GetEventBit( oexLONG x_lEventMask );

public:

	//==============================================================
	// RecvFrom()
	//==============================================================
	/// Reads data from the socket
	/**
		\param [in] x_pData		-	Receives the socket data
		\param [in] x_uSize		-	Size of buffer in pData
		\param [in] x_puRead	-	Receives the number of bytes read
		\param [in] x_uFlags	-	Socket receive flags

		\return Number of bytes read or c_InvalidSocket if failure.

		\see
	*/
	oexUINT RecvFrom( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead = oexNULL, oexUINT x_uFlags = 0 );


	//==============================================================
	// RecvFrom()
	//==============================================================
	/// Reads data from the socket and returns a CStr object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    CStr8 RecvFrom( oexUINT x_uMax = 0, oexUINT x_uFlags = 0 );

	//==============================================================
	// ReadFromBin()
	//==============================================================
	/// Reads data from the socket and returns a CStr object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    CBin ReadFromBin( oexUINT x_uMax = 0, oexUINT x_uFlags = 0 )
	{	CStr8 s = RecvFrom( x_uMax, x_uFlags );
		return CBin( s.Mem(), s.Length() );
	}

	//==============================================================
	// Recv()
	//==============================================================
	/// Reads data from the socket
	/**
		\param [in] x_pData		-	Receives the socket data
		\param [in] x_uSize		-	Size of buffer in pData
		\param [in] x_puRead	-	Receives the number of bytes read
		\param [in] x_uFlags	-	Socket receive flags

		\return Number of bytes read or c_InvalidSocket if failure.

		\see
	*/
	oexUINT Recv( oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead = oexNULL, oexUINT x_uFlags = 0 );


	//==============================================================
	// Recv()
	//==============================================================
	/// Reads data from the socket and returns a CStr object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    CStr8 Recv( oexUINT x_uMax = 0, oexUINT x_uFlags = 0 );

	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from the socket and returns a CStr object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    CStr8 Read( oexUINT x_uMax = 0, oexUINT x_uFlags = 0 )
	{	return Recv( x_uMax, x_uFlags ); }

	//==============================================================
	// ReadBin()
	//==============================================================
	/// Reads data from the socket and returns a CBin object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    CBin ReadBin( oexUINT x_uMax = 0, oexUINT x_uFlags = 0 )
	{	CStr8 s = Recv( x_uMax, x_uFlags );
		return CBin( s.Mem(), s.Length() );
	}

	// +++ This could be made much more efficient
	//==============================================================
	// AppendBin()
	//==============================================================
	/// Reads data from the socket and returns a CBin object
	/**
		\param [in] x_uMax		-   Maximum amount of data to return
		\param [in] x_uFlags	-	Socket receive flags

		\return CStr containing data

		\see
	*/
    oex::oexINT AppendBin( CBin *pBin, oexUINT x_uMax = 0, oexUINT x_uFlags = 0 )
	{	CStr8 s = Recv( x_uMax, x_uFlags );
		if ( pBin ) pBin->AppendBuffer( s.Ptr(), s.Length() );
		return s.Length();
	}

	//==============================================================
	// SendTo()
	//==============================================================
	/// Writes data to the socket
	/**
		\param [in] x_pData		-	Buffer containing write data
		\param [in] x_uSize		-	Size of the buffer in pData
		\param [in] x_puSent	-	Receives the number of bytes written
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT SendTo( oexCONST oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 );

	//==============================================================
	// SendTo()
	//==============================================================
	/// Writes a NULL terminated string to the socket
	/**
		\param [in] x_pStr		-	Pointer to NULL terminated string
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT SendTo( oexCSTR8 x_pStr, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	return SendTo( (oexPVOID)x_pStr, zstr::Length( x_pStr ), x_puSent, x_uFlags ); }

	//==============================================================
	// SendTo()
	//==============================================================
	/// Writes a string to the socket
	/**
		\param [in] x_sStr		-	String to be sent
		\param [in] x_uMax		-	Maximum number of bytes to send
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT SendTo( oexCONST CStr8 &x_sStr, oexUINT x_uMax = 0, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	if ( !x_uMax || x_uMax > x_sStr.Length() ) x_uMax = x_sStr.Length();
		return SendTo( (oexPVOID)x_sStr.Ptr(), x_uMax, x_puSent, x_uFlags );
	}

	//==============================================================
	// SendToBin()
	//==============================================================
	/// Writes binary data to the socket
	/**
		\param [in] x_sBin		-	String to be sent
		\param [in] x_uMax		-	Maximum number of bytes to send
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT SendToBin( oexCONST CBin &x_sBin, oexUINT x_uMax = 0, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	CBin &sBin = (CBin&)x_sBin;
		if ( !x_uMax || x_uMax > sBin.getUsed() ) x_uMax = sBin.getUsed();
		return SendTo( (oexPVOID)sBin.Ptr(), x_uMax, x_puSent, x_uFlags );
	}

	//==============================================================
	// Send()
	//==============================================================
	/// Writes data to the socket
	/**
		\param [in] x_pData		-	Buffer containing write data
		\param [in] x_uSize		-	Size of the buffer in pData
		\param [in] x_puSent	-	Receives the number of bytes written
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT Send( oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 );

	//==============================================================
	// Send()
	//==============================================================
	/// Writes a NULL terminated string to the socket
	/**
		\param [in] x_pStr		-	Pointer to NULL terminated string
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT Send( oexCSTR8 x_pStr, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	return Send( (oexPVOID)x_pStr, zstr::Length( x_pStr ), x_puSent, x_uFlags ); }

	//==============================================================
	// Send()
	//==============================================================
	/// Writes a string to the socket
	/**
		\param [in] x_sStr		-	String to be sent
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT Send( oexCONST CStr8 &x_sStr, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	return Send( (oexPVOID)x_sStr.Ptr(), x_sStr.Length(), x_puSent, x_uFlags ); }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the socket
	/**
		\param [in] x_sStr		-	String to be sent
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT Write( oexCONST CStr8 &x_sStr, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	return Send( (oexPVOID)x_sStr.Ptr(), x_sStr.Length(), x_puSent, x_uFlags ); }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes binary data to the socket
	/**
		\param [in] x_sBin		-	String to be sent
		\param [in] x_uMax		-	Maximum number of bytes to send
		\param [in] x_puSent	-	Number of bytes sent
		\param [in] x_uFlags	-	Socket write flags

		\return Number of bytes sent or c_InvalidSocket if failure.

		\see
	*/
	oexUINT WriteBin( oexCONST CBin &x_sBin, oexUINT x_uMax = 0, oexUINT *x_puSent = oexNULL, oexUINT x_uFlags = 0 )
    {	CBin &sBin = (CBin&)x_sBin;
		if ( !x_uMax || x_uMax > sBin.getUsed() ) x_uMax = sBin.getUsed();
		return Send( (oexPVOID)sBin.Ptr(), x_uMax, x_puSent, x_uFlags );
	}

public:

	//==============================================================
	// GetPeerName()
	//==============================================================
	/// Gets the remote socket information
	/**
		\param [out] x_pName	-	Receives the remote address of the connected socket.
		\param [out] x_puPort	-	Receives the remote TCP port of the connected socket.

		\return Returns non-zero if success.
	*/
	oexBOOL GetPeerName( oexSTR x_pName, oexUINT *x_puPort = oexNULL ) { return 0; }

	//==============================================================
	// GetSocketName()
	//==============================================================
	/// Gets the local socket information
	/**
		\param [out] x_pName	-	Receives the local address of the connected socket.
		\param [out] x_pdwPort	-	Receives the local TCP port of the connected socket.

		\return Returns non-zero if success.
	*/
	oexBOOL GetSocketName( oexSTR x_pName, oexUINT *x_puPort = oexNULL ) { return 0; }

    /// Shuts down the socket
    oexBOOL Shutdown();

	/// Byte conversions
	static oexUINT32 hton_l( oexUINT32 v );
	static oexUINT32 ntoh_l( oexUINT32 v );
	static oexUINT16 hton_s( oexUINT16 v );
	static oexUINT16 ntoh_s( oexUINT16 v );

public:

	/// Invalid socket handle value
    static oexCONST t_SOCKET vInvalidSocket()
    {   return c_InvalidSocket; }

    /// Invalid socket event value
    static oexCONST t_SOCKETEVENT vInvalidEvent()
    {   return c_InvalidEvent; }

    /// Invalid socket event value
    static oexCONST t_SOCKET vSocketError()
    {   return c_SocketError; }

	/// Converts windows event flags to unix event flags
	static oexINT FlagWinToNix( oexINT x_nFlag );

	/// Converts unix event flags to windows event flags
	static oexINT FlagNixToWin( oexINT x_nFlag );

	/// Returns the number of reads on the socket
	oexULONG getNumReads() { return m_uReads; }

	/// Returns the number of writes on the socket
	oexULONG getNumWrites() { return m_uWrites; }

	/// Returns the number of accepts on the socket
	oexULONG getNumAccepts() { return m_uAccepts; }

	/// Set event hook default
	void setHookEvents( oexBOOL b ) { m_bHookEvents = b; }

	/// Non-zero to hook events by default
	oexBOOL getHookEvents() { return m_bHookEvents; }

private:

	/// Socket API initialization return code
	static oexLONG			m_lInit;

	/// Socket handle
	t_SOCKET				m_hSocket;

	/// Non-zero if this class should free the socket on destruction
	oexBOOL					m_bFree;

	/// Last error code
	oexUINT					m_uLastError;

	/// Temporarily holds error value for GetErrorMsg()
    CStr					m_sError;

    /// String used to connect
    CStr                    m_sConnect;

    /// Peer address structure
    CIpAddress              m_addrPeer;

    /// Peer address structure
    CIpAddress              m_addrLocal;

	/// Socket event handle
    t_SOCKETEVENT			m_hSocketEvent;

    /// Connect state
    oexUINT                 m_uConnectState;

    /// Event state flags
    oexUINT                 m_uEventState;

    /// Event status flags
    oexUINT                 m_uEventStatus[ eNumEvents ];

	/// Pointer to event object (UNIX only)
	oexPVOID				m_pEventObject;

    /// Socket family
    oexUINT                 m_uSocketFamily;

    /// Socket type
    oexUINT                 m_uSocketType;

    /// Socket protocol
    oexUINT                 m_uSocketProtocol;

	/// Number of reads on the socket
	oexULONG				m_uReads;

	/// Number of writes on the socket
	oexULONG				m_uWrites;

	/// Number of accepts
	oexULONG				m_uAccepts;

	/// Activity timeout
	CTimeout				m_toActivity;

	/// Maximum idle time before timeout
	oexULONG				m_uActivityTimeout;

	/// Non-zero if events have been hooked
	oexBOOL					m_bEventsHooked;

	/// Flags
	oexULONG				m_uFlags;

	/// If non-zero, events will be hooked by default
	oexBOOL					m_bHookEvents;

};


