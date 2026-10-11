#!/bin/sh
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
#
# Builds the HPKGs of SEN Ontology Native Interface: sen_oni (the installer, bin/mime as sen_mime and scripts/oni.sh as sen_oni) and
# one package sen_ontology_<name> for each ontology (its MIME types as compiled resources in data/sen/ontologies/<name>).
# Run make first (on Haiku). The packages are written to the repository root; set REVISION to override the revision (default 1).
set -e
cd "$(dirname "$0")"

[ -f bin/mime ] || { echo "bin/mime not found, run make first" >&2; exit 1; }
ARCH=$(getarch)
REVISION=${REVISION:-1}
ONI_VERSION=1.0.0~dev
STAGE=package-build
OUT=$(pwd)

rm -rf $STAGE

# --- the installer
mkdir -p $STAGE/oni/bin $STAGE/oni/documentation/packages/sen_oni
cp -a bin/mime $STAGE/oni/bin/sen_mime
cp -a scripts/oni.sh $STAGE/oni/bin/sen_oni
chmod 755 $STAGE/oni/bin/sen_oni
cp -a README.md LICENSE $STAGE/oni/documentation/packages/sen_oni/
sed -e "s|@VERSION@|$ONI_VERSION-$REVISION|g" -e "s|@ARCH@|$ARCH|g" package/PackageInfo.oni.in > $STAGE/oni/.PackageInfo
rm -f sen_oni-*.hpkg
package create -C $STAGE/oni "sen_oni-$ONI_VERSION-$REVISION-$ARCH.hpkg"
echo "created sen_oni-$ONI_VERSION-$REVISION-$ARCH.hpkg"

# --- the ontologies (the test ontology is not packaged)
resource_value()
{
	# $1: rdef file, $2: name of the string resource
	sed -n "s/^resource([0-9]*, \"$2\") \"\(.*\)\";\$/\1/p" "$1" | head -n1
}

rm -f sen_ontology_*.hpkg
for folder in ontologies/*/; do
	name=$(basename "$folder")
	[ "$name" = test ] && continue
	rdef="$folder/ontology.rdef"
	[ -f "$rdef" ] || continue

	version=$(resource_value "$rdef" SEN:onto:version)
	description=$(resource_value "$rdef" SEN:onto:description | sed 's/"/\\"/g')
	summary="SEN ontology: $name"
	requires=
	[ "$name" = core ] || requires="	sen_ontology_core"

	target=$STAGE/$name/data/sen/ontologies/$name
	mkdir -p "$target" $STAGE/$name/documentation/packages/sen_ontology_$name
	# the types as compiled resources, in the same folders (entity/, relation/,...) as in the source
	find "$folder" -name '*.rdef' | while read -r file; do
		rel=${file#$folder}
		mkdir -p "$target/$(dirname "$rel")"
		rc -o "$target/${rel%.rdef}.rsrc" "$file"
	done
	[ -f "$folder/README.md" ] && cp "$folder/README.md" $STAGE/$name/documentation/packages/sen_ontology_$name/
	cp LICENSE $STAGE/$name/documentation/packages/sen_ontology_$name/

	sed -e "s|@NAME@|$name|g" -e "s|@VERSION@|$version-$REVISION|g" -e "s|@SUMMARY@|$summary|g" \
		-e "s|@DESCRIPTION@|$description|g" -e "s|@REQUIRES@|$requires|" package/PackageInfo.ontology.in \
		> $STAGE/$name/.PackageInfo
	package create -C $STAGE/$name "sen_ontology_$name-$version-$REVISION-any.hpkg"
	echo "created sen_ontology_$name-$version-$REVISION-any.hpkg"
done
rm -rf $STAGE
