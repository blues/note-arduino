************************
Calling the Notecard API
************************

The `Notecard API <https://dev.blues.io/api-reference/notecard-api/introduction/>`_ is JSON-based. note-c uses a modified version of the `cJSON library <https://github.com/DaveGamble/cJSON>`_ for working with JSON. The core JSON struct used in note-c is ``J``. You should treat this type as opaque; you will only ever need to work with ``J *`` when using note-c.

To make a Notecard API call, first create a JSON request:

.. code-block:: c

   J *req = NoteNewRequest("note.add");

When ``req`` is serialized to a string, it looks like this:

.. code-block:: json

   {"req": "note.add"}

To add fields to the request, there are a number of ``JAdd*`` functions:

* :c:func:`JAddBoolToObject`
* :c:func:`JAddNumberToObject`
* :c:func:`JAddStringToObject`
* :c:func:`JAddObjectToObject`
* :c:func:`JAddArrayToObject`

For example, to add the field ``"file": "data.qo"`` to ``req``, use :c:func:`JAddStringToObject`:

.. code-block:: c

   JAddStringToObject(req, "file", "data.qo")

Now, when ``req`` is serialized, it will have the ``"file"`` field:

.. code-block:: json

   {"req": "note.add", "file": "data.qo"}

Once the request has been prepared, send it to the Notecard with :c:func:`NoteRequestResponse`:

.. code-block:: c

   J *rsp = NoteRequestResponse(req);

``rsp`` holds the JSON response from the Notecard. Use :c:func:`NoteResponseError` to check the response for errors:

.. code-block:: c

   if (rsp == NULL || NoteResponseError(rsp)) {
      // handle error
   }

``NoteRequestResponse`` will take care of freeing the passed in request object, but you must free the response with :c:func:`NoteDeleteResponse`:

.. code-block:: c

   NoteDeleteResponse(rsp);

The function :c:func:`NoteRequest` is useful when you don't care about the details of the Notecard's response, just whether the API call was successful or not. It returns true if the API call was successful and false if there was an error sending the request or if there was an error in the response (i.e. it calls :c:func:`NoteResponseError` internally):

.. code-block:: c

   if (NoteRequest(req)) {
      // success
   }
   else {
      // handle error
   }

:c:func:`NoteRequest` frees the request object for you.

.. note::
   To use plain C-strings as the JSON interface into note-c, rather than ``J *``, check out :c:func:`NoteRequestResponseJSON`

Examples
========

For examples of real applications calling the Notecard API with note-c, check out some of the `Zephyr <https://zephyrproject.org/>`_-based `Blues App Accelerators <https://github.com/blues/app-accelerators>`_:

* `Valve Monitor and Control <https://github.com/blues/app-accelerators/tree/main/09-valve-monitor/firmware/zephyr>`_
* `Temperature and Humidity Monitor <https://github.com/blues/app-accelerators/tree/main/18-temperature-and-humidity-monitor/firmware/zephyr>`_
