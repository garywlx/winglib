/*------------------------------------------------------------------
// oex_typedefs.h
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

typedef long						oexRESULT;

#if defined( _MSC_VER )
typedef __int16                     oexINT16;
typedef unsigned __int16            oexUINT16;
typedef __int32						oexINT32;
typedef unsigned __int32			oexUINT32;
typedef __int64						oexINT64;
typedef unsigned __int64			oexUINT64;
#else
typedef short                       oexINT16;
typedef unsigned short              oexUINT16;
typedef int				 	        oexINT32;
typedef unsigned int    			oexUINT32;
typedef long long			   		oexINT64;
typedef unsigned long long		    oexUINT64;
#endif

#define oexMAXUINT16	( (OEX_NAMESPACE::oexUINT16) ~0 )
#define oexMAXUINT32	( (OEX_NAMESPACE::oexUINT32) ~0 )
#define oexMAXUINT64	( (OEX_NAMESPACE::oexUINT64) ~(0ULL) )

#define oexMAXINT16		( (OEX_NAMESPACE::oexINT16) ( oexMAXUINT16 >> 1 ) )
#define oexMAXINT32		( (OEX_NAMESPACE::oexINT32) ( oexMAXUINT32 >> 1 ) )
#define oexMAXINT64		( (OEX_NAMESPACE::oexINT64) ( oexMAXUINT64 >> 1 ) )

typedef int							oexINT;
typedef unsigned int				oexUINT;
typedef long						oexLONG;
typedef unsigned long				oexULONG;
typedef short						oexSHORT;
typedef unsigned short				oexUSHORT;
typedef float						oexFLOAT;
typedef double						oexDOUBLE;
typedef long double 				oexLONGDOUBLE;
typedef bool						oexBOOL;
typedef char						oexCHAR;
typedef unsigned char				oexUCHAR;

#define oexMAXUSHORT	oexMAXUINT16
#define oexMAXUINT		oexMAXUINT32

#define oexMAXSHORT		oexMAXINT16
#define oexMAXINT		oexMAXINT32

#if defined( OEX_CPU_64 )
#	define oexMAXULONG		oexMAXUINT64
#	define oexMAXLONG		oexMAXINT64
#else
#	define oexMAXULONG		oexMAXUINT32
#	define oexMAXLONG		oexMAXINT32
#endif

#define oexMINFLOAT			(1e-999)
#define oexMAXFLOAT			(9.999999999999999e999)

#define oexMINDOUBLE		(1e-999)
#define oexMAXDOUBLE		(9.999999999999999e999)

// String types
typedef char                        oexCHAR8;
typedef oexCHAR8 *                  oexSTR8;
typedef const oexCHAR8 *            oexCSTR8;
typedef short                       oexCHAR16;
typedef oexCHAR16 *                 oexSTR16;
typedef const oexCHAR16 *           oexCSTR16;
typedef long                        oexCHAR32;
typedef oexCHAR32 *                 oexSTR32;
typedef const oexCHAR32 *           oexCSTR32;
typedef wchar_t                     oexCHARW;
typedef oexCHARW *                  oexSTRW;
typedef const oexCHARW *            oexCSTRW;

#if defined( oexUNICODE )
	typedef wchar_t					oexTCHAR;
#else
	typedef char					oexTCHAR;
#endif

typedef oexTCHAR*					oexSTR;
typedef oexCONST oexTCHAR*			oexCSTR;

typedef void*						oexPVOID;
typedef void oexCONST *				oexCPVOID;

typedef unsigned char				oexBYTE;
typedef oexBYTE*					oexPBYTE;

typedef void*						oexTYPEOF_PTR;

/// Position type
typedef oexLONG						oexPos;

/// Size type
typedef oexULONG					oexSize;

struct oexTime
{
	/// Seconds
	oexUINT32		tv_sec;
	
	/// Micro seconds
	oexUINT32		tv_usec;	
};

#define OEXTIME_USECS_PER_SEC	1000000

template < typename T >
	oexUINT64 oexGetUSecs( const T &t )
	{	return oexUINT64( t.tv_sec ) * oexUINT64( OEXTIME_USECS_PER_SEC ) + oexUINT64( t.tv_usec ); }

template < typename T1, typename T2 >
	T1& oexSetTime( T1 &dst, const T2 &t )
	{	dst.tv_usec = oexUINT32( t % OEXTIME_USECS_PER_SEC );
		dst.tv_sec = oexUINT32( t / OEXTIME_USECS_PER_SEC );
		return dst; 
	}

template < typename T1, typename T2, typename T3 >
	T1& oexSetTime( T1 &dst, const T2 sec, const T3 usec )
	{	dst.tv_sec = sec;
		dst.tv_usec = usec;
		return dst;
	}

template < typename T1, typename T2 >
	T1& oexCopyTime( T1 &dst, const T2 &src )
	{	dst.tv_sec = src.tv_sec;
		dst.tv_usec = src.tv_usec;
		return dst;
	}

struct oexGUID
{
    union
    {
        struct
        {
            oexUINT32   Data1;
            oexUINT16   Data2;
            oexUINT16   Data3;
            oexUCHAR    Data4[ 8 ];
        };
		
		struct
		{
			oexUINT32	u1;
			oexUINT32	u2;
			oexUINT32	u3;
			oexUINT32	u4;
		};

        oexUCHAR        ucBuf[ 16 ];
    };

};


/// Finger saver ;)
/// This class can be used efficiently as a return type
class oexAUTOGUID : public oexGUID
{
public:

    /// Default constructor
    oexAUTOGUID() {}

    /// Copy constructor
    oexAUTOGUID( oexCONST oexGUID &r )
    {   Copy( r ); }

    /// Copy constructor
    oexAUTOGUID( oexCONST oexGUID *p )
    {   Copy( p ); }

    /// Copy constructor
    oexAUTOGUID( oexCONST oexAUTOGUID &r )
    {   Copy( (oexGUID&)r ); }

    /// Copy constructor
    oexAUTOGUID( oexCONST oexAUTOGUID *p )
    {   Copy( (oexGUID*)p ); }

	/// Create GUID from components
    oexAUTOGUID( oexUINT32 x_Data1, oexUINT16 x_Data2, oexUINT16 x_Data3,
                 oexUCHAR x_Data4_0, oexUCHAR x_Data4_1, oexUCHAR x_Data4_2, oexUCHAR x_Data4_3,
                 oexUCHAR x_Data4_4, oexUCHAR x_Data4_5, oexUCHAR x_Data4_6, oexUCHAR x_Data4_7 )
	{
		Data1 = x_Data1;
		Data2 = x_Data2;
		Data3 = x_Data3;
		Data4[ 0 ] = x_Data4_0;
		Data4[ 1 ] = x_Data4_1;
		Data4[ 2 ] = x_Data4_2;
		Data4[ 3 ] = x_Data4_3;
		Data4[ 4 ] = x_Data4_4;
		Data4[ 5 ] = x_Data4_5;
		Data4[ 6 ] = x_Data4_6;
		Data4[ 7 ] = x_Data4_7;
	}

    /// Copy operator
    oexAUTOGUID& operator = ( oexCONST oexGUID &r )
    {   return Copy( r ); }

    /// Copy operator
    oexAUTOGUID& operator = ( oexCONST oexGUID *p )
    {   return Copy( p ); }

    /// Copy operator
    oexAUTOGUID& operator = ( oexCONST oexAUTOGUID &r )
    {   return Copy( (oexGUID&)r ); }

    /// Copy operator
    oexAUTOGUID& operator = ( oexCONST oexAUTOGUID *p )
    {   return Copy( (oexGUID*)p ); }

    /// Copies guid
    oexAUTOGUID& Copy( oexCONST oexGUID &r )
    {   *(oexINT64*)this = *(oexINT64*)&r;
        ( (oexINT64*)this )[ 1 ] = ( (oexINT64*)&r )[ 1 ];
        return *this;
    }

    /// Copies guid
    oexAUTOGUID& Copy( oexCONST oexGUID *p )
    {   *(oexINT64*)this = *(oexINT64*)p;
        ( (oexINT64*)this )[ 1 ] = ( (oexINT64*)p )[ 1 ];
        return *this;
    }

    /// GUID reference
//  operator oexGUID& ()
//  {   return *this; }

    /// GUID pointer
    operator oexGUID* ()
    {   return this; }

};

