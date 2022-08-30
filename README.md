# Goal
Benchmark restroom using http request on the same machine (not to take under account the network).

The test should be as light as possible, the CPU usage of restroom should be always as next to 100% as possible.

# Environment
5 instances of restroom on 5 different ports (25000,...,25004).

# Test using BASH
```
cd bash
time ./benchmark.sh
```

# Test using libcurl (multi perform)
```
cd c
make benchmark
time ./benchmark
```
