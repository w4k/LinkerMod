#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#pragma comment(lib, "detours/Detours.lib")
#include "detours/Typedef.h"
#include "detours/Detours.h"

#define ASSERT(x)

#include "T5.h"
#include "com_files.h"
#include "r_image_load_common.h"
#include "r_image_load_obj.h"