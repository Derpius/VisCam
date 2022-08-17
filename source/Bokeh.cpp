#include "VisCam.h"

using namespace glm;
using namespace VisTrace;

inline float CircleOfConfusion(const Camera* cam, float S2)
{
	float S1 = cam->focusDistance;
	float N = cam->fStop;
	float f = cam->focalLength;

	float S1F = (S1 * 2.54f * 10.f) - f; // Convert the focus distance from hammer units to mm
	float c = (abs(S2 - S1) / S2) * (f * f / (N * S1F));
	return c;
}

constexpr uint8_t SAMPLE_COUNT = 71;
constexpr vec2 DISK_KERNEL[SAMPLE_COUNT] = {
    vec2(0,0),
    vec2(0.2758621,0),
    vec2(0.1719972,0.21567768),
    vec2(-0.061385095,0.26894566),
    vec2(-0.24854316,0.1196921),
    vec2(-0.24854316,-0.11969208),
    vec2(-0.061384983,-0.2689457),
    vec2(0.17199717,-0.21567771),
    vec2(0.51724136,0),
    vec2(0.46601835,0.22442262),
    vec2(0.32249472,0.40439558),
    vec2(0.11509705,0.50427306),
    vec2(-0.11509704,0.50427306),
    vec2(-0.3224948,0.40439552),
    vec2(-0.46601835,0.22442265),
    vec2(-0.51724136,0),
    vec2(-0.46601835,-0.22442262),
    vec2(-0.32249463,-0.40439564),
    vec2(-0.11509683,-0.5042731),
    vec2(0.11509732,-0.504273),
    vec2(0.32249466,-0.40439564),
    vec2(0.46601835,-0.22442262),
    vec2(0.7586207,0),
    vec2(0.7249173,0.22360738),
    vec2(0.6268018,0.4273463),
    vec2(0.47299224,0.59311354),
    vec2(0.27715522,0.7061801),
    vec2(0.056691725,0.75649947),
    vec2(-0.168809,0.7396005),
    vec2(-0.3793104,0.65698475),
    vec2(-0.55610836,0.51599306),
    vec2(-0.6834936,0.32915324),
    vec2(-0.7501475,0.113066405),
    vec2(-0.7501475,-0.11306671),
    vec2(-0.6834936,-0.32915318),
    vec2(-0.5561083,-0.5159932),
    vec2(-0.37931028,-0.6569848),
    vec2(-0.16880904,-0.7396005),
    vec2(0.056691945,-0.7564994),
    vec2(0.2771556,-0.7061799),
    vec2(0.47299215,-0.59311366),
    vec2(0.62680185,-0.4273462),
    vec2(0.72491735,-0.22360711),
    vec2(1,0),
    vec2(0.9749279,0.22252093),
    vec2(0.90096885,0.43388376),
    vec2(0.7818315,0.6234898),
    vec2(0.6234898,0.7818315),
    vec2(0.43388364,0.9009689),
    vec2(0.22252098,0.9749279),
    vec2(0,1),
    vec2(-0.22252095,0.9749279),
    vec2(-0.43388385,0.90096885),
    vec2(-0.62349,0.7818314),
    vec2(-0.7818317,0.62348956),
    vec2(-0.90096885,0.43388382),
    vec2(-0.9749279,0.22252093),
    vec2(-1,0),
    vec2(-0.9749279,-0.22252087),
    vec2(-0.90096885,-0.43388376),
    vec2(-0.7818314,-0.6234899),
    vec2(-0.6234896,-0.7818316),
    vec2(-0.43388346,-0.900969),
    vec2(-0.22252055,-0.974928),
    vec2(0,-1),
    vec2(0.2225215,-0.9749278),
    vec2(0.4338835,-0.90096897),
    vec2(0.6234897,-0.7818316),
    vec2(0.78183144,-0.62348986),
    vec2(0.90096885,-0.43388376),
    vec2(0.9749279,-0.22252086),
};

void Bokeh(IRenderTarget* pRt, const Camera* cam, const IRenderTarget* pDepth)
{
    uint16_t width = pRt->GetWidth(), height = pRt->GetHeight();
    vec3* pMainImage = static_cast<vec3*>(malloc(sizeof(vec3) * width * height));
    memcpy(pMainImage, pRt->GetRawData(), sizeof(vec3) * width * height);

	//#pragma omp parallel for collapse(2)
	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			// Calculate circle of confusion
			float coc = CircleOfConfusion(cam, pDepth->GetPixel(x, y).r);

            // Calculate radius of CoC in pixels on the image plane
            // We compute this using the camera's sensor height in mm, and the coc (also in mm)
            float kernelRadius = coc / cam->sensorHeight / 2 * static_cast<float>(height);

			// Blur
            vec3 colour(0.f);
            for (uint8_t sample = 0; sample < SAMPLE_COUNT; sample++) {
                vec2 kernelVal = DISK_KERNEL[sample] * kernelRadius;
                uvec2 texel = static_cast<uvec2>(floor(kernelVal)) + uvec2(x, y);
                texel = clamp(texel, uvec2(0, 0), uvec2(width - 1, height - 1));

                colour += pMainImage[texel.y * width + texel.x];
            }

            colour /= SAMPLE_COUNT;
			pRt->SetPixel(x, y, Pixel{ colour.r, colour.g, colour.b });
		}
	}

    free(pMainImage);
}
