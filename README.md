# ``ctx``: Key-value `C++` datastructures for organised hierarchical storage
[![Build Status](https://travis-ci.org/mfherbst/ctx.svg?branch=master)](https://travis-ci.org/mfherbst/ctx)
[![Licence](https://img.shields.io/github/license/mfherbst/ctx.svg)](LICENSE)

A common pattern in numerical computation is that input parameters or
computed simulation data should be stored in a tree-like, hierarchical fashion.
This is no surprise, since different steps of a simulation algorithm
often deal with larger numbers of parameters or data,
where only a subset is of concern for any other part of the
complete simulation procedure.
Thus it makes sense to keep the data, which deals with
one step of the simulation, well-separated from
the data of any other step.
A tree-like structure, where different steps
associate with different subtrees
is a very intuitive approach to achieve that,
effectively shielding the context of objects
related to one step from the objects of others.
At the same time this naturally allows to represent the structure
of the simulation framework in the storage scheme as well.

This library offers a `C++` implementation of
of such a tree-like string-to-value mapping, the `CtxMap`.
While the key in such a mapping is taken to be a path-like
string such as `/this/is/a/path/to/a/value`,
the value can be of arbitrary type.
My means of rich functionality, such as views into subtrees
or iterators over ranges of keys,
navigating and accessing such a hierarchical data storage
from different parts of a code is greatly facilitated.

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
to the concept of a `context` storage as it was implemented
in the `libctx` library of the
quantum-chemistry package [Q-Chem](https://q-chem.com).
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
Under the hood the more flexible `CtxMap` interface
is always accessible, allowing new take to make
use of it in a backwards-compatible way.
As of March 2019, `ctx` has fully replaced `libctx`
inside the [Q-Chem](https://q-chem.com) source tree.

## Differences between Q-Chem's ``libctx`` and ``ctx``
As mentioned above ``ctx`` and ``libctx`` largely have the same interface,
but in order to simplify the implementation with the
``CtxMap`` support for a few features were dropped.
Most notable deviations from ``libctx``:

- **Support** for the deprecated ``std::auto_ptr`` class **has been dropped**.
  The last `ctx` version with ``std::auto_ptr`` is v2.4.1.
- [``params``](src/ctx/params.h) has no support for iterators
- Keys inside [``params``](src/ctx/params.h) objects may not
  contain the character `"/"`.

Improvements over ``libctx`` include:
- The ``libctx::root_storage`` of ``libctx::context`` objects is a ``CtxMap``.
  In other words, ``libctx::context`` is really only a wrapper
  around ``CtxMap``.
  Since all changes done by either acting on the ``root_storage`` or
  the ``libctx::context`` wrapper effect the same underlying object
  in memory, both interfaces can be used simultaneously, e.g.
  
  ```cpp
  ctx::CtxMap stor{{"bla", 5}};
  libctx::context ctx(stor);

  ctx.update("bla", rc_ptr<int>(new int(6)));
  stor.update("bla", 7);

  std::cout << *ctx.get<int>("bla");
  ```
  
  will print the value ``7`` which has been set via the ``stor`` object,
  i.e. the ``CtxMap`` interface.
- From a ``ctx::context`` the wrapped ``CtxMap`` object,
  with its richer functionality can be accessed using the ``map()`` function.
- ``libctx::params`` objects contain a ``CtxMap`` to store their data.
  It can be referenced (and altered) using the ``map()`` function as well.
- Thanks to the ``CtxMap``, ``libctx::context`` objects can now be printed,
  including the data types of the stored data and (for some data types)
  even the values. This is implemented via the usual ``operator<<``
  functionality.

## Citation [![DOI](https://zenodo.org/badge/163624261.svg)](https://zenodo.org/badge/latestdoi/163624261) [![bibtex](https://img.shields.io/badge/bibtex-download_citation-red.svg)](https://michael-herbst.com/publications/2019.03.14_ctx.bib)
You can cite `ctx` using this
[`bibtex` entry](https://michael-herbst.com/publications/2019.03.14_ctx.bib)
or simply as  

Michael F. Herbst. *ctx: Key-value C++ datastructures for organised hierachical storage* (2019). DOI [10.5281/zenodo.2590706](https://doi.org/10.5281/zenodo.2590706).
