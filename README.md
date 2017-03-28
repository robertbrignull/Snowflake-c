# Snowflake
This generates snowflakes by simulating random walks of individual particles until they collide with the snowflake.

This idea was first catalysed by a friend doing the same thing for a university project. I then decided to also have a go and see how fast I could get it. The initial plan was to go for high speed whilst still maintaining correctness of the algorithm as even small inaccuracies can cause large changes to the overall structure of the snowflake.

To walk a particle, we don't simulate every step but make large jumps. We also work entirely with floating point numbers rather than integers to stop the square grid influencing the snowflake shape. The algorithm is to find the closest element of the current flake to our particle, if we have not collided then use the distance to the flake and move the particle that far in some random direction and repeat. This is correct as a random walk has a uniform chance of hitting any point on a surrounding circle.

We use a bsp to store the snowflake as that was deemed to be the fastest datastructure for querying the nearest element to a point.

The output format is always png because it is lossless and offers excellent compression.

### Use

Compile by running from the root of the project `cmake .; make`.

Then run `./snowflake --help` for more instructions on how to use the program.

For example, run `./snowflake gen -n 300000 -d 6 -t full -o output.flake` to generate a flake and then `./snowflake render -i output.flake -o output.png` to render it to an image.

To render a series of snapshots of the flake use `./snowflake render -i output.flake -o output_%06d.png -m -f 300`. To then combine the multiple images into a movie, use something like `ffmpeg -framerate 30 -pattern_type glob -i '*.png' -c:v libx264rgb -pix_fmt bgr24 out.mp4`.
