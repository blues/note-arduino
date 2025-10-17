*************
API Reference
*************

.. contents:: Table of Contents
   :depth: 2

These pages cover the core functions and types that make up the note-c API.

Dynamic Memory and Timing Hooks
===============================

Functions
---------

.. doxygenfunction:: NoteSetFn

Types
-----

.. doxygentypedef:: mallocFn

.. doxygentypedef:: freeFn

.. doxygentypedef:: delayMsFn

.. doxygentypedef:: getMsFn


I/O Hooks
=========

Serial Hooks
------------

Functions
^^^^^^^^^

.. doxygenfunction:: NoteSetFnSerial

Types
^^^^^

.. doxygentypedef:: serialResetFn

.. doxygentypedef:: serialTransmitFn

.. doxygentypedef:: serialAvailableFn

.. doxygentypedef:: serialReceiveFn

I2C
---

Functions
^^^^^^^^^

.. doxygenfunction:: NoteSetFnI2C

Types
^^^^^

.. doxygentypedef:: i2cResetFn

.. doxygentypedef:: i2cTransmitFn

.. doxygentypedef:: i2cReceiveFn

Macros
^^^^^^

.. doxygendefine:: NOTE_I2C_ADDR_DEFAULT

.. doxygendefine:: NOTE_I2C_MTU_DEFAULT

.. doxygendefine:: NOTE_I2C_MTU_MAX

Mutex Hooks
===========

Notecard Mutex
--------------

Functions
^^^^^^^^^

.. doxygenfunction:: NoteSetFnNoteMutex

Types
^^^^^

.. doxygentypedef:: mutexFn

I2C Mutex
---------

Functions
^^^^^^^^^

.. doxygenfunction:: NoteSetFnI2CMutex

Types
^^^^^

See :c:type:`mutexFn`.

Notecard API Calls
==================

Functions
---------

.. doxygenfunction:: NoteNewRequest

.. doxygenfunction:: NoteRequestResponse

.. doxygenfunction:: NoteRequestResponseWithRetry

.. doxygenfunction:: NoteRequest

.. doxygenfunction:: NoteRequestWithRetry

.. doxygendefine:: NoteResponseError

.. doxygendefine:: NoteDeleteResponse

.. doxygenfunction:: NoteRequestResponseJSON

JSON Manipulation
=================

Functions
---------

.. doxygenfunction:: JAddBinaryToObject

.. doxygenfunction:: JAddBoolToObject

.. doxygenfunction:: JAddNumberToObject

.. doxygenfunction:: JAddStringToObject

.. doxygenfunction:: JAddObjectToObject

.. doxygenfunction:: JAddArrayToObject

.. doxygenfunction:: JCreateObject

.. doxygenfunction:: JDelete

.. doxygenfunction:: JFree

.. doxygenfunction:: JGetArray

.. doxygenfunction:: JGetBinaryFromObject

.. doxygenfunction:: JGetBool

.. doxygenfunction:: JGetInt

.. doxygenfunction:: JGetNumber

.. doxygenfunction:: JGetObject

.. doxygenfunction:: JGetString

.. doxygenfunction:: JIsPresent

.. doxygenfunction:: JMalloc

.. doxygenfunction:: JParse

.. doxygenfunction:: JPrintUnformatted


Types
-----

.. doxygenstruct:: J

.. doxygentypedef:: JNUMBER

.. doxygentypedef:: JINTEGER

.. doxygentypedef:: JUINTEGER

Macros
------

.. doxygendefine:: N_CJSON_NESTING_LIMIT

Logging
=======

Hooks
-----

Functions
^^^^^^^^^

.. doxygenfunction:: NoteGetFnDebugOutput

.. doxygenfunction:: NoteSetFnDebugOutput

Types
^^^^^

.. doxygentypedef:: debugOutputFn

Control
-------

Functions
^^^^^^^^^

.. doxygenfunction:: NoteSetLogLevel

.. doxygenfunction:: NoteDebugSyncStatus

Macros
^^^^^^

.. doxygendefine:: NOTE_C_LOG_LEVEL_ERROR

.. doxygendefine:: NOTE_C_LOG_LEVEL_WARN

.. doxygendefine:: NOTE_C_LOG_LEVEL_INFO

.. doxygendefine:: NOTE_C_LOG_LEVEL_DEBUG

.. doxygendefine:: SYNCSTATUS_LEVEL_MAJOR

.. doxygendefine:: SYNCSTATUS_LEVEL_MINOR

.. doxygendefine:: SYNCSTATUS_LEVEL_DETAILED

.. doxygendefine:: SYNCSTATUS_LEVEL_ALGORITHMIC

.. doxygendefine:: SYNCSTATUS_LEVEL_ALL