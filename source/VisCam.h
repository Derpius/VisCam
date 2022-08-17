#pragma once

#include "vistrace.h"
#include"GarrysMod/Lua/Interface.h"
#include "glm/glm.hpp"

#include "Camera.h"

#include <omp.h>

void Bokeh(VisTrace::IRenderTarget* pRt, const Camera* cam, const VisTrace::IRenderTarget* pDepth);
