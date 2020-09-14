# cpphots

An implementation of [HOTS](https://www.neuromorphic-vision.com/public/publications/57/publication.pdf).

## Install

The library depends on [Eigen](http://eigen.tuxfamily.org) and [Sepia](https://github.com/neuromorphic-paris/sepia) (which is included automatically).
For the optional plotting utility, the library depends on Python >= 3, with its development files, and the packages listed in `requirements.txt`. This can be disable by passing `-DBUILD_PLOTS=OFF` to `cmake`.
`cpphots` can be built with `cmake` using the normal pipeline of commands:

```
cmake -S . -B build
cmake --build build
cmake --build build --target install
```

## Usage

After installation, `cpphots` can be included in a `cmake`-based project as follows:

```
find_package(cpphots REQUIRED)

target_include_directories(<target> PRIVATE ${CPPHOTS_INCLUDE_DIRS})

target_link_libraries(<target> ${CPPHOTS_LIBRARIES})
```

## Documentation

After configuring with `cmake`, the documentation can be built with [doxygen](https://www.doxygen.nl) by running `cmake --build build --target doc`.
