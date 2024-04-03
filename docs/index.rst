.. emu documentation master file, created by
   sphinx-quickstart on Fri Mar  4 15:58:27 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to emu's documentation!
===============================

.. toctree::
   :maxdepth: 2
   :caption: Contents:



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Docs
====

C++
----

.. doxygenstruct:: emu::scoped

You can also generate new function with partial application
of values or callables as following :

.. list-table:: Title
   :widths: 25 25 50
   :header-rows: 1

   * -
     - regular order
     - reverse order
   * - value
     - partial
     - rpartial
   * - callable
     - partial_apply
     - rpartial_apply
