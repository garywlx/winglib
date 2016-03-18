/*------------------------------------------------------------------
// mem_leak.cpp
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

CMemLeak::CMemLeak()
{
#if defined( OEX_MEMLEAK_DEBUG )
	m_nDuplicateAdds = 0;
#endif

	m_nPoolOverflows = 0;
	m_uTotalAllocations = 0;
	m_uCurrentAllocations = 0;
	m_uPeakAllocations = 0;
	m_nPoolBits = 0;
	m_nPoolMask = 0;
	m_nPoolSize = 0;
	m_pPool = oexNULL;
	m_bFreeze = oexFALSE;
}

CMemLeak::~CMemLeak()
{
	Destroy();
}

void CMemLeak::Destroy()
{
	m_nPoolBits = 0;
	m_nPoolMask = 0;
	m_nPoolSize = 0;

	if ( m_pPool )
	{
		SRawAllocator sa = CMem::GetDefaultAllocator();
		if ( sa.fFree )
			sa.fFree( m_pPool );
		m_pPool = oexNULL;
	} // end if

#if defined( OEX_MEMLEAK_DEBUG )
	m_nDuplicateAdds = 0;
#endif

	m_nPoolOverflows = 0;
	m_uPeakAllocations = 0;
	m_uTotalAllocations = 0;
	m_uCurrentAllocations = 0;

	m_bFreeze = oexFALSE;

}

oexBOOL CMemLeak::Create( t_size x_nPoolBits )
{
	Destroy();

	// Ensure valid pool bits value
	if ( x_nPoolBits < eDefaultPoolBits )
		x_nPoolBits = eDefaultPoolBits;

	// Number of bits in the pool
	m_nPoolBits = x_nPoolBits;

	// Get the pool mask
	m_nPoolMask = cmn::BitsToMask( m_nPoolBits );

	// What's the pool size?
	m_nPoolSize = cmn::BitsToValue( m_nPoolBits );

	// Get the default allocator
	SRawAllocator sa = CMem::GetDefaultAllocator();
	if ( !sa.fMalloc )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Allocate pool
	m_pPool = (oexCPVOID*)sa.fMalloc( m_nPoolSize * sizeof( oexCPVOID ) );
	if ( !m_pPool )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Initialize the pool
	oexZeroMemory( m_pPool, m_nPoolSize * sizeof( oexCPVOID ) );

	return oexTRUE;
}

oexINT CMemLeak::Add( oexCPVOID p )
{
	if ( !m_pPool || !p || m_bFreeze )
		return -1;

	// Calculate starting offset
	t_size uOffset = ( oexPtrToLong( p ) >> 4 ) & m_nPoolMask;
	t_size uStart = uOffset;

	do
	{
		// +++ There's actually a threading problems here that could
		//     cause a few random pointers to get annihilated, along
		//     with other issues, it's not worth a lock, this is only 
		//     for debugging.

		// Can we save it here?
		if ( !m_pPool[ uOffset ] )
		{
			// Save pointer
			m_pPool[ uOffset ] = p;

			// Reduce impact from thread conflicts
			if ( m_pPool[ uOffset ] == p )
			{
				// Track numbers
				m_uTotalAllocations++;
				m_uCurrentAllocations++;
				if ( m_uCurrentAllocations > m_uPeakAllocations )
					m_uPeakAllocations = m_uCurrentAllocations;

				return 0;

			} // end if

		} // end if

#if defined( OEX_MEMLEAK_DEBUG )

		// Check for duplicate
		else if ( m_pPool[ uOffset ] == p )
		{
			// Count a duplicate attempt
			m_nDuplicateAdds++;

			oexEcho( oexT( "CMemLeak : Duplicate pointer added" ) );

			return -1;

		} // end else if

#endif

		// Calculate next offset
		uOffset = ( uOffset + 1 ) & m_nPoolMask;

	} while ( uStart != uOffset );

	// Count a pool overflow
	m_nPoolOverflows++;

#if defined( OEX_MEMLEAK_DEBUG )

	// This means leak detection isn't working for this pointer
	oexEcho( oexT( "CMemLeak : Tracking pool is full!" ) );

#endif

	return -1;
}

oexINT CMemLeak::Check( oexCPVOID p )
{
	if ( !m_pPool || !p )
		return -1;

	// Calculate starting offset
	t_size uOffset = ( oexPtrToLong( p ) >> 4 ) & m_nPoolMask;

	t_size uStart = uOffset;
	do
	{
		// Is this the one we're looking for?
		if ( m_pPool[ uOffset ] == p )

			return (oexINT)uOffset;

		// Calculate next offset
		uOffset = ( uOffset + 1 ) & m_nPoolMask;

	} while ( uStart != uOffset );

	return -1;
}

oexINT CMemLeak::Remove( oexCPVOID p )
{
	if ( !m_pPool || !p || m_bFreeze )
		return -1;

	// Calculate starting offset
	t_size uOffset = ( oexPtrToLong( p ) >> 4 ) & m_nPoolMask;

	t_size uStart = uOffset;
	do
	{
		// Is this the one we're looking for?
		if ( m_pPool[ uOffset ] == p )
		{
			// Remove from pool
			m_pPool[ uOffset ] = 0;

			// Track allocations
			if ( m_uCurrentAllocations )
				m_uCurrentAllocations--;
#if defined( OEX_MEMLEAK_DEBUG )
			else
				oexEcho( oexT( "CMemLeak : m_uCurrentAllocations wants to go negative?" ) );
#endif

			return (oexINT)uOffset;

		} // end if

		// Calculate next offset
		uOffset = ( uOffset + 1 ) & m_nPoolMask;

	} while ( uStart != uOffset );

#if defined( OEX_MEMLEAK_DEBUG )
//	oexEcho( oexT( "CMemLeak : Block not found" ) );
//	oexASSERT( 0 );
#endif

	return -1;
}

CStr CMemLeak::Report( t_size *pLeaks )
{
	if ( !m_pPool )
		return oexT( "No Pool" );

	CStr s = oexMks( oexNL, oexT( "------------------ Memory Report --------------------" ), oexNL, oexNL,
					 oexT( "Total Allocations   : " ), m_uTotalAllocations, oexNL,
					 oexT( "Peak Allocations    : " ), m_uPeakAllocations, oexNL,
					 oexT( "Current Allocations : " ), m_uCurrentAllocations, oexNL,
					 oexT( "Pool Overflows      : " ), m_nPoolOverflows, oexNL,
#if defined( OEX_MEMLEAK_DEBUG )
					 oexT( "Duplicate Adds      : " ), m_nDuplicateAdds, oexNL,
#endif
					 oexNL
					 );


	t_size uOffset = 0;
    oexTCHAR szMsg[ 1024 * 16 ] = oexEMPTY_STRING;
	do
	{
		// Memory here?
		if ( m_pPool[ uOffset ] )
		{
			// New message
			*szMsg = 0;

			// Get the block report
			if ( !CAlloc::GetBlockReport( m_pPool[ uOffset ], 0, szMsg, sizeof( szMsg ) ) )
				s << oexFmt( oexT( "0x%x : <unrecognized block>" ), oexPtrToLong( m_pPool[ uOffset ] ) );

			// Append to string
			else
				s << oexFmt( oexT( "0x%x : " ), oexPtrToLong( m_pPool[ uOffset ] ) ) << szMsg << oexNL;

			// Track number of leaks
			if ( pLeaks )
				(*pLeaks)++;

		} // end if

		// Calculate next offset
		uOffset = ( uOffset + 1 ) & m_nPoolMask;

	// For each valid
	} while ( uOffset );

	s << oexNL << oexT( "---------------- End Memory Report ------------------" ) << oexNL;

	return s;
}
