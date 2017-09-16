#pragma once

#include <stdio.h>
#include "args/arg_parsing.h"

void create_snowflake(int N, FILE *log, flake_impl impl, int symmetry_degree, symmetry_type_enum symmetry_type, int silent);
