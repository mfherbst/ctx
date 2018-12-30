# ``ctx``: Key-value datastructure for organised hierarchical storage

A common pattern in numerical computation is that input parameters or
computed simulation data occurs in a tree-like, hierarchical fashion.
That is to say, that different steps of a simulation algorithm
often deal with larger numbers of parameters or data,
which is not necessarily of concern for any other part of the full simulation.
As a result, such objects are best grouped together
in one part of a larger tree, separate from
another subtree with the context of data and parameters
for yet another step of the full procedure.

This library allows to achieve exactly that
by providing a tree-like string-to-value mapping, the `CtxMap`.
While the key in such a mapping is forced to be a path-like
string such as `/this/is/a/path/to/a/value`,
the value can be of arbitrary type.
My means of rich functionality, such as views into subtrees
or iterators over ranges of keys,
navigating and accessing such a hierarchical data storage
is greatly facilitated.

Other key features include:
  - **Based on `shared_ptr`**: All data is stored as `std::shared_ptr`
    integrating well into the `C++` standard library and modern
    `C++` codes.
  - **Type safety**: Even though arbitrary types may be stored
    inside a `CtxMap`, an explicit checking mechanism makes sure
    that the type is kept consistent.
  - **Thread-safety**: Access to the `CtxMap` *should* be thread-safe,
    even though this has not been properly tested.

## Obtaining and building ``ctx``
Check out the ``ctx`` git repository.
All compilers starting from ``clang-3.5`` and ``gcc-4.8`` should
be able to build the code. ``C++11`` support is required.

To build and test:
```
mkdir build && cd build
cmake ..
cmake --build .
ctest
```

## Motivation
The driving force behind `ctx` was to provide a more modern approach
to the concept of a `context` storage as implemented in the
`libctx` library
within the quantum-chemistry package [Q-Chem](https://q-chem.com)
For this the design of data structures such as the
[PamMap](https://github.com/mfherbst/pammap),
and the
[GenMap](https://github.com/lazyten/krims#genmap-a-hierachical-dictionary-for-managing-data-of-arbitrary-type)
was expanded to support all functionality of `libctx`,
leading to the `CtxMap`.
Based on the `CtxMap` back-end, `ctx` provides
a compatibility layer,
which offers (almost) the same interface as the original `libctx`
by E. Epifanovsky *et. al.*.

## Replacing Q-Chem's ``libctx`` by ``ctx``
In theory it should be as simple as replacing the ``libctx``
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
perfectly and has been used in this way for over two years
in production.  

Other points to mention:
- ``ctx`` and ``libctx`` largely have the same interface,
  but in order to simplify the implementation with the
  ``CtxMap`` I decided to drop support for some stuff.
  To the best of my knowledge these features are not used anywhere.
  If this leads to problems when compiling Q-Chem code,
  please let [me](AUTHORS.md) know. This is considered a bug.
- Most notable deviations from ``libctx``:
	- [``params``](src/ctx/params.h) has no support for iterators
	- Keys inside [``params``](src/ctx/params.h) objects may not
	  contain the character `"/"`.

### Improvements over ``libctx``
- This library has extra functionality via the ``map()`` function, which
  offers access to the internal ``CtxMap`` object.
- The ``root_storage`` of ``ctx::context`` objects is exactly the ``CtxMap``.
  In other words, ``ctx::context`` is really only a wrapper around ``CtxMap``.
  Since all changes done by either acting on the ``root_storage`` as well
  as the ``ctx::context`` effect the other object, both interfaces
  can be used simultaneously, e.g.
  
  ```cpp
  ctx::CtxMap stor{{"bla", 5}};
  ctx::context ctx(stor);

  ctx.update("bla", rc_ptr<int>(new int(6)));
  stor.update("bla", 7);

  std::cout << *ctx.get<int>("bla");
  ```
  
  will print the value ``7`` which has been set via the ``stor`` object,
  i.e. the ``CtxMap`` interface.
- ``ctx::params`` objects contain a ``CtxMap`` to store their data.
  It can be referenced (and altered) using the ``map()`` function.
- Thanks to the ``CtxMap``, ``ctx::context`` objects can now be printed,
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
Now you may configure and build this project.
In order to have the tests use the ``libctx`` of
``external/libctx``, set the cmake property
``TEST_QCHEM_LIBCTX`` via the commandline flag ``-DTEST_QCHEM_LIBCTX=ON``
when you run cmake.
