#!/bin/bash

set -e

# USAGE: ./speed-test.sh [num-particles]
# - num-particles: defalt 1000000

# where we'll output our flake
FLAKE=/tmp/output.flake
IMAGE=/tmp/output.png

# how big should the flake be
PARTICLES=${1:-1000000}

# build everything
echo Building executable
echo

cd build
cmake ..
make clean snowflake

# wipe any existing runs
FLAKE=/tmp/output.flake
IMAGE=/tmp/output.png

rm -f ${FLAKE} ${IMAGE}

# generate the flake
echo
echo Generating ${PARTICLES} particle flake

time ./snowflake gen --output ${FLAKE} --num-particles ${PARTICLES} --silent

# render the flake
echo
echo Rendering the flake

time ./snowflake render --input ${FLAKE} --output ${IMAGE} --silent

# Let the user know we're done
echo
echo All tests done

# Attempt to open the rendered flake as proof
echo Opening rendered flake in image viewer
xdg-open ${IMAGE}
