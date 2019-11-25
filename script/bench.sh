#!/bin/bash

INPUT_SIZE=(100 1000 10000 100000 1000000 10000000 100000000)

CHAR_COUNT=(10    # 0-9
            26    # a-z
            95    # all displaybles ascii chars
            255)  # a full byte extent

for char_count in ${CHAR_COUNT[@]} ; do
    for input_size in ${INPUT_SIZE[@]} ; do
        echo "Starting test with (input_size=$input_size, char_count=$char_count)"
        ../build/huffmanbench --benchmark_repetitions=10 --input_size=$input_size --char_count=$char_count
    done
done