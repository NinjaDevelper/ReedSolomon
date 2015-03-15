[![Build Status](https://travis-ci.org/StorjPlatform/ReedSolomon.svg?branch=master)](https://travis-ci.org/StorjPlatform/ReedSolomon)
[![Coverage Status](https://coveralls.io/repos/StorjPlatform/ReedSolomon/badge.svg?branch=master)](https://coveralls.io/r/StorjPlatform/ReedSolomon?branch=master)

# Reed-Solomon Coding with Interleaving
This is a library for Reed-Solomon Coding with Interleaving , available as a C program and python module.

About Reed-Solomon coding, please refer [Reed–Solomon codes for coders](http://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders).

For interleaving, data will be considered as K x (interleaving length) matrix (interleaving length= (file size % K)+1).
Then Reed Solomon coding is adapted to each column, and add parity bits to the end of column.

For example, when RS(K=200,N=208) is used to 1,000,000 bytes data, which means interleaving length=5,000 ,
you can recover data even if you cannot download sequential 5,000*8 bytes.

![](http://i.imgur.com/m1EmTY5.png?1)

## Requirements
This library only requires `g++`. However, the python modules require that either Python 2.x or 3.x is installed.

## Installation

To compile the C program:

    $ make 

To build the python module:

    $ make python
    
To run the associated tests:

    $ make test
    $ ./test

## Usage
Once installed, you can use `ReedSolomon` to encode a file to "encoded.dat".

    $ ./reedSolomon e <filename_of_shard>

When you want to recover erasure,

    $ ./reedSolomon m 
    $ ./reedSolomon d <filename_of_shard>

The command with "m" option embed 500000+1024*300 errors to "encoded.dat",
and "d" option recovers erasure.

If you are using Python, you can use `encodeFile()` to test for coding, or you can use `decodeFile()` to recover erasure:

    import reedSolomon
    infile_path = 'data/origin.dat'
    outfile_path = 'data/encoded.dat'
    reedSolomon.encodeFile(infile_path,outfile_path,200,208)

    import reedSolomon
    infile_path = 'data/error.dat'
    outfile_path = 'data/recovered.dat'
    size=12345  #file size of origin.dat
    errLoc=[12342,1323,1662,11344] #list of error location in file
    reedSolomon.dencodeFile(infile_path,outfile_path,200,208,size,errLoc)

Refer [here](https://rawgit.com/StorjPlatform/ReedSolomon/master/doc/html/class_reed_solomon.html) for using in library.

## Performance

Using the following test environment...

    * Compiler: 4.9.2 20150304 (prerelease) (GCC)
    * Kernel: Linux 3.18.6-1-ARCH #1 SMP PREEMPT Sat Feb 7 08:44:05 CET 2015 x86_64 GNU/Linux
    * CPU:  Celeron(R) CPU G1840 @ 2.80GHz 
    * Memory: 4 GB

...a 32 MB data shard can be coded RS(200,208) in approximately 1.1 seconds:

```
$time ./reedSolomon e data/rand32m.dat

real    0m1.050s
user    0m1.023s
sys     0m0.027s
```

...a 32 MB data shard with 807,200 bytes errors can be recovered in approximately 2.1 seconds:

```
$time ./reedSolomon d data/rand32m.dat

real    0m2.103s
user    0m2.047s
sys     0m0.057s
```


# Contribution
Improvements to the codebase and pull requests are encouraged.

#Reference
1. Reed–Solomon codes for coders.  [ONLINE] Available at: http://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders. [Accessed 18 March 2015].
