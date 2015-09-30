
_self.load_module( "ffmpeg", "" );

function _init()
{
	local ffmpeg_root = _self.root( "_ffmpeg_" )
	CSqFile().mkdir( ffmpeg_root );

	local test_img = _self.path( "../media/wall_street.jpg" );
	local test_avi = _self.path( "../media/nurse_shark.avi" );

	// **************************************************
	// Encode / Decode tests

	// Load a picture
	local img = CSqImage();
	if ( !img.Load( test_img, "" ) )
	{	_self.echo( "failed to load image" );
		return;
	} // end if

	_self.echo( "Loaded picture " + img.getWidth() + "x" + img.getHeight() );

	run_test( ffmpeg_root, "MJPEG", img, CFfDecoder().LookupCodecId( "MJPG" ), -1,
			  CFfConvert().AV_PIX_FMT_YUV420P );

//	run_test( ffmpeg_root, "WMV1", img, CFfEncoder().CODEC_ID_WMV1, 0,
//			  CFfConvert().AV_PIX_FMT_YUV420P );

//	run_test( ffmpeg_root, "WMV2", img, CFfEncoder().CODEC_ID_WMV2, 0,
//			  CFfConvert().AV_PIX_FMT_YUV420P );

	run_test( ffmpeg_root, "MSMPEG4V2", img, CFfDecoder().LookupCodecId( "MP42" ), 0,
			  CFfConvert().AV_PIX_FMT_YUV420P );

	run_test( ffmpeg_root, "MPEG4", img, CFfDecoder().LookupCodecId( "MP4V" ), 0,
			  CFfConvert().AV_PIX_FMT_YUV420P );

	run_test( ffmpeg_root, "FLV1", img, CFfDecoder().LookupCodecId( "FLV1" ), 0,
			  CFfConvert().AV_PIX_FMT_YUV420P );

	img.Resample( 352, 288, 0 );
	run_test( ffmpeg_root, "H263", img, CFfDecoder().LookupCodecId( "H263" ), 0,
			  CFfConvert().AV_PIX_FMT_YUV420P );

//	run_test( ffmpeg_root, "H264", img, CFfDecoder().LookupCodecId( "H264" ), 0,
//			  CFfConvert().AV_PIX_FMT_YUV420P );

	// **************************************************
	// AVI test

	test_avi_read( ffmpeg_root, test_avi );

	test_avi_write( ffmpeg_root, _self.build_path( ffmpeg_root, "_a_ffmpeg_write_test.avi" ) );

	// **************************************************
	// RTSP test

//	test_rtsp( ffmpeg_root, "rtsp://a1352.l1857053128.c18570.g.lq.akamaistream.net/D/1352/18570/v0001/reflector:53128", 30 );
//	test_rtsp( ffmpeg_root, "rtsp://v2.cache1.c.youtube.com/CkgLENy73wIaPwlnoDu0pt7zDRMYDSANFEIJbXYtZ29vZ2xlSARSB3Jlc3VsdHNaDkNsaWNrVGh1bWJuYWlsYOmkotHXgfvJRgw=/0/0/0/video.3gp", 15 );
//	test_rtsp( ffmpeg_root, "rtsp://video2.multicasttech.com/AFTVSciFi3GPP296.sdp", 15 );

	_self.echo( "" );
}

function test_rtsp( root, url, fps )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-     RTSP Test : " + url );
	_self.echo( "----------------------------------------------" );

	local vid_in = CFfContainer();
	local file_info = CSqMulti();
	if ( !vid_in.Open( url, file_info ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + vid_in.getWidth() + "x" + vid_in.getHeight() );
	_self.echo( " -> " + file_info.serialize() );
/*
	local dec = CFfDecoder();
	if ( !dec.Create( vid_in.getVideoCodecId(), vid_in.getVideoFormat(),
					  vid_in.getWidth(), vid_in.getHeight(), 0 ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if
*/
	local vid_out = CFfContainer();
	if ( !vid_out.Create( _self.build_path( root, "_ffmpeg_rtsp.avi" ), "", file_info ) )
	{	_self.echo( "failed to create file" );
		return;
	} // end if

	local codec_id = vid_in.getVideoCodecId();
//	local codec_id = CFfEncoder().CODEC_ID_MSMPEG4V2;
	local transcode = codec_id != vid_in.getVideoCodecId();

	if ( 0 > vid_out.AddVideoStream( codec_id, vid_in.getWidth(), vid_in.getHeight(), fps ) )
	{	_self.echo( "failed to add video stream" );
		return;
	} // end if

	if ( !vid_out.InitWrite() )
	{	_self.echo( "failed to initialize file" );
		return;
	} // end if

	local i = 0;
	local stream = -1;
	local frame = CSqBinary();
	local frame_info = CSqMulti();
	local tframe = CSqBinary();
	local tc = CFfTranscode();

	if ( transcode )
		if ( !tc.Init( vid_in.getWidth(), vid_in.getHeight(),
					   vid_in.getVideoCodecId(), codec_id ) )
		{	_self.echo( "failed to create transcoder" );
			return;
		} // end if

	while ( i < ( fps * 30 ) && 0 <= ( stream = vid_in.ReadFrame( frame, frame_info ) ) )
	{
		if ( vid_in.getVideoStream() == stream )
		{
			if ( !transcode )
				vid_out.WriteFrame( frame, frame_info );

			else if ( tc.Transcode( frame, tframe, frame_info ) )
				vid_out.WriteFrame( tframe, frame_info );

			_self.print( "\r" + i++ + " " );
			_self.flush();

		} // end if
		else
			_self.echo( "non video frame" );

	} // end while

	_self.echo( "\rsuccess : Frames written = " + i );
}

function test_avi_write( root, file )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      AVI Write Test" );
	_self.echo( "----------------------------------------------" );

	local img = CSqImage();
	if ( !img.Load( _self.build_path( root, "_z_avi_0.jpg" ), "" ) )
	{	_self.echo( "failed to find avi input image" );
		return;
	} // end if

	local vid = CFfContainer();
	if ( !vid.Create( file, "", CSqMulti() ) )
	{	_self.echo( "failed to create file" );
		return;
	} // end if

	if ( 0 > vid.AddVideoStream( CFfDecoder().LookupCodecId( "MP42" ), img.getWidth(), img.getHeight(), 15 ) )
	{	_self.echo( "failed to add video stream" );
		return;
	} // end if

	if ( !vid.InitWrite() )
	{	_self.echo( "failed to initialize file" );
		return;
	} // end if

	local enc = CFfEncoder();
	if ( !enc.Create( vid.getVideoCodecId(), CFfConvert().AV_PIX_FMT_YUV420P,
					  img.getWidth(), img.getHeight(), 15, 2000000, CSqMulti() ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	local i = 0;
	local frame = CSqBinary();
	do
	{
		local nEnc = enc.EncodeImage( img, frame, CSqMulti() );
		if ( !nEnc )
		{	_self.echo( "Encode() failed" );
			return;
		} // end if

		if ( !vid.WriteFrame( frame, CSqMulti( "flags=1" ) ) )
		{	_self.echo( "failed to write frame to avi" );
			return;
		} // end if

		_self.print( "\r" + i + " " );
		_self.flush();

	} while ( img.Load( _self.build_path( root, "_z_avi_" + ++i + ".jpg" ), "" ) );

	_self.echo( "\rsuccess : Frames written = " + i );
}

function test_avi_read( root, file )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      AVI Read Test" );
	_self.echo( "----------------------------------------------" );

	local vid = CFfContainer();
	if ( !vid.Open( file, CSqMulti() ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + vid.getWidth() + "x" + vid.getHeight() );

	local dec = CFfDecoder();
	if ( !dec.Create( vid.getVideoCodecId(), vid.getVideoFormat(),
					  vid.getWidth(), vid.getHeight(), 0, 0, CSqMulti() ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if

	local i = 0;
	local stream = -1;
	local frame = CSqBinary();
	local frame_info = CSqMulti();
	while ( 0 <= ( stream = vid.ReadFrame( frame, frame_info ) ) )
	{
		if ( vid.getVideoStream() == stream )
		{
			local img = CSqImage();
			if ( !dec.DecodeImage( frame, img, frame_info, 1 ) )
			{	_self.echo( "failed to decode image" );
				return;
			} // end if

			// Save this frame
			img.Save( _self.build_path( root, "_z_avi_" + i++ + ".jpg" ), "" );

			_self.print( "\r" + i + " " );
			_self.flush();

		} // end if

	} // end while

	_self.echo( "\rsuccess : Frames read " + i );

	return;
}

function run_test( root, name, img, fmt, cmp, cs )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      Codec : " + name );
	_self.echo( "----------------------------------------------" );

	if ( !fmt )
	{	_self.echo( "invalid codec id" );
		return;
	} // endi f

	local frame = CSqBinary();
	local cimg = CSqImage();

	cimg.CopyImage( img );

	test_encode( frame, cimg, fmt, cmp, cs );

	if ( !frame.getUsed() )
		return;

	test_decode( frame, cimg, fmt, cmp, cs );

	CSqFile().put_contents_bin( _self.build_path( root, "_ffmpeg_" + name + ".raw" ), frame );

	cimg.Save( _self.build_path( root, "_ffmpeg_" + name + ".png" ), "" );
}


function test_encode( frame, img, fmt, cmp, cs )
{
	_self.echo( "\nTesting encoder ( " + img.getWidth() + " x " + img.getHeight() + " ) ..." );

	local enc = CFfEncoder();
	if ( !enc.Create( fmt, cs, img.getWidth(), img.getHeight(), 15, 0, CSqMulti( "cmp=" + cmp ) ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	local nEnc = enc.EncodeImage( img, frame, CSqMulti() );
	if ( !nEnc )
	{	_self.echo( "Encode() failed" );
		return;
	} // end if

	_self.echo( "success : Encoded " + nEnc + " bytes" );
}

function test_decode( frame, img, fmt, cmp, cs )
{
	_self.echo( "\nTesting decoder..." );

	local dec = CFfDecoder();
	if ( !dec.Create( fmt, cs, img.getWidth(), img.getHeight(), 15, 0, CSqMulti( "cmp=" + cmp ) ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if

	if ( !dec.DecodeImage( frame, img, CSqMulti(), 1 ) )
	{	_self.echo( "failed to decode image" );
		return;
	} // end if

	_self.echo( "success: Decoded Image " + img.getWidth() + "x" + img.getHeight() );
}
