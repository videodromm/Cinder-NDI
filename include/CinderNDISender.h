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

		void setFramerate( int numerator, int denominator );

		void sendSurface( ci::Surface& surface);
		void sendSurface( ci::Surface&, long long timecode, bool async = false );
		void sendSurfaceForceSync();

		void sendMetadata( const ci::XmlTree& metadataString );
		void sendMetadata( const ci::XmlTree& metadataString, long long timecode );
	private:
		int						mFramerateNumerator, mFramerateDenominator;
		NDIlib_send_instance_t	mNdiSender;
		std::string				mName;
};
