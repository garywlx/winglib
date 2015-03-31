/*------------------------------------------------------------------
// file.h
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
// CFile
//
/// File access
/**

*/
//==================================================================
class CFile
{
public:

	/// Size type
	typedef oexFILESIZE_T t_size;

public:

	/// Restores the file position when the class is destroyed
	class CRestoreFilePos
	{
	public:

		/// Constructor
		CRestoreFilePos( CFile *x_pF )
		{	m_pf = x_pF;
			m_llPos = ( m_pf ) ? x_pF->GetPtrPos() : 0;
		}

		/// Destructor
		~CRestoreFilePos()
		{	Restore(); }

		/// Restores file position
		oexBOOL Restore()
		{	CFile *pF = m_pf;
			m_pf = oexNULL;
			if ( !pF || !pF->IsOpen() )
				return oexFALSE;
			pF->SetPtrPosBegin( m_llPos );
			return oexTRUE;
		}

		/// Returns the stored set point
		oexINT64 Get() { return m_llPos; }

		/// Sets the restore point
		oexINT64 Set( oexINT64 llPos ) { return m_llPos = llPos; }

		/// Sets the file handle
		void SetFileObject( CFile *x_pF ) { m_pf = x_pF; }

		/// Returns the file handle
		CFile* GetFileObject() { return m_pf; }

		/// Cancels the restore
		void Cancel() {	m_pf = oexNULL; }

	private:

		/// File handle
		CFile			*m_pf;

		/// Previous position
		oexINT64		m_llPos;
	};


public:

    /// Default constructor
    CFile()
    {
        // Start with an invalid file handle
        m_hFile = os::CBaseFile::vInvalid();

        m_nLastError = 0;

    }

    /// Destructor
    ~CFile()
    {
    	Destroy();
    }

    /// Creates a new file, fails if file already exists
    CFile& CreateNew(      oexCSTR x_pFile,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite,
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisCreateNew, x_eAccess, x_eShare ); }

    /// Creates a new file, truncates any existing file
    CFile& CreateAlways(   oexCSTR x_pFile,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite,
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisCreateAlways, x_eAccess, x_eShare ); }

    /// Opens an existing file, fails if file doesn't exist
    CFile& OpenExisting(   oexCSTR x_pFile,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite,
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisOpenExisting, x_eAccess, x_eShare ); }

    /// Opens an existing file, creates if file doesn't exist
    CFile& OpenAlways(     oexCSTR x_pFile,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite,
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisOpenAlways, x_eAccess, x_eShare ); }

    /// Creates file
    CFile& Create(     oexCSTR x_pFile, oexUINT x_eDisposition,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite,
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   Destroy();
        m_hFile = os::CBaseFile::Create( x_pFile, x_eDisposition, x_eAccess, x_eShare, 0, &m_nLastError );
        if ( os::CBaseFile::vInvalid != m_hFile ) m_sFileName = x_pFile;
        return *this;
    }

    /// Closes the file
    oexBOOL Close() { return Destroy(); }

    /// Closes the file
    oexBOOL Destroy();

    /// Returns the file size
    t_size Size() { return os::CBaseFile::Size( m_hFile ); }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from file
	/**
		\param [out] x_pBuf			-	Receives data read from file
		\param [in]  x_llSize		-	Size of buffer in x_pBuf
		\param [out] x_pllRead		-	Receives number of bytes read

		\return Non-zero if success

		\see
	*/
    oexBOOL Read( oexPVOID x_pBuf, t_size x_llSize, t_size *x_pllRead = oexNULL )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return oexFALSE;
        return os::CBaseFile::Read( m_hFile, x_pBuf, x_llSize, x_pllRead, &m_nLastError );
    }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from file
	/**
		\param [in]  x_uSize		-	Number of bytes to read

		\return CStr object containing the data

		\see
	*/
    CStr8 Read( t_size x_llSize = 0 );

	//==============================================================
	// Write()
	//==============================================================
	/// Writes data to file
	/**
		\param [in]  x_pBuf			-	Data to write
		\param [in]  x_uSize		-	Number of bytes to write
		\param [out] x_puWritten    -	Receives number of bytes written

		\return Non-zero if success

		\see
	*/
    oexBOOL Write( oexCPVOID x_pBuf, t_size x_llSize, t_size *x_pllWritten = oexNULL )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return oexFALSE;
        return os::CBaseFile::Write( m_hFile, x_pBuf, x_llSize, x_pllWritten, &m_nLastError );
    }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_sStr			-	Data to write

		\return Non-zero if success

		\see
	*/
    oexBOOL Write( oexCONST CStr8 x_sStr, t_size *x_pllWritten = oexNULL )
    {   return Write( x_sStr.Ptr(), x_sStr.LengthInBytes(), x_pllWritten );
    }

#if !defined( OEX_NOWCHAR )

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_sStr			-	Data to write

		\return Non-zero if success

		\see
	*/
    oexBOOL Write( oexCONST CStrW x_sStr, t_size *x_pllWritten = oexNULL )
    {   return Write( CStr8().ToMb( x_sStr ), x_pllWritten );
    }

#endif

	//==============================================================
	// Flush()
	//==============================================================
	/// Syncs file memory cache with disk
	/**
		\return Non-zero if success

		\see
	*/
    oexBOOL Flush()
    {   return os::CBaseFile::Flush( m_hFile );}

    /// File write operator
    CFile& operator << ( CStr8 x_sStr ) { Write( x_sStr ); return *this; }

    /// Returns the file name
    CStr GetFileName() { return m_sFileName; }


	//==============================================================
	// GetPtrPos()
	//==============================================================
	/// Returns the current 64-bit file pointer position
	/**
		\return 64-bit file pointer position

		\see
	*/
	t_size GetPtrPos()
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return -1;
        return os::CBaseFile::SetPointer( m_hFile, 0, os::CBaseFile::eFileOffsetCurrent );
    }

	//==============================================================
	// SetPtrPos()
	//==============================================================
	/// Sets the 64-bit file pointer position
	/**
		\param [in] x_llPos	    -	64-bit file offset pointer
		\param [in] x_nMethod	-	Relative specifier for offset

		\return Non-zero if success

		\see
	*/
    t_size SetPtrPos( t_size x_llPos, oexINT x_nMethod )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return -1;
        return os::CBaseFile::SetPointer( m_hFile, x_llPos, x_nMethod );
    }

    //==============================================================
	// SetPtrPosBegin()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the beginning of the file
	/**
		\param [in] offset	-	64-bit file offset pointer

		\return Non-zero if success

		\see
	*/
    t_size SetPtrPosBegin( t_size x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetBegin ); }

	//==============================================================
	// SetPtrPosEnd()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the end of the file
	/**
		\param [in] x_llPos	-	64-bit file offset pointer

		\return Non-zero if success

		\see
	*/
    t_size SetPtrPosEnd( t_size x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetEnd ); }

	//==============================================================
	// SetPtrPosCur()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the current file pointer
	/**
		\param [in] x_llPos	-	64-bit file offset pointer

		\return Non-zero if success

		\see
	*/
    t_size SetPtrPosCur( t_size x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetCurrent ); }

    /// Returns non-zero if the specified file is open
    oexBOOL IsOpen() { return os::CBaseFile::vInvalid() != m_hFile; }

    /// Closes and deletes the current file
    oexBOOL Delete()
    {   if ( !IsOpen() )
            return oexFALSE;
        CStr s = GetFileName();
        Destroy();
        return Delete( s.Ptr() );
    }

	/// Returns the last error code
	oexINT GetLastError()
	{	return m_nLastError; }

	/// Finds the specified string of characters in the file
	/**
		\param [in]	x_pStr	- Pointer to the sequence of characters to find
		\param [in] x_llLen	- The number of characters in x_pStr
		\param [in] x_llMax	- The maximum number of characters to search from
							  the current file pointer position.  If set to
							  zero or less, search continues until the end of
							  the file is reached.

		\return If the string is found, the function returns the offset
		        of the string from the start of the file and sets the file
				pointer to the start of the string.  If the string is not found
				the function returns -1 and the file pointer is not changed.
	*/
	t_size FindInFile( oexPVOID x_pStr, t_size x_llLen, t_size x_llMax = 0 );

public:

    /// Deletes the specified file
    static oexBOOL Delete( oexCSTR x_pFile )
    {   return os::CBaseFile::Delete( x_pFile ); }

    /// Deletes a path
    static oexBOOL DeletePath( oexCSTR x_pPath, oexBOOL x_bEmpty = oexFALSE );

    /// Creates a path
    static oexBOOL CreatePath( oexCSTR x_pPath );

    /// Returns non-zero if the path exists
    static oexBOOL Exists( oexCSTR x_pPath )
    {   return os::CBaseFile::DoesExist( x_pPath ); }

    /// Renames the specified file
    static oexBOOL Rename( oexCSTR x_pOld, oexCSTR x_pNew )
    {   return os::CBaseFile::Rename( x_pOld, x_pNew ); }

    /// Renames the specified file
    static oexBOOL Copy( oexCSTR x_pOld, oexCSTR x_pNew )
    {   return os::CBaseFile::Copy( x_pOld, x_pNew ); }

	/// Returns the contents of a file
	static CStr8 GetContents( oexCSTR x_pFile )
	{	if ( !x_pFile || !*x_pFile ) return CStr8();
		return oex::CFile().OpenExisting( x_pFile ).Read();
	}

	/// Removes folder, folder must be empty
    static oexBOOL RemoveFolder( oexCSTR x_pDir )
	{	return os::CBaseFile::RemoveFolder( x_pDir );
	}
	
	/// Writes the data to a file
	static oexBOOL PutContents( oexCSTR x_pFile, const CStr8 &sData )
	{	if ( !x_pFile || !*x_pFile ) return oexFALSE;
		return oex::CFile().CreateAlways( x_pFile ).Write( sData ); 
	}

	/// Appends a string to the end of a file
	static t_size AppendContents( const CStr &sFile, const CStr &sData )
	{	oex::CFile f;
		if ( !f.OpenAlways( sFile.Ptr() ).IsOpen() )
			return 0;
		f.SetPtrPosEnd( 0 );
		return f.Write( sData.Ptr(), sData.Length() );
	}

	/// Appends binary data to the end of a file
	static t_size AppendContents( const CStr &sFile, CBin &bin )
	{	if ( !bin.getUsed() ) return 0;
		oex::CFile f;
		if ( !f.OpenAlways( sFile.Ptr() ).IsOpen() )
			return 0;
		f.SetPtrPosEnd( 0 );
		return f.Write( bin.Ptr(), bin.getUsed() );
	}


private:

    /// File handle
    os::CBaseFile::t_HFILE              m_hFile;

    /// Name of the open file
    CStr                                m_sFileName;

    /// Last error value
    oexINT                              m_nLastError;

};

