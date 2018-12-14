#pragma once

// Based on example on https://github.com/syoyo/tinyobjloader
vector<Primitive *> loadOBJ( const char *filename, const Material &defaultMat );