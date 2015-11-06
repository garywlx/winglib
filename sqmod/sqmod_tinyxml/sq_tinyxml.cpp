
// sq_tinyxml.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqXml, CSqXml )
	SQBIND_STATIC_FUNCTION( CSqXml, Encode )
	SQBIND_STATIC_FUNCTION( CSqXml, Decode )
SQBIND_REGISTER_CLASS_END()

void CSqXml::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqXml );
}

CSqXml::CSqXml()
{_STT();
}

CSqXml::~CSqXml()
{_STT();
	Destroy();
}

void CSqXml::Destroy()
{_STT();
}

static int _Decode( sqbind::CSqMulti *out, TiXmlElement *in, int bIndexed, int nCaseI, int nDepth = 0 )
{_STT();

	// Sanity check
	if ( !out || !in || nDepth > CSqXml::eMaxParseDepth )
		return 0;

	int nDecoded = 0;
	int nIdx = 0;

	// Add child elements
	do
	{
		// Only element types
		if ( in->Type() != TiXmlNode::TINYXML_ELEMENT )
			continue;

		// Apply some sane limit
		int nMax = CSqXml::eMaxParseElements;

		// Grab the tag name
		const char *pName = in->Value();
		sqbind::stdString sName;
		if ( pName && *pName )
		{
			oex::CStr sT = oexMbToStr( pName );

			if ( nCaseI )
				sT.ToLower();
			
			sName = sqbind::oex2std( sT );
			
		} // end if
		
		sqbind::CSqMulti *r = oexNULL;
		if ( bIndexed )
		{
			// Use numeric index
			r = &(*out)[ oexMks( nIdx++ ).Ptr() ];

			// Save name if valid
			if ( sName.length() )
				(*r)[ oexT( "$" ) ] = sqbind::oex2std( oexMbToStr( oex::CStr8( pName ) ) );

		} // end if

		else
		{
			// Ensure valid name and that it exists
			if ( !sName.length() || (*out).isset( sName ) )
				continue;

			// Use name as index
			r = &(*out)[ sName ];

		} // end else

		if ( !r )
			continue;

		// First add the attributes
		TiXmlAttribute *pAttrib = in->FirstAttribute();
		while ( nMax-- && pAttrib )
		{
			// Get name and value
			const char *pAtt = pAttrib->Name();
			const char *pValue = pAttrib->Value();
			if ( pAtt && *pAtt && pValue && *pValue )
			{	oex::CStr sT = oexMbToStr( pAtt );
				if ( nCaseI )
					sT.ToLower();	
				sqbind::stdString sAtt = sqbind::oex2std( sT );
				if ( !r->isset( sAtt ) )
					nDecoded++, (*r)[ sAtt ] = sqbind::oex2std( oexMbToStr( oex::CStr8( pValue ) ) );
			} // end if

			// Next attribute
			pAttrib = pAttrib->Next();

		} // end while

		// Default value?
		const char *pText = in->GetText();
		if ( pText && *pText )
		{	nDecoded++;
			r->set( oexMbToStrPtr( pText ) );
		} // end if

		// Child elements?
		if ( in->FirstChildElement() )
			nDecoded += _Decode( r, in->FirstChildElement(), bIndexed, nCaseI, nDepth + 1 );

	} while ( ( in = in->NextSiblingElement() ) );

	return nDecoded;
}

int CSqXml::Decode( const sqbind::stdString &sData, sqbind::CSqMulti *pOut, int bIndexed, int nCaseI )
{_STT();

	if ( !sData.length() || !pOut )
		return 0;

	try
	{
		// Parse XML
		TiXmlDocument	xmlDoc;

#if !defined( oexUNICODE )
		if ( !xmlDoc.Parse( sData.c_str() ) )
#else
		if ( !xmlDoc.Parse( oexStrToMb( sqbind::std2oex( sData ) ).Ptr() ) )
#endif
			; // return 0;

		// Decode into array
		return _Decode( pOut, xmlDoc.FirstChildElement(), bIndexed, nCaseI );

	}
	catch( ... )
	{
		// Just flag an error
		oexERROR( 0, oexMks( oexT( "Exception while decoding XML string : " ), 
						sqbind::std2oex( sData ) ) );

	} // end catch

	return 0;
}

static int _Encode( TiXmlNode *pNode, sqbind::CSqMulti *pData, int bIndexed, int nDepth = 0 )
{_STT();
	if ( !pNode || !pData )
		return 0;

	// For each element
	for ( sqbind::CSqMulti::iterator it = pData->list().begin(); it != pData->list().end(); it++ )
	{
		// Key name?
		if ( it->first == oexT( "$" ) )
			; // Ignore

		else
		{
			// Is it just an attribute
			if ( nDepth && !it->second.size() )
			{
				((TiXmlElement*)pNode)->SetAttribute( oexStrToMbPtr( it->first.c_str() ), oexStrToMbPtr( it->second.str().c_str() ) );

			} // end else if

			// Handle nested tag
			else
			{
				TiXmlNode *pItem = oexNULL;

				if ( !bIndexed )
				{	if ( it->first.length() )
						pItem = pNode->InsertEndChild( TiXmlElement( oexStrToMbPtr( it->first.c_str() ) ) );
				} // end if

				else if ( it->second.isset( oexT( "$" ) ) && it->second[ oexT( "$" ) ].str().length() )
					pItem = pNode->InsertEndChild( TiXmlElement( oexStrToMbPtr( it->second[ oexT( "$" ) ].str().c_str() ) ) );

				// Did we get an item?
				if ( pItem )
				{
					// Default value?
					if ( it->second.str().length() )
						pItem->InsertEndChild( TiXmlText( oexStrToMbPtr( it->second.str().c_str() ) ) );

					// Encode sub items
					_Encode( pItem, &it->second, bIndexed, nDepth + 1 );

				} // end if

			} // end else if

		} // end if

	} // end for

	return 1;
}

sqbind::stdString CSqXml::Encode( sqbind::CSqMulti *pData,
								  const sqbind::stdString &sLineBreak,
								  const sqbind::stdString &sTab, int bIndexed )
{_STT();
	if ( !pData )
		return oexT( "" );

	// Encode the list into xml
	TiXmlDocument	xmlDoc;

	// Do the encode
	if ( !_Encode( &xmlDoc, pData, bIndexed ) )
		return oexT( "" );

	// Setup a printer
	TiXmlPrinter printer;
	printer.SetIndent( oexStrToMbPtr( sTab.c_str() ) );
	printer.SetLineBreak( oexStrToMbPtr( sLineBreak.c_str() ) );
	xmlDoc.Accept( &printer );

	// Return the encoded xml
	return oexMbToStrPtr( printer.CStr() );
}

