# Snowflake
This generates snowflakes by simulating random walks of individual particles until they collide with the snowflake.

This idea was first catalysed by a friend doing the same thing for a university project. I then decided to also have a go and see how fast I could get it. The initial plan was to go for high speed whilst still maintaining correctness of the algorithm as even small inaccuracies can cause large changes to the overall structure of the snowflake.

To walk a particle, we don't simulate every step but make large jumps. We also work entirely with floating point numbers rather than integers to stop the square grid influencing the snowflake shape. The algorithm is to find the closest element of the current flake to our particle, if we have not collided then use the distance to the flake and move the particle that far in some random direction and repeat. This is correct as a random walk has a uniform chance of hitting any point on a surrounding circle.

We use a BSP to store the snowflake as that was deemed to be the fastest datastructure for querying the nearest element to a point.

The output format is TGA because that was the easiest to manually write.
