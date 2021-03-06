
#include "stdafx.h"

#if defined( _WIN32_WCE )
#	include <windows.h>
#endif

#if defined( __ANDROID__ )
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#	include <android/log.h>
#	include "importgl.h"
//extern "C" int importGLInit();
#	define _MSG( s ) __android_log_print( ANDROID_LOG_INFO, OEX_PROJECT_NAME, "%s(%d): %s\n", __FILE__, __LINE__, s ), \
					 printf( "%s(%d): %s\n", __FILE__, __LINE__, s )
#else
#	define _MSG( s ) printf( "%s(%d): %s\n", __FILE__, __LINE__, s )
#endif

class MyEventReceiver : public irr::IEventReceiver
{
private:
	irr::IrrlichtDevice *Device;
public:
	MyEventReceiver ( irr::IrrlichtDevice *device ): Device ( device ) {}

	virtual bool OnEvent(const irr::SEvent& event)
	{_STT();
		if (event.EventType == irr::EET_GUI_EVENT)
		{
			irr::s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
				case irr::gui::EGET_BUTTON_CLICKED:
				if (id == 2)
				{
					Device->closeDevice();
					return true;
				} break;

				default :
					break;
			}
		}

		return false;
	}
};


/// Irrlicht device
irr::IrrlichtDevice			*g_pDevice = oexNULL;
irr::gui::IGUIStaticText 	*g_text;
int 						g_frames = 0;
MyEventReceiver				*g_receiver = oexNULL;

void Close()
{_STT();
	if ( g_pDevice )
	{	g_pDevice->closeDevice();
		g_pDevice->drop();
		g_pDevice = oexNULL;
	} // end if
}

int Init( int width, int height )
{_STT();

#if defined( _WIN32_WCE ) || defined( __ANDROID__ )
printf( "%s(%d)\n", __FILE__, __LINE__ );

#if defined( _IRR_COMPILE_WITH_OGLES2_ )
printf( "%s(%d)\n", __FILE__, __LINE__ );


#if defined( __ANDROID__ )

#	if !defined( DISABLE_IMPORTGL )
		importGLInit();
#	endif

	_MSG( glGetString( GL_VERSION ) );

#endif

	_MSG( "Using: EDT_OGLES2" );
    g_pDevice = irr::createDevice( irr::video::EDT_OGLES2, 
								   irr::core::dimension2d< irr::u32 >( 320, 480 ), 
								   16, false, false, false, 0 );
	
//	g_pDevice = irr::createDevice( irr::video::EDT_OGLES2,
//								   irr::core::dimension2d< irr::u32 >( 320, 240 ),
//								   16, true );

	_MSG( "irr::createDevice() succeeded" );
								   
#elif defined( _IRR_COMPILE_WITH_OGLES1_ )
printf( "%s(%d)\n", __FILE__, __LINE__ );

	_MSG( "Using: EDT_OGLES1" );
	g_pDevice = irr::createDevice( irr::video::EDT_OGLES1,
								   irr::core::dimension2d< irr::u32 >( 320, 240 ),
								   16, true );

#else
printf( "%s(%d)\n", __FILE__, __LINE__ );

	_MSG( "Using: EDT_BURNINGSVIDEO" );
	g_pDevice = irr::createDevice( irr::video::EDT_BURNINGSVIDEO,
								   irr::core::dimension2d< irr::u32 >( 320, 240 ),
								   16, true );

#endif

//	g_pDevice = irr::createDevice( irr::video::EDT_OGLES1, irr::core::dimension2d<irr::u32>(width, height),
//	     						   16, true, false, false, 0 );


//	irr::SIrrlichtCreationParameters param;
//	oexZeroMemory( &param, sizeof( param ) );
//	param.Bits = 16;
//	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
//	param.DriverType = irr::video::EDT_OGLES1;
//	param.DriverType = irr::video::EDT_SOFTWARE;
//	g_pDevice = irr::createDeviceEx( param );

#elif defined( OEX_IPHONE )
	_MSG( "OEX_IPHONE" );

	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::u32>( width, height );
	param.DriverType = irr::video::EDT_OGLES1;
	g_pDevice = irr::createDeviceEx( param );

#else
	_MSG( "Initializing Irrlicht" );

	irr::SIrrlichtCreationParameters param;
	param.Bits = 16;
	param.AntiAlias = true;
	param.WindowSize = irr::core::dimension2d<irr::s32>( width, height );
	param.DriverType = irr::video::EDT_OPENGL;
	g_pDevice = irr::createDeviceEx( param );

#endif

//	m_pDevice = irr::createDevice( irr::video::EDT_OPENGL, irr::core::dimension2d<irr::s32>( 640, 480 ), 16,
//				   			  false, false, false, 0 );

printf( "%s(%d)\n", __FILE__, __LINE__ );
	if ( !g_pDevice )
		oexERROR( 0, oexT( "Failed to create irrlicht device" ) );

	else
	{
		irr::scene::ISceneManager *smgr = g_pDevice->getSceneManager();
		irr::video::IVideoDriver *driver = g_pDevice->getVideoDriver();
		irr::gui::IGUIEnvironment* guienv = g_pDevice->getGUIEnvironment();

		_MSG( "Creating event receiver" );
		
		g_receiver = new MyEventReceiver(g_pDevice);
		g_pDevice->setEventReceiver(g_receiver);
		g_pDevice->setWindowCaption(L"Irrlicht Demo");

		_MSG( "Creating GUI button and FPS display" );

		g_text = guienv->addStaticText(L"FPS: xx",irr::core::rect<irr::s32>(60,5,200,20), false );
		guienv->addButton(irr::core::rect<int>(10,5,50,20), 0, 2, L"Quit");

		{ // Create scene

			_MSG( "Creating Scene" );
			
			driver->setAmbientLight( irr::video::SColorf( .5f, .5f, .5f ) );
			smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, 0 ),
									 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );
			smgr->addLightSceneNode( 0, irr::core::vector3df( 0, 100, -50 ),
									 irr::video::SColorf( 0.5f, 0.5f, 0.5f ), 100 );

			_MSG( "Adding camera" );
//			irr::scene::ICameraSceneNode *camera =
				smgr->addCameraSceneNode( 0, irr::core::vector3df( 0, 30, -40 ),
										  irr::core::vector3df( 0, 5, 0 ) );

			_MSG( "Adding cube" );
			irr::scene::ISceneNode *node = smgr->addCubeSceneNode( 10.f );
			if ( node )
			{
				irr::scene::ISceneNodeAnimator* rotate =
					smgr->createRotationAnimator( irr::core::vector3df( 0, 0.4f, 0 ) );

				node->addAnimator( rotate );

				rotate->drop();

			} // end if

		} // end Create Scene

	} // end else

	_MSG( "Initialization success" );
	
	return 1;
}

int Draw()
{_STT();
	if ( !g_pDevice || !g_pDevice->run() )
		return -1;

	if (++g_frames==10)
	{
		irr::core::stringw str = L"FPS: ";
		str += (int)g_pDevice->getVideoDriver()->getFPS();

		g_text->setText ( str.c_str() );
		g_frames=0;
	}

	g_pDevice->getVideoDriver()->beginScene( true, true, irr::video::SColor( 255, 100, 101, 140 ) );
	g_pDevice->getSceneManager()->drawAll();
	g_pDevice->getGUIEnvironment()->drawAll();
	g_pDevice->getVideoDriver()->endScene();

	return 0;
}

#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
#else
int main( int argc, char* argv[] )
#endif
{
    // Initialize the oex library
	oexINIT();

	// Init irrilcht device
	Init( 240, 320 );

	// Run the scene
	while ( !Draw() )
		;
//		oexSleep( 30 );

	if ( g_receiver )
		delete g_receiver;
	g_receiver = oexNULL;

	// Uninitialize the oex library
    oexUNINIT();

	return 0;
}

