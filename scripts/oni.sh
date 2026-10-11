#!/bin/bash
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
#
# Installs ontologies: their MIME types (the .rdef or compiled .rsrc files of an ontology folder) and the placeholder file of each
# ontology in the SEN configuration, which is related to the MIME types that it provides (by the SEN server).
#
# In a package this is the command sen_oni. Ontology packages bring their ontologies as folders in data/sen/ontologies, which this
# script puts into the MIME database of the user:
#
#   sen_oni --update                    makes the installed ontologies match the ontology folders that packages provide: new and
#                                       changed ones are installed, those whose package is gone are removed. The SEN server runs this
#                                       when it starts and when packages change.
#   sen_oni [install] <folder>...       installs the given ontology folders (from a source tree: ontologies/core, ontologies/*)
#   sen_oni --remove <ontology>...      removes ontologies and the MIME types that no other installed ontology provides
#   sen_oni --list                      lists the installed ontologies

set -e

# the MIME type installer: built into bin/ of the repository, or the command of the package
ONI_ROOT=$(cd "$(dirname "$0")/.." && pwd)
if [ -x "$ONI_ROOT/bin/mime" ] && [ -d "$ONI_ROOT/schema" ]; then
    SEN_MIME="$ONI_ROOT/bin/mime"
else
    SEN_MIME=sen_mime
fi

# SEN config (the home folder is not always set when packages run scripts: ask the system)
SETTINGS=$(finddir B_USER_SETTINGS_DIRECTORY)
SEN_CONFIG_ONTO=$SETTINGS/sen/ontologies
# what is installed from where: <ontology>.types (one MIME type per line), .source (the folder), .sum (to see changes)
STATE=$SETTINGS/sen/oni

oni_output=$(mktemp -d "${TMPDIR:-/tmp}/oni.XXXXXX")
trap 'rm -rf "$oni_output"' EXIT
mkdir -p "$SEN_CONFIG_ONTO" "$STATE"

# folders with ontologies from packages (installed for the system or for the user), and the same non-packaged
function provided_folders()
{
    local dir
    for dir in B_SYSTEM_DATA_DIRECTORY B_USER_DATA_DIRECTORY B_SYSTEM_NONPACKAGED_DATA_DIRECTORY B_USER_NONPACKAGED_DATA_DIRECTORY; do
        echo "$(finddir $dir)/sen/ontologies"
    done
}

# the MIME types of the ontology that is installed, one per line, and its supertypes
types_file=
supertypes_file=

function create_mime_type()
{
    local file=$1
    local rsrc_path=$file
    if [ "${file##*.}" = rdef ]; then
        local type_name=$(basename --suffix=.rdef "$file")
        rsrc_path=$oni_output/$(dirname "$file")/$type_name.rsrc
        mkdir -p "$(dirname "$rsrc_path")" && rc -o "$rsrc_path" "$file" || return 1
    fi

    local install_out
    install_out=$("$SEN_MIME" install "$rsrc_path") || { echo "$install_out"; return 1; }
    echo "$install_out"
    # remember the type for the relations of the ontology: not a supertype (entity, relation,...), which is a folder in the
    # MIME database that holds the types of all ontologies, so a relation to it would show them all
    echo "$install_out" | sed -n 's/^successfully installed MIME type \(.*\/.*\)\.$/\1/p' >> $types_file
    # (older installers related the ontology to its supertypes, too: those relations are removed below)
    echo "$install_out" | sed -n 's/^successfully installed MIME type \([^\/]*\)\.$/\1/p' >> $supertypes_file
}

# the ontology and type files of a folder: .rdef from a source tree, .rsrc from a package (the first has precedence)
function type_files()
{
    local folder=$1 depth=$2 ext
    for ext in rdef rsrc; do
        if [ "$depth" = top ]; then
            find "$folder" -maxdepth 1 -type f -name "*.$ext" | sort
        else
            find "$folder" -mindepth 2 -type f -name "*.$ext" | sort
        fi
    done
}

function folder_sum()
{
    (cd "$1" && find . -type f | sort | while read -r f; do md5sum "$f"; done | md5sum | cut -d' ' -f1)
}

function install_ontology()
{
    local folder=${1%/}
    local ontology_name=$(basename $folder)
    types_file=$oni_output/$ontology_name.types
    supertypes_file=$oni_output/$ontology_name.supertypes
    : > $types_file
    : > $supertypes_file

    # (taken before the files are read: a package that is activated right now changes the folder, and the next update then installs it again)
    local sum
    sum=$(folder_sum "$folder")

    echo creating ontology $ontology_name from $folder...

    # The descriptor of the ontology itself is ontology.rdef/.rsrc, not a MIME type
    local descriptor=
    local file
    # First, process the files in the top-level directory to create any super types first
    while IFS= read -r file; do
        case "$(basename "$file")" in
            ontology.rdef|ontology.rsrc) descriptor=$file; continue;;
        esac
        echo "  $file ..."
        create_mime_type "$file" || { echo "Aborting."; exit 1; }
    done < <(type_files "$folder" top)

    # Then, process the files in subdirectories only
    while IFS= read -r file; do
        echo "  $file ..."
        create_mime_type "$file" || { echo "Aborting."; exit 1; }
    done < <(type_files "$folder" sub)

    echo registering ontology in SEN configuration...

    # the ontology is a placeholder file with the attributes of the ontology (also replaces what older installers made: a folder)
    # (an existing file stays, so that its relations stay valid)
    local sen_onto_path=$SEN_CONFIG_ONTO/$ontology_name
    [ -d "$sen_onto_path" ] && rm -rf "$sen_onto_path"
    [ -e "$sen_onto_path" ] || touch "$sen_onto_path"
    # the type and the metadata of the ontology (from its schema) are resources of ontology.rdef: they become its attributes
    local descriptor_rsrc=$descriptor
    if [ "${descriptor##*.}" = rdef ]; then
        descriptor_rsrc=$oni_output/$ontology_name.rsrc
        rc -o "$descriptor_rsrc" "$descriptor"
    fi
    resattr -o $sen_onto_path "$descriptor_rsrc"

    # the ontology provides its MIME types: navigate from the ontology to a type (and open it in FileTypes). This needs the SEN
    # server (it stores the relations): without it run this again later, relating twice does no harm.
    echo relating the ontology to its MIME types...
    local mime_type
    while read -r mime_type; do
        "$SEN_MIME" relate "$sen_onto_path" "$mime_type" defines "defined by" || \
            echo "  could not relate $ontology_name to $mime_type, is the SEN server running?"
    done < $types_file
    # not to supertypes: they are folders with the types of all ontologies. Relations that an older installer made are removed.
    while read -r mime_type; do
        "$SEN_MIME" unrelate "$sen_onto_path" "$mime_type" 2>/dev/null || true
    done < $supertypes_file

    # what was installed from where, for the next update and for removal
    cp $types_file "$STATE/$ontology_name.types"
    echo "$folder" > "$STATE/$ontology_name.source"
    echo "$sum" > "$STATE/$ontology_name.sum"

    echo "Done: $ontology_name."
}

# Removes an ontology, and the MIME types of it that no other installed ontology provides (they stay with the types of the others).
function remove_ontology()
{
    local ontology_name=$1
    if [ ! -f "$STATE/$ontology_name.types" ] && [ ! -e "$SEN_CONFIG_ONTO/$ontology_name" ]; then
        echo "ontology $ontology_name is not installed." >&2
        return 1
    fi
    echo removing ontology $ontology_name...

    local mime_type other kept
    if [ -f "$STATE/$ontology_name.types" ]; then
        while read -r mime_type; do
            kept=
            for other in "$STATE"/*.types; do
                [ "$other" = "$STATE/$ontology_name.types" ] && continue
                if [ -f "$other" ] && grep -qxF "$mime_type" "$other"; then kept=1; break; fi
            done
            if [ -z "$kept" ]; then
                "$SEN_MIME" uninstall "$mime_type" || true
            else
                echo "  $mime_type stays: another ontology provides it, too."
            fi
        done < "$STATE/$ontology_name.types"
    fi
    rm -f "$SEN_CONFIG_ONTO/$ontology_name" "$STATE/$ontology_name".{types,source,sum}
    echo "Removed: $ontology_name."
}

function update_ontologies()
{
    local present=" " dir folder name

    # new and changed ontologies from packages
    while read -r dir; do
        [ -d "$dir" ] || continue
        for folder in "$dir"/*; do
            [ -f "$folder/ontology.rsrc" ] || [ -f "$folder/ontology.rdef" ] || continue
            name=$(basename "$folder")
            present="$present$name "
            if [ -f "$STATE/$name.sum" ] && [ "$(cat "$STATE/$name.sum")" = "$(folder_sum "$folder")" ] \
                    && [ -e "$SEN_CONFIG_ONTO/$name" ]; then
                continue
            fi
            install_ontology "$folder"
        done
    done < <(provided_folders)

    # ontologies whose package is gone (those installed from a source tree stay)
    local state_source
    for state_source in "$STATE"/*.source; do
        [ -f "$state_source" ] || continue
        name=$(basename --suffix=.source "$state_source")
        case "$present" in *" $name "*) continue;; esac
        case "$(cat "$state_source")" in
            */sen/ontologies/*) remove_ontology "$name";;
        esac
    done
}

function list_ontologies()
{
    local state_source name
    for state_source in "$STATE"/*.source; do
        [ -f "$state_source" ] || continue
        name=$(basename --suffix=.source "$state_source")
        echo "$name  ($(wc -l < "$STATE/$name.types" | tr -d ' ') types, from $(cat "$state_source"))"
    done
}

function usage()
{
    echo "usage: $(basename "$0") [install] <ontology folder>...   e.g. ontologies/core, or ontologies/* for all" >&2
    echo "       $(basename "$0") --update | --list | --remove <ontology>..." >&2
}

if [ $# -eq 0 ]; then
    usage
    exit 1
fi

case "$1" in
    --update) update_ontologies; exit 0;;
    --list) list_ontologies; exit 0;;
    --remove)
        shift
        [ $# -gt 0 ] || { usage; exit 1; }
        for ontology in "$@"; do remove_ontology "$ontology" || true; done
        exit 0;;
    install) shift;;
    -h|--help) usage; exit 0;;
esac

installed=0
for ontology in "$@"; do
    if [ ! -d "$ontology" ] || { [ ! -f "$ontology/ontology.rdef" ] && [ ! -f "$ontology/ontology.rsrc" ]; }; then
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
