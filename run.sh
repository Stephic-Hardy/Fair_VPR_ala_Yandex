#!/bin/bash

files=(
  1.json
  2.json
  3.json
  4.json
  5.json
  6.json
  7.json
  8.json
  9.json
  10.json
  11.json
  12.json
  13.json
  14.json
  15.json
  16.json
  17.json
  18.json
  19.json
  20.json
  21.json
  22.json
  23.json
  24.json
  25.json
)

app=$1
path=$2
output=$3

if [[ -z "$app" || -z "$path" || -z "$output" ]]; then
  echo "Использование: $0 <бинарь> <путь до папки> <выходной файл>"
  exit 1
fi

[[ "${path}" != */ ]] && path="${path}/"

for file in "${files[@]}"; do
  ./${app} -p "${path}${file}" -s solution.json -t 0 >> ${output}
done

echo "✅ Готово!"
