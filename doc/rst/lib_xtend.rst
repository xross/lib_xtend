############################
lib_xtend: Plugins for xcore
############################

************
Introduction
************

``lib_xtend`` provides extension mechanism for `xcore` based ASSP's via a plugin-like framework.
It's allows code to be build and executed wihout needing to have access to the full application
code-base via a plugin, or "`xtender`".

This is useful for producing ASSP like devices when the core funcitonality is provided and "locked
down" but additonal functionalilty/customisation is required by a user.

The flexabilty afforded by the system allows the ASSP developer to choose what level of
custimisation a user can provide. For example, an audio bassed ASSP may provide a an I2C interface
with calls the the extender to provide byte arrays to be written to said interface, a call to
`xtend_i2c_write()`, say.

The ASSP eveloper may chose allow ever futher customisation, encourangin the user to write their
own CODEC acces functions, be it SPI, UART or I2C, a call to `xtend_config_dac()`, say.

Importantly, the system requires no special tools or libraries to build ``lib_xtend`` into a
application or to build an `xtender`, other than the standard `XMOS` XTC toolchain.

*****************
Authoring an ASSP
*****************

Building with lib_xtend
=======================

``lib_xtend`` is intended to be used with the `XCommon CMake <https://www.xmos.com/file/xcommon-cmake-documentation/?version=latest>`_
, the `XMOS` application build and dependency management system.

To use this library in an application include ``lib_xtend`` in the application's ``APP_DEPENDENT_MODULES`` list in
`CMakeLists.txt`, for example:

.. code-block:: cmake

    set(APP_DEPENDENT_MODULES "lib_xtend")

.. note:: Dependent modules should be pinned to release versions where possible, otherwise the
   latest commit on the `develop` branch will be used.  For further details on managing modules,
   pinning to a release version and other options, please see the page `xcommon-cmake Dependency Management <https://www.xmos.com/documentation/XM-015090-PC/html/doc/dependency_management.html>`_.

All ``lib_xtend`` functions can be accessed via the ``xtend.h`` header file, for example:

.. code-block:: C

    #include "xtend.h"

Using lib_xtend
===============

Using ``lib_xtend`` breaks down to a few simple steps:

#. Load the `xtender` from flash, typically the data partition using ``xtend_read_flash()``

#. initalse the relevant ``lib_xtend`` data structure with the blob read usinf ``xtend_init()``

#. Locate the function or functions that need to be run from the `xtender` using ``xtend_find()``

#. Finally call the located function using ``xtend_call()``

.. note::

   If the `xtender` is not located in the data partition of the flash then implement a custom
   function to read in the `xtender` binary from the chose storage medium and pass the data to
    ``xtend_init()``.


What to provide to customisers
==============================

* a valid extender_export.S file
* a skeleton plugin xc/c file with empty functions
* A CMakeLists.txt file to build the plugin ``xe`` and ultimately the `xtender` binary

Security considerations
=======================

It is expected that the functionality provided by ``lib_xtend`` will be used in conjunction with
secure boot and secure flash to enure than only trusted code is executed on the device.

Since `lib_xtend` allows custom code to be executed on the device it is important to guard against
AES key leakage.

TODO CHECK DEFAUL XBURN BEHAVIOUR AND PROVIDE GUIDANCE


*********************
Authoring an `xtender`
*********************

Building an `xtender`
====================

It is recommended to use the ``xcommon-cmake`` build system like any other code base and implement
the functions as requested by the ASSP provider.

.. Note::

    the xtender_export.S file *must* be passed to the compiler first, this is currently achived by
    mis-use of the SRCS variables in the `CMakeLists.txt` file to achieve this.

Once the ``xe`` has been built it needs transforming into a binary blob that can be programmed to a flash
device. A `CMake` function is provided for convenience in `xtend_utils.cmake` called
``xtend_create_blob`` that can be used from the xtender's `CMakeLists.txt` file. By default this
builds a file named ``xtend_blob.bin`` in the project ``bin`` directory. However, for
competeness the steps are described below::

    xobjdump --strip plugin.xe # This produces plugin.xb
    xobjdump --split plugin.xb

The above produces a set of files including ``image_n0c0.bin`` which is the image that should be
programmed into the flash devices data partition.

..note::

    xobjdump is provided as part of the XTC tools.

For testing purposes a header file can be produced that contains the binary blob as a C array::

    xxd -i image_n0c0.bin > plugin_blob.h

.. note::

    xxd is a standard Unix tool, it may need installing separately on Windows.

Programming a plugin
====================

Progam the binary blob containing the `xtender` into the data partition of the flash device using xflash, remembering
to allocated some space for the main program if you intend to run from flash::

    xflash --boot-partition-size=0x20000 --data ./xtender/bin/xtend_blob.bin --target=XK-EVK-XU316

The main program can then be run as normal using `xrun`::

    xrun ./bin/apptest.xe

Or flashed for standalone operation::

    xflash --boot-partition-size=0x20000 --target=XK-EVK-XU316 ./bin/app_test.xe

The `xtender` and the program can be flash in one command::

    xflash --boot-partition-size=0x20000 --data ./xtender/bin/xtend_blob.bin --target=XK-EVK-XU316 ./bin/app_test.xe

*********************
Printing for a plugin
*********************

It may be desirable for a plugin to print to the console. Note, enabling printing in the plugin
will cause printing (and other syscalls e.g. program exit) in the main application not to function.

xsim
====

The simualator needs to knows when the syscall is located. To do this,
discover the value of ``syscall_off`` in ``xplug_header_t``. This can be done by printing it.

Then use the ``--syscall-address`` option to xsim to tell it where the syscall is located. For
example::

    xsim --syscall-address tile[0] 0x123456

xrun
====

There currently no way of telling xrun/xgdb where syscall is located so instead the symbol table
can be modified. Firstly ontain the address of ``_DoSyscall`` in the plugin, then::

    xobjdump --split test_xplug_print.xe

    llvm-objcopy --strip-symbol=_DoSyscall image_n0c0.elf out.elf

    llvm-objcopy --add-symbol=_DoSyscall=ABS:0x123456,global,function out.elf out2.elf

    xobjdump test_xplug_print.xe -r 0,0,out2.elf

Naturally, this approach also works for the simulator but is more effort than using the
``--syscall-address`` option.



*******************
Example application
*******************

Building the example
====================

This section assumes that the `XMOS XTC Tools <https://www.xmos.com/software-tools/>`_ have been
downloaded and installed. The required version is specified in the accompanying ``README``.

Installation instructions can be found `here <https://xmos.com/xtc-install-guide>`_.

Special attention should be paid to the section on
`Installation of Required Third-Party Tools <https://www.xmos.com/documentation/XM-014363-PC/html/installation/install-configure/install-tools/install_prerequisites.html>`_.

The application is built using the `xcommon-cmake <https://www.xmos.com/file/xcommon-cmake-documentation/?version=latest>`_
build system, which is provided with the XTC tools and is based on `CMake <https://cmake.org/>`_.

The ``lib_template`` software ZIP package should be downloaded and extracted to a chosen working
directory.

To configure the build, the following commands should be run from an XTC command prompt:

.. code-block:: bash

    cd lib_template/examples/app_template
    cmake -G "Unix Makefiles" -B build

If any dependencies are missing they will be retrieved automatically during this step.

The application binaries should then be built using ``xmake``:

.. code-block:: bash

    xmake -j -C build

Binary artifacts (.xe files) will be generated under the appropriate subdirectories of the
``app_template/bin`` directory — one for each supported build configuration.

For subsequent builds, the ``cmake`` step may be omitted.
If ``CMakeLists.txt`` or other build files are modified, ``cmake`` will be re-run automatically
by ``xmake`` as needed.

Running the example
===================

From an XTC command prompt, the following command should be run from the ``examples/app_template``
directory:

.. code-block:: bash

    xrun ./bin/app_template.xe

Alternatively, the application can be programmed into flash memory for standalone execution:

.. code-block:: bash

   xflash ./bin/app_template.xe

***************
Further reading
***************

* `Safeguard IP and device authenticity <https://www.xmos.com/documentation/XM-014363-PC/html/tools-guide/tutorials/safeguard-ip/safeguard.html>`_

*************
API Reference
*************

Doxygen documentation



