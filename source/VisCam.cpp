#include "VisCam.h"

using namespace VisTrace;
using namespace GarrysMod::Lua;

static int g_IRenderTargetId = -1;
int Camera::id{ -1 };

LUA_FUNCTION(CreateCamera)
{
	LUA->PushUserType_Value(Camera{}, Camera::id);
	return 1;
}

LUA_FUNCTION(Camera_SensorHeight)
{
	LUA->CheckType(1, Camera::id);
	Camera* pCam = LUA->GetUserType<Camera>(1, Camera::id);

	pCam->sensorHeight = LUA->CheckNumber(2);

	return 0;
}

LUA_FUNCTION(Camera_FocalLength)
{
	LUA->CheckType(1, Camera::id);
	Camera* pCam = LUA->GetUserType<Camera>(1, Camera::id);

	pCam->focalLength = LUA->CheckNumber(2);
	pCam->aperture = pCam->focalLength / pCam->fStop;

	return 0;
}

LUA_FUNCTION(Camera_FStop)
{
	LUA->CheckType(1, Camera::id);
	Camera* pCam = LUA->GetUserType<Camera>(1, Camera::id);

	pCam->fStop = LUA->CheckNumber(2);
	pCam->aperture = pCam->focalLength / pCam->fStop;

	return 0;
}

LUA_FUNCTION(Camera_FocusDistance)
{
	LUA->CheckType(1, Camera::id);
	LUA->GetUserType<Camera>(1, Camera::id)->focusDistance = LUA->CheckNumber(2);
	return 0;
}

LUA_FUNCTION(VisTraceRT_ApplyCamera)
{
	LUA->CheckType(1, g_IRenderTargetId); // self
	LUA->CheckType(2, Camera::id);
	LUA->CheckType(3, g_IRenderTargetId); // depth

	IRenderTarget* pRt = *LUA->GetUserType<IRenderTarget*>(1, g_IRenderTargetId);
	Camera* pCam = LUA->GetUserType<Camera>(2, Camera::id);
	const IRenderTarget* pDepth = *LUA->GetUserType<IRenderTarget*>(3, g_IRenderTargetId);

	if (pRt->GetFormat() != RTFormat::RGBFFF) LUA->ThrowError("Output buffer must be in the format RGBFF (standard HDR render buffer)");
	if (pDepth->GetFormat() != RTFormat::RF) LUA->ThrowError("Depth buffer must be in the format RF");

	Bokeh(pRt, pCam, pDepth);

	return 0;
}

LUA_FUNCTION(VisTraceInit)
{
	LUA->PushSpecial(SPECIAL_REG);
	LUA->GetField(-1, "VisTraceRT_id");
	if (!LUA->IsType(-1, Type::Number))
		LUA->ThrowError("Failed to get RenderTarget metatable (was VisTrace loaded correctly?)");
	g_IRenderTargetId = LUA->GetNumber();
	LUA->Pop(2);

	LUA->PushMetaTable(g_IRenderTargetId);
		LUA->PushCFunction(VisTraceRT_ApplyCamera);
		LUA->SetField(-2, "ApplyCamera");
	LUA->Pop();

	Camera::id = LUA->CreateMetaTable("VisCamera");
		LUA->Push(-1);
		LUA->SetField(-2, "__index");
		LUA->PushCFunction(Camera_SensorHeight);
		LUA->SetField(-2, "SensorHeight");
		LUA->PushCFunction(Camera_FocalLength);
		LUA->SetField(-2, "FocalLength");
		LUA->PushCFunction(Camera_FStop);
		LUA->SetField(-2, "FStop");
		LUA->PushCFunction(Camera_FocusDistance);
		LUA->SetField(-2, "FocusDistance");
	LUA->Pop();

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->CreateTable();
		LUA->PushCFunction(CreateCamera);
		LUA->SetField(-2, "CreateCamera");
	LUA->SetField(-2, "viscam");

	LUA->Pop();

	return 0;
}

GMOD_MODULE_OPEN()
{
	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->GetField(-1, "hook");
	LUA->GetField(-1, "Add");
	LUA->PushString("VisTraceInit");
	LUA->PushString("VisCam");
	LUA->PushCFunction(VisTraceInit);
	LUA->Call(3, 0);
	LUA->Pop(2);
	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}
