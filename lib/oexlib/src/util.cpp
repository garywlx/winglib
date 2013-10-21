/*------------------------------------------------------------------
// util.cpp
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

OEX_USING_NAMESPACE

CStr8 CUtil::md5( CStr8 s )
{_STT();
	oexGUID hash;
	return CBase16::Encode( oss::CMd5::Transform( &hash, s.Ptr(), s.Length() ), sizeof( hash ) );

}

#if !defined( OEX_NOWCHAR )
CStrW CUtil::md5( CStrW s )
{_STT();
	oexGUID hash;
	CStr8 sMb = oexStrWToMb( s );
	CStrW _s = oexMbToStrW( CBase16::Encode( oss::CMd5::Transform( &hash, sMb.Ptr(), sMb.Length() ), sizeof( hash ) ) );
  	return _s;
}
#endif

CStr8 CUtil::guid( CStr8 s )
{_STT();
	oexGUID hash;
	return *oss::CMd5::Transform( &hash, s.Ptr(), s.Length() );

}

#if !defined( OEX_NOWCHAR )
CStrW CUtil::guid( CStrW s )
{_STT();
	oexGUID hash;
	CStr8 sMb = oexStrWToMb( s );
	return *oss::CMd5::Transform( &hash, sMb.Ptr(), sMb.Length() );
}
#endif

// +++ This isn't really thread safe, it's ok if we assume that
//     Output capture will only be turned off after all other threads
//	   have been released, which is currently the case.
//	   PS : CCircBuf is internally thread safe.
//static CCircBuf *g_pCircBuf = oexNULL;
static CFifoSync *g_pFifoSync = oexNULL;
oexBOOL CUtil::EnableOutputCapture( oexUINT x_uBuffers, oexUINT x_uSize )
{
	if ( x_uBuffers && x_uSize && !g_pFifoSync )
	{	if ( 4 > x_uSize ) x_uSize = 4;
		g_pFifoSync = OexAllocConstruct< CFifoSync >( oexFALSE, x_uSize,
													  CCircBuf::eWmOverwrite, x_uBuffers );
	} // end if
	else if ( ( !x_uBuffers || !x_uSize ) && g_pFifoSync )
		OexAllocDelete( g_pFifoSync ), g_pFifoSync = oexNULL;
	return oexTRUE;
}

//static oexLock g_lock;
static oexUINT g_uTimestampFreq = 0;
static oexUINT g_uNextTimestamp = 0;
static oexTCHAR g_szFile[ 8 * 1024 ] = { 0 };
oexBOOL CUtil::EnableOutputFileCapture( oexCSTR x_pFile, oexUINT x_uTimestampFreq )
{
//	oexAutoLock ll( g_lock );
//	if ( !ll.IsLocked() )
//		return oexFALSE;

	g_uTimestampFreq = x_uTimestampFreq;
	if ( !x_pFile || !*x_pFile )
		*g_szFile = 0;
	zstr::Copy( g_szFile, oexSizeOfArray( g_szFile ), x_pFile );
	return *g_szFile;
}

oexBOOL CUtil::isOutputBuffer()
{
	return ( g_pFifoSync || *g_szFile ) ? oexTRUE : oexFALSE;
}

//CCircBuf* CUtil::getOutputBuffer()
CFifoSync* CUtil::getOutputBuffer()
{
	return g_pFifoSync;
}

oexBOOL CUtil::AddOutput( oexCSTR x_pStr, oexUINT x_uSize, oexBOOL x_bNewLine )
{
	// Sanity checks
	if ( !isOutputBuffer() || !x_pStr || !*x_pStr )
		return oexFALSE;

	if ( !x_uSize )
		x_uSize = zstr::Length( x_pStr );

	if ( !x_uSize )
		return oexFALSE;

	// Write to file?
	if ( g_szFile[ 0 ] )
	{	oex::CFile f;
		if ( f.OpenAlways( g_szFile ).IsOpen() )
		{
			CStr8 s;
			f.SetPtrPosEnd( 0 );

			// Add timestamp if needed
			if ( g_uTimestampFreq )
			{	oexUINT uGmt = oexGetUnixTime();
				if ( uGmt > g_uNextTimestamp )
				{	g_uNextTimestamp = uGmt + g_uTimestampFreq;
					s << oexNL8 "   --- " << uGmt << " - "
					  << oexStrToMb( oexLocalTimeStr( oexTEXT( "%Y/%c/%d - %g:%m:%s.%l" ) oexTEXT( oexNL8 ) ) );
				} // end if
			} // end if

			// Sux that it doesn't have a lf at the end
			s << oexStrToMb( CStr( x_pStr, x_uSize ) ) << oexNL8;

			// Write to file
			f.Write( s );

		} // end if
	} // end if

	// Write the data to the buffer
	if ( g_pFifoSync )
		g_pFifoSync->Write( x_pStr, x_uSize * sizeof( x_pStr[ 0 ] ) );

	return oexTRUE;
}

CStr CUtil::BinToAsciiHexStr( CBin *x_pBin, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines )
{_STT();
	if ( !x_pBin || !x_pBin->getUsed() )
		return oexT( "" );

	if ( 0 >= x_uLen || x_pBin->getUsed() < x_uLen )
		x_uLen = x_pBin->getUsed();

	return BinToAsciiHexStr( x_pBin->Ptr(), x_uLen, x_nLineLen, x_nMaxLines );
}

CStr CUtil::BinToAsciiHexStr( oexCPVOID x_pBuf, oexSIZE_T x_uLen, oexSIZE_T x_nLineLen, oexSIZE_T x_nMaxLines )
{_STT();

	// Sanity checks
	if ( !x_pBuf || 0 >= x_uLen || 0 >= x_nLineLen || 0 >= x_nMaxLines )
		return oexT( "" );

	// Allocate buffer for string
	oexSIZE_T nBytes = ( ( x_nLineLen * 4 ) + x_nLineLen + 4 ) * x_nMaxLines;
	CStr s; CStr::t_char *p = s.OexAllocate( nBytes );
	if ( !p )
		return oexT( "" );

	oexSIZE_T i = 0, b = 0;
	oexBYTE *pBuf = (oexBYTE*)x_pBuf;
	oexSIZE_T szNL = zstr::Length( oexNL8 );
	for ( oexSIZE_T l = 0; 0 < nBytes && i < x_uLen && ( !x_nMaxLines || l < x_nMaxLines ); l++, i += x_nLineLen )
	{
		for ( b = 0; 0 < nBytes && ( i + b ) < x_uLen && b < x_nLineLen; b++ )
			os::CSys::StrFmt( p, nBytes, oexT( "%0.2X " ), (oexUINT)pBuf[ i + b ] ),
			p += 3, nBytes -= 3;

		while ( 0 < nBytes && b < x_nLineLen )
			zstr::Copy( p, nBytes, oexT( "   " ) ),
			p += 3, nBytes -= 3, b++;

		// Append space
		zstr::Copy( p, nBytes, oexT( "  " ) ),
		p += 2, nBytes -= 2;

		for ( b = 0; 0 < nBytes && ( i + b ) < x_uLen && b < x_nLineLen; b++ )
			if ( pBuf[ i + b ] >= oexT( ' ' ) && pBuf[ i + b ] <= oexT( '~' ) )
				*( p++ ) = pBuf[ i + b ], nBytes--;
			else
				*( p++ ) = oexT( '.' ), nBytes--;

		// Append new line
		zstr::Copy( p, nBytes, oexNL ),
		p += szNL, nBytes -= szNL;

	} // end for

	return s;
}

CStr CUtil::Fingerprint( CBin *buf, oexINT w, oexINT h, oexINT frame )
{
	if ( !buf || !buf->getUsed() )
		return oexT( "" );

	// Glyphs
	int bits = 3;
	int mask = 0x07;
	oex::oexCSTR glyph = oexT( " .-+=|ox^*$@&#0()%!~:" );

	// Allocate space

	int sz = w * h;
	int snl = oex::zstr::Length( oexNL );

	oex::CStr s;
	oex::oexSTR p = s.OexAllocate( sz + ( h * snl ) );

	for ( int i = 0, y = 0; y < h; y++ )
	{
		// Init frame
		for ( int x = 0; x < w; x++ )
			if ( !frame || ( ( !y || ( y + 1 ) == h ) && ( !x || ( x + 1 ) == w ) ) )
				p[ i++ ] = oexT( ' ' );
			else if ( !y || ( y + 1 ) == h )
				p[ i++ ] = oexT( '-' );
			else if ( !x || ( x + 1 ) == w )
				p[ i++ ] = oexT( '|' );
			else
				p[ i++ ] = glyph[ 0 ];

		// Terminate
		if ( y >= ( h - 1 ) )
			p[ i++ ] = 0;

		// New line
		else
		{	p[ i++ ] = oexNL[ 0 ];
			if ( 1 < snl ) p[ i++ ] = oexNL[ 1 ];
		} // end else

	} // end for

	// Hash the data
	oex::oexGUID hash;
	oex::oss::CMd5::Transform( &hash, buf->Ptr(), buf->getUsed() );
	oex::oexUCHAR *psrc = (oex::oexUCHAR*)&hash;
	oex::oexINT nsrc = sizeof( hash );

	// Draw something
	int x =psrc[ 0 ] % ( w - 2 );
	int y = psrc[ 1 ] % ( h - 2 );
	int d = 0, m = 0;
	int n = 0, max = nsrc * 8;

	while ( n < max )
	{
		// Keep cursor in bounds
		if ( x <= 1 ) x = w - 2;
		else if ( x >= ( w - 2 ) ) x = 1;
		if ( y <= 1 ) y = h - 2;
		else if ( y >= ( h - 2 ) ) y = 1;

		// Pick the glyph
		int g = 0, o = x + ( y * ( w + snl ) );
		if ( max > n ) oex::cmn::CopyBits( &g, 0, psrc, n, ( ( max - n ) < bits ) ? ( max - n ) : bits ), n += bits;

		// Increment colliding glyphs
		int u = g & mask;
		while ( glyph[ u ] && p[ o ] != glyph[ u ] ) u++;
		if ( glyph[ u ] && glyph[ u + 1 ] ) p[ o ] = glyph[ u + 1 ];
		else p[ o ] = glyph[ g & mask ];

		// Get new speed and direction
		if ( !m )
		{	if ( max > n )
				oex::cmn::CopyBits( &m, 0,psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
			if ( max > n )
				oex::cmn::CopyBits( &d, 0, psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
		} // end if
		else
			m--;

		// Move cursor
		switch( d )
		{	case 0 : x += 1; break;
			case 1 : x -= 1; break;
			case 2 : y += 1; break;
			default : y -= 1; break;
		} // end switch

	} // end while

	return s;
}


CStr CUtil::Mandelbrot( int w, int h, int x1, int y1, int x2, int y2 )
{
	CStr s;

	// Cool link
	// http://mrl.nyu.edu/~perlin/

	int k = 2;
	float mx = x2 - x1, my = y2 - y1;
	float sy = my / (float)h;
	float sx = mx / (float)w;

	float i, j, r, x = 0, y = y1;
	for ( int py = 0; py < h; py++ )
	{
		x = x1;
		for ( int px = 0; px < w; px++ )
		{
			i = j = k = r = 0;
			do
			{
				r = j;

				j = r * r - i * i - 2 + x / 25;

				i = 2 * r * i + y / 10;

			} while ( j * j + i * i < 11 && k++ < 111 );

//			s += oexT( " .:-;!/>)|&IH%*#" )[ k & 15 ];
			s += oexT( " .-:=!+coeCO80&#" )[ k & 15 ];

			x += sx;

		} // end for

		y += sy;
		s += oexNL;

	} // end while

	return s;
}

template< typename T >
	void CUTIL_FP_SET_PIXEL( T *p, oexINT x, oexINT y, T *c, oexINT ci, oexINT s, oexINT w )
	{	c = &c[ ci * 3 ]; w *= 3; x *= s; y *= s;
		for ( oexINT _y = 0; _y < s; _y++ )
			for ( oexINT _x = 0; _x < s; _x++ )
			{	T *_p = &p[ ( y + _y ) * w + ( x + _x ) * 3 ];
				_p[ 0 ] = c[ 0 ], _p[ 1 ] = c[ 1 ], _p[ 2 ] = c[ 2 ];
			} // end for
	}

template< typename T >
	void CUTIL_FP_XOR_PIXEL( T *p, oexINT x, oexINT y, T *c, oexINT ci, oexINT s, oexINT w )
	{	c = &c[ ci * 3 ]; w *= 3; x *= s; y *= s;
		for ( oexINT _y = 0; _y < s; _y++ )
			for ( oexINT _x = 0; _x < s; _x++ )
			{	T *_p = &p[ ( y + _y ) * w + ( x + _x ) * 3 ];
				_p[ 0 ] |= c[ 0 ], _p[ 1 ] |= c[ 1 ], _p[ 2 ] |= c[ 2 ];
			} // end for
	}

static CBin::t_byte g_default_colors[] =
{
	32,		32,		32,
	32,		32,		32,
	0x80,	0,		0,
	0,		0x80,	0,
	0,		0,		0x80,
	0x7F,	0,		0,
	0,		0x7F,	0,
	0,		0,		0x7F
};

oexBOOL CUtil::Fingerprint( CBin *buf, CBin *img, oexINT fmt, oexINT w, oexINT h, CBin *col, oexINT scale )
{
	if ( 0 >= w || 0 >= h )
		return oexFALSE;

	if ( !buf || !buf->getUsed() || !img || !img->getUsed() )
		return oexFALSE;

	// Glyphs
	int bits = 3;
	int mask = 0x07;

	if ( scale < 1 ) scale = 1;
	int _w = w / scale;
	int _h = h / scale;

	// Ensure space
	int sz = w * h * 3;
	if ( img->getUsed() < (CBin::t_size)sz )
		return oexFALSE;

	// Pointers
	CBin::t_byte *p = img->_Ptr();
	CBin::t_byte *c = ( col && col->getUsed() ) ? col->_Ptr() : g_default_colors;
	oexINT ncols = ( ( col && col->getUsed() ) ? col->getUsed() : sizeof( g_default_colors ) ) / 3;

	// Initialize the image
	for ( int i = 0, y = 0; y < _h; y++ )
	{
		// Init frame
		for ( int x = 0; x < _w; x++ )
			if ( ( !y || ( y + 1 ) == _h ) && ( !x || ( x + 1 ) == _w ) )
				CUTIL_FP_SET_PIXEL( p, x, y, c, 1, scale, w ), i++;
			else if ( !y || ( y + 1 ) == _h )
				CUTIL_FP_SET_PIXEL( p, x, y, c, 0, scale, w ), i++;
			else if ( !x || ( x + 1 ) == _w )
				CUTIL_FP_SET_PIXEL( p, x, y, c, 0, scale, w ), i++;
			else
				CUTIL_FP_SET_PIXEL( p, x, y, c, 1, scale, w ), i++;

	} // end for

	// Hash the data
	oex::oexGUID hash;
	oex::oss::CMd5::Transform( &hash, buf->Ptr(), buf->getUsed() );
	oex::oexUCHAR *psrc = (oex::oexUCHAR*)&hash;
	oex::oexINT nsrc = sizeof( hash );

	// Draw something
	int x = psrc[ 0 ] % ( _w - 2 );
	int y = psrc[ 1 ] % ( _h - 2 );
	int d = 0, m = 0;
	int n = 0, max = nsrc * 8;

	while ( n < max )
	{
		// Keep cursor in bounds
		if ( x <= 1 ) x = _w - 2;
		else if ( x >= ( _w - 2 ) ) x = 1;
		if ( y <= 1 ) y = _h - 2;
		else if ( y >= ( _h - 2 ) ) y = 1;

		// Pick the glyph
		int g = 0; //, o = x + ( y * _w );
		if ( max > n )
			oex::cmn::CopyBits( &g, 0, psrc, n, ( ( max - n ) < bits ) ? ( max - n ) : bits ), n += bits;

		// Skip border and background colors
		g += 2;

		// Do we have enough colors?
		while ( g >= ncols ) g--;
		if ( g ) CUTIL_FP_XOR_PIXEL( p, x, _h - y - 1, c, g, scale, w );

		// Get new speed and direction
		if ( !m )
		{	if ( max > n )
				oex::cmn::CopyBits( &m, 0,psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
			if ( max > n )
				oex::cmn::CopyBits( &d, 0, psrc, n, ( ( max - n ) < 2 ) ? ( max - n ) : 2 ), n += 2;
		} // end if
		else
			m--;

		// Move cursor
		switch( d )
		{	case 0 : x += 1; break;
			case 1 : x -= 1; break;
			case 2 : y += 1; break;
			default : y -= 1; break;
		} // end switch

	} // end while

	return oexTRUE;
}

template< typename T >
	void CUTIL_SET_PIXEL24( T *b, oexINT sw, oexINT x, oexINT y, oexINT *c )
	{	oexINT o = y * sw + ( x * 3 );
		b[ o + 2 ] = (T)c[ 0 ];
		b[ o + 1 ] = (T)c[ 1 ];
		b[ o ] = (T)c[ 2 ];
	}

template< typename T >
	void CUTIL_SCALE_PIXEL24( T *b, oexINT sw, oexINT x, oexINT y, oexINT *c1, oexINT *c2, oexINT scale )
	{	oexINT o = y * sw + ( x * 3 );
		b[ o + 2 ] = (T)c1[ 0 ] + (T)( (oexINT)( c2[ 0 ] - c1[ 0 ] ) * scale / (oexINT)255 );
		b[ o + 1 ] = (T)c1[ 1 ] + (T)( (oexINT)( c2[ 1 ] - c1[ 1 ] ) * scale / (oexINT)255 );
		b[ o ] = (T)c1[ 2 ] + (T)( (oexINT)( c2[ 2 ] - c1[ 2 ] ) * scale / (oexINT)255 );
	}

// Bresenham
oexBOOL CUtil::DrawLine( CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw, oexINT *pc, oexINT x1, oexINT y1, oexINT x2, oexINT y2 )
{
	if ( 0 >= w || 0 >= h )
		return oexFALSE;

	if ( !img || !img->getUsed() )
		return oexFALSE;

	// Adjust range on variables
	x1 = cmn::Range( x1, 0, w - 1 );
	y1 = cmn::Range( y1, 0, h - 1 );
	x2 = cmn::Range( x2, 0, w - 1 );
	y2 = cmn::Range( y2, 0, h - 1 );

	oexBYTE *p = (oexBYTE*)img->_Ptr();
	oexINT xe = x2 - x1; if ( 0 > xe ) xe = -xe;
	oexINT ye = y2 - y1; if ( 0 > ye ) ye = -ye;
	oexINT xm = 0, xd = ( x1 < x2 ) ? 1 : -1;
	oexINT ym = 0, yd = ( y1 < y2 ) ? 1 : -1;
	while ( y1 != y2 || x1 != x2 )
	{
		CUTIL_SET_PIXEL24( p, sw, x1, y1, pc );

		ym += ye;
		if ( ym >= xe )
		{	ym -= xe;
			if ( y1 != y2 )
				y1 += yd;
		} // end while

		xm += xe;
		if ( xm >= ye )
		{	xm -= ye;
			if ( x1 != x2 )
				x1 += xd;
		} // end while

	} // end while

	// Set the last pixel
	CUTIL_SET_PIXEL24( p, sw, x2, y2, pc );

	return oexTRUE;
}

double CUtil::BinAverage( CBin *x_pBin, oexSIZE_T x_uOffset, oexSIZE_T x_uInterval, oexINT fmt )
{
	if ( !x_pBin || !x_pBin->getUsed() )
		return 0;

	// Ensure interval
	if ( 0 >= x_uInterval )
		x_uInterval = 1;

	double acc = 0, qty = 0;
	oexLONG used = x_pBin->getUsed();
	const void *ptr = x_pBin->Ptr();

	// Sanity check
	if ( 0 >= used || !ptr )
		return 0;

	// Did the user supply an offset?
	if ( 0 < x_uOffset )
	{
		// Do we have enough?
		if ( used <= x_uOffset )
			return 0;

		// Add byte offset
		ptr = (char*)ptr + x_uOffset;
		used -= x_uOffset;

	} // end if

	switch( fmt )
	{
		default :
			break;

		case obj::tInt8 :
		{	oexLONG sz = used;
			const char *p = (const char*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tUInt8 :
		{	oexLONG sz = used;
			const unsigned char *p = (const unsigned char*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tInt16 :
		{	oexLONG sz = used / sizeof( short );
			const short *p = (const short*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tUInt16 :
		{	oexLONG sz = used / sizeof( unsigned short );
			const unsigned short *p = (const unsigned short*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tInt32 :
		{	oexLONG sz = used / sizeof( int );
			const int *p = (const int*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tUInt32 :
		{	oexLONG sz = used / sizeof( unsigned int );
			const unsigned int *p = (const unsigned int*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tFloat :
		{	oexLONG sz = used / sizeof( float );
			const float *p = (const float*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

		case obj::tDouble :
		{	oexLONG sz = used / sizeof( double );
			const double *p = (const double*)ptr;
			for ( oexLONG i = 0; i < sz; i += x_uInterval )
				acc += p[ i ], qty++;
		} break;

	} // end switch

	if ( !qty )
		return 0;

	return acc / qty;
}


#define CUTIL_YMARGIN 2
oexBOOL CUtil::GraphFloat( CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw, oexINT *pc, oexFLOAT *pf, oexINT n, oexFLOAT scale, oexFLOAT min, oexFLOAT max )
{
	if ( 0 >= w || 0 >= h || 1 >= n )
		return oexFALSE;

	if ( !img || !img->getUsed() )
		return oexFALSE;

	// Ensure space
	int sz = sw * h;
	if ( img->getUsed() < sz )
		return oexFALSE;

	// Was a range provided?
	if ( min >= max )
	{	min = pf[ 0 ], max = pf[ 0 ];
		for ( oexINT i = 1; i < n; i++ )
			if ( min > ( pf[ i ] * scale ) ) min = ( pf[ i ] * scale );
			else if ( max < ( pf[ i ] * scale ) ) max = ( pf[ i ] * scale );
	} // end if

	// Calculate range
	oexFLOAT range = max - min;
	if ( !range ) range = .001;

	// Draw the graph
	oexINT x = 0, xl = 0, mx = 0, mn = 0, ni = 0;
	oexFLOAT y, yl = ( CUTIL_YMARGIN / 2 ) + ( ( ( pf[ 0 ] * scale ) - min ) * ( h - CUTIL_YMARGIN ) / range );
	while ( ni < n || x < ( w - 1 ) )
	{
		// Next y
		y = (oexINT)( (oexFLOAT)( ( CUTIL_YMARGIN / 2 ) ) + ( ( ( pf[ ni ] * scale ) - min ) * ( (oexFLOAT)( h - CUTIL_YMARGIN ) ) / range ) );

		// Next x
		mx += w;
		while ( mx >= n )
		{	if ( x < ( w - 1 ) )
				x++;
			mx -= n;
		} // end if

		// Draw the line
		DrawLine( img, fmt, w, h, sw, pc, xl, yl, x, y );

		// Next value
		mn += n;
		while ( mn >= w )
		{	if ( ni < n )
				ni++;
			mn -= w;
		} // end while

		// Last position
		xl = x;
		yl = y;

	} // end for

	return oexTRUE;
}

CStr CUtil::BuildPath( oexBOOL x_bShared, oexINT x_nId, oexCONST CStr &x_sPath, oexTCHAR tSep )
{	return oexBuildPath( oexGetSysFolder( x_bShared, x_nId ), x_sPath, tSep ); }

oexINT CUtil::CompareVersion( oexCONST CStr &x_sV1, oexCONST CStr &x_sV2, oexCONST CStr &x_sSep )
{
	oex::TList< oex::CStr > v1 = oex::CParser::Explode( x_sV1, x_sSep );
	oex::TList< oex::CStr > v2 = oex::CParser::Explode( x_sV2, x_sSep );

	oexSIZE_T i = 0;
	while ( i < v1.Size() && i < v2.Size() )
	{
		oex::oexINT64 n1 = v1[ i ]->ToInt64();
		oex::oexINT64 n2 = v2[ i ]->ToInt64();

		if ( n1 < n2 )
			return -1;

		else if ( n1 > n2 )
			return 1;

		i++;

	} // end while

	// Assume extra stuff is a higher version?
	if ( v1.Size() < v2.Size() )
		return -1;
	else if ( v1.Size() > v2.Size() )
		return 1;

	// Ok, equal
	return 0;
}

oexBOOL CUtil::Graph( oexINT nSamples, oexINT nInterval, oexINT nType,
					  CBin *img, oexINT fmt, oexINT w, oexINT h, oexINT sw,
					  oexINT *fg, oexINT *bg, oexCPVOID p, oexINT n,
					  oexCSTR pParams )
{
	if ( 0 >= w || 0 >= h || 1 >= n )
		return oexFALSE;

	if ( !img || !img->getUsed() )
		return oexFALSE;

	// Get data size
	int nSize = oex::obj::StaticSize( oex::CUtil::eDtMask & nType );
	if ( 0 >= nSize )
		return oexFALSE;

	if ( 0 >= nSamples )
		nSamples = n / nSize;

	// Ensure space
	int sz = sw * h;
	if ( img->getUsed() < sz )
		return oexFALSE;

	// Pointer to image data
	oexBYTE *pi = (oexBYTE*)img->_Ptr();

	CPropertyBag pb;
	if ( pParams && *pParams )
		pb = CParser::Deserialize( pParams );

	// +++ Wow, this desperately needs improvement
	if ( eGtHorzFft == ( eGtMask & nType ) )
	{
		// There is no good default interval
		if ( 0 >= nInterval )
			return oexFALSE;

		// Find the range
		oexFLOAT fMin = 0, fMax = 0;
		for( oexINT i = 0, c = 0; i < nSamples; i++ )
		{	//oexFLOAT fV = ( (oexFLOAT*)p )[ i ];
			oexFLOAT fV;
			switch( oex::CUtil::eDtMask & nType )
			{	case obj::tDouble : fV = oex::cmn::Abs( ( (oexDOUBLE*)p )[ i ] ); break;
				case obj::tFloat : fV = oex::cmn::Abs( ( (oexFLOAT*)p )[ i ] ); break;
				case obj::tInt8 : fV = oex::cmn::Abs( ( (oexCHAR*)p )[ i ] ); break;
				case obj::tInt16 : fV = oex::cmn::Abs( ( (oexSHORT*)p )[ i ] ); break;
				case obj::tInt32 : fV = oex::cmn::Abs( ( (oexINT*)p )[ i ] ); break;
				case obj::tInt64 : fV = oex::cmn::Abs( ( (oexINT64*)p )[ i ] ); break;
				case obj::tUInt8 : fV = oex::cmn::Abs( ( (oexUCHAR*)p )[ i ] ); break;
				case obj::tUInt16 : fV = oex::cmn::Abs( ( (oexUSHORT*)p )[ i ] ); break;
				case obj::tUInt32 : fV = oex::cmn::Abs( ( (oexUINT*)p )[ i ] ); break;
				case obj::tUInt64 : fV = oex::cmn::Abs( ( (oexUINT64*)p )[ i ] ); break;
				default : return oexFALSE;
			} // end switch
			if ( oex::cmn::Abs( fV ) < 1000000.f )
			{	if ( !c )
					fMin = fV, fMax = fV, c++;
				else if ( fMin > fV )
					fMin = fV;
				else if ( fMax < fV )
					fMax = fV;
			} // end if
		} // end for

		oexFLOAT fRange = pb[ oexT( "range" ) ].ToFloat();
		if ( !fRange )
			fRange = fMax - fMin;
		if ( !fRange )
			return oexFALSE;

		oexINT nMag = oex::cmn::Max( 0, pb[ oexT( "mag" ) ].ToInt() );
		oexINT nHo = oex::cmn::Max( 0, pb[ oexT( "ho" ) ].ToInt() );
		if ( 0 >= nMag ) nMag = 1;
		if ( 0 >= nHo || nHo >= h ) nHo = 0;

		oexINT scale = -1;
		for ( int x = 0, i = 0; x < w; x++, i += nInterval )
			for ( int y = nHo, o = 0, _o = -1; y < h; y++, o++ )
			{
				if ( ( o / nMag ) != _o )
				{
					_o = o / nMag;
					if ( _o >= nInterval )
						scale = -1;
					else
					{
						int _i = i + _o;
						if (  _i >= nSamples )
							scale = -1;
						else
						{
							//oexFLOAT f = oex::cmn::Abs( ( (oexFLOAT*)p )[ _i ] );
							oexFLOAT f;
							switch( oex::CUtil::eDtMask & nType )
							{	case obj::tDouble : f = oex::cmn::Abs( ( (oexDOUBLE*)p )[ _i ] ); break;
								case obj::tFloat : f = oex::cmn::Abs( ( (oexFLOAT*)p )[ _i ] ); break;
								case obj::tInt8 : f = oex::cmn::Abs( ( (oexCHAR*)p )[ _i ] ); break;
								case obj::tInt16 : f = oex::cmn::Abs( ( (oexSHORT*)p )[ _i ] ); break;
								case obj::tInt32 : f = oex::cmn::Abs( ( (oexINT*)p )[ _i ] ); break;
								case obj::tInt64 : f = oex::cmn::Abs( ( (oexINT64*)p )[ _i ] ); break;
								case obj::tUInt8 : f = oex::cmn::Abs( ( (oexUCHAR*)p )[ _i ] ); break;
								case obj::tUInt16 : f = oex::cmn::Abs( ( (oexUSHORT*)p )[ _i ] ); break;
								case obj::tUInt32 : f = oex::cmn::Abs( ( (oexUINT*)p )[ _i ] ); break;
								case obj::tUInt64 : f = oex::cmn::Abs( ( (oexUINT64*)p )[ _i ] ); break;
								default : return oexFALSE;
							} // end switch
							if ( f <= fMin ) scale = 0;
							else if ( f >= fMax ) scale = 255;
							else
							{	f -= fMin;
								if ( f >= fRange ) scale = 255;
								else scale = (oexINT)( f * 255. / fRange );
							} // end else

						} // end else
					} // end else

				} // end if

				if ( 0 <= scale )
					CUTIL_SCALE_PIXEL24( pi, sw, x, y, bg, fg, scale );

			} // end for

	} // end if

	return oexTRUE;
}

CStr CUtil::ReadStdin( oexLONG lMax )
{_STT();

	oex::CStr s;
	if ( 0 < lMax )
		if ( !s.OexAllocate( lMax ) )
			return s;

	char buf[ 1024 ];
	oex::oexLONG lTotal = 0;
	do
	{
		// How much to try and read?
		oex::oexLONG lRead = sizeof( buf );
		if ( 0 < lMax && lRead > lMax - lTotal )
			lRead = lMax - lTotal;

		// Read some data
		lRead = oexRead_stdin( buf, lRead );
		if ( 0 >= lRead )
			return s;

		// Add to buffer
		s.Append( oexMbToStr( oex::CStr8( buf, lRead ) ) );
		lTotal += lRead;

	} while ( lTotal < lMax );

	return s;
}

#define CPUVAL( name, i, b, m ) \
	(*r)[ oexT( name ) ] = ( ( ( (unsigned int)ci[ i ] ) & ( m << ( b ) ) ) >> ( b ) );

oexINT CUtil::GetCpuInfo( CPropertyBag *pb )
{_STT();

	if ( !pb )
		return 0;

	char cs[ 256 ] = { 0 };
	int *csi = (int*)cs;
	int ci[ 4 ] = { -1 };

	os::CSysUtil::i_cpuid( ci, 0 );
	if ( 0 > ci[ 0 ] )
		return 0;

	int ids = ci[ 0 ];
	if ( 0 >= ids )
		return 0;

	// CPU id's
	(*pb)[ oexT( "ids" ) ] = ids;

	// CPU string
	csi[ 0 ] = ci[ 1 ]; csi[ 1 ] = ci[ 3 ];
	csi[ 2 ] = ci[ 2 ]; csi[ 3 ] = 0;
	(*pb)[ oexT( "cpu" ) ] = CStr( oexMbToStr( cs ) ).TrimWhiteSpace();

	CPropertyBag *r;

	os::CSysUtil::i_cpuid( ci, 1 );
	r = &(*pb)[ oexT( "features" ) ];

	CPUVAL( "stepping_id",     0,  0, 0x0f );
	CPUVAL( "model",           0,  4, 0x0f );
	CPUVAL( "family",          0,  8, 0x03 );
	CPUVAL( "type",            0, 12, 0x0f );
	CPUVAL( "exmodel",         0, 16, 0x0f );
	CPUVAL( "exfamily",        0, 20, 0xff );

	CPUVAL( "brand_idx",       1,  0, 0xff );
	CPUVAL( "clflush_size",    1,  8, 0xff );
	CPUVAL( "logical_cpus",    1, 16, 0xff );
	CPUVAL( "apic_id",         1, 24, 0xff );

	CPUVAL( "sse3",            2,  0, 1 );
	CPUVAL( "monitor",         2,  3, 1 );
	CPUVAL( "cplds",           2,  4, 1 );
	CPUVAL( "vmex",            2,  5, 1 );
	CPUVAL( "smx",             2,  6, 1 );
	CPUVAL( "ess",             2,  7, 1 );
	CPUVAL( "tmon2",           2,  8, 1 );
	CPUVAL( "ssse3",           2,  9, 1 );
	CPUVAL( "l1ctxid",         2, 10, 1 );
	CPUVAL( "cmpxchg16b",      2, 13, 1 );
	CPUVAL( "tpr_update",      2, 14, 1 );
	CPUVAL( "perf_debug_msr",  2, 15, 1 );
	CPUVAL( "mmdp",            2, 18, 1 );
	CPUVAL( "sse41",           2, 19, 1 );
	CPUVAL( "sse42",           2, 20, 1 );
	CPUVAL( "x2apic",          2, 21, 1 );
	CPUVAL( "movebe",          2, 22, 1 );
	CPUVAL( "popcnt",          2, 23, 1 );
	CPUVAL( "xsave",           2, 26, 1 );
	CPUVAL( "xsetbv",          2, 27, 1 );
	CPUVAL( "avx",             2, 28, 1 );

	CPUVAL( "x86_fpu",         3,  0, 1 );
	CPUVAL( "v86_mode",        3,  1, 1 );
	CPUVAL( "dbgext",          3,  2, 1 );
	CPUVAL( "pgszext",         3,  3, 1 );
	CPUVAL( "ts_counter",      3,  4, 1 );
	CPUVAL( "rdmsr",           3,  5, 1 );
	CPUVAL( "page_addr_ext",   3,  6, 1 );
	CPUVAL( "mce",             3,  7, 1 );
	CPUVAL( "cmpxchg8b",       3,  8, 1 );
	CPUVAL( "apic",            3,  9, 1 );
	CPUVAL( "sysenter",        3, 11, 1 );
	CPUVAL( "mtr_reg",         3, 12, 1 );
	CPUVAL( "pte_global",      3, 13, 1 );
	CPUVAL( "mca",             3, 14, 1 );
	CPUVAL( "cmov",            3, 15, 1 );
	CPUVAL( "page_attr_tbl",   3, 16, 1 );
	CPUVAL( "pate_size_ext",   3, 17, 1 );
	CPUVAL( "proc_serial_num", 3, 18, 1 );
	CPUVAL( "cflush",          3, 19, 1 );
	CPUVAL( "debug_store",     3, 21, 1 );
	CPUVAL( "thclk",           3, 22, 1 );
	CPUVAL( "mmx",             3, 23, 1 );
	CPUVAL( "fxsave",          3, 24, 1 );
	CPUVAL( "sse",             3, 25, 1 );
	CPUVAL( "sse2",            3, 26, 1 );
	CPUVAL( "self_snoop",      3, 27, 1 );
	CPUVAL( "hyper_threading", 3, 28, 1 );
	CPUVAL( "thermal_mon",     3, 29, 1 );
	CPUVAL( "x64",             3, 30, 1 );
	CPUVAL( "pending_brk_en",  3, 31, 1 );

	os::CSysUtil::i_cpuid( ci, 0x80000000 );
	ids = ci[ 0 ] - 0x80000000;
	if ( 0 >= ids )
		return 1;

	(*pb)[ oexT( "exids" ) ] = ids;

	os::CSysUtil::i_cpuid( ci, 0x80000001 );
	r = &(*pb)[ oexT( "ext" ) ];

	CPUVAL( "lahf",           2,  0, 1 );
	CPUVAL( "cmp_legacy",     2,  1, 1 );
	CPUVAL( "secure_vm",      2,  2, 1 );
	CPUVAL( "ext_apic_reg",   2,  3, 1 );
	CPUVAL( "movcr8",         2,  4, 1 );
	CPUVAL( "lzcnt",          2,  5, 1 );
	CPUVAL( "sse4a",          2,  6, 1 );
	CPUVAL( "ma_sse",         2,  7, 1 );
	CPUVAL( "3dnow_prefetch", 2,  8, 1 );
	CPUVAL( "osvw",           2,  9, 1 );
	CPUVAL( "ibs",            2, 10, 1 );
	CPUVAL( "xop",            2, 11, 1 );
	CPUVAL( "skinit",         2, 12, 1 );
	CPUVAL( "wdt",            2, 13, 1 );
	CPUVAL( "lwp",            2, 15, 1 );
	CPUVAL( "fma4",           2, 16, 1 );
	CPUVAL( "nodeid",         2, 19, 1 );
	CPUVAL( "tbm",            2, 21, 1 );
	CPUVAL( "topex",          2, 22, 1 );

	CPUVAL( "x87_fpu",        3,  0, 1 );
	CPUVAL( "vme",            3,  1, 1 );
	CPUVAL( "debug_ext",      3,  2, 1 );
	CPUVAL( "page_size_ext",  3,  3, 1 );
	CPUVAL( "ts_counter",     3,  4, 1 );
	CPUVAL( "msr",            3,  5, 1 );
	CPUVAL( "pae",            3,  6, 1 );
	CPUVAL( "mce",            3,  7, 1 );
	CPUVAL( "cmpxchg8b",      3,  8, 1 );
	CPUVAL( "apic",           3,  9, 1 );
	CPUVAL( "syscall",        3, 11, 1 );
	CPUVAL( "mtrr",           3, 12, 1 );
	CPUVAL( "pge",            3, 13, 1 );
	CPUVAL( "mca",            3, 14, 1 );
	CPUVAL( "cmov",           3, 15, 1 );
	CPUVAL( "page_attr_tbl",  3, 16, 1 );
	CPUVAL( "pse36",          3, 17, 1 );
	CPUVAL( "nx",             3, 20, 1 );
	CPUVAL( "mmxext",         3, 22, 1 );
	CPUVAL( "mmx",            3, 23, 1 );
	CPUVAL( "fxsr",           3, 24, 1 );
	CPUVAL( "ffxsr",          3, 25, 1 );
	CPUVAL( "page_1gb",       3, 26, 1 );
	CPUVAL( "rdtscp",         3, 27, 1 );
	CPUVAL( "long_mode",      3, 29, 1 );
	CPUVAL( "3dnow_ext",      3, 30, 1 );
	CPUVAL( "3dnow",          3, 31, 1 );

	os::CSysUtil::i_cpuid( ci, 0x80000002 );
	csi[ 0 ] = ci[ 0 ]; csi[ 1 ] = ci[ 1 ];
	csi[ 2 ] = ci[ 2 ]; csi[ 3 ] = ci[ 3 ];

	os::CSysUtil::i_cpuid( ci, 0x80000003 );
	csi[ 4 ] = ci[ 0 ]; csi[ 5 ] = ci[ 1 ];
	csi[ 6 ] = ci[ 2 ]; csi[ 7 ] = ci[ 3 ];

	os::CSysUtil::i_cpuid( ci, 0x80000004 );
	csi[ 8 ] = ci[ 0 ]; csi[ 9 ] = ci[ 1 ];
	csi[ 10 ] = ci[ 2 ]; csi[ 11 ] = ci[ 3 ];

	csi[ 12 ] = 0;

	(*pb)[ oexT( "brand" ) ] = oex::CStr( oexMbToStr( cs ) ).TrimWhiteSpace();

	os::CSysUtil::i_cpuid( ci, 0x80000004 );

	return 1;
}