#!/bin/bash
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
#
# Installs ontologies: their MIME types (the .rdef files of an ontology folder) and the placeholder file of each ontology in the
# SEN configuration, which is related to the MIME types that it provides (by the SEN server).
#
#   scripts/oni.sh ontologies/core ontologies/books     one or more ontology folders
#   scripts/oni.sh ontologies/*                         all of them

set -e

if [ $# -eq 0 ]; then
    echo "usage: $(basename "$0") <ontology folder>..., e.g. $(basename "$0") ontologies/core, or ontologies/* for all" >&2
    exit 1
fi

# the MIME type installer is built into bin/ of the repository
ONI_ROOT=$(cd "$(dirname "$0")/.." && pwd)

# SEN config
SEN_CONFIG_ONTO=$HOME/config/settings/sen/ontologies

oni_output=/tmp/.oni-out
mkdir -p $SEN_CONFIG_ONTO $oni_output

# the MIME types of the ontology that is installed, one per line, and its supertypes
types_file=
supertypes_file=

function create_mime_type()
{
    type_name=$(basename --suffix=.rdef $1)
    type_path=$(dirname $1)
    rsrc_path=$oni_output/$type_path/$type_name.rsrc

    mkdir -p "$(dirname "$rsrc_path")" && rc -o $rsrc_path $1 || return 1

    install_out=$("$ONI_ROOT/bin/mime" install $rsrc_path) || { echo "$install_out"; return 1; }
    echo "$install_out"
    # remember the type for the relations of the ontology: not a supertype (entity, relation,...), which is a folder in the
    # MIME database that holds the types of all ontologies, so a relation to it would show them all
    echo "$install_out" | sed -n 's/^successfully installed MIME type \(.*\/.*\)\.$/\1/p' >> $types_file
    # (older installers related the ontology to its supertypes, too: those relations are removed below)
    echo "$install_out" | sed -n 's/^successfully installed MIME type \([^\/]*\)\.$/\1/p' >> $supertypes_file
    rm $rsrc_path
}

function install_ontology()
{
    local folder=${1%/}
    local ontology_name=$(basename $folder)
    types_file=$oni_output/$ontology_name.types
    supertypes_file=$oni_output/$ontology_name.supertypes
    : > $types_file
    : > $supertypes_file

    echo creating ontology $ontology_name from resource definitions...

    # First, process .rdef files in the top-level directory to create any super types first
    # (ontology.rdef is the description of the ontology itself, not a MIME type)
    local file
    for file in "$folder"/*.rdef; do
        [ -e "$file" ] || continue
        [ "$(basename "$file")" = ontology.rdef ] && continue

        echo "  $file ..."
        create_mime_type "$file" || { echo "Aborting."; exit 1; }
    done

    # Then, process .rdef files in subdirectories only
    while IFS= read -r -d '' file; do
        echo "  $file ..."
        create_mime_type "$file" || { echo "Aborting."; exit 1; }
    done < <(find "$folder" -mindepth 2 -iname "*.rdef" -print0)

    echo registering ontology in SEN configuration...

    # the ontology is a placeholder file with the attributes of the ontology (also replaces what older installers made: a folder)
    # (an existing file stays, so that its relations stay valid)
    local sen_onto_path=$SEN_CONFIG_ONTO/$ontology_name
    [ -d "$sen_onto_path" ] && rm -rf "$sen_onto_path"
    [ -e "$sen_onto_path" ] || touch "$sen_onto_path"
    # the type and the metadata of the ontology (from its schema) are resources of ontology.rdef: they become its attributes
    rc -o $oni_output/$ontology_name.rsrc "$folder/ontology.rdef" && \
    resattr -o $sen_onto_path $oni_output/$ontology_name.rsrc && \
    rm $oni_output/$ontology_name.rsrc

    # the ontology provides its MIME types: navigate from the ontology to a type (and open it in FileTypes). This needs the SEN
    # server (it stores the relations): without it run this again later, relating twice does no harm.
    echo relating the ontology to its MIME types...
    local mime_type
    while read -r mime_type; do
        "$ONI_ROOT/bin/mime" relate "$sen_onto_path" "$mime_type" defines "defined by" || \
            echo "  could not relate $ontology_name to $mime_type, is the SEN server running?"
    done < $types_file
    # not to supertypes: they are folders with the types of all ontologies. Relations that an older installer made are removed.
    while read -r mime_type; do
        "$ONI_ROOT/bin/mime" unrelate "$sen_onto_path" "$mime_type" 2>/dev/null || true
    done < $supertypes_file

    echo "Done: $ontology_name."
}

installed=0
for ontology in "$@"; do
    if [ ! -d "$ontology" ] || [ ! -f "$ontology/ontology.rdef" ]; then
        echo "skipping $ontology: not an ontology folder (there is no ontology.rdef in it)." >&2
        continue
    fi
    install_ontology "$ontology"
    installed=$((installed + 1))
done

if [ $installed -eq 0 ]; then
    echo "nothing installed." >&2
    exit 1
fi
exit 0
