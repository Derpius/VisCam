#pragma once

struct Camera
{
	static int id;

	float focalLength = 50.f;
	float fStop = 1.f / 4.f;
	float focusDistance = 50.f;
	float sensorHeight = 24.f;
	float aperture = 50.f * 4.f;
};
