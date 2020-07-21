# cpphots

An implementation of [HOTS](https://www.neuromorphic-vision.com/public/publications/57/publication.pdf).

## Install

The library depends on [Eigen](http://eigen.tuxfamily.org) and [Sepia](https://github.com/neuromorphic-paris/sepia).
The folder containing sepia.hpp must be specified to `cmake` using the `SEPIA_DIR` flag.

## Documentation

After running `cmake`, the documentation can be built with [doxygen](https://www.doxygen.nl) by running `make doc`.
