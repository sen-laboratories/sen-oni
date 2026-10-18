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
# What is installed from where is in the placeholder of the ontology (with the attributes of the ontology): SEN:onto:source is the folder,
# SEN:onto:sum tells when it changed (and is only there when the installation was complete), the MIME types that the ontology provides
# are the targets of its relations (sen_mime provides).
ATTR_SOURCE=SEN:onto:source
ATTR_SUM=SEN:onto:sum

oni_output=$(mktemp -d "${TMPDIR:-/tmp}/oni.XXXXXX")
trap 'rm -rf "$oni_output"' EXIT
mkdir -p "$SEN_CONFIG_ONTO"

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

function get_attr()
{
    # $1: attribute, $2: placeholder
    catattr -d "$1" "$2" 2>/dev/null || true
}

function set_attr()
{
    addattr -t string "$1" "$3" "$2"
}

# The entities of an ontology are templates of the New menu of the file manager (a submenu with the name of the ontology), so that people can make
# objects of the new types at once. A template is an empty file of the type; ours have SEN:onto:template, which is how they are found when the
# ontology is removed (templates that people made or changed themselves are left alone).
ATTR_TEMPLATE=SEN:onto:template
TEMPLATES="$SETTINGS/Tracker/Tracker New Templates"

function template_folder()
{
    local name=$1
    echo "$TEMPLATES/$(echo "${name:0:1}" | tr '[:lower:]' '[:upper:]')${name:1}"
}

function add_templates()
{
    local ontology_name=$1 types=$2
    local folder mime_type description
    folder=$(template_folder "$ontology_name")
    while read -r mime_type; do
        case "$mime_type" in entity/*) ;; *) continue;; esac
        # (the name that people read: the short description of the type)
        description=$(catattr -d META:S:DESC "$SETTINGS/mime_db/$mime_type" 2>/dev/null || true)
        [ -n "$description" ] || description=${mime_type#*/x-vnd.sen-labs.entity.}
        if [ ! -d "$folder" ]; then
            mkdir -p "$folder"
            addattr -t bool _trk/_template_submenu 1 "$folder" 2>/dev/null || true
            addattr -t string $ATTR_TEMPLATE "$ontology_name" "$folder" 2>/dev/null || true
        fi
        [ -e "$folder/$description" ] && continue
        touch "$folder/$description"
        addattr -t mime BEOS:TYPE "$mime_type" "$folder/$description"
        addattr -t string $ATTR_TEMPLATE "$ontology_name" "$folder/$description"
        echo "  template: $description"
    done < "$types"
}

function remove_templates()
{
    local ontology_name=$1
    local folder file
    folder=$(template_folder "$ontology_name")
    [ -d "$folder" ] || return 0
    for file in "$folder"/*; do
        if [ -f "$file" ] && [ "$(get_attr $ATTR_TEMPLATE "$file")" = "$ontology_name" ]; then rm -f "$file"; fi
    done
    # (the folder goes when it is empty and is ours)
    if [ "$(get_attr $ATTR_TEMPLATE "$folder")" = "$ontology_name" ]; then
        rmdir "$folder" 2>/dev/null || true
    fi
}

# Ontologies that were installed before there were templates get them once (a template that people delete is not made again at every update).
ATTR_TEMPLATES_DONE=SEN:onto:templates

function ensure_templates()
{
    local ontology_name=$1 placeholder=$SEN_CONFIG_ONTO/$1
    [ -n "$(get_attr $ATTR_TEMPLATES_DONE "$placeholder")" ] && return 0
    local list=$oni_output/$ontology_name.provided
    "$SEN_MIME" provides "$placeholder" > "$list" 2>/dev/null || return 0
    add_templates "$ontology_name" "$list" && set_attr $ATTR_TEMPLATES_DONE "$placeholder" 1
}

# Whether the MIME types of an ontology folder are all in the MIME database (a type that was removed, e.g. by an interrupted removal of the ontology,
# is installed again by the next update, though the folder did not change).
function types_present()
{
    local folder=${1%/} file rel
    while read -r file; do
        rel=${file#$folder/}
        [ "$rel" = ontology.rsrc ] && continue
        [ -e "$SETTINGS/mime_db/${rel%.rsrc}" ] || return 1
    done < <(find "$folder" -type f -name '*.rsrc')
    return 0
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
    # (the lists are read first: nothing in the loops may take from them, and a list that is empty is noticed below)
    local top_files sub_files
    mapfile -t top_files < <(type_files "$folder" top)
    mapfile -t sub_files < <(type_files "$folder" sub)
    local expected=0 processed=0

    # First, process the files in the top-level directory to create any super types first
    for file in "${top_files[@]}"; do
        case "$(basename "$file")" in
            ontology.rdef|ontology.rsrc) descriptor=$file; continue;;
        esac
        expected=$((expected + 1))
        echo "  $file ..."
        create_mime_type "$file" </dev/null || { echo "Aborting $ontology_name."; return 1; }
        processed=$((processed + 1))
    done

    # Then, process the files in subdirectories only
    for file in "${sub_files[@]}"; do
        [ -n "$file" ] || continue
        expected=$((expected + 1))
        echo "  $file ..."
        create_mime_type "$file" </dev/null || { echo "Aborting $ontology_name."; return 1; }
        processed=$((processed + 1))
    done

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
        rc -o "$descriptor_rsrc" "$descriptor" || return 1
    fi
    resattr -o $sen_onto_path "$descriptor_rsrc" || return 1
    # (complete again when everything below has worked)
    rmattr $ATTR_SUM "$sen_onto_path" 2>/dev/null || true
    set_attr $ATTR_SOURCE "$sen_onto_path" "$folder"

    # the ontology provides its MIME types: navigate from the ontology to a type (and open it in FileTypes). This needs the SEN
    # server (it stores the relations): without it run this again later, relating twice does no harm.
    echo adding the entities to the templates...
    if add_templates "$ontology_name" "$types_file"; then
        set_attr $ATTR_TEMPLATES_DONE "$sen_onto_path" 1
    else
        echo "  could not add all templates."
    fi

    echo relating the ontology to its MIME types...
    local mime_type related=1
    # (a type that was not processed, whatever the reason: the ontology is not complete, the next update does it again)
    if [ $processed -ne $expected ] || [ $processed -eq 0 ]; then
        echo "  only $processed of $expected types were processed."
        related=0
    fi
    while read -r mime_type; do
        "$SEN_MIME" relate "$sen_onto_path" "$mime_type" defines "defined by" || {
            echo "  could not relate $ontology_name to $mime_type, is the SEN server running?"
            related=0
        }
    done < $types_file
    # not to supertypes: they are folders with the types of all ontologies. Relations that an older installer made are removed.
    while read -r mime_type; do
        "$SEN_MIME" unrelate "$sen_onto_path" "$mime_type" 2>/dev/null || true
    done < $supertypes_file

    # complete: without the sum (no server, so no relations) the next update installs the ontology again
    if [ $related = 1 ]; then
        set_attr $ATTR_SUM "$sen_onto_path" "$sum"
    else
        echo "  $ontology_name is not complete, it is installed again by the next update."
    fi

    echo "Done: $ontology_name."
}

# Removes an ontology, and the MIME types of it that no other installed ontology provides (they stay with the types of the others).
function remove_ontology()
{
    local ontology_name=$1
    local placeholder=$SEN_CONFIG_ONTO/$ontology_name
    if [ ! -e "$placeholder" ]; then
        echo "ontology $ontology_name is not installed." >&2
        return 1
    fi
    echo removing ontology $ontology_name...
    # (an interrupted removal - the server restarts while packages are updated - must not look complete: without the sum the next update installs the
    # ontology again if its package is there, and finishes the removal if it is not)
    rmattr $ATTR_SUM "$placeholder" 2>/dev/null || true

    local mime_type other kept types
    types=$("$SEN_MIME" provides "$placeholder" 2>/dev/null || true)
    while read -r mime_type; do
        [ -n "$mime_type" ] || continue
        # the relation of the ontology to the type goes in any case
        "$SEN_MIME" unrelate "$placeholder" "$mime_type" 2>/dev/null || true
        kept=
        for other in "$SEN_CONFIG_ONTO"/*; do
            [ -f "$other" ] && [ "$other" != "$placeholder" ] || continue
            if "$SEN_MIME" provides "$other" 2>/dev/null | grep -qxF "$mime_type"; then kept=1; break; fi
        done
        if [ -z "$kept" ]; then
            "$SEN_MIME" uninstall "$mime_type" || true
        else
            echo "  $mime_type stays: another ontology provides it, too."
        fi
    done <<< "$types"
    remove_templates "$ontology_name"
    rm -f "$placeholder"
    echo "Removed: $ontology_name."
}

# The file manager saves the columns of a folder when it has shown it and does not use the defaults after that: the folder of the ontologies would keep
# the columns of an older version (without the attributes of the type of an ontology). The columns are removed once for each version of the layout.
ONTOLOGY_VIEW_VERSION=2

function reset_folder_view()
{
    local current
    current=$(catattr -d SEN:view:version "$SEN_CONFIG_ONTO" 2>/dev/null || true)
    [ "$current" = "$ONTOLOGY_VIEW_VERSION" ] && return 0
    local attribute
    for attribute in _trk/columns_le _trk/columns; do
        rmattr "$attribute" "$SEN_CONFIG_ONTO" 2>/dev/null || true
    done
    addattr -t string SEN:view:version "$ONTOLOGY_VIEW_VERSION" "$SEN_CONFIG_ONTO" 2>/dev/null || true
}

function update_ontologies()
{
    reset_folder_view

    local present=" " dir folder name

    # new and changed ontologies from packages
    while read -r dir; do
        [ -d "$dir" ] || continue
        for folder in "$dir"/*; do
            [ -f "$folder/ontology.rsrc" ] || [ -f "$folder/ontology.rdef" ] || continue
            name=$(basename "$folder")
            present="$present$name "
            if [ -e "$SEN_CONFIG_ONTO/$name" ] && [ "$(get_attr $ATTR_SUM "$SEN_CONFIG_ONTO/$name")" = "$(folder_sum "$folder")" ] \
                    && { [ -f "$folder/ontology.rdef" ] || types_present "$folder"; }; then
                ensure_templates "$name" || true
                continue
            fi
            # (one that cannot be installed does not keep the others from it: it has no sum, so the next update tries it again)
            install_ontology "$folder" || echo "$name could not be installed, it is tried again by the next update."
        done
    done < <(provided_folders)

    # ontologies whose package is gone (those installed from a source tree stay)
    local placeholder source gone=()
    for placeholder in "$SEN_CONFIG_ONTO"/*; do
        [ -f "$placeholder" ] || continue
        name=$(basename "$placeholder")
        case "$present" in *" $name "*) continue;; esac
        source=$(get_attr $ATTR_SOURCE "$placeholder")
        case "$source" in
            */sen/ontologies/*) gone+=("$name");;
        esac
    done
    # (packages are changed one after the other: what is gone may be back a moment later, so it is looked at again before the types are removed)
    if [ ${#gone[@]} -gt 0 ]; then
        sleep 3
        for name in "${gone[@]}"; do
            ontology_provided "$name" || remove_ontology "$name"
        done
    fi
}

# Whether a package provides the ontology (its folder is in one of the data folders).
function ontology_provided()
{
    local dir
    while read -r dir; do
        { [ -f "$dir/$1/ontology.rsrc" ] || [ -f "$dir/$1/ontology.rdef" ]; } && return 0
    done < <(provided_folders)
    return 1
}

function list_ontologies()
{
    local placeholder name
    for placeholder in "$SEN_CONFIG_ONTO"/*; do
        [ -f "$placeholder" ] || continue
        name=$(basename "$placeholder")
        echo "$name  ($("$SEN_MIME" provides "$placeholder" 2>/dev/null | wc -l | tr -d ' ') types, from $(get_attr $ATTR_SOURCE "$placeholder"))"
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
    install_ontology "$ontology" || exit 1
    installed=$((installed + 1))
done

if [ $installed -eq 0 ]; then
    echo "nothing installed." >&2
    exit 1
fi
exit 0
