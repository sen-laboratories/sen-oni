#!/bin/bash
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.

set -e

if [ ! -d "$1" ] || [ ! -f "$1/ontology.rdef" ]; then
    echo "usage: $(basename "$0") <ontology folder, e.g. ontologies/core>" >&2
    exit 1
fi

# the MIME type installer is built into bin/ of the repository
ONI_ROOT=$(cd "$(dirname "$0")/.." && pwd)

# SEN config
SEN_CONFIG_ONTO=$HOME/config/settings/sen/ontologies

# Haiku MIME config
MIME_DB_PATH=$HOME/config/settings/mime_db
META_MIME_TYPE=application/x-vnd.Be-meta-mime

mkdir -p $SEN_CONFIG_ONTO

oni_output=/tmp/.oni-out
ontology_name=$(basename $1)


function create_mime_type()
{
    type_name=$(basename --suffix=.rdef $1)
    type_path=$(dirname $1)
    rsrc_path=$oni_output/$type_path/$type_name.rsrc

    mkdir -p "$(dirname "$rsrc_path")" && \
    rc -o $rsrc_path $1 && \
    "$ONI_ROOT/bin/mime" install $rsrc_path &&
    rm $rsrc_path || false
}

echo creating ontology $ontology_name from resource definitions...

# First, process .rdef files in the top-level directory to create any super types first
# (ontology.rdef is the description of the ontology itself, not a MIME type)
for file in "$1"/*.rdef; do
    # Check if any .rdef files exist in the top-level directory
    [ -e "$file" ] || continue
    [ "$(basename "$file")" = ontology.rdef ] && continue
    
    echo "  $file ..."
    create_mime_type "$file" || (echo "Aborting."; exit 1)
done

# Then, use find to process .rdef files in subdirectories only
find "$1" -mindepth 2 -iname "*.rdef" -print0 | while IFS= read -r -d '' file
do
    echo "  $file ..."
    create_mime_type "$file" || (echo "Aborting."; exit 1)
done

echo registering ontology in SEN configuration...

# the ontology is a placeholder file with the attributes of the ontology (also replaces what older installers made: a folder)
sen_onto_path=$SEN_CONFIG_ONTO/$ontology_name
rm -rf "$sen_onto_path"
touch "$sen_onto_path"
# the type and the metadata of the ontology (from its schema) are resources of ontology.rdef: they become its attributes
mkdir -p $oni_output
rc -o $oni_output/$ontology_name.rsrc "$1/ontology.rdef" && \
resattr -o $sen_onto_path $oni_output/$ontology_name.rsrc && \
rm $oni_output/$ontology_name.rsrc

echo Done.
exit 0
