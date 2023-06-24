#!/bin/bash

RUNS=(523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662)


for RUN in 523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662
do
    echo "Starting check of $RUN."
    root -l -b -q "CheckDuplicates.C+(\"data/${RUN}/o2trac_its.root\")"
    echo "$RUN done."
done
echo "All done."