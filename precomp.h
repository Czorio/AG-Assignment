// add your includes to this file instead of to individual .cpp files
// to enjoy the benefits of precompiled headers:
// - fast compilation
// - solve issues with the order of header files once (here)
// do not include headers in header files (ever).

// Prevent expansion clashes (when using std::min and std::max):
#define NOMINMAX

#define MAX_IDLE_FPS 60.f

#define SCRWIDTH 512
#define SCRHEIGHT 512
#define TILESIZE 64

#define IMPORTANCE_SAMPLING
//#define BVH_DEBUG
#define BVH_MIN_SAH_COUNT 8
#define BVHDEPTH 128
#define BINCOUNT 32 // this can also be reduced for faster construction

#define MAXRAYDEPTH 8
#define SAMPLES 4
#define ITERATIONS 1024

#define SHADOWBIAS 0.001f
#define REFLECTIONBIAS 0.01f
#define REFRACTIONBIAS 0.001f
#define EPSILON 0.0001f

#define FIREFLY 10.f

#define AMBIENTLIGHT 0.f

// Uncomment to move to final scene
//#define FINAL

// #define FULLSCREEN
// #define ADVANCEDGL	// faster if your system supports it

// Glew should be included first
#include <GL/glew.h>
// Comment for autoformatters: prevent reordering these two.
#include <GL/gl.h>

#ifdef _WIN32
// Followed by the Windows header
#include <Windows.h>

// Then import wglext: This library tries to include the Windows
// header WIN32_LEAN_AND_MEAN, unless it was already imported.
#include <GL/wglext.h>

// Extra definitions for redirectIO
#include <fcntl.h>
#include <io.h>
#endif

// External dependencies:
#include <FreeImage.h>
#include <SDL2/SDL.h>

// C++ headers
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

// Namespaced C headers:
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Header for AVX, and every technology before it.
// If your CPU does not support this, include the appropriate header instead.
// See: https://stackoverflow.com/a/11228864/2844473
#include <immintrin.h>

// clang-format off

// "Leak" common namespaces to all compilation units. This is not standard
// C++ practice but a mere simplification for this small project.
using namespace std;

#include "surface.h"
#include "template.h"

using namespace Tmpl8;

#include "Color.h"
#include "Material.h"
#include "Light.h"
#include "Ray.h"
#include "Camera.h"
#include "Sample.h"
#include "Primitive.h"
#include "OBJLoader.h"
#include "BVH.h"
#include "Renderer.h"

#include "game.h"
// clang-format on