#include "bsp.h"

// Creates an empty flake with the given size
flake *new_flake(double S);

// Destroys the flake, freeing memory
void destroy_flake(flake *f);

// A convenience method to create a new flake of the given
// size, copy across all points, and destroy the old flake
flake *change_flake_size(flake *f, double new_size);

// Adds a point to the flake
void add_point_to_flake(flake *f, double x, double y);

// Returns the distance to the nearest point in the flake
flake_result find_nearest_in_flake(flake *fb, double x, double y);
