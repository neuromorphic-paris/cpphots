# cpphots

An implementation of [HOTS](https://www.neuromorphic-vision.com/public/publications/57/publication.pdf).

## Install

The library depends on [Eigen](http://eigen.tuxfamily.org) and [Sepia](https://github.com/neuromorphic-paris/sepia). `cpphots` can be built with `cmake` using the normal pipeline of commands, but the folder containing sepia.hpp must be specified to `cmake` using the `SEPIA_DIR` flag:

```
mkdir build && cd build
cmake -DSEPIA_DIR=<path/to/sepia> ..
make
make install
```

## Usage

After installation, `cpphots` can be included in a `cmake`-based project as follows:

```
find_package(cpphots REQUIRED)

target_include_directories(<target> PRIVATE ${CPPHOTS_INCLUDE_DIRS})

target_link_libraries(<target> ${CPPHOTS_LIBRARIES})
```

## Documentation

After running `cmake`, the documentation can be built with [doxygen](https://www.doxygen.nl) by running `make doc`.
