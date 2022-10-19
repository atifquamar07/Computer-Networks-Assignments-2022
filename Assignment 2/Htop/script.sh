#!/bin/bash
start=$(date +%s%N)

for i in $(seq 1 10)
do
    ./pthreadclient &
done

end=$(date +%s%N)

echo ""
echo "Elapsed time: $(($end-$start)) ns"
echo ""

wait