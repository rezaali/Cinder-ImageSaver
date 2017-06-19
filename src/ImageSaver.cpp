#include "ImageSaver.h"

#include "cinder/Camera.h"
#include "cinder/app/App.h"

#include "Paths.h"
#include "Tiler.h"

using namespace reza::paths;
using namespace reza::tiler;
using namespace ci;
using namespace std;

namespace reza {
namespace img {

ImageSaver::ImageSaver(
	const ci::app::WindowRef &window,
	const std::function<void()> &drawFn,
	const std::function<void( glm::vec2, glm::vec2, glm::vec2, glm::vec2 )> &drawBgFn,
	const std::function<void( glm::vec2, glm::vec2, glm::vec2, glm::vec2 )> &drawPostFn )
	: mWindowRef( window ), mDrawFn( drawFn ), mDrawBgFn( drawBgFn ), mDrawPostFn( drawPostFn )
{
}

ImageSaver::~ImageSaver()
{
}

void ImageSaver::update()
{
	if( mSaveImage ) {
		ivec2 windowSize = mWindowRef->getSize();
		ivec2 outputSize = windowSize * mSizeMultiplier;

		auto render = Tiler::create( outputSize, windowSize, mWindowRef, true );
		render->setMatrices( mCam );

		if( mDrawBgFn ) {
			render->setDrawBgFn( [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
				mDrawBgFn( ul, ur, lr, ll );
			} );
		}

		if( mDrawFn ) {
			render->setDrawFn( [this]() {
				mDrawFn();
			} );
		}

		if( mDrawPostFn ) {
			render->setDrawPostFn( [this]( const vec2 &ul, const vec2 &ur, const vec2 &lr, const vec2 &ll ) {
				mDrawPostFn( ul, ur, lr, ll );
			} );
		}

		if( createDirectory( mSaveImagePath ) ) {
			fs::path high = mSaveImagePath;
			high += fs::path( mSaveImageName + "." + mSaveImageExtension );
			writeImage( high, render->getSurface(), ImageTarget::Options().quality( 1.0 ) );
		}
		mSaveImage = false;
	}
}

void ImageSaver::save( const CameraPersp &cam, const fs::path &path, const string &filename, const string &extension )
{
	mSaveImage = true;
	mCam = cam;
	mSaveImagePath = path;
	mSaveImageName = filename;
	mSaveImageExtension = extension;
}

} // namespace img
} // namespace reza
