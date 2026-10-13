<p align="center">
  <img src="assets/images/sen-oni-logo.jpg" width=360 />
</p>

<h1 align="center">SEN Ontology Native Interface</h1>

<p align="center">
This repository defines entities, their properties and relationships as resources to be consumed by SEN for installation in Haiku and its MIME type based file system featuring custom indexed attributes.
</p>

# About

The aim is to provide a foundation of essential types to work with, so not every user has to start from scratch.
The structure follows a common schema so it can be easily consumed and understood.

Additionally, helpers for relation extraction and navigation, as well as entity and structure extraction may be provided.

Where possible, existing ontologies from schema.org, wikidata and similar should be used as a starting point.
However, they are often too complex and do not always define relations, or use properties instead.

Lastly, also existing definitions from ONI should be reused where possible, and only referenced to indicate dependencies.

Still under development but getting there.

# How it works

The ontologies are written as [LinkML](https://linkml.io) schemas in `schema/` (one per ontology, `sen-core.yaml` is the meta ontology
that the others import). The Haiku resource definitions in `ontologies/` and the C++ headers in `include/` are **generated** from them, see
[generator/README.md](generator/README.md):

```sh
./generate.sh                      # schema -> ontologies/*.rdef, ontology.rdef, include/SenOnto*.h
./install-headers.sh               # on Haiku: installs the headers next to the SEN API (sento)
make                               # on Haiku: builds bin/mime, the MIME type installer
./scripts/oni.sh ontologies/core   # on Haiku: compiles and installs an ontology (MIME types, attribute info, indices)
```

Indices for attributes marked searchable are created on every mounted volume; an index is never removed.


# Packages

Each ontology is a native Haiku package `sen_ontology_<name>` (`core`, `books`, `movies`, `music`, `sourcecode`), the installer is the
package `sen_oni`. They are in the [SEN Labs Kiri repository](https://github.com/sen-laboratories/kiri):

```sh
pkgman add-repo https://kiri.sen-labs.org/x86_64      # once
pkgman install sen_ontology_books                     # brings sen_oni and sen_ontology_core with it
```

An ontology package contains the MIME types of the ontology as compiled resources in `data/sen/ontologies/<name>/`. The MIME
database of the user is changed by the installer `sen_oni`, which the SEN server runs when it starts and when a package is added or
removed (the output is in `~/config/settings/sen/ontologies-update.log`). You can run it yourself, too:

```sh
sen_oni --update            # installs new and changed ontologies, removes those whose package is gone
sen_oni --list              # what is installed
sen_oni --remove books      # removes an ontology and the MIME types that no other ontology provides
sen_oni ontologies/core     # installs an ontology folder from a source tree (rdef files), for development
```

A MIME type that several ontologies provide stays installed until the last of them is removed.

The **entities** of an ontology (its types below `entity/`) become templates of the *New* menu of the file manager: a submenu with the name of the ontology in
`~/config/settings/Tracker/Tracker New Templates`, one empty file of the type for each entity, so that people can make objects of the new types at once. The templates are
marked (`SEN:onto:template`); removing the ontology removes those, and templates that people made or changed are left alone. Ontologies that were installed before get them once.

To build the packages on Haiku (`make` first, the packages are written to the repository root):

```sh
./package.sh                # sen_oni and one package for each ontology; REVISION=2 ./package.sh for a new revision
```
