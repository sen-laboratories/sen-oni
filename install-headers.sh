#!/bin/bash
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
#
# Installs the generated ontology headers (attribute names and MIME types, SenOnto*.h) next to the SEN API headers of sento.
cd "$(dirname "$0")"
USER_INCLUDES=$(findpaths -e B_FIND_PATH_HEADERS_DIRECTORY | grep /config/non-packaged | head -1)
# (the folder is not there on a system that never had non-packaged headers: it is made)
[ -n "$USER_INCLUDES" ] || USER_INCLUDES="$(finddir B_USER_NONPACKAGED_DEVELOP_DIRECTORY)/headers"

mkdir -p "$USER_INCLUDES/sen" && cp include/*.h "$USER_INCLUDES/sen/" && \
echo "Installed the ontology headers to $USER_INCLUDES/sen." ||
echo "Error installing the ontology headers to $USER_INCLUDES/sen: $?"
