#!/bin/bash

runs=($(sed -rn 's,.*data([0-9]{2})/.*_([0-9]+)ipb.*,\1 \2,p' \
  $(sed -r 's,(.*/).*,\1data_*_all.sh,' <<< "$0") \
| awk '
BEGIN{
  ds[15] = "a"
  ds[16] = "a"
  ds[17] = "d"
  ds[18] = "e"
} { a[ds[$1]]+=$2; t+=$2 } END{ for (i in a) print "mc16" i ":" a[i]/t }'))

# runs=(mc16a:0.451721 mc16d:0.548279 mc16e:1)

dir=out/h024
out=$dir/mc16_merged

mkdir -p $out

for name in $(
  find $(sed -r "s,([^ ]*):[^ ]+,$dir/\\1,g" <<< ${runs[@]}) \
  | sed -rn 's,.*/(.*)\.root$,\1,p' \
  | sort -u
)
do
  # echo $name
  echo \
  ./bin/merge_mc ${runs[@]/#/$dir\/} -o $out/$name
done

