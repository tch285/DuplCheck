#!/bin/bash

RUNS=(523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662)


for i in 1 2 3 4 5 6 7
do
    echo "Starting check of IR $i."
    root -l -b -q "CheckDuplMCComb.C+(${i}, 3)"
    echo "IR $i done."
done
echo "All done."