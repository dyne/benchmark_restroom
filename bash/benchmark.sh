#/bin/bash
for i in $(seq 0 4); do
	./benchmark_just1.sh $i &
	pids[${i}]=$!
done

for pid in ${pids[*]}; do
	wait $pid
done
