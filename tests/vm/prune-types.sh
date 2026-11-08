#!/bin/bash
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
#
# An update of an ontology takes out the types that the new version does not have any more (the MIME type, the relation of the ontology to it, the template of an entity), and
# keeps a type that another ontology provides, too. Uses a small ontology of its own, "prunetest", made from the spatial ontology; removes it at the end.
# Needs, in the Haiku VM: the SEN server (it stores the relations), oni.sh with bin/mime next to it (a built sen-oni tree), run from that tree: tests/vm/prune-types.sh
HERE=$(cd "$(dirname "$0")" && pwd)
ONI="$HERE/../../scripts/oni.sh"
SPATIAL="$HERE/../../ontologies/spatial"
SETTINGS=$(finddir B_USER_SETTINGS_DIRECTORY)
T=${TMPDIR:-/tmp}/prunetest$$
failed=0
check() { desc=$1; shift; if "$@" >/dev/null 2>&1; then echo "ok   $desc"; else echo "FAIL $desc"; failed=1; fi; }
provides() { "$HERE/../../bin/mime" provides "$SETTINGS/sen/ontologies/prunetest"; }
has() { provides | grep -qxF "$1"; }
installed() { [ -e "$SETTINGS/mime_db/$1" ]; }

REL=relation/x-vnd.sen-labs.relation.prunetest
ENT=entity/x-vnd.sen-labs.entity.prunetest
rm -rf "$T"; mkdir -p "$T/prunetest/relation" "$T/prunetest/entity"
sed 's/SEN:onto:description") ".*"/SEN:onto:description") "A test ontology."/' "$SPATIAL/ontology.rdef" > "$T/prunetest/ontology.rdef"
for name in keep gone; do
	sed "s#relation/x-vnd.sen-labs.relation.spatial.equal#$REL.$name#g" "$SPATIAL/relation/x-vnd.sen-labs.relation.spatial.equal.rdef" > "$T/prunetest/relation/x-vnd.sen-labs.relation.prunetest.$name.rdef"
done
sed "s#entity/x-vnd.sen-labs.entity.place#$ENT.thing#g; s#\"Place\"#\"Prune Thing\"#" "$SPATIAL/entity/x-vnd.sen-labs.entity.place.rdef" > "$T/prunetest/entity/x-vnd.sen-labs.entity.prunetest.thing.rdef"

bash "$ONI" install "$T/prunetest" > "$T/first.log" 2>&1
check "the ontology provides the first type" has $REL.keep
check "... and the second" has $REL.gone
check "... and the entity" has $ENT.thing
check "the type is in the MIME database" installed $REL.gone
check "the entity has a template" test -e "$SETTINGS/Tracker/Tracker New Templates/Prunetest/Prune Thing"

# the next version: the second relation and the entity are gone
rm "$T/prunetest/relation/x-vnd.sen-labs.relation.prunetest.gone.rdef" "$T/prunetest/entity/x-vnd.sen-labs.entity.prunetest.thing.rdef"
bash "$ONI" install "$T/prunetest" > "$T/second.log" 2>&1
check "a type that is still there stays" has $REL.keep
check "a type that is gone is not provided any more" test ! "$(provides | grep -cxF $REL.gone)" -gt 0
check "... and is out of the MIME database" test ! -e "$SETTINGS/mime_db/$REL.gone"
check "the entity is gone" test ! -e "$SETTINGS/mime_db/$ENT.thing"
check "its template is gone" test ! -e "$SETTINGS/Tracker/Tracker New Templates/Prunetest/Prune Thing"
check "the type that stays is still in the MIME database" installed $REL.keep

bash "$ONI" --remove prunetest > "$T/remove.log" 2>&1
check "the ontology is removed" test ! -e "$SETTINGS/sen/ontologies/prunetest"
check "its last type is removed" test ! -e "$SETTINGS/mime_db/$REL.keep"
[ $failed = 0 ] || { echo "--- first"; cat "$T/first.log"; echo "--- second"; cat "$T/second.log"; }
rm -rf "$T"
[ $failed = 0 ] && echo "all ok" || echo FAILED
exit $failed
