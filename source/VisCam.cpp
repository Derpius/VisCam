#include "VisCam.h"

using namespace VisTrace;
using namespace GarrysMod::Lua;

static int g_IRenderTargetId = -1;

LUA_FUNCTION(CreateCamera)
{
	LUA->PushUserType_Value(Camera{}, Camera::id);
	return 1;
}

LUA_FUNCTION(Camera_FocalLength)
{
	LUA->CheckType(1, Camera::id);
	LUA->GetUserType<Camera>(1, Camera::id)->focalLength = LUA->CheckNumber(2);
	return 0;
}

LUA_FUNCTION(Camera_FStop)
{
	LUA->CheckType(1, Camera::id);
	LUA->GetUserType<Camera>(1, Camera::id)->fStop = LUA->CheckNumber(2);
	return 0;
}

LUA_FUNCTION(Camera_FocusDistance)
{
	LUA->CheckType(1, Camera::id);
	LUA->GetUserType<Camera>(1, Camera::id)->focusDistance = LUA->CheckNumber(2);
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

	LUA->PushSpecial(SPECIAL_GLOB);

	Camera::id = LUA->CreateMetaTable("VisCamera");
		LUA->Push(-1);
		LUA->SetField(-2, "__index");
		LUA->PushCFunction(Camera_FocalLength);
		LUA->SetField(-2, "FocalLength");
		LUA->PushCFunction(Camera_FStop);
		LUA->SetField(-2, "FStop");
		LUA->PushCFunction(Camera_FocusDistance);
		LUA->SetField(-2, "FocusDistance");
	LUA->Pop();

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
