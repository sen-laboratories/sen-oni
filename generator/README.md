<!--
SPDX-License-Identifier: MIT
SPDX-FileCopyrightText: 2026 SEN Labs e.U.
-->
# ONI generator

The ontologies of SEN are described as [LinkML](https://linkml.io) schemas in `../schema/`. `oni_gen.py` generates from them

- the Haiku resource definitions in `../ontologies/<ontology>/` (the `.rdef` files that `scripts/oni.sh` compiles and installs, and a `manifest.properties`),
- a C++ header per ontology in `../include/` (`SenOnto<Name>.h`) with the names of the attributes and MIME types, so that no code needs string literals of them.

```sh
./generate.sh           # writes the files (needs Python 3 with linkml-runtime)
./generate.sh --check   # writes nothing and fails if a committed file is out of date (CI)
python3 -m unittest discover -s tests -v
```

Do not edit the generated files; change the schema and generate again. The vector icons are hex files in `../schema/icons/`.

## What the schema says

A **class** is a MIME type, a **slot** is a file attribute. Standard LinkML elements are used where they exist (`title`, `description`,
`is_a`, `range`, `slot_uri`); what is specific to Haiku and SEN is in `annotations`.

| Where | Annotation | Meaning |
|-------|-----------|---------|
| schema | `oni_author`, `oni_stable`, `oni_schema_url` | the manifest of the ontology (the `version` and `description` of the schema are used, too) |
| class | `mime` | the MIME type, required: a supertype (`entity`) or `supertype/name` |
| class | `supertype: true` | the class is a supertype (the resource gets the meta MIME type) |
| class | `app`, `extensions`, `icon`, `folder_icon` | preferred application (a signature; never set for file types of other programs), file extensions, the vector icon (`schema/icons/<name>.hex`), the icon of the relation folder |
| class (relation) | `bidir`, `dynamic`, `contained`, `label`, `inverse_label` | the flavor of the relation (`SEN:bidir`, `SEN:dynamic`, `SEN:self`) and the labels of both directions, written to `SEN:REL:CONFIG` |
| slot | `attribute` | the name of the file attribute, `<prefix>:<name>`, required (an established vocabulary first, see the developer guide) |
| slot | `bfs_type` | overrides the type of the `range` (`CSTR`, `LONG`, `SHRT`, `LLNG`, `BOOL`, `TIME`, `FLOT`, `DBLE`, `RREF`) |
| slot | `viewable`, `editable`, `searchable`, `width`, `alignment`, `display_as` | the attribute info that Tracker shows; `searchable` creates an index on every mounted volume |

The `title` of a slot is its public name (the column title), the `title` of a class its short description.

## What the generator checks

It refuses a schema in which a class has no MIME type or two classes have the same one, a slot has no valid attribute name or no
title, an attribute is defined twice with a different type or index flag (one name must have one type), an attribute that BFS cannot
index is marked searchable, an icon file is missing, or an entity has relation flavors. The arrays of the attribute info are written
completely and aligned, because the installer reads them by position.

The tests also compare the ontologies with `sento` (the constants of the API must name attributes that exist) and `sensei` (the
attribute mappings and types that plugins declare must exist), when those repositories are next to this one.
