#pragma once

#include <memory>
#include <windows.h>
#include <string>
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

#include <Processing.NDI.Lib.h>

class CinderNDISender{
	public:
		CinderNDISender( const std::string name );
		~CinderNDISender();

		void sendSurface( const ci::SurfaceRef& surface);
		void sendSurface( const ci::SurfaceRef&, long long timecode );

		void sendMetadata( const ci::XmlTree& metadataString );
		void sendMetadata( const ci::XmlTree& metadataString, long long timecode );
	private:
		NDIlib_send_instance_t	mNdiSender;
		std::string				mName;
};
