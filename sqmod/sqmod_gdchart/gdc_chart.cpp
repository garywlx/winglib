// gdc_chart.cpp

#include "stdafx.h"

#include "gdc.h"
#include "gdchart.h"
#include "gd1.3/gd.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CGdcChart, CGdcChart )

	SQBIND_MEMBER_FUNCTION( CGdcChart, SaveChart )
	SQBIND_MEMBER_FUNCTION( CGdcChart, CreateChart )
	SQBIND_MEMBER_FUNCTION( CGdcChart, CreateChartBin )

//	SQBIND_ENUM( CGdcChart::GDC_LINE, GDC_LINE )
//	SQBIND_ENUM( CGdcChart::GDC_AREA, GDC_AREA )
//	SQBIND_ENUM( CGdcChart::GDC_BAR, GDC_BAR )

SQBIND_REGISTER_CLASS_END()


#if defined( GD_CUSTOM_ALLOCATOR )
extern "C"
{
	void* gd_malloc( GD_size_t size )
	{
		return oex::CMem::New( size, oexLINE, oexTEXT( oexFILE ) );
	}

	extern "C" void* gd_calloc( GD_size_t num, GD_size_t size )
	{
		void *ptr = oex::CMem::New( num * size, oexLINE, oexTEXT( oexFILE ) );
		oexZeroMemory( ptr, num * size );
		return ptr;
	}

	extern "C" void* gd_realloc( void *ptr, GD_size_t size )
	{
		return oex::CMem::Resize( ptr, size, oexLINE, oexTEXT( oexFILE ) );
	}

	extern "C" void gd_free( void *ptr )
	{
		return oex::CMem::Delete( ptr );
	}

};
#endif

/// Unfortunately, the gdchart library is not thread safe
oexLock CGdcChart::m_lock;

void CGdcChart::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CGdcChart );
}

CGdcChart::CGdcChart()
{_STT();
}

CGdcChart::~CGdcChart()
{_STT();
}

#define MAX_DIMENSIONS		8

sqbind::CSqMulti CGdcChart::CreateChart( const sqbind::stdString &x_sType,
									  	 const sqbind::stdString &x_sParams,
									  	 const sqbind::stdString &x_sData,
									  	 sqbind::CSqImage *x_pImg )
{_STT();
	if ( !x_pImg )
		return oexT( "" );

	sqbind::CSqMulti mParams( x_sParams );
	sqbind::CSqMulti mData( x_sData );

	// Get image width / height
	int nWidth = mParams[ oexT( "width" ) ].toint();
	int nHeight = mParams[ oexT( "height" ) ].toint();
	int nType = mParams[ oexT( "type" ) ].toint();
	if ( 0 >= nWidth || 0 >= nHeight )
		return oexT( "" );

	int nDataPts = 0;
	oex::TMem< oex::oexCSTR > memLabels;
	oex::TMem< float > memPts[ MAX_DIMENSIONS + 1 ];

	int nDimensions = 0;
	for ( sqbind::CSqMulti::iterator it = mData.begin();
		  nDimensions < MAX_DIMENSIONS && mData.end() != it; it++ )
	{
		// Use smallest size
		if ( !nDataPts )
		{
			nDataPts = it->second[ oexT( "data" ) ].size();
			if ( !nDataPts )
			{//	oexERROR( 0, oexT( "No data" ) );
				return oexT( "" );
			} // end if

			if ( !memLabels.OexNew( nDataPts ).Ptr() )
			{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
				return oexT( "" );
			} // end if

		} // end if

		// Allocate space for data
		if ( !memPts[ nDimensions ].OexNew( nDataPts ).Ptr() )
		{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
			return oexT( "" );
		} // end if
		memPts[ nDimensions ].Zero();

		// Get data points
		int i = 0;
		for ( sqbind::CSqMulti::iterator itPt = it->second[ oexT( "data" ) ].begin();
			  i < nDataPts && it->second[ oexT( "data" ) ].end() != itPt; itPt++ )
		{
			// Grab pointer to label on first dimension
			if ( !nDimensions )
				memLabels[ i ] = itPt->first.c_str();

			// Grab point value
			memPts[ nDimensions ][ i++ ] = oexStrToFloat( itPt->second.str().c_str() );

		} // end for

		// Count a dimension
		nDimensions++;

	} // end for

	// GDChart sometimes tries to read from an extra dimension???
	if ( !memPts[ nDimensions ].OexNew( nDataPts ).Ptr() )
	{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
		return oexT( "" );
	} // end if
	memPts[ nDimensions ].Zero();

	// Lock the gdchart library
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return oexT( "" );

	// Settings
	GDC_generate_gif = FALSE;
	GDC_hold_img = GDC_EXPOSE_IMAGE;

	// Defaults
	GDC_BGColor = 0xffffff;
	GDC_GridColor = 0x000000;
	GDC_LineColor = 0x000000;
	GDC_PlotColor = 0x00ff00;
	GDC_ylabel_fmt = (char*)"%.3f";

	unsigned long _SetColor[ MAX_DIMENSIONS + 9 ] = { 0xff0000, 0x00ff00, 0x00ffff,
													  0xffff00, 0x0000ff, 0xff00ff,
													  0xffffff, 0xa0a0a0, 0x808080 };
	GDC_SetColor = _SetColor;

	// User settings
	if ( mParams.isset( oexT( "col_bg" ) ) )
		GDC_BGColor = oexHtmlToRgb( mParams[ oexT( "col_bg" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_grid" ) ) )
		GDC_GridColor = oexHtmlToRgb( mParams[ oexT( "col_grid" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_labels" ) ) )
		GDC_LineColor = oexHtmlToRgb( mParams[ oexT( "col_labels" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot" ) ) )
		GDC_PlotColor = GDC_SetColor[ 0 ] = GDC_SetColor[ 1 ] = GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
			= oexHtmlToRgb( mParams[ oexT( "col_plot" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot2" ) ) )
		GDC_SetColor[ 1 ] = GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
			= oexHtmlToRgb( mParams[ oexT( "col_plot2" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot3" ) ) )
		GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
			= oexHtmlToRgb( mParams[ oexT( "col_plot3" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot4" ) ) )
		GDC_SetColor[ 3 ]
			= oexHtmlToRgb( mParams[ oexT( "col_plot4" ) ].str().c_str() );

	// Write out the graph to the pipe
	switch ( nDimensions )
	{
	case 1:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   memPts[ 0 ].Ptr(), memPts[ 1 ].Ptr() );
		break;

	case 2:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   memPts[ 0 ].Ptr(), memPts[ 1 ].Ptr(), memPts[ 2 ].Ptr() );
		break;

	case 3:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   memPts[ 0 ].Ptr(), memPts[ 1 ].Ptr(), memPts[ 2 ].Ptr(), memPts[ 3 ].Ptr() );
		break;

	case 4:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   memPts[ 0 ].Ptr(), memPts[ 1 ].Ptr(), memPts[ 2 ].Ptr(), memPts[ 3 ].Ptr(),
				   memPts[ 4 ].Ptr() );
		break;

	default :
		oexERROR( 0, oexMks( oexT( "Invalid graph dimensions : " ), nDimensions ) );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
		break;

	} // end switch

	// Get the image buffer info
	gdImagePtr graph = (gdImagePtr)GDC_image;
	if ( !oexCHECK_PTR( graph ) || !oexCHECK_PTR( graph->pixels ) )
	{	oexERROR( 0, oexT( "Inavlid image pointer" ) );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
	} // end if

	// Create image to hold the data
	if ( !x_pImg->isValid()
		 || x_pImg->getWidth() != graph->sx
		 || x_pImg->getHeight() != graph->sy )
		if ( !x_pImg->Create( graph->sx, graph->sy ) )
			return oexT( "" );

	// Image size
	long lSize = x_pImg->Obj().GetImageSize();
	if ( 0 >= lSize )
	{	oexERROR( 0, oexMks( oexT( "Inavlid image size : " ), lSize ).Ptr() );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
	} // end if

	int sw = graph->sx;
	int sh = graph->sy;
	unsigned char **s = graph->pixels;
	unsigned char *d = (unsigned char*)x_pImg->Obj().GetBits();

	for ( int y = 0, u = sh - 1; y < sh; y++, u-- )
		for ( int x = 0; x < sw; x++, d += 3 )
		{
			unsigned char b = s[ u ][ x ];
			d[ 0 ] = graph->red[ b & 0xff ];
			d[ 1 ] = graph->green[ b & 0xff ];
			d[ 2 ] = graph->blue[ b & 0xff ];

		} // end for

	// Save image information
	sqbind::CSqMulti mImg;
	mImg[ oexT( "type" ) ] = x_sType;
	mImg[ oexT( "width" ) ] = sqbind::oex2std( oexMks( graph->sx ) );
	mImg[ oexT( "height" ) ] = sqbind::oex2std( oexMks( graph->sy ) );
	mImg[ oexT( "size" ) ] = sqbind::oex2std( oexMks( lSize ) );


	// Lose the image buffer
	gdImageDestroy( graph );
	GDC_image = NULL;
	GDC_SetColor = NULL;

	return mImg;
}

// iii GDChart doesn't handle really big numbers very well
#define GDCHART_RANGE 100000000000.f
sqbind::CSqMulti CGdcChart::CreateChartBin( const sqbind::stdString &x_sType,
									  		const sqbind::stdString &x_sParams,
											sqbind::CSqMulti *x_mData,
									  		sqbind::CSqImage *x_pImg )
{_STT();
	if ( !x_pImg || !x_mData )
		return oexT( "" );

	sqbind::CSqMulti mParams( x_sParams );

	// Get image width / height
	int nWidth = mParams[ oexT( "width" ) ].toint();
	int nHeight = mParams[ oexT( "height" ) ].toint();
	int nType = mParams[ oexT( "type" ) ].toint();
	float fRange = mParams.isset( oexT( "range" ) ) 
				   ? mParams[ oexT( "range" ) ].tofloat() 
				   : GDCHART_RANGE;
	if ( 0 >= nWidth || 0 >= nHeight )
		return oexT( "" );

	int nDataPts = 0;
	oex::TMem< oex::oexCSTR > memLabels;
	sqbind::CSqBinary bin[ MAX_DIMENSIONS + 1 ];

	int nDimensions = 0;
	for ( sqbind::CSqMulti::iterator it = (*x_mData)[ oexT( "data" ) ].begin();
		  nDimensions < MAX_DIMENSIONS && (*x_mData)[ oexT( "data" ) ].end() != it; it++ )
	{
		// Get a pointer to the data
		bin[ nDimensions ] = oexGetBin( it->second.str().c_str() );

		// How many data points in this buffer?
		int pts = bin[ nDimensions ].getUsed() / sizeof( float );
		if ( pts )
		{
			float *pF = (float*)bin[ nDimensions ]._Ptr();
			if ( pF )
			{
				// Enforce a reasonable range
				for ( int i = 0; i < pts; i++ )
					if ( pF[ i ] > fRange )
						pF[ i ] = fRange;
					else if ( pF[ i ] < -fRange )
						pF[ i ] = -fRange;

				// Do we have a size?
				if ( !nDataPts )
					nDataPts = pts;

				// Use smallest size
				else if ( pts < nDataPts )
					nDataPts = pts;

				// Count a dimension
				nDimensions++;

			} // end if

		} // end if

	} // end for

	// Did we find any data to graph?
	if ( !nDimensions )
		return oexT( "" );

	// For some stupid reason, we have to have as many labels as points
	if ( !memLabels.OexNew( nDataPts ).Ptr() )
	{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
		return oexT( "" );
	} // end if

	// How many labels provided?
	int l = (*x_mData)[ oexT( "labels" ) ].size();
	if ( l )
	{
		sqbind::CSqMulti::iterator it = (*x_mData)[ oexT( "labels" ) ].begin();
		for ( int i = 0, m = 0; i < nDataPts; i++ )
		{	
			// Save label into array
			if ( it != (*x_mData)[ oexT( "labels" ) ].end() )
				memLabels[ i ] = it->second.str().c_str();
			else
				memLabels[ i ] = oexT( "" );

			// Good ol' Bresenham
			m += l;
			while ( m > nDataPts )
				m -= nDataPts, it++;

		} // end for
	} // end if

	// Use blank labels
	else
		for ( int i = 0; i < nDataPts; i++ )
			memLabels[ i ] = oexT( "" );

	// Lock the gdchart library
	oexAutoLock ll( m_lock );
	if ( !ll.IsLocked() )
		return oexT( "" );

	// Settings
	GDC_generate_gif = FALSE;
	GDC_hold_img = GDC_EXPOSE_IMAGE;

	// Defaults
	GDC_BGColor = 0xffffff;
	GDC_GridColor = 0x000000;
	GDC_LineColor = 0x000000;
	GDC_PlotColor = 0x00ff00;
	GDC_ylabel_fmt = (char*)"%.3f";

	unsigned long _SetColor[ MAX_DIMENSIONS + 9 ] = { 0xff0000, 0x00ff00, 0x00ffff,
													  0xffff00, 0x0000ff, 0xff00ff,
													  0xffffff, 0xa0a0a0, 0x808080 };
	GDC_SetColor = _SetColor;

	// User settings
	if ( mParams.isset( oexT( "col_bg" ) ) )
		GDC_BGColor = oexHtmlToRgb( mParams[ oexT( "col_bg" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_grid" ) ) )
		GDC_GridColor = oexHtmlToRgb( mParams[ oexT( "col_grid" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_labels" ) ) )
		GDC_LineColor = oexHtmlToRgb( mParams[ oexT( "col_labels" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot1" ) ) )
		GDC_PlotColor = GDC_SetColor[ 0 ] = GDC_SetColor[ 1 ] = GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
					  = GDC_SetColor[ 4 ] = GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
					  = oexHtmlToRgb( mParams[ oexT( "col_plot1" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot2" ) ) )
		GDC_SetColor[ 1 ] = GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
						  = GDC_SetColor[ 4 ] = GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot2" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot3" ) ) )
		GDC_SetColor[ 2 ] = GDC_SetColor[ 3 ]
						  = GDC_SetColor[ 4 ] = GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot3" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot4" ) ) )
		GDC_SetColor[ 3 ] = GDC_SetColor[ 4 ] = GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot4" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot5" ) ) )
		GDC_SetColor[ 4 ] = GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot5" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot6" ) ) )
		GDC_SetColor[ 5 ] = GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot6" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot7" ) ) )
		GDC_SetColor[ 6 ] = GDC_SetColor[ 7 ]
						  = oexHtmlToRgb( mParams[ oexT( "col_plot7" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot8" ) ) )
		GDC_SetColor[ 7 ] = oexHtmlToRgb( mParams[ oexT( "col_plot8" ) ].str().c_str() );

	// Write out the graph to the pipe
	switch ( nDimensions )
	{
	case 1:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(),
				   bin[ 0 ].Ptr() );
		break;

	case 2:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(),
				   bin[ 0 ].Ptr() );
		break;

	case 3:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(),
				   bin[ 0 ].Ptr() );
		break;

	case 4:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(), bin[ 3 ].Ptr(),
				   bin[ 0 ].Ptr() );
		break;

	case 5:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(), bin[ 3 ].Ptr(),
				   bin[ 4 ].Ptr(), bin[ 0 ].Ptr() );
		break;

	case 6:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(), bin[ 3 ].Ptr(),
				   bin[ 4 ].Ptr(), bin[ 5 ].Ptr(), bin[ 0 ].Ptr() );
		break;

	case 7:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(), bin[ 3 ].Ptr(),
				   bin[ 4 ].Ptr(), bin[ 5 ].Ptr(), bin[ 6 ].Ptr(), bin[ 0 ].Ptr() );
		break;

	case 8:
		out_graph( nWidth, nHeight, 0, (GDC_CHART_T)nType,
				   nDataPts, (char**)memLabels.Ptr(), nDimensions,
				   bin[ 0 ].Ptr(), bin[ 1 ].Ptr(), bin[ 2 ].Ptr(), bin[ 3 ].Ptr(),
				   bin[ 4 ].Ptr(), bin[ 5 ].Ptr(), bin[ 6 ].Ptr(), bin[ 7 ].Ptr(), bin[ 0 ].Ptr() );
		break;

	default :
		oexERROR( 0, oexMks( oexT( "Invalid graph dimensions : " ), nDimensions ) );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
		break;

	} // end switch

	// Get the image buffer info
	gdImagePtr graph = (gdImagePtr)GDC_image;
	if ( !oexCHECK_PTR( graph ) || !oexCHECK_PTR( graph->pixels ) )
	{	oexERROR( 0, oexT( "Invalid image pointer" ) );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
	} // end if

	// Create image to hold the data
	if ( !x_pImg->isValid()
		 || x_pImg->getWidth() != graph->sx
		 || x_pImg->getHeight() != graph->sy )
		if ( !x_pImg->Create( graph->sx, graph->sy ) )
			return oexT( "" );

	// Image size
	long lSize = x_pImg->Obj().GetImageSize();
	if ( 0 >= lSize )
	{	oexERROR( 0, oexMks( oexT( "Inavlid image size : " ), lSize ).Ptr() );
		GDC_image = NULL;
		GDC_SetColor = NULL;
		return oexT( "" );
	} // end if

	int sw = graph->sx;
	int sh = graph->sy;
	unsigned char **s = graph->pixels;
	unsigned char *d = (unsigned char*)x_pImg->Obj().GetBits();

	for ( int y = 0, u = sh - 1; y < sh; y++, u-- )
		for ( int x = 0; x < sw; x++, d += 3 )
		{
			unsigned char b = s[ u ][ x ];
			d[ 0 ] = graph->red[ b & 0xff ];
			d[ 1 ] = graph->green[ b & 0xff ];
			d[ 2 ] = graph->blue[ b & 0xff ];

		} // end for

	// Save image information
	sqbind::CSqMulti mImg;
	mImg[ oexT( "type" ) ] = x_sType;
	mImg[ oexT( "width" ) ] = sqbind::oex2std( oexMks( graph->sx ) );
	mImg[ oexT( "height" ) ] = sqbind::oex2std( oexMks( graph->sy ) );
	mImg[ oexT( "size" ) ] = sqbind::oex2std( oexMks( lSize ) );

	// Lose the image buffer
	gdImageDestroy( graph );
	GDC_image = NULL;
	GDC_SetColor = NULL;

	return mImg;
}

int CGdcChart::SaveChart(	const sqbind::stdString &x_sFile,
							const sqbind::stdString &x_sParams,
							const sqbind::stdString &x_sData )
{_STT();
	// Create the chart
	sqbind::CSqImage img;
	sqbind::CSqMulti mChart = CreateChart( x_sFile, x_sParams, x_sData, &img );
	if ( !img.isValid() )
		return -10;

	return img.Save( x_sFile, oexNULL );
}
