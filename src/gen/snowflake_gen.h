#pragma once

#include <stdio.h>
#include "args/arg_parsing.h"

struct bsp_t;

void create_snowflake(int N, FILE *log, int symmetry_degree, symmetry_type_enum symmetry_type, int silent);
