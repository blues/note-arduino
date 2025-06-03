Getting Started
===============

Notecard Background
-------------------

Before using note-c, it's essential to understand the fundamentals of communicating with a Notecard. Read the `Notecard Overview <https://dev.blues.io/notecard/notecard-walkthrough/overview/>`_ on the Blues developer site to familiarize yourself with the communications protocol. The protocol is JSON-based and supports `serial and I2C connections <https://dev.blues.io/notecard/notecard-walkthrough/notecard-interfaces/>`_.

Getting the Source Code
-----------------------

To use note-c in your project, you can `download or clone the git repository from GitHub <https://github.com/blues/note-c>`_. The library is composed of all the .c and .h files prefixed with "n\_" in the root directory of the repository. Simply add the .c files alongside your other source files and make sure the .h files are on an include path visible to your compiler.

Using the Library
-----------------

The pages below cover how to use note-c in your project.

* :doc:`library_initialization`
* :doc:`calling_the_notecard_api`
