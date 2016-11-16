#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CinderNDISender.h"

using namespace ci;
using namespace ci::app;

class BasicSenderApp : public App {
  public:
	BasicSenderApp();

	void update() override;
	void draw() override;
  private:
	CinderNDISender			mSender;
	ci::SurfaceRef 			mSurface;
	uint8_t					mIndexNew, mIndexOld;
	gl::FboRef				mFbo[2];
	gl::PboRef				mPbo[2];
};

BasicSenderApp::BasicSenderApp()
: mSender( "test-cinder-video" )
, mIndexNew{ 0 }
, mIndexOld{ 1 }
{
	mFbo[0] = gl::Fbo::create( getWindowWidth(), getWindowHeight(), false );
	mFbo[1] = gl::Fbo::create( getWindowWidth(), getWindowHeight(), false );

	mSurface = ci::Surface::create( getWindowWidth(), getWindowHeight(), true, SurfaceChannelOrder::BGRA );

	mPbo[0] = gl::Pbo::create( GL_PIXEL_PACK_BUFFER, getWindowWidth() * getWindowHeight() * 4, 0, GL_STREAM_READ );
	mPbo[1] = gl::Pbo::create( GL_PIXEL_PACK_BUFFER, getWindowWidth() * getWindowHeight() * 4, 0, GL_STREAM_READ );
}

void BasicSenderApp::update()
{
	getWindow()->setTitle( "CinderNDI-Sender - " + std::to_string( (int) getAverageFps() ) + " FPS" );

	if( mSurface )
	{
		gl::ScopedFramebuffer sFbo( mFbo[mIndexOld] );
		gl::ScopedBuffer scopedPbo( mPbo[mIndexNew] );
		
		gl::readBuffer( GL_COLOR_ATTACHMENT0 );
		gl::readPixels( 0, 0, mFbo[mIndexOld]->getWidth(), mFbo[mIndexOld]->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		mPbo[mIndexOld]->getBufferSubData( 0, mFbo[mIndexOld]->getWidth() * mFbo[mIndexOld]->getHeight() * 4, mSurface->getData() ); 
	}

	{
		gl::ScopedFramebuffer sFbo( mFbo[mIndexNew] );
		gl::ScopedViewport sVp( 0, 0, mFbo[mIndexNew]->getWidth(), mFbo[mIndexNew]->getHeight() );
		gl::clear( ColorA::black() );
		gl::ScopedColor pushCol{ ColorA::white() };
		gl::drawSolidRect( Rectf{ 0,0, float( 0.5f * (glm::sin( getElapsedSeconds() ) + 1.0f) ) * app::getWindowWidth(), float( app::getWindowHeight() ) } );
	}

	long long timecode = app::getElapsedFrames();

	XmlTree msg{ "ci_meta", "test string" };
	mSender.sendMetadata( msg, timecode );
	mSender.sendSurface( *mSurface, timecode );
}

void BasicSenderApp::draw()
{
	gl::clear( ColorA::black() );
	gl::draw( mFbo[mIndexNew]->getColorTexture() );
	std::swap( mIndexNew, mIndexOld );
}

void prepareSettings( BasicSenderApp::Settings* settings )
{
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicSenderApp, RendererGl, prepareSettings )
