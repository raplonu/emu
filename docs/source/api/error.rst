.. _error-handling:

Error Handling
==============

This page documents the macros provided to simplify error handling in the EMU library.

Error Handling Macros
---------------------

These macros are defined in ``include/core/emu/error.hpp``.

.. doxygendefine:: EMU_RETURN_EC
      :project: emu

.. doxygendefine:: EMU_RETURN_UN_EC
      :project: emu

.. doxygendefine:: EMU_THROW
      :project: emu

.. doxygendefine:: EMU_THROW_WHAT
      :project: emu

.. doxygendefine:: EMU_TRUE_OR_RETURN_EC
      :project: emu

.. doxygendefine:: EMU_TRUE_OR_RETURN_UN_EC
      :project: emu

.. doxygendefine:: EMU_TRUE_OR_THROW
      :project: emu

.. doxygendefine:: EMU_TRUE_OR_THROW_WHAT
      :project: emu

.. doxygendefine:: EMU_CHECK_OR_RETURN_EC
      :project: emu

.. doxygendefine:: EMU_CHECK_OR_RETURN_UN_EC
      :project: emu

.. doxygendefine:: EMU_CHECK_OR_THROW
      :project: emu

.. doxygendefine:: EMU_CHECK_OR_THROW_WHAT
      :project: emu

.. doxygendefine:: EMU_UNWRAP_OR_RETURN_EC
      :project: emu

.. doxygendefine:: EMU_UNWRAP_OR_RETURN_UN_EC
      :project: emu

.. doxygendefine:: EMU_UNWRAP_OR_THROW
      :project: emu

.. doxygendefine:: EMU_UNWRAP_OR_THROW_WHAT
      :project: emu

.. doxygendefine:: EMU_UNWRAP_RES_OR_THROW
      :project: emu

.. doxygendefine:: EMU_UNWRAP_RES_OR_THROW_WHAT
      :project: emu


Error Generation Macros
-----------------------

These macros are defined in ``include/core/emu/error/generation.hpp``.

.. doxygendefine:: EMU_GENERATE_ERROR_CATEGORY_DEFINITION
      :project: emu

.. doxygendefine:: EMU_GENERATE_ERROR_CATEGORY
      :project: emu

.. doxygendefine:: EMU_GENERATE_ERROR_EXCEPTION
      :project: emu

.. doxygendefine:: EMU_GENERATE_ERROR_FUNCTION
      :project: emu

.. doxygendefine:: EMU_GENERATE_ERROR_INFRA
      :project: emu
