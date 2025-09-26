:orphan:

###########################
lib_xtnd: Plugins for xcore
###########################

:vendor: @xross
:version: 0.1.0
:scope: Demo
:description: Plugin-like system for xcore
:category: Gerneral purpose
:keywords:
:devices: xcore-200, xcore.ai

*******
Summary
*******

``lib_xtnd`` (xcore Tactical Node Delivery) allows load and execution of code from flash (not XIP) where the whole application is
not known at compile time. This is useful for applications that wish to allow some limited form of
extensibililty by a user, for example for adding custom code to an ASSP.

********
Features
********

* Provides an easy way to bundle and call code from a plugin (or "xtender") binary
* No special tools or libraries required to build or use
* Look up plugin functions by simple names
* Runs plugin “startup” functions (constructors) when loaded such that resource such as ports can
  be used
* Small, fast(ish), and comes with clear examples to copy
* No dependencies on other tools or libraries (other than lquadflash for loading from flash)

****
TODO
****

* Use of thread local timer for thread number > 0 (add test)
* Thread local timers work because timer for thread 0 is always set as 1 in the timers table
   - This means it will be shared between the host and plugin - could be a problem
   - Consider problem case and add test
* Add a free function to unload the xtender
* Use a single tile xn when building the xtender
* use xmap --first FILE rather than ordering of src files to xcc
* Split ASSP and author and user documentation
* Check for fn count > number of functions in blob
* Documentation improvements
* Remove debug prints (use lib_logging)
* Make tests use some shared code...
* Add hashing for function names
* Support sharing a lock between host and plugin?
* Run destructors
* Automate function count in user code(?)
* Security features
* Allow export table to live anywhere in the binary(?
* Blob checking - checksum/signature etc
* Can we get a free CRC from from an xflash upgrade image?
  * Use a program running in xsim to generate the CRC?
* Patch plugin for syscalls? (or find another soluton)
* Remove warning: xmap: Warning: Image base on command line overrides config file for tile 0, node 0.
    - This is because xmap makes a internal config, there is an option (-c, undocumented) to pass a config file we could use


Missing Tests
=============

* xc lock allocated
* Bad magic number
* Bad version number
* Missing function
* Blob too big
* Blob too small
* Flash load

************
Known issues
************

* Limited code integrity checks: no checksum/signature; trust on load.
* Printing from plugin code is a faff (must use un-documented --syscall-address option to xsim)
* Fixed call signature: all exported functions assumed int f(int,int).
* No destructor support yet.
* No isolation or sandbox: plugin code can crash or corrupt host state.
* Blobs must be built for exact layout - build system a bit a abused to achieve this
* Minimal error reporting: failures inside plugin are indistinguishable from valid return codes.
* Stack usage hard-coded: trampoline declares fixed stack words; oversized plugin frames risk overflow.

****************
Development repo
****************

* `lib_xplug <https://www.github.com/xross/lib_xplug>`_ (https://www.github.com/xross/lib_xplug)

**************
Required tools
**************

* XMOS XTC Tools: 15.3.1

*********************************
Required libraries (dependencies)
*********************************

* None

*************************
Related application notes
*************************

* None

*******
Support
*******

Issues can be raised against the software at using GitHub `issues <https://github.com/xross/lib_xtnd/issues>`_.
