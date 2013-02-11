// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <math.h>  
#include "Angel.h"

// TODO: reference additional headers your program requires here
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define NEAR 1
#define FAR 100
#define FOV SCREEN_HEIGHT
#define ASPECT  float(SCREEN_WIDTH)/float(SCREEN_HEIGHT)