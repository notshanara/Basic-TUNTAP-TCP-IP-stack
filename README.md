 Key points Covered:
  - Ethernet parsing, frame encapsulation and dispatching. (Data link - Layer2)
  - ARP Request/Reply processing and resolution cache (Address resolution - Layer 3)

NEEDED:
Runs only on Linux based operating systems
GCC compiler and MAKE
Root Priviledge or CAP_NET_ADMIN capacity - needed to allocated TAP interface

Building:-

gcc-Iinclude src/*.c -o tcp_ip_stack


Running:-

sudo ./tcp_ip_stack
    
REPOSITORY STRUCTURE

tcp_ip_stack/
├── include/
│   ├── arp.h         # ARP frame definitions & handler headers
│   ├── basic.h       # Base types and macros
│   ├── ethernet.h    # Ethernet II frame header definitions
│   ├── netdev.h      # Virtual network device management interface
│   ├── syshead.h     # System header includes & dependencies
│   ├── tuntap_if.h   # Linux TUN/TAP interface definitions
│   └── utils.h       # Utility functions (checksums, byte conversions)
└── src/
    ├── arp.c         # ARP protocol implementation & cache management
    ├── ethernet.c    # Ethernet frame parsing & processing
    ├── main.c        # Main entry point and packet processing loop
    ├── netdev.c      # Network device initialization & setup
    ├── tuntap_if.c   # TAP device allocation & read/write primitives
    └── utils.c       # Helper utilities
