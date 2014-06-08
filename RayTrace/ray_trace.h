#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

#include "vector3f.hpp"
#include "primitive.hpp"
#include "kdtree.hpp"

using namespace std;

vector3f get_ray_trace(ray &r, kdtree &root, int depth = 0);