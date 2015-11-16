MapReduce
---------
The purpose of this program is to compute the number of occurrences of each word in a given file. The program implements a map/reduce paradigm in shared memory using pthreads.


Dependencies
------------

* cmake *[apt-get install cmake]*
* check (only to compile/launch unit tests) *[apt-get install check]*


Building MapReduce
------------------

    % mkdir -p build && cd build && cmake .. && make && cd ..


Building and running unit tests
-------------------------------
    % mkdir -p build && cd build && cmake -D BUILD_TESTS:BOOL=TRUE .. && make 

After building MapReduce, test it using:

    % ctest


Running MapReduce
-----------------

    % bin/mapred [OPTION...] <file> <Nthreads> 


* **file:** path to a file containing words
* **Nthreads:** number of threads to use


Options are :

    -p, --profiling            Activate profiling
    -q, --quiet                Do not output results
    -t, --type=MR_TYPE         Mapreduce type (0=PARALLEL, 1=SEQUENTIAL)
    -w, --wtype=WS_TYPE        WordStreamer type (0=SCATTER, 1=INTERLEAVE)
    -?, --help                 Give this help list
        --usage                Give a short usage message
    -V, --version              Print program version



Examples with provided samples
-----------------------------

    % bin/mapred data/lorem_small.txt 2

will display:

    ...
    turpis=2
    ullamcorper=1
    ultrices=2
    ultricies=1
    urna=5
    ut=4
    varius=1
    vehicula=1
    vel=1
    velit=1
    venenatis=2
    vitae=1
    vivamus=4
    vulputate=1


Running Mapreduce in quiet and profiling modes:

    % bin/mapred data/lorem_medium.txt 4 -q -p

will display something similar to:

    |-[WordStreamer 0] get: 8.196 ms
    |-[Dictionary] hash function: 0.483 ms
    |-[Dictionary] put: 2.299 ms
    |-[WordStreamer 1] get: 2.200 ms
    |-[Dictionary] hash function: 0.472 ms
    |-[Dictionary] put: 2.451 ms
    |-[WordStreamer 2] get: 2.276 ms
    |-[Dictionary] hash function: 0.476 ms
    |-[Dictionary] put: 2.395 ms
    |-[WordStreamer 3] get: 2.278 ms
    |-[Dictionary] hash function: 0.476 ms
    |-[Dictionary] put: 2.215 ms
    |-[MapReduce] map: 8.036 ms
    |-[MapReduce] reduce: 0.912 ms
    |---> TOTAL: 12.545 ms
    |---> [File Size: 0.087 MB]  ->  6.904 MB/s
    |---> [Words: 13436]  ->  1.071 MWords/s


Benchmark
---------

Tests are performed with an *Intel i5-3427U CPU* @ 1.80GHz (4 SMT, 2 cores) and a SSD *Kingston SSDNow mS200* as a support storage for a 100MB source file. Best performances are obtained wih the scatter mode.

    % ./mapred lorem_verylarge.txt 4 -w 0 -q -p
    |-[WordStreamer 0] get: 844.262 ms
    |-[Dictionary] hash function: 151.677 ms
    |-[Dictionary] put: 2508.536 ms
    |-[WordStreamer 1] get: 808.657 ms
    |-[Dictionary] hash function: 152.090 ms
    |-[Dictionary] put: 2548.914 ms
    |-[WordStreamer 2] get: 805.632 ms
    |-[Dictionary] hash function: 151.849 ms
    |-[Dictionary] put: 2545.457 ms
    |-[WordStreamer 3] get: 810.067 ms
    |-[Dictionary] hash function: 151.784 ms
    |-[Dictionary] put: 2558.705 ms
    |-[MapReduce] map: 3567.679 ms
    |-[MapReduce] reduce: 21.714 ms
    |---> TOTAL: 3592.929 ms
    |---> [File Size: 100.502 MB]  ->  27.972 MB/s
    |---> [Words: 15533581]  ->  4.323 MWords/s


![Performance test](/data/scatter.png)
*(averaged data across 10 runs)*
