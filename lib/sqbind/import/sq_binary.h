/*------------------------------------------------------------------
// sq_binary.h
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

#define SQBIND_SQBINARY_DECLARE_INT_TYPE( t )													\
	t_size findMasked##t( oex::oex##t val, oex::oex##t masked, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.findMasked##t( val, masked, x_nStart, x_nMax ); }

#define SQBIND_SQBINARY_DECLARE_TYPE( t )									\
	oex::oex##t get##t( t_size x_nOffset )									\
	{ return m_bin.get##t( x_nOffset ); }									\
	CSqBinary* set##t( t_size x_nOffset, oex::oex##t val )					\
	{ m_bin.set##t( x_nOffset, val ); return this; }						\
	oex::oex##t getAbs##t( t_size x_nOffset )								\
	{ return m_bin.getAbs##t( x_nOffset ); }								\
	CSqBinary* setAbs##t( t_size x_nOffset, oex::oex##t val )				\
	{ m_bin.setAbs##t( x_nOffset, val ); return this; }						\
	t_size find##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )		\
	{ return m_bin.find##t( val, x_nStart, x_nMax ); }						\
	t_size findMin##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.findMin##t( val, x_nStart, x_nMax ); }					\
	t_size findMax##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.findMax##t( val, x_nStart, x_nMax ); }					\
	t_size findRange##t( oex::oex##t min, oex::oex##t max, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.findRange##t( min, max, x_nStart, x_nMax ); }					\
	t_size skip##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )		\
	{ return m_bin.skip##t( val, x_nStart, x_nMax ); }						\
	t_size skipMin##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.skipMin##t( val, x_nStart, x_nMax ); }					\
	t_size skipMax##t( oex::oex##t val, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.skipMax##t( val, x_nStart, x_nMax ); }					\
	t_size skipRange##t( oex::oex##t min, oex::oex##t max, t_size x_nStart, t_size x_nMax )	\
	{ return m_bin.skipRange##t( min, max, x_nStart, x_nMax ); }					\
	oex::oex##t LE_get##t( t_size x_nOffset )								\
	{ return m_bin.LE_get##t( x_nOffset ); }								\
	CSqBinary* LE_set##t( t_size x_nOffset, oex::oex##t val )				\
	{ m_bin.LE_set##t( x_nOffset, val ); return this; }						\
	oex::oex##t LE_getAbs##t( t_size x_nOffset )							\
	{ return m_bin.LE_getAbs##t( x_nOffset ); }								\
	CSqBinary* LE_setAbs##t( t_size x_nOffset, oex::oex##t val )			\
	{ m_bin.LE_setAbs##t( x_nOffset, val ); return this; }					\
	oex::oex##t BE_get##t( t_size x_nOffset )								\
	{ return m_bin.BE_get##t( x_nOffset ); }								\
	CSqBinary* BE_set##t( t_size x_nOffset, oex::oex##t val )				\
	{ m_bin.BE_set##t( x_nOffset, val ); return this; }						\
	oex::oex##t BE_getAbs##t( t_size x_nOffset )							\
	{ return m_bin.BE_getAbs##t( x_nOffset ); }								\
	CSqBinary* BE_setAbs##t( t_size x_nOffset, oex::oex##t val )			\
	{ m_bin.BE_setAbs##t( x_nOffset, val ); return this; }					\
	t_size size##t()														\
	{ return sizeof( oex::oex##t ); }										\
	int Allocate##t( t_size x_nSize )										\
	{	return Allocate( sizeof( oex::oex##t ) * x_nSize ); }

// namespace
namespace sqbind
{
	class CSqImage;

    class CSqBinary
    {
	public:

		/// Size type
		typedef oex::CBin::t_size		t_size;

		/// Byte type
		typedef oex::CBin::t_byte		t_byte;

		/// Buffer type
		typedef oex::CBin				t_buffer;

	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqBinary )
			_SQBIND_CLASS_CTOR1( CSqBinary, OT_STRING ) ( (t_byte*)sa.GetString( 2 ), sq_getsize( x_v, 2 ) )
			_SQBIND_CLASS_CTOR1( CSqBinary, OT_INTEGER ) ( sa.GetInt( 2 ) )
			_SQBIND_CLASS_CTOR2( CSqBinary, OT_INTEGER, OT_INTEGER ) ( sa.GetInt( 2 ), sa.GetInt( 3 ) )
		SQBIND_CLASS_CTOR_END( CSqBinary )

		/// Default constructor
		CSqBinary() {}

		/// Allocate constructor
		CSqBinary( t_size x_size ) : m_bin( x_size ) { }
		CSqBinary( t_size x_size, t_size x_used ) : m_bin( x_size, x_used ) { }

		/// Copy constructor
		CSqBinary( const CSqBinary &r ) : m_bin( r.m_bin ) { }

		/// Construct from string
		CSqBinary( const stdString &s ) { m_bin.MemCpy( s.data(), s.length() ); }

		/// Rvalue Copy constructor
//#if defined( OEX_CPP0X )
//		CSqBinary( CSqBinary &&r ) : m_bin( r.m_bin ) { }
//#endif

		/// Assignment operator
		CSqBinary& operator = ( const CSqBinary &r )
		{	m_bin = r.m_bin; return *this; }

		/// Copy constructor
		CSqBinary( const t_buffer &r ) : m_bin( r ) { }

		/// Assignment operator
		CSqBinary& operator = ( const t_buffer &r )
		{	m_bin = r; return *this; }

		/// Construct from raw buffer
		CSqBinary( const void *x_ptr, t_size x_size, int x_bFree = 0 )
		{	if ( x_bFree )
				m_bin.setBuffer( x_ptr, x_size, 0, oex::oexTRUE );
			else
				m_bin.MemCpy( x_ptr, x_size );
		}

		/// Construct from raw buffer
		CSqBinary( const void *x_ptr, t_size x_size, t_size x_offset, int x_bFree )
		{	m_bin.setBuffer( x_ptr, x_size, x_offset, x_bFree );
		}

		/// Registers the class
		static void Register( sqbind::VM vm );

		/// Frees the memory
		void Free() { m_bin.Destroy(); }

		/// Allocate specified amount of memory
		t_size Allocate( t_size x_nSize ) { m_bin.Allocate( x_nSize ); return m_bin.Size(); }

		/// Resizes memory block
		t_size Resize( t_size x_nNewSize ) { m_bin.Resize( x_nNewSize ); return m_bin.Size(); }

		/// One step resize + used
		CSqBinary* Using( t_size x_nSize, t_size x_nUsed )
		{	m_bin.Resize( x_nSize ); m_bin.setUsed( x_nUsed ); return this; }

		/// Initializes all byetes in the memory block to zero
		void Zero() { m_bin.Zero(); }

		/// Copies memory object
		t_size Copy( CSqBinary *x_p ) { if ( !x_p ) return 0; return m_bin.Copy( &x_p->m_bin ); }

		/// Special copy that will copy to the dst buffer unconditionally
		t_size CopyBytes( CSqBinary *x_p, t_size x_uBytes );

		/// Shares memory object
		t_size Share( CSqBinary *x_p ) { if ( !x_p ) return 0; return m_bin.Share( &x_p->m_bin ); }

		/// Ensures we have our own buffer
		t_size Unshare() { return m_bin.Unshare(); }

		/// Returns the raw size of the buffer
		t_size Size() { return m_bin.Size(); }

		/// Returns the raw size of the buffer
		t_size RawSize() { return m_bin._Size(); }

		/// Returns the value of the byte at i
		int get( t_size x_i ) { return (int)m_bin.get( x_i ); }

		/// Sets the value of the byte at i
		void set( t_size x_i, int x_v ) { m_bin.set( x_i, (t_byte)x_v ); }

		/// Sets the number of bytes in the buffer being used
		void setUsed( t_size x_n ) { m_bin.setUsed( x_n ); }

		/// Returns the number of bytes in the buffer being used
		t_size getUsed() { return m_bin.getUsed(); }

		/// Sets the buffer offset
		void setOffset( t_size x_o ) { m_bin.setOffset( x_o ); }

		/// Returns the current buffer offset
		t_size getOffset() { return m_bin.getOffset(); }

		/// Sets the share name
		void SetName( const stdString &s ) { m_bin.SetName( s.c_str() ); }

		/// Returns the share name
		stdString GetName() { return m_bin.GetName(); }

		/// Sets plain share flag
		void PlainShare( int x_bPlain ) { m_bin.PlainShare( x_bPlain ? oex::oexTRUE : oex::oexFALSE ); }

		/// Returns non-zero if plain share
		int IsPlainShare() { return m_bin.IsPlainShare() ? 1 : 0; }

		/// Returns a string representation of the data
		stdString getString()
		{	oex::CStr s = oexMbToStr( m_bin.getString() );
			return stdString( s.Ptr(), (sqbind::stdString::size_type)s.Length() );
		}

		/// Returns a string representation of the data
		stdString8 getString8()
		{	return stdString8( (stdByte)m_bin.Ptr(), (sqbind::stdString8::size_type)m_bin.getUsed() ); }

		/// Initializes the binary buffer from a string
		t_size setString( const stdString &s )
		{	return m_bin.setString( oexStrToMb( oex::CStr( s.c_str(), s.length() ) ) ); }

		/// Initializes the binary buffer from a string
		t_size setString8( const stdString8 &s )
		{	return m_bin.setString( oex::CStr8( s.c_str(), s.length() ) ); }

		/// Sets a sub string into the binary object
		t_size setSubString( const stdString &s, int start, int len )
		{	if ( start >= (int)s.length() || ( start + len ) > (int)s.length() ) { Free(); return 0; }
			return m_bin.setString( oexStrToMb( oex::CStr( &(s.c_str()[ start ]), len ) ) );
		}

		/// Sets a sub string into the binary object
		t_size setSubString8( const stdString8 &s, int start, int len )
		{	if ( start >= (int)s.length() || ( start + len ) > (int)s.length() ) { Free(); return 0; }
			return m_bin.setString( oex::CStr8( &(s.c_str()[ start ]), len ) );
		}

		/// Gets a sub string into the binary object
		stdString getSubString( int start, int len )
		{	if ( start >= (int)m_bin.getUsed() ) return oexT( "" );
			int max = m_bin.getUsed() - start;
			if ( 0 >= len || len > max ) len = max;
			return oex2std( oexMbToStr( oex::CStr8( m_bin.Ptr(), m_bin.getUsed(), start, len ) ) );
		}

		/// Gets a sub string into the binary object
		stdString8 getSubString8( int start, int len )
		{	if ( start >= (int)m_bin.getUsed() ) return "";
			int max = m_bin.getUsed() - start;
			if ( 0 >= len || len > max ) len = max;
			return oex2std8( oex::CStr8( m_bin.Ptr(), m_bin.getUsed(), start, len ) );
		}

		/// Appends the string data to the buffer
		t_size appendString( const stdString &s )
		{	return m_bin.appendString( oexStrToMb( oex::CStr( s.c_str(), s.length() ) ) ); }

		/// Return const pointer to raw buffer
		const t_byte* Ptr( t_size o = 0 ) { return m_bin.Ptr( o ); }

		/// Return writable pointer to raw buffer
		t_byte* _Ptr( t_size o = 0 ) { return m_bin._Ptr( o ); }

		/// Sets a raw buffer
        void setBuffer( const void *x_ptr, t_size x_size, t_size x_offset, int x_bFree )
        {	m_bin.setBuffer( x_ptr, x_size, x_offset, x_bFree ? oex::oexTRUE : oex::oexFALSE ); }

		/// Copies data from a raw buffer
		t_size MemCpy( oexCONST t_byte *x_ptr, t_size x_size )
		{	return m_bin.MemCpy( x_ptr, x_size ); }

		/// Appends the specified buffer
		t_size Append( CSqBinary *x_p )
		{	if ( !x_p ) return 0; return m_bin.Append( &x_p->m_bin ); }

		/// Appends the specified raw buffer
		t_size AppendBuffer( const void *x_pBuf, t_size x_nBytes )
		{	return m_bin.AppendBuffer( (const t_byte*)x_pBuf, x_nBytes ); }

		/// Shift data in buffer to the left
		t_size LShift( t_size x_nBytes )
		{	return m_bin.LShift( x_nBytes ); }

		/// Inserts specified number of bytes at the specified position
		t_size Insert( t_size x_nBytes, t_size x_nOffset )
		{	return m_bin.Insert( x_nBytes, x_nOffset ); }

		/// Inserts specified number of bytes at the specified position
		t_size InsertBin( CSqBinary *x_p, t_size x_nOffset )
		{	if ( x_p && x_p->getUsed() )
				if ( m_bin.Insert( x_p->getUsed(), x_nOffset ) )
					m_bin.MemCpyAt( x_p->Ptr(), x_nOffset, x_p->getUsed() );
			return getUsed();
		}

		/// Copys specified number of bytes at the specified position
		t_size MemCpyAt( CSqBinary *x_p, t_size x_nOffset )
		{	if ( x_p && x_p->getUsed() )
				m_bin.MemCpyAt( x_p->Ptr(), x_nOffset, x_p->getUsed() );
			return getUsed();
		}

		/// Copys specified number of bytes at the specified position
		t_size MemCpyNumAt( CSqBinary *x_p, t_size x_nOffset, t_size x_nSize )
		{	if ( x_p && x_p->getUsed() )
			{	if ( x_nSize > x_p->getUsed() )
					x_nSize = x_p->getUsed();
				m_bin.MemCpyAt( x_p->Ptr(), x_nOffset, x_nSize );
			} // end if
			return getUsed();
		}

		/// Returns a ascii / hex string
		sqbind::stdString AsciiHexStr( int nLineLen, int nMaxLines )
		{	oex::CStr s = oexBinToAsciiHexStr( &m_bin, 0, nLineLen, nMaxLines );
			return sqbind::stdString( s.Ptr(), (sqbind::stdString::size_type)s.Length() );
		}

		/// Returns an ascii 'fingerprint' of the binary buffer
		sqbind::stdString Fingerprint( int w, int h, int frame )
		{	oex::CStr s = oexFingerprint( &m_bin, w, h, frame );
			return sqbind::stdString( s.Ptr(), (sqbind::stdString::size_type)s.Length() );
		}

		/// Returns an ascii 'fingerprint' of the binary buffer
		int FingerprintImage( CSqImage *img, CSqBinary *col, int scale );

		/// Returns an ascii 'fingerprint' of the binary buffer
		int FingerprintBin( CSqBinary *img, int fmt, int w, int h, CSqBinary *col, int scale )
		{	if ( !img || !col ) return 0;
			return oexFingerprint( &m_bin, &img->Mem(), fmt, w, h, &col->Mem(), scale );
		}

		/// Graphs an array of floats
		int GraphFloat( CSqImage *img, CSqColor *c, float scale, float min, float max );

		/// Creates the specified graph
		int Graph( int nSamples, int nInterval, int nType, CSqImage *img, CSqColor *bg, CSqColor *fg, const stdString &sParams );

		/// Scales a range of values
		int Scale( int nType, int nOffset, int nSamples, int nE, double dScale )
		{	return m_bin.Scale( nType, nOffset, nSamples, nE, dScale ); }

		/// Threshold a range of values
		int Threshold( int nType, int nOffset, int nSamples, int nE, double dUpper, double dUpperDef, double dLower, double dLowerDef )
		{	return m_bin.Threshold( nType, nOffset, nSamples, nE, dUpper, dUpperDef, dLower, dLowerDef ); }

		/// Calculates a grouped average
		int GroupAvg( int nType, int nOffset, int nInterval, int nGroups, CSqBinary *in, int nFlags )
		{	if ( !in ) return 0;
			return m_bin.GroupAvg( nType, nOffset, nInterval, nGroups, in->Mem(), nFlags );
		}

		/// Average data in buffer
		double Average( int x_nOffset, int x_nInterval, int fmt );

		/// Randomizes the buffer
		/**
			\param [in] nStart	-	Offset to start randomizing
			\param [in] nEnd	-	Offset to stop randomizing, if
									zero, the entire buffer will be
									randomized.
		*/
		int Randomize( int nStart, int nEnd )
		{	return m_bin.Randomize( nStart, nEnd ); }

		/// Locates the specified string in the buffer
		t_size Find( const sqbind::stdString &s, int x_nStart, int x_nMax )
		{	return m_bin.Find( s.data(), s.length(), x_nStart, x_nMax ); }

		/// Locates the specified string in the buffer
		t_size FindBin( CSqBinary *p, int x_nStart, int x_nMax )
		{	return m_bin.Find( p->m_bin, x_nStart, x_nMax ); }

		/// Returns a view of the specified location
		int Sub( CSqBinary *p, int x_nStart, int x_nSize )
		{	if ( !p ) return 0;
			 *p = m_bin.Sub( x_nStart, x_nSize );
			 return p->getUsed();
		}

		/// Base64 encodes data
		stdString base64_encode()
		{	return oex2std( oexMbToStr( m_bin.base64_encode() ) ); }

		/// Base64 encodes data
		CSqBinary base64_encode_bin()
		{	return CSqBinary( m_bin.base64_encode() ); }

		/// Base64 decodes data
		stdString base64_decode()
		{	return oex2std( oexMbToStr( m_bin.base64_decode() ) ); }

		/// Base64 decodes data
		CSqBinary base64_decode_bin()
		{	return CSqBinary( m_bin.base64_decode() ); }

		/// Base16 encodes data
		stdString base16_encode()
		{	return oex2std( oexMbToStr( m_bin.base16_encode() ) ); }

		/// Base16 encodes data
		CSqBinary base16_encode_bin()
		{	return CSqBinary( m_bin.base16_encode() ); }

		/// Base16 decodes data
		stdString base16_decode()
		{	return oex2std( oexMbToStr( m_bin.base16_decode() ) ); }

		/// Base16 decodes data
		CSqBinary base16_decode_bin()
		{	return CSqBinary( m_bin.base16_decode() ); }

		/// Compress data
		stdString compress()
		{	return oex2std( oexMbToStr( m_bin.compress() ) ); }

		/// Compress data
		CSqBinary compress_bin()
		{	return CSqBinary( m_bin.compress() ); }

		/// Uncompresses data
		stdString uncompress()
		{	return oex2std( oexMbToStr( m_bin.uncompress() ) ); }

		/// Uncompresses data
		CSqBinary uncompress_bin()
		{	return CSqBinary( m_bin.uncompress() ); }

		/// Base64 encodes data
		void base64_encode_str( const stdString &s )
		{	m_bin.base64_encode( oexStrToMb( std2oex( s ) ) ); }

		/// Base64 encodes data
		void base64_decode_str( const stdString &s )
		{	m_bin.base64_decode( oexStrToMb( std2oex( s ) ) ); }

		/// Base16 encodes data
		void base16_encode_str( const stdString &s )
		{	m_bin.base16_encode( oexStrToMb( std2oex( s ) ) ); }

		/// Base16 encodes data
		void base16_decode_str( const stdString &s )
		{	m_bin.base16_decode( oexStrToMb( std2oex( s ) ) ); }

		/// Compress data
		void compress_str( const stdString &s )
		{	m_bin.compress( oexStrToMb( std2oex( s ) ) ); }

		/// Uncompresses data
		void uncompress_str( const stdString &s )
		{	m_bin.uncompress( oexStrToMb( std2oex( s ) ) ); }

		/// Returns a view of the specified location
		CSqBinary getSub( int x_nStart, int x_nSize )
		{	return m_bin.Sub( x_nStart, x_nSize ); }

		/// Returns non-zero if the file mapped share already existed
		int Existing() { return m_bin.Existing(); }

		/// Value returned on failure
		int failed() { return m_bin.failed(); }

		/// Returns reference to buffer object
		t_buffer& Mem() { return m_bin; }

		/// Returns reference to buffer object
		operator t_buffer&() { return m_bin; }

		// Accessor functions
		SQBIND_SQBINARY_DECLARE_TYPE( CHAR );
		SQBIND_SQBINARY_DECLARE_TYPE( UCHAR );
		SQBIND_SQBINARY_DECLARE_TYPE( SHORT );
		SQBIND_SQBINARY_DECLARE_TYPE( USHORT );
		SQBIND_SQBINARY_DECLARE_TYPE( INT );
		SQBIND_SQBINARY_DECLARE_TYPE( UINT );
		SQBIND_SQBINARY_DECLARE_TYPE( LONG );
		SQBIND_SQBINARY_DECLARE_TYPE( ULONG );
		SQBIND_SQBINARY_DECLARE_TYPE( INT64 );
		SQBIND_SQBINARY_DECLARE_TYPE( UINT64 );
		SQBIND_SQBINARY_DECLARE_TYPE( FLOAT );
		SQBIND_SQBINARY_DECLARE_TYPE( DOUBLE );

		SQBIND_SQBINARY_DECLARE_INT_TYPE( CHAR );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( UCHAR );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( SHORT );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( USHORT );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( INT );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( UINT );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( LONG );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( ULONG );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( INT64 );
		SQBIND_SQBINARY_DECLARE_INT_TYPE( UINT64 );

	private:

		t_buffer		m_bin;

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqBinary, CSqBinary )

