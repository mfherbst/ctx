# ``ctx``: A drop-in replacement for Q-Chem's ``libctx``

``ctx`` is a drop-in replacement for the default context library ``libctx``
of the Q-Chem quantum chemistry software. It provides (almost) the same
interface as ``libctx``, but uses a different backend, namely the
``krims::ParameterMap`` class of the
[krims](https://github.com/linalgwrap/krims.git) library.
The main purpose of this library is therefore to provide an interface between
``libctx::context`` objects and ``krims::ParameterMap`` objects.

## Obtaining and building ``ctx``
Check out the ``ctx`` git repository on ``ccsvn.iwr.uni-heidelberg.de``.
All compilers starting from ``clang-3.5`` and ``gcc-4.8`` should be able to build the code.
``C++11`` support is absolutely required.

``ctx`` depends on [krims](https://github.com/linalgwrap/krims.git) and
(if testing is enabled) on the third-party library
[Catch](https://github.com/philsquared/Catch/) as well.
Both dependencies are automatically downloaded and compiled if you set the
cmake property ``AUTOCHECKOUT_MISSING_REPOS`` is set to ``ON``.

In order to build with tests (recommended) run
```
mkdir build && cd build
cmake -DAUTOCHECKOUT_MISSING_REPOS=ON ..
cmake --build .
ctest
```

In order to build without tests run
```
mkdir build && cd build
cmake -DKRIMS_ENABLE_TESTS=OFF ..
cmake --build .
ctest
```

## Motivation for re-inventing the wheel
One might wonder why I decided to re-write ``libctx`` from scratch.
My reasons were:
- ``libctx`` has a very verbose and clunky interface.
- The ``rc_ptr`` implementation of a reference-counted pointer is
  not thread-safe.
- The ``ParameterMap`` we recently implemented into the ``krims``
  library has a more intuitive interface when it comes to updating
  or storing data. It also provides more features for accessing
  stored values. Error messages are more verbose.
- ``context`` objects in ``libctx`` do not allow to
  iterate over the keys they contain.
  One always has to *know* what keys are sensible.

## Replacing Q-Chem's ``libctx`` by ``ctx``
It should be as simple as replacing the ``libctx``
directory by the files of this repository.
If there are problems compiling Q-Chem this way,
please let [me](AUTHORS.md) know.

### A word of warning
I have thoroughly tested both ``ctx`` and ``libctx``
using the unit test suite in this project and made
sure that both libraries show the identical behaviour.
Of cause there could be things I have missed, so
please beware that ``ctx`` is *not exactly} ``libctx``.   

Other points to note:
- ``ctx`` and ``libctx`` largely show have the same interface,
  but in order to simplify the implementation with the
  ``krims::ParameterMap`` I decided to drop support for some stuff.
  To the best of my knowledge these features have been unused anyway.
  If this leads to problems when compiling code,
  please let [me](AUTHORS.md) know. I consider this a bug.
- Most notable this concerns:
	- [``params``](src/ctx/params.h) has no support for iterators
	- [``params``](src/ctx/params.h) may not take keys containing the character "/".

### Improvements over ``libctx``
- TODO: Mention extra interface to ``context`` and ``params`` via a ``ParameterMap`` reference.

### Testing Q-Chem's ``libctx`` against the ``ctx`` test suite
It is possible to run the test suite of this library against Q-Chem's ``libctx`` library code.   

For this the build process expects that ``libctx`` is already checked out and
built under ``external/libctx``. You can roughly achieve this like so
```
# First checkout libctx into external/libctx
cd external
./get_libctx.sh

# Compile libctx into external/libctx/build
cd libctx
mkdir build
cd build
cmake ..
make -j 4
```
Now you can configure the built of this project as usual.
Just set the cmake property ``TEST_QCHEM_LIBCTX`` via
the flag ``-DTEST_QCHEM_LIBCTX=ON`` when you run run cmake.

