#include "VisCam.h"

using namespace glm;
using namespace VisTrace;

inline float CircleOfConfusion(const Camera& cam, float S2)
{
	float S1 = cam.focusDistance;
	float N = cam.fStop;
	float f = cam.focalLength;

	float c = (abs(S2 - S1) / S2) * (f * f / (N * (S1 - f)));
	return clamp(c, 0.f, 1.f);
}

void Bokeh(IRenderTarget* pRt, const Camera& cam, const IRenderTarget* pDepth)
{
	#pragma omp parallel for collapse(2)
	for (uint16_t y = 0; y < pRt->GetHeight(); y++) {
		for (uint16_t x = 0; x < pRt->GetWidth(); x++) {
			pRt->SetPixel(x, y, Pixel{ CircleOfConfusion(cam, pDepth->GetPixel(x, y).r) });
		}
	}
}
