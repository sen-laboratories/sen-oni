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
./generate.sh                      # schema -> ontologies/*.rdef, manifests, include/SenOnto*.h
./install-headers.sh               # on Haiku: installs the headers next to the SEN API (sento)
make                               # on Haiku: builds bin/mime, the MIME type installer
./scripts/oni.sh ontologies/core   # on Haiku: compiles and installs an ontology (MIME types, attribute info, indices)
```

Indices for attributes marked searchable are created on every mounted volume; an index is never removed.

