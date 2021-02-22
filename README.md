# cpphots

[![Documentation Status](https://readthedocs.org/projects/cpphots/badge/?version=latest)](https://cpphots.readthedocs.io/en/latest/?badge=latest)

An implementation of [HOTS](https://www.neuromorphic-vision.com/public/publications/57/publication.pdf).

## Install

The library depends on [Eigen](http://eigen.tuxfamily.org) and [Sepia](https://github.com/neuromorphic-paris/sepia) (which is included automatically).
`cpphots` can be built with `cmake` using the normal pipeline of commands:

```
cmake -S . -B build
cmake --build build
cmake --build build --target install
```

### Main configuration options:
| Option             | default | description                            | dependencies                      |
|:-------------------|:-------:|:---------------------------------------|:----------------------------------|
| `DOUBLE_PRECISION` | `OFF`   | use double precision for time surfaces |                                   |
| `BUILD_PLOTS`      | `ON`    | build plotting utilities               | Python 3 (`requirements.txt`)     |
| `BUILD_EXAMPLES`   | `OFF`   | build examples executables             |                                   |
| `BUILD_TEST`       | `OFF`   | build test suite                       |                                   |
| `BUILD_DOCS`       | `OFF`   | configure for building documentation   | [doxygen](https://www.doxygen.nl) |
| `WITH_SPHINX`      | `OFF`   | build documentation with sphynx        | Python 3 (`docs/requirements.txt`)|


## Usage

After installation, `cpphots` can be included in a `cmake`-based project as follows:

```
find_package(cpphots REQUIRED)

target_include_directories(<target> PRIVATE ${CPPHOTS_INCLUDE_DIRS})

target_link_libraries(<target> ${CPPHOTS_LIBRARIES})
```

## Documentation

An online version of the documentation can be found [here](https://cpphots.readthedocs.io/en/latest/). Offline documentation con be built with [doxygen](https://www.doxygen.nl) (and optionally with [sphinx](https://www.sphinx-doc.org/)) as follows: after configuring with `cmake`, run `cmake --build build --target docs`.
