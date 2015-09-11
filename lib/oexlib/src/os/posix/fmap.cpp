/*------------------------------------------------------------------
// fmap.cpp
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

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Ensure size
oexSTATIC_ASSERT( sizeof( CFMap::t_HFILEMAP ) == sizeof( FILE* ) );

oexCONST CFMap::t_HFILEMAP CFMap::c_Failed = (oexCONST CFMap::t_HFILEMAP)-1;

CFMap::t_HFILEMAP CFMap::Create( CFMap::t_HFILEMAP x_hFile, oexPVOID *x_pMem, oexFILESIZE_T x_llSize, oexFILESIZE_T *x_pllSize, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexFILESIZE_T x_llOffset )
{
    // Sanity checks
    if ( !oexCHECK_PTR( x_pMem ) || CFMap::c_Failed == x_hFile )
	{	oexERROR( -1, oexT( "Bad parameter" ) );
        return CFMap::c_Failed;
	} // end if

	int fd = oexPtrToInt( x_hFile );
//	int fd = dup( (int)x_hFile );
	if ( 0 > fd )
	{	oexERROR( -1, oexT( "dup() returned error : unable to duplicate file handle" ) );
		return CFMap::c_Failed;
	} // end if

	oexPVOID pMem = mmap( oexNULL, x_llSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, x_llOffset );
	if ( c_Failed == pMem )
	{	oexERROR( errno, CStr().Fmt( oexT( "mmap failed to allocate block : handle=%d, size=%d, offset=%d" ), fd, (int)x_llSize, (int)x_llOffset ) );
		close( fd );
		return oexNULL;
	} // end if

	if ( x_pMem )
		*x_pMem = pMem;

	if ( x_pllSize )
		*x_pllSize = x_llSize;

	return (t_HFILEMAP)fd;
}

// +++ Total hack, replace soon please!!!
oexLock						g_lstFileMappingInfoLock;
TAssoList< oexINT, CStr8 > 	g_lstFileMappingInfo;

CFMap::t_HFILEMAP CFMap::Create( oexCSTR x_pFile, oexPVOID *x_pMem, oexFILESIZE_T x_llSize, oexFILESIZE_T *x_pllSize, oexCSTR x_pName, etAccess x_eAccess, oexBOOL *x_pbAlreadyExists, oexFILESIZE_T x_llOffset )
{

// +++ Need to get some equivalent on the arm
#ifndef OEX_NOSHM

    // Sanity checks
    if ( !oexCHECK_PTR( x_pMem ) || !oexCHECK_PTR_NULL_OK( x_pName ) )
	{	oexERROR( -1, oexT( "Bad parameter" ) );
        return CFMap::c_Failed;
	} // end if

	// Create path
	CStr8 sPath;

	if ( x_pFile )
		sPath = oexStrToMb( x_pFile );

	else if ( x_pName )
	{
		// Convert name to multi-byte
		CStr8 sName = oexStrToMb( x_pName );
		if ( !sName.Length() )
		{	oexERROR( -1, oexT( "file mapping name is zero length" ) );
			return CFMap::c_Failed;
		} // end if

		// Attempt to allocate space
		oexSTR8 pPath = sPath.OexAllocate( sName.Length() + PATH_MAX * 2 );
		if ( !pPath )
		{	oexERROR( errno, oexT( "Error allocating memory for file path string" ) );
			return CFMap::c_Failed;
		} // end if

		// Attempt to create a temporary file
//		zstr::Copy( pPath, oexT( "/tmp/oex.tmp.XXXXXXXX" ) );

		// Make a temporary file #406080
//		mkstemp( sPath._Ptr() );

		// Ensure temp path exists
//		if ( !oexExists( oexTmpPath ) )
//		{	oexCreatePath( oexTmpPath );
//			if ( !oexExists( oexTmpPath ) )
//			{	oexERROR( -1, oexT( "Invalid Temp path : " oexTmpPath ) );
//				return CFMap::c_Failed;
//			} // end else
//		} // end if
			
		// Attempt to build a file name
//		zstr::Copy( pPath, oexTmpPath "/027ed44e-1208-4a38-9a48-9f97df77250b." );

        zstr::Copy( pPath, "/.oex.shared." );
		zstr::Append( pPath, oexMd5( sName ).Ptr() );

	} // end if

	else
	{	oexERROR( -1, oexT( "neither valid file or name specified" ) );
		return CFMap::c_Failed;
	} // end else

	// Assume doesn't exist
	oexBOOL bExists = oexFALSE;

	// Attempt to open existing file
	int fd = shm_open( sPath.Ptr(), O_RDWR, 0777 );
	if ( oexPtrToInt( c_Failed ) == fd )
	{
		// Create file
		fd = shm_open( sPath.Ptr(), O_RDWR | O_CREAT, 0777 );
		if ( oexPtrToInt( c_Failed ) == fd )
		{	oexERROR( errno, CStr().Fmt( oexT( "shm_open failed : %s" ), sPath.Ptr() ) );
			return CFMap::c_Failed;
		} // end if

		if ( -1 == ftruncate( fd, x_llSize ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "ftruncate failed : Unable to size shared memory file : %s : size=%d" ), sPath.Ptr(), (int)x_llSize ) );
			shm_unlink( sPath.Ptr() );
			return CFMap::c_Failed;
		} // end if

	} // end if

	// File already existed
	else
		bExists = oexTRUE;

	// Map memory
	oexPVOID pMem = mmap( oexNULL, x_llSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, x_llOffset );
//	oexPVOID pMem = mmap( oexNULL, x_llSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, x_llOffset );
	if ( c_Failed == pMem )
	{	oexERROR( errno, CStr().Fmt( oexT( "mmap failed to allocate block : size=%d, fd=%d, offset=%d, file=%s" ), (int)x_llSize, (int)fd, (int)x_llOffset, sPath.Ptr() ) );
		shm_unlink( sPath.Ptr() );
		return CFMap::c_Failed;
	} // end if

	// Ensure ref count is not zero for existing memory
//	if ( bExists && !CAlloc::GetRefCount( pMem ) )
//		bExists = oexFALSE;

	// Go ahead and unlink
//	shm_unlink( sPath.Ptr() );

    // +++ This whole thing is kind of a hack, need to find a way to unlink
    //     after the last reference dissappears, you know, like Windows ;)
    // We will be the one to release it
    if ( !bExists )
    {
        // Add file mapping name to list
        oexAutoLock ll( g_lstFileMappingInfoLock );
        if ( ll.IsLocked() )
            g_lstFileMappingInfo[ fd ] = sPath;

    } // end if

	if ( x_pMem )
		*x_pMem = pMem;

	if ( x_pllSize )
		*x_pllSize = x_llSize;

	// Save exists flag
	if ( x_pbAlreadyExists )
		*x_pbAlreadyExists = bExists;

	return (t_HFILEMAP)fd;
#else

	oexERROR( 0, oexT( "Not implemented" ) );

	return CFMap::c_Failed;

#endif
}

oexBOOL CFMap::Release( CFMap::t_HFILEMAP x_hFileMap, oexPVOID x_pMem, oexFILESIZE_T x_llSize )
{
	// Do we have a valid memory pointer
	if ( x_pMem && oexCHECK_PTR( x_pMem ) )

		// Unmap the memory
		munmap( x_pMem, x_llSize );

// +++ Need to get some equivalent on the arm
#ifndef OEX_NOSHM

    // Ensure valid file handle
    if ( c_Failed == x_hFileMap )
        return oexFALSE;

    // Lock the list
    oexAutoLock ll( g_lstFileMappingInfoLock );
    if ( ll.IsLocked() && g_lstFileMappingInfo.IsKey( oexPtrToInt( x_hFileMap ) ) )
    {
        // Attempt to retrieve path
        CStr8 sPath = g_lstFileMappingInfo[ oexPtrToInt( x_hFileMap ) ];

        // Unlink this name
        if ( sPath.Length() )
            shm_unlink( sPath.Ptr() );

        // Lose this mapping
        g_lstFileMappingInfo.Unset( oexPtrToInt( x_hFileMap ) );

    } // end if

// +++ Do we need to release the file handle here?
    close( (int)(long)x_hFileMap );

#endif

	return oexTRUE;
}
