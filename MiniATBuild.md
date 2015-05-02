LLVM/MiniAT Build Guide
=======================

This guide details how to build llvm with support for the MiniAT backend.

# Prerequisites
    CMake           >=2.8.8
    GNU Make        3.79, 3.79.1
    GCC             >=4.7.0
    python          >=2.7
    GNU M4          1.4
    GNU Autoconf    2.60
    GNU Automake    1.9.6
    libtool         1.5.22
    zlib            >=1.2.3.4

# Quick Start
Assuming you have the required software, you can build the system with the following commands:

    git clone https://github.com/romejoe/miniat-LLVM-backend.git
    cd miniat-LLVM-backend/build
    git checkout release_36
    mkdir build
    cd build
    cmake ../
    make #-j<Number of Parallel Jobs>
    
For additional guidance, see [Getting Started with the LLVM System](http://llvm.org/releases/3.6.0/docs/GettingStarted.html)