DECurrent firmware structure
============================

The device has three main input sources:

* Up to 200 MSps x 8bit logic signal capture through QuadSPI peripheral.
* Up to 3 MSps x 16bit x 3 analog captures using ADC1-3 peripherals.
* Up to 96 MSps x 2bit logic signal capture for the USB analyzer, using
  SPI3 and SPI6 peripherals

Additionally there is one output channel:

* Up to 4 MSps DAC1 output

All the captured data is transmitted to control PC using USB 2.0 High-Speed
(480Mbit/s) interface.

Memory management
-----------------
Capture buffers are allocated from a shared pool of 16kB blocks. These comprise
the majority of 512kB AXI-SRAM and SRAM1/2/3/4 memory areas. Data is collected
using MDMA.

Thread stacks are stored in the 128kB DTCM memory for fastest access.

Data flow
---------
Data from input peripherals is transferred to RAM using MDMA block. Once a block
has been filled, the MDMA interrupt handler will set up the next transfer block
and add the previous one to a processing queue.

Each input source has its own thread handling the queued blocks. The block data
is compressed and embedded into a protobuf message, which is stored in a new
memory block. A single compressed message may contain data from multiple input
blocks. The result blocks are stored in another queue for transmission by USB.

The USB thread takes queued blocks and transmits them to PC using a bulk IN
endpoint. The blocks are divided to 512 byte USB packets.
