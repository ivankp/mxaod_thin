#!/bin/bash

dir=${0%/*}

runs=($(
sed -rn 's,.*data([0-9]{2})/.*_([0-9]+)ipb.*,\1 \2,p' $dir/data_*_all.sh \
| awk '
BEGIN{
  ds[15] = "a"
  ds[16] = "a"
  ds[17] = "d"
  ds[18] = "e"
} { a[ds[$1]]+=$2; t+=$2 } END{ for (i in a) print "mc16" i ":" a[i]/t }'
))

printf '%s\n' "${runs[@]}"

dir=$(readlink -f "$dir/../out/h024")
out=$dir/mc16_merged

mkdir -p $out

for name in $(
  find $(sed -r "s,([^ ]*):[^ ]+,$dir/\1,g" <<< ${runs[@]}) \
  | sed -rn 's,.*/(.*\.root)$,\1,p' \
  | sort -u
)
do
  echo ${name%.root}
  # echo \
  ./bin/merge_mc \
  $(sed -r "s,([^ ]*)(:[^ ]*),$dir/\1/$name\2,g" <<< ${runs[@]}) \
  -o $out/$name
done

