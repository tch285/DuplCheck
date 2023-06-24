#!/bin/bash -e

RUNS=(523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662)

for RUN in ${RUNS[@]}
do
    echo "Starting check of $RUN."
    root -l -b -q "CheckDuplRun.C+(${RUN}, 3)"
    root -l -b -q "CheckDuplRun.C+(${RUN}, 2)"
    echo "$RUN done."
done


# PREFIX=536
# RUNS=(340 402 490 608 899 908)

# for SUFFIX in ${RUNS[@]}
# do
#     RUN=$PREFIX$SUFFIX
#     echo "Starting check of $RUN."
#     root -l -b -q "CheckDuplRun.C+(${RUN}, 3)"
#     root -l -b -q "CheckDuplRun.C+(${RUN}, 2)"
#     echo "$RUN done."
# done

echo "All done."