====================
 SMARTBoard studies
====================

This project aims to study and solve Linux kernel oopsing related to
SMARTBoard usage.

Problem description
===================

SMARTBoard connected to a machine via USB while SMARTBoardService-binary
is running causes sporadic kernel oopses. Others have reported same kind
of issues too [#]_.

Altough oopsing is quite sporadic in normal conditions, it can be made
almost certain by scanning USB bus simultaneously with several ``lsusb``
processes in parallel. The exact test setup and reproduction steps are
described in `Test setup`_.

Full console logs are available under ``data/`` accompanied with machine
related information.

.. [#] http://lkml.indiana.edu/hypermail/linux/kernel/1112.1/00007.html

Goals
=====

1. Develop a reliable method for reproducing the issue with *specific*
   hardware, i.e. SMARTBoard connected to our test equipment.

2. Develop a reliable method for reproducing the issue with *generic*
   USB hardware, e.g. memory stick or webcam.

3. Identify the root cause and pinpoint the exact code section.

4. Develop a patch which fixes the issue and send it upstream.

Test setup
==========

So far, we have tested SMARTBoard in LTSP-environment, connected to fat
clients. We have tested it in two different machines which differ only
in hardware. The client image in both machines is basically the same and
is based on Ubuntu 10.04 (Lucid Lynx).

Host machines:

* ``dell-optiplex-790``
* ``fujitsu-siemens-esprimo-e``

Details of hardware configuration of each machine can be found under
``data/``.

Hosts were configured to output their console log to serial console with
following kernel command line [*]_::

  console=ttyS1,115200n81

.. [*] ``9600n81`` was tested also but it didn't work reliably and
       printed byte soup from time to time. Weird.

Steps to reproduce
==================

1. Login as unprivileged user, SMARTBoard connected via USB.
2. Make sure that all the necessary SMARTBoard-related processes up and
   running. In our machines, following processes were found to be running::

     vieras    3263  0.0  0.0   1828   540 ?        S    17:00   0:00 /bin/sh /opt/SMART Technologies/SMART Product Drivers/bin/SMARTBoardService
     vieras    3265  0.0  0.0   1828   540 ?        S    17:00   0:00 /bin/sh /opt/SMART Technologies/SMART Product Drivers/bin/SMART Board Tools
     vieras    3271  1.4  0.9 209180 19748 ?        Sl   17:00   0:10 /opt/SMART Technologies/SMART Product Drivers/bin/.SMARTBoardService_elf
     vieras    3272  0.0  0.9  78380 18968 ?        Sl   17:00   0:00 /opt/SMART Technologies/SMART Product Drivers/bin/.SMART Board Tools_elf
     vieras    3352  0.0  0.0   1828   540 ?        S    17:00   0:00 /bin/sh /opt/SMART Technologies/SMART Product Drivers/bin/Marker
     vieras    3357  0.1  0.9 104012 20156 ?        Sl   17:00   0:00 /opt/SMART Technologies/SMART Product Drivers/bin/.Marker_elf
     vieras    3360  0.0  0.0   1828   536 ?        S    17:00   0:00 /bin/sh /opt/SMART Technologies/SMART Product Drivers/bin/FloatingTools
     vieras    3365  0.0  0.9 103000 19624 ?        Sl   17:00   0:00 /opt/SMART Technologies/SMART Product Drivers/bin/.FloatingTools_elf

3. Run ``lsusb_bomb.sh``.

   This is a very simple script which runs 15 loops of ``lsusb``
   processes in parallel. The end result is that there are always about
   15 ``lsusb`` processes running for a very long time.

4. Wait until the kernel oopses, couple of minutes approximately.

Analysis
========

