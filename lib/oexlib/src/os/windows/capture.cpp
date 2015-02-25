/*------------------------------------------------------------------
// capture.cpp
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

#ifndef OEX_NOVIDEO

#if !defined( OEX_NOVFW )
// *** Video For Windows
#include <vfw.h>
#include "stdio.h"

#endif

#if !defined( OEX_NODSHOW )

// Just to make things a little easier
#include "comptr.h"
#include <string>

// *** strmiids.lib
// Problem: Fatal error LNK1103: debugging information corrupt.
// MS changed from V8-V9, get hot fix
// http://support.microsoft.com/kb/949009/

#endif
#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;
using namespace OEX_NAMESPACE::vid;

#ifndef OEX_NOVIDEO

// *** DirectShow
#	if !defined( OEX_NODSHOW )
#		include <comdef.h>
#		include <dshow.h>
#		include <streams.h>
#		include "vfw2.hpp"
#	endif

#	if !defined( OEX_NOVFW )
#		include "vfw1.hpp"
#	endif

#endif

CCapture::CCapture()
{
	m_uType = oexVIDSUB_AUTO;
	m_pDevice = oexNULL;
}

/// Destructor
CCapture::~CCapture()
{
	Destroy();
}

oexINT CCapture::GetDevices( oexUINT x_uType, oex::CPropertyBag *pList )
{
#ifdef OEX_NOVIDEO
	return 0;
#else

	if ( !pList )
		return 0;

	switch( x_uType )
	{

			case oexVIDSUB_AUTO :
#if !defined( OEX_NODSHOW )
			case oexVIDSUB_DSHOW :

				if ( !CDsCapture::EnumFilters( pList, CLSID_VideoInputDeviceCategory ) )
					return pList->Size();

				if ( x_uType != oexVIDSUB_AUTO )
					return 0;
#endif

#if !defined( OEX_NOVFW )
			case oexVIDSUB_VFW :
				return CVfwCap::GetDevices( x_uType, pList );
				break;
#endif
			default :
				break;

	} // end switch

	return 0;

#endif
}

oexBOOL CCapture::Destroy()
{
#ifdef OEX_NOVIDEO
	return oexFALSE;
#else

	/// Lose device if any
	if ( oexCHECK_PTR( m_pDevice ) )
	{
		switch( m_uType )
		{
#if !defined( OEX_NOVFW )
			case oexVIDSUB_VFW :
				OexAllocDestruct( (CV4w1*)m_pDevice );
				break;
#endif

#if !defined( OEX_NODSHOW )
			case oexVIDSUB_DSHOW :
				OexAllocDestruct( (CV4w2*)m_pDevice );
				break;
#endif
			default :
				return oexFALSE;

		} // end switch

#if defined( OEX_WINDOWS )

	// In Windows, you can't immediately reopen the capture device,
	// so this is just to make sure no one tries
	oexSleep( 1000 );

#endif

	} // end if

	m_uType = oexVIDSUB_AUTO;
	m_pDevice = oexNULL;

	return oexTRUE;
#endif
}

oexBOOL CCapture::Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
{
#ifdef OEX_NOVIDEO
	return oexFALSE;
#else
	// Lose previous device
	Destroy();

	// Allocate a new capture device
	switch( x_uType )
	{
		case oexVIDSUB_AUTO :

#if !defined( OEX_NODSHOW )

			m_pDevice = OexAllocConstruct< CV4w2 >();
			if ( !oexCHECK_PTR( m_pDevice ) )
			{	Destroy();
				return oexFALSE;
			} // end if

			// Try VFW2
			if ( m_pDevice->Open( x_uType, x_uDevice, x_uSource, x_nWidth, x_nHeight, x_uFormat, x_fFps, x_bInit ) )
			{	m_uType = oexVIDSUB_DSHOW;
				return oexTRUE;
			} // end if

			OexAllocDelete< CV4w2 >( (CV4w2*)m_pDevice );
			m_pDevice = oexNULL;
#endif

#if !defined( OEX_NOVFW )
		case oexVIDSUB_VFW :
			m_uType = oexVIDSUB_VFW;
			m_pDevice = OexAllocConstruct< CV4w1 >();
			break;
#endif

#if !defined( OEX_NODSHOW )
		case oexVIDSUB_DSHOW :
			m_uType = oexVIDSUB_DSHOW;
			m_pDevice = OexAllocConstruct< CV4w2 >();
			break;
#endif

		default :
			break;

	} // end switch
	
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Attempt to open the capture device
	if ( !m_pDevice->Open( x_uType, x_uDevice, x_uSource, x_nWidth, x_nHeight, x_uFormat, x_fFps, x_bInit ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
#endif
}

oexBOOL CCapture::Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
{
#ifdef OEX_NOVIDEO
	return oexFALSE;
#else
	// Lose previous device
	Destroy();

	// Allocate a new capture device
	switch( x_uType )
	{
		case oexVIDSUB_AUTO :

#if !defined( OEX_NODSHOW )

			m_pDevice = OexAllocConstruct< CV4w2 >();
			if ( !oexCHECK_PTR( m_pDevice ) )
			{	Destroy();
				return oexFALSE;
			} // end if

			// Try VFW2
			if ( S_OK == m_pDevice->Open( x_uType, x_pFile, x_nWidth, x_nHeight, x_uFormat, x_fFps, x_bInit ) )
			{	m_uType = oexVIDSUB_DSHOW;
				return oexTRUE;
			} // end if

			OexAllocDelete< CV4w2 >( (CV4w2*)m_pDevice );
			m_pDevice = oexNULL;

#endif

#if !defined( OEX_NOVFW )
		case oexVIDSUB_VFW :
			m_uType = x_uType;
			m_pDevice = OexAllocConstruct< CV4w1 >();
			break;
#endif

#if !defined( OEX_NODSHOW )
		case oexVIDSUB_DSHOW :
			m_uType = x_uType;
			m_pDevice = OexAllocConstruct< CV4w2 >();
			break;
#endif

		default :
			break;

	} // end switch

	if ( !oexCHECK_PTR( m_pDevice ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Attempt to open the capture device
	if ( !m_pDevice->Open( x_uType, x_pFile, x_nWidth, x_nHeight, x_uFormat, x_fFps, x_bInit ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;

#endif
}

