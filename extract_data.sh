#!/bin/bash -e

# RUNS=(523306 523786 526596 526938 527821 528448 528798 529115 529324 529462 529662)
PREFIX=536
# RUNS=(340 402 490 608 899 908)
RUNS=(908)

for SUFFIX in ${RUNS[@]}
do
    RUN=$PREFIX$SUFFIX
    echo "Attempting extraction for ${RUN}"
    mkdir -p ~/alice/mc_real/data/$RUN
    scp -i ~/.ssh/id_rsa its@flpits4:/data/local_reco/$RUN/o2trac_its.root ~/alice/mc_real/data/$RUN/ 2>/dev/null || echo 'File does not exist!'
done
echo "All done."

