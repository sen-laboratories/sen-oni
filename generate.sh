#!/bin/sh
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
#
# Generates the resource definitions (ontologies/), manifests and the C++ headers (include/) from the LinkML schemas in
# schema/. Needs Python 3 with linkml-runtime (pip install linkml-runtime). With --check nothing is written and the exit
# code says whether the committed files are up to date (used by CI).
set -e
cd "$(dirname "$0")"
PYTHON=${PYTHON:-python3}
[ -x ../.venv-linkml/bin/python ] && PYTHON=../.venv-linkml/bin/python
exec $PYTHON generator/oni_gen.py \
	schema/sen-core.yaml schema/sen-books.yaml schema/sen-movies.yaml schema/sen-music.yaml \
	schema/sen-sourcecode.yaml schema/sen-test.yaml \
	--rdef-dir ontologies --header-dir include "$@"
