#pragma once

struct Camera
{
	static int id;

	float focalLength = 1.f;
	float fStop = 1.f / 16.f;
	float focusDistance = 50.f;
};
