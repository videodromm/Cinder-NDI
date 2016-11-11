#include "CinderNDISender.h"

#include "cinder/Log.h"
#include "cinder/Surface.h"

#include <Processing.NDI.Send.h>

CinderNDISender::CinderNDISender( const std::string name )
: mName{ name }, mNdiSender{ nullptr }
{
	if( ! NDIlib_is_supported_CPU() ) {
		CI_LOG_E( "Failed to initialize NDI because of unsupported CPU!" );
	}

	if( ! NDIlib_initialize() ) {
		CI_LOG_E( "Failed to initialize NDI!" );
	}

	NDIlib_send_create_t NDI_send_create_desc = { mName.c_str(), nullptr, true, false };
	mNdiSender = NDIlib_send_create( &NDI_send_create_desc );
}

CinderNDISender::~CinderNDISender()
{
	if( mNdiSender ) {
		NDIlib_send_destroy( mNdiSender );
	}
	NDIlib_destroy();
}


void CinderNDISender::sendSurface( const ci::SurfaceRef& surface )
{
	sendSurface( surface, NDIlib_send_timecode_synthesize );
}

void CinderNDISender::sendSurface( const ci::SurfaceRef& surface, long long timecode )
{
	if( surface && NDIlib_send_get_no_connections( mNdiSender, 0 ) ) {
		NDIlib_tally_t NDI_tally;
		NDIlib_send_get_tally( mNdiSender, &NDI_tally, 0 );

		const NDIlib_video_frame_t NDI_video_frame = {
			(unsigned int)( surface->getWidth() ),
			(unsigned int)( surface->getHeight() ),
			NDIlib_FourCC_type_BGRA,
			60000, 1001,
			(float)surface->getWidth()/(float)surface->getHeight(),
			true,
			timecode,
			surface->getData(),
			(unsigned int)( surface->getRowBytes() )
		};

		NDIlib_send_send_video( mNdiSender, &NDI_video_frame );
	}
}

void CinderNDISender::sendMetadata( const ci::XmlTree& metadataString )
{
	sendMetadata( metadataString, NDIlib_send_timecode_synthesize );
}

void CinderNDISender::sendMetadata( const ci::XmlTree& xmlTree, long long timecode )
{
	auto str = ci::toString( xmlTree );

	if( NDIlib_send_get_no_connections( mNdiSender, 0 ) ) {
		const NDIlib_metadata_frame_t NDI_metadata = {
			(unsigned int)(str.size()),
			timecode,
			const_cast<CHAR*>(str.c_str())
		};

		CI_LOG_I( "Sending: " << str );
		NDIlib_send_send_metadata( mNdiSender, &NDI_metadata );
	}
}
