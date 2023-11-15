#! /bin/bash

for benchmark in 1 2 3
do 
    for prefetches in next_n_lines stride distance
    do 
        for num in 1 2 3 4 5 6 7 8 9 10
        do
            echo "$prefetches benchmark: $benchmark aggression: $num"
            ($PIN -t $PF_EXAMPLE/obj-intel64/prefetcher_example.so -pref_type $prefetches -o output-stats.out -- $BENCH_PATH/microBench$benchmark.exe -aggr $num)
            cat output-stats.out
            echo ""
        done
    done
done
