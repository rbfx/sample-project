#!/usr/bin/env bash
# Pack a runnable copy of the project.
# Usage: package_dist.sh <bin_dir> <sdk_dir> <config> <data_dir> <dist_dir>
#   bin_dir:  directory with the executable, plugin libraries and managed files.
#   sdk_dir:  engine SDK root. Its bin/<config> runtime libraries and bin/CoreData are copied.
#   data_dir: project Data directory.
set -euo pipefail
bin_dir=$1 sdk_dir=$2 config=$3 data_dir=$4 dist=$5

mkdir -p "$dist/bin"
cp -a "$bin_dir"/. "$dist/bin/"
# Shared engine libraries next to the executable (no-op for static builds).
find "$sdk_dir/bin/$config" -maxdepth 1 -type f \
    \( -name '*.so' -o -name '*.so.*' -o -name '*.dll' -o -name '*.dylib' \) \
    -exec cp -a {} "$dist/bin/" \;
cp -a "$sdk_dir/bin/CoreData" "$dist/CoreData"
cp -a "$data_dir" "$dist/Data"
find "$dist" \( -name '*.pdb' -o -name '*.o' \) -exec unlink {} \;
cat > "$dist/bin/ResourceRoot.ini" <<INI
CoreData=../CoreData
Data=../Data
INI
