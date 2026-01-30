**********************
Library Initialization
**********************

.. contents:: Table of Contents
   :depth: 2

In order to send and receive data to and from the Notecard, note-c requires several hooks (i.e. callbacks) to be set. The implementations of the hooks are platform-specific and hence cannot be provided by note-c itself.

Blues develops several open source ports of these hooks for different platforms to accelerate getting started with note-c. The level of abstraction of these ports varies. For example, `note-stm32l4 <https://github.com/blues/note-stm32l4>`_ implements these hooks for the STM32L4 series of MCUs using the STM32L4 HAL, while `note-arduino <https://github.com/blues/note-arduino>`_ is a more generic port that allows you to use note-c with essentially any Arduino-compatible board.

For a comprehensive listing of note-c ports, go to :doc:`ports`.

For an example and detailed breakdown of implementing the hooks for a specific platform, go to :ref:`library_initialization:Example: note-c Hooks for STM32L4`.

Dynamic Memory and Timing Hooks
===============================

A port must set the dynamic memory and timing hooks using :c:func:`NoteSetFn` for:

* Allocating memory
* Freeing memory
* Millisecond delay
* Millisecond counter

I/O Hooks
=========

note-c maintains one active Notecard I/O interface (serial or I2C) at a time. Calling :c:func:`NoteSetFnSerial` sets the active interface to serial, and calling :c:func:`NoteSetFnI2C` sets the active interface to I2C.

Serial
------

:c:func:`NoteSetFnSerial` sets the serial hooks for:

* Resetting the serial peripheral
* Transmitting bytes
* Checking if there's a byte available to receive
* Receiving a single byte

I2C
---

:c:func:`NoteSetFnI2C` sets the I2C hooks for:

* Resetting the I2C peripheral
* Transmitting bytes
* Receiving bytes

:c:func:`NoteSetFnI2C` also requires two additional parameters: the I2C address of the Notecard and the maximum number of bytes to send to the Notecard in a single I2C chunk. To use the defaults for these two values, use the macros :c:macro:`NOTE_I2C_ADDR_DEFAULT` and :c:macro:`NOTE_I2C_MTU_DEFAULT`, respectively.

The I2C hooks are slightly more complex than the serial hooks. The transmit and receive hooks must implement the Notecard's serial-over-I2C protocol:

   *Unlike many fixed-length and register-based I2C protocols, the Notecard defines a variable-length, serial-over-I2C protocol that allows developers to handle JSON requests and responses in a similar manner as a direct serial connection.*

Familiarize yourself with the protocol using `this guide <https://dev.blues.io/guides-and-tutorials/notecard-guides/serial-over-i2c-protocol/>`_ from the Blues developer site. :ref:`library_initialization:Example: note-c Hooks for STM32L4` provides a detailed breakdown of implementing the protocol for STM32L4. While this example uses the STM32L4 HAL, the overall approach is generalizable to other platforms.

Mutex Hooks
===========

Notecard Mutex
--------------

A port must set the Notecard mutex hooks using :c:func:`NoteSetFnNoteMutex` if it intends to support multiple concurrent tasks that communicate with the Notecard. Specifically, the port must set lock and unlock hooks. Internally, note-c will call the lock hook before every transaction with the Notecard begins, and it will call the unlock hook after every transaction ends. This ensures that one task doesn't interrupt another task in the middle of a transaction.

The simplest implementation of these hooks relies on a flag indicating if access to the Notecard is locked by some other task. The lock hook sleeps/yields until the flag indicates the Notecard is unlocked and then acquires the lock itself:

.. code-block:: c

   void lockNotecard(void)
   {
       while (notecardLocked) {
           NoteDelayMs(10); // sleep 10 ms between checks
       }

       notecardLocked = true;
   }

The unlock hook simply clears the flag:

.. code-block:: c

   void unlockNotecard(void)
   {
       notecardLocked = false;
   }

I2C Mutex
---------

A port must set the I2C mutex hooks using :c:func:`NoteSetFnI2CMutex` if it intends to support multiple concurrent tasks that drive the same I2C bus that the Notecard is on. Specifically, the port must set lock and unlock hooks. The simple lock-unlock implementation described previously in :ref:`library_initialization:Notecard Mutex` can be used for these hooks, too. Internally, note-c will call the lock hook before every I2C transaction with the Notecard begins, and it will call the unlock hook after every I2C transaction ends. The other tasks using I2C must be amended to do the same.

Example: note-c Hooks for STM32L4
=================================

`note-stm32l4 <https://github.com/blues/note-stm32l4>`_ is the note-c port for the STM32L4 series of MCUs. This section walks through how the various note-c hooks are implemented for this platform. Notecard and I2C mutex hooks are not covered.

Dynamic Memory and Timing Hooks
-------------------------------

This line in `Src/main.c <https://github.com/blues/note-stm32l4/blob/master/Src/main.c>`_ sets the dynamic memory and timing hooks:

.. code-block:: c

   NoteSetFn(malloc, free, delay, millis);

``malloc``
^^^^^^^^^^

The memory allocation hook is set to ``malloc`` from libc.

``free``
^^^^^^^^

The memory freeing hook is set to ``free`` from libc.

``delay``
^^^^^^^^^

.. code-block:: c

   void delay(uint32_t ms) {
      HAL_Delay(ms);
   }

``delay`` calls into the STM32 HAL to delay for the specified number of milliseconds.

``millis``
^^^^^^^^^^

.. code-block:: c

   long unsigned int millis() {
      return (long unsigned int) HAL_GetTick();
   }

``millis`` uses the HAL to get the value of a continuously running millisecond counter.

I/O Hooks
---------

The code below from `Src/main.c <https://github.com/blues/note-stm32l4/blob/master/Src/main.c>`_ determines which I/O interface gets used, depending on the value of the ``NOTECARD_USE_I2C`` macro.

.. code-block:: c

   #if NOTECARD_USE_I2C
      NoteSetFnI2C(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MTU_DEFAULT, _noteI2CReset, _noteI2CTransmit, _noteI2CReceive);
   #else
      NoteSetFnSerial(_noteSerialReset, _noteSerialTransmit, _noteSerialAvailable, _noteSerialReceive);
   #endif

`_noteSerialReset`
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   bool _noteSerialReset() {
      MX_USART1_UART_DeInit();
      MX_USART1_UART_Init();

      return true;
   }

The serial reset hook reinitializes the serial peripheral connected to the Notecard.

`_noteSerialTransmit`
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   void _noteSerialTransmit(uint8_t *text, size_t len, bool flush) {
       HAL_UART_Transmit(&huart1, text, len, 5000);
   }

The serial transmit hook uses the HAL to write the data in the buffer to the serial peripheral connected to the Notecard.

`_noteSerialAvailable`
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   bool _noteSerialAvailable() {
      return (serialFillIndex != serialDrainIndex);
   }

For receiving serial data from the Notecard, note-stm32l4 uses a circular buffer. In the receive interrupt handler for the UART peripheral, this buffer gets populated with the received data. ``serialFillIndex`` is the write index into that buffer, and ``serialDrainIndex`` is the read index. If the two indices are equal, there's nothing new to read. If they aren't equal, there is something to read.

`_noteSerialReceive`
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   char _noteSerialReceive() {
       char data;
       while (!_noteSerialAvailable()) ;
       if (serialDrainIndex < sizeof(serialBuffer))
           data = serialBuffer[serialDrainIndex++];
       else {
           data = serialBuffer[0];
           serialDrainIndex = 1;
       }
       return data;
   }

The serial receive hook returns the byte from the circular receive buffer at the read index, handling the case where the index wraps around to 0.

`_noteI2CReset`
^^^^^^^^^^^^^^^^

.. code-block:: c

   bool _noteI2CReset(uint16_t DevAddress) {
       MX_I2C1_DeInit();
       MX_I2C1_Init();

       return true;
   }

The I2C reset hook reinitializes the I2C peripheral used to communicate with the Notecard.

`_noteI2CTransmit`
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   const char *_noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
       char *errstr = NULL;
       int writelen = sizeof(uint8_t) + Size;
       uint8_t *writebuf = malloc(writelen);
       if (writebuf == NULL) {
           errstr = "i2c: insufficient memory (write)";
       } else {
           writebuf[0] = Size;
           memcpy(&writebuf[1], pBuffer, Size);
           HAL_StatusTypeDef err_code = HAL_I2C_Master_Transmit(&hi2c1, DevAddress<<1, writebuf, writelen, 250);
           free(writebuf);
           if (err_code != HAL_OK) {
               errstr = "i2c: HAL_I2C_Master_Transmit error";
           }
       }
       return errstr;
   }

To frame the data to transmit to the Notecard using the serial-over-I2C protocol, the host first adds a byte indicating the number of bytes in the payload, which is ``Size``:

.. code-block:: c

   writebuf[0] = Size;

Then, it adds on the payload of ``Size`` bytes, which is pointed to by ``pBuffer``:

.. code-block:: c

   memcpy(&writebuf[1], pBuffer, Size);

Then the host calls the platform-specific method to send those bytes over I2C, which is ``HAL_I2C_Master_Transmit`` in this case.

`_noteI2CReceive`
^^^^^^^^^^^^^^^^^^

.. code-block:: c

   const char *_noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
       const char *errstr = NULL;
       HAL_StatusTypeDef err_code;

       // Retry transmit errors several times, because it's harmless to do so
       for (int i=0; i<3; i++) {
           uint8_t hdr[2];
           hdr[0] = (uint8_t) 0;
           hdr[1] = (uint8_t) Size;
           HAL_StatusTypeDef err_code = HAL_I2C_Master_Transmit(&hi2c1, DevAddress<<1, hdr, sizeof(hdr), 250);
           if (err_code == HAL_OK) {
               errstr = NULL;
               break;
           }
           errstr = "i2c: HAL_I2C_Master_Transmit error";
       }

       // Only receive if we successfully began transmission
       if (errstr == NULL) {

           int readlen = Size + (sizeof(uint8_t)*2);
           uint8_t *readbuf = malloc(readlen);
           if (readbuf == NULL) {
               errstr = "i2c: insufficient memory (read)";
           } else {
               err_code = HAL_I2C_Master_Receive(&hi2c1, DevAddress<<1, readbuf, readlen, 10);
               if (err_code != HAL_OK) {
                   errstr = "i2c: HAL_I2C_Master_Receive error";
               } else {
                   uint8_t availbyte = readbuf[0];
                   uint8_t goodbyte = readbuf[1];
                   if (goodbyte != Size) {
                       errstr = "i2c: incorrect amount of data";
                   } else {
                       *available = availbyte;
                       memcpy(pBuffer, &readbuf[2], Size);
                   }
               }
               free(readbuf);
           }
       }

       // Done
       return errstr;

   }

Setting aside the retry logic at the start and the error checking, the above code is simpler than it initially appears. To initiate a read with the Notecard, the serial-over-I2C protocol specifies that the host first send a 2-byte packet: a 0 followed by a byte indicating the max amount of data the host is prepared to receive. That's ``Size`` in this case, which is the available space in the receive buffer ``pBuffer``.

.. code-block:: c

   hdr[0] = (uint8_t) 0;
   hdr[1] = (uint8_t) Size;

Then, the host calls the platform-specific method to send those bytes over I2C.

Now, to read the data from the Notecard, the host calls the platform-specific method for an I2C read (``HAL_I2C_Master_Receive``). The packet it receives from the Notecard has a 2-byte header. The first byte indicates the number of bytes still available to read from the Notecard after this packet:

.. code-block:: c

   uint8_t availbyte = readbuf[0];

The second byte is the number of bytes in the current packet:

.. code-block:: c

   uint8_t goodbyte = readbuf[1];

The rest of the packet is the payload:

.. code-block:: c

   memcpy(pBuffer, &readbuf[2], Size);

Note that the number of bytes still available is returned to the caller via the ``available`` parameter:

.. code-block:: c

   *available = availbyte;

This lets note-c know if it should call this hook again to keep reading.
