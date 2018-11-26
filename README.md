# ``ctx``: A drop-in replacement for Q-Chem's ``libctx``

``ctx`` is a drop-in replacement for the default context library ``libctx``
of the Q-Chem quantum chemistry software. It provides (almost) the same
interface as ``libctx``, but uses a different back-end, namely
the ``PamMap`` class based on `shared_ptr` objects.

## Obtaining and building ``ctx``
Check out the ``ctx`` git repository.
All compilers starting from ``clang-3.5`` and ``gcc-4.8`` should be able to build the code.
``C++11`` support is absolutely required.

To build and test:
```
mkdir build && cd build
cmake ..
cmake --build .
ctest
```

## Motivation for re-inventing the wheel
One might wonder why I decided to re-write ``libctx`` from scratch.
My reasons were:
- ``libctx`` has a very verbose and clunky interface.
- The ``rc_ptr`` implementation of a reference-counted pointer is
  not thread-safe.
- The ``PamMap`` is a slightly specialised variant of the ``GenMap``
  I implemented a while back into [``krims``](https::/lazyten.org/krims).
  Both the ``PamMap`` as well as the ``GenMap`` have a more intuitive
  interface when it comes to updating  or storing data. It also provides
  more features for accessing stored values.
- ``context`` objects in ``libctx`` do not allow to
  iterate over the keys or the value they contain.
  One always has to *know* what keys are sensible. Printing the values
  and types of all keys in the context is not possible as far as I know.

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
please beware that ``ctx`` is *not exactly* ``libctx``.
It should be noted, however, that *adcman* work with ``ctx``
perfectly and has been used by myself in this way for
quite some time now.  

Other points to mention:
- ``ctx`` and ``libctx`` largely have the same interface,
  but in order to simplify the implementation with the
  ``PamMap`` I decided to drop support for some stuff.
  To the best of my knowledge these features are not used anywhere.
  If this leads to problems when compiling Q-Chem code,
  please let [me](AUTHORS.md) know. I consider this a bug.
- Most notable deviations from ``libctx``:
	- [``params``](src/ctx/params.h) has no support for iterators
	- Keys inside [``params``](src/ctx/params.h) objects may not
	  contain the character "/".

### Improvements over ``libctx``
- This library has extra functionality via the interface to the internal ``PamMap``
  objects.
- The ``root_storage`` of ``ctx::context`` objects is exactly the ``PamMap``.
  In other words, ``ctx::context`` is really only a wrapper around ``PamMap``.
  Since all changes done by either acting on the ``root_storage`` as well
  as the ``ctx::context`` effect the other object, both interfaces
  can be used simultaneously, e.g.
  
  ```cpp
  ctx::PamMap stor{{"bla", 5}};
  ctx::context ctx(stor);

  ctx.update("bla", rc_ptr<int>(new int(6)));
  stor.update("bla",7);

  std::cout << *ctx.get<int>("bla");
  ```
  
  will print the value ``7`` which has been set via the ``stor`` object,
  i.e. the ``PamMap`` interface.
- ``ctx::params`` objects contain a ``PamMap`` to store their data.
  It can be referenced (and altered) using the ``map()`` function.
- Thanks to the ``PamMap``, ``ctx::context`` objects can now be printed,
  including the data types of the stored data and (for some data types)
  even the values. This is implemented via the usual ``operator<<``
  functionality.

### Testing Q-Chem's ``libctx`` against the ``ctx`` test suite
It is possible to run the test suite of this library against Q-Chem's ``libctx`` library code.   

For this the build process expects that ``libctx`` is already checked out and
built under ``external/libctx``. You can roughly achieve this like so
```
# First checkout libctx into external/libctx
cd external
svn checkout https://url/to/libctx/trunk libctx

# Compile libctx into external/libctx/build
cd libctx
mkdir build
cd build
cmake ..
make -j 4
```
Now you can configure the built of this project.
In order to also make the test which employs the ``libctx``
in ``external/libctx`` set the cmake property
``TEST_QCHEM_LIBCTX`` via the commandline flag ``-DTEST_QCHEM_LIBCTX=ON``
when you run cmake.
