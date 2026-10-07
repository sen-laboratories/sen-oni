#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
"""
oni_gen: generates the artifacts of an ONI ontology from its LinkML schema.

    oni_gen.py <schema.yaml>... --rdef-dir ontologies --header-dir include

For every schema it writes
  * the Haiku resource definitions (.rdef) of the MIME types of the classes defined in the schema,
  * manifest.properties (read by the ontology installer),
  * a C++ header with the constants of the attribute names and MIME types (SenOnto<Name>.h).

The vocabulary of annotations that it understands is described in README.md. Generation is deterministic: the same schema
always gives the same files, so that CI can check that the committed files are up to date.
"""
import argparse
import os
import re
import sys
from dataclasses import dataclass, field

from linkml_runtime.utils.schemaview import SchemaView

# LinkML range -> BFS / Haiku type code
RANGE_TYPES = {
    'string': 'CSTR', 'uriorcurie': 'CSTR', 'uri': 'CSTR', 'integer': 'LONG', 'boolean': 'BOOL',
    'datetime': 'TIME', 'date': 'TIME', 'float': 'FLOT', 'double': 'DBLE',
}
BFS_TYPES = {'CSTR', 'LONG', 'SHRT', 'LLNG', 'BOOL', 'TIME', 'FLOT', 'DBLE', 'RREF'}
# BFS can index only these
INDEXABLE = {'CSTR', 'LONG', 'SHRT', 'LLNG', 'TIME', 'FLOT', 'DBLE'}
ATTR_NAME_PATTERN = re.compile(r'^[A-Za-z][A-Za-z0-9_]*(:[A-Za-z0-9_.\-]+)+$')
MAX_ATTR_NAME = 255   # B_ATTR_NAME_LENGTH


class SchemaError(Exception):
    pass


def annotation(obj, key, default=None):
    """The value of an annotation of a LinkML element, or the default."""
    annotations = getattr(obj, 'annotations', None)
    if annotations and key in annotations:
        return annotations[key].value
    return default


@dataclass
class Attribute:
    slot: str
    name: str          # name of the BFS attribute
    public: str
    type: str
    viewable: bool = True
    editable: bool = True
    searchable: bool = False
    width: int = 60
    alignment: int = 0
    display_as: str = ''
    description: str = ''


def camel(name):
    return ''.join(part[:1].upper() + part[1:] for part in re.split(r'[_\W]+', name) if part)


def one_line(text):
    return ' '.join((text or '').split())


def rdef_string(text):
    return '"' + one_line(text).replace('\\', '\\\\').replace('"', '\\"') + '"'


def read_attribute(sv, slot_name):
    slot = sv.get_slot(slot_name)
    if slot is None:
        raise SchemaError(f'unknown slot {slot_name}')
    name = annotation(slot, 'attribute')
    if not name:
        raise SchemaError(f'slot {slot_name} has no "attribute" annotation (name of the file attribute)')
    if not ATTR_NAME_PATTERN.match(name) or len(name) > MAX_ATTR_NAME:
        raise SchemaError(f'slot {slot_name}: "{name}" is not a valid attribute name (<prefix>:<name>)')
    type_code = annotation(slot, 'bfs_type')
    if type_code is None:
        range_name = slot.range or sv.schema.default_range or 'string'
        if range_name not in RANGE_TYPES:
            raise SchemaError(f'slot {slot_name}: range {range_name} has no BFS type, set the annotation bfs_type')
        type_code = RANGE_TYPES[range_name]
    if type_code not in BFS_TYPES:
        raise SchemaError(f'slot {slot_name}: unknown bfs_type {type_code}')
    if not slot.title:
        raise SchemaError(f'slot {slot_name} has no title (the public name of the attribute)')
    searchable = bool(annotation(slot, 'searchable', False))
    if searchable and type_code not in INDEXABLE:
        raise SchemaError(f'slot {slot_name}: a {type_code} attribute cannot be indexed')
    return Attribute(
        slot=slot_name, name=name, public=slot.title, type=type_code,
        viewable=bool(annotation(slot, 'viewable', True)), editable=bool(annotation(slot, 'editable', True)),
        searchable=searchable, width=int(annotation(slot, 'width', 60)),
        alignment=int(annotation(slot, 'alignment', 0)), display_as=str(annotation(slot, 'display_as', '')),
        description=one_line(slot.description))


def all_attributes(sv):
    """All attributes of the schema and its imports, with the consistency check: one attribute name means one definition."""
    attributes = {}
    for slot_name in sv.all_slots():
        attribute = read_attribute(sv, slot_name)
        known = attributes.get(attribute.name)
        if known is not None and (known.type, known.searchable) != (attribute.type, attribute.searchable):
            raise SchemaError(
                f'attribute {attribute.name} is defined twice with different type or index flag '
                f'({known.slot}: {known.type}/{known.searchable}, {slot_name}: {attribute.type}/{attribute.searchable}); '
                f'one name must have one type, and the installer removes the index of an attribute marked as not searchable')
        attributes[attribute.name] = attribute
    return attributes


@dataclass
class MimeClass:
    cls_name: str
    mime: str
    supertype: bool
    title: str
    description: str
    app: str = None
    extensions: str = None
    icon: str = None
    folder_icon: str = None
    attributes: list = field(default_factory=list)
    config: dict = None


def read_classes(sv, schema_dir):
    classes = []
    seen = {}
    for cls_name, cls in sv.schema.classes.items():
        mime = annotation(cls, 'mime')
        if not mime:
            raise SchemaError(f'class {cls_name} has no "mime" annotation (its MIME type)')
        if mime in seen:
            raise SchemaError(f'MIME type {mime} is used by {seen[mime]} and {cls_name}')
        seen[mime] = cls_name
        supertype = bool(annotation(cls, 'supertype', False))
        if supertype == ('/' in mime):
            raise SchemaError(f'class {cls_name}: {mime} looks like a {"sub" if supertype else "super"}type')
        if not cls.title:
            raise SchemaError(f'class {cls_name} has no title (short description of the type)')
        mc = MimeClass(cls_name, mime, supertype, one_line(cls.title), one_line(cls.description),
                       app=annotation(cls, 'app'), extensions=annotation(cls, 'extensions'),
                       icon=annotation(cls, 'icon'), folder_icon=annotation(cls, 'folder_icon'))
        for icon in (mc.icon, mc.folder_icon):
            if icon and not os.path.exists(os.path.join(schema_dir, 'icons', icon + '.hex')):
                raise SchemaError(f'class {cls_name}: icon file icons/{icon}.hex does not exist')
        mc.attributes = [read_attribute(sv, slot) for slot in (cls.slots or [])]
        flags = {key: annotation(cls, key) for key in ('bidir', 'dynamic', 'contained', 'label', 'inverse_label')}
        if any(value is not None for value in flags.values()):
            if not (mime.startswith('relation/') or mime == 'relation'):
                raise SchemaError(f'class {cls_name}: relation flavors on a type that is not a relation')
            mc.config = flags
        classes.append(mc)
    return classes


def indent_lines(lines, tabs=1):
    return [('\t' * tabs + line) if line else line for line in lines]


def hex_block(schema_dir, icon):
    with open(os.path.join(schema_dir, 'icons', icon + '.hex')) as f:
        return ['\t' + line.strip() for line in f if line.strip()]


def boolean(value):
    return 'true' if value else 'false'


def render_rdef(mc, schema_file, schema_dir):
    out = [f'// Generated from {schema_file} by oni_gen.py: do not edit.',
           '// SPDX-License-Identifier: MIT',
           '// SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.', '']
    n = 0

    def resource(text):
        nonlocal n
        out.append(f'resource({n}, {text}')
        n += 1

    if mc.supertype:
        resource('"BEOS:TYPE") #\'MIMS\' "application/x-vnd.Be-meta-mime";')
    resource(f'"META:TYPE") {rdef_string(mc.mime)};')
    resource(f'"META:L:DESC") #\'MLDC\' {rdef_string(mc.description)};')
    resource(f'"META:S:DESC") #\'MSDC\' {rdef_string(mc.title)};')
    if mc.app:
        resource(f'"META:PREF_APP") #\'MSIG\' {rdef_string(mc.app)};')
    if mc.extensions:
        resource('"META:EXTENS") message {')
        out.append(f'\t"extensions" = {rdef_string(mc.extensions)}')
        out.append('};')
    if mc.attributes:
        resource('"META:ATTR_INFO") message(233) {')
        body = []
        for key, fmt in (('name', lambda a: rdef_string(a.name)), ('public_name', lambda a: rdef_string(a.public)),
                         ('type', lambda a: f"'{a.type}'"), ('viewable', lambda a: boolean(a.viewable)),
                         ('editable', lambda a: boolean(a.editable)), ('searchable', lambda a: boolean(a.searchable)),
                         ('width', lambda a: str(a.width)), ('alignment', lambda a: str(a.alignment)),
                         ('display_as', lambda a: rdef_string(a.display_as))):
            for attribute in mc.attributes:
                body.append(f'\t"attr:{key}" = {fmt(attribute)}')
        body.append(f'\t"type" = {rdef_string(mc.mime)}')
        out.append(',\n'.join(body))
        out.append('};')
    if mc.config is not None:
        c = mc.config
        resource('"SEN:REL:CONFIG") message(233) {')
        entries = []
        if c['bidir'] is not None:
            entries.append(f'\t"SEN:bidir" = {boolean(c["bidir"])}')
        if c['dynamic']:
            entries.append('\t"SEN:dynamic" = true')
        if c['contained']:
            entries.append('\t"SEN:self" = true')
        if c['label']:
            entries.append("\t\"SEN:relation\" = message('SCrd') {\n\t\t\"SEN:REL:Label\" = " + rdef_string(c['label']) + '\n\t}')
        if c['inverse_label']:
            entries.append("\t\"SEN:inverse\" = message('SCrd') {\n\t\t\"SEN:REL:Label\" = " + rdef_string(c['inverse_label']) + '\n\t}')
        out.append(',\n'.join(entries))
        out.append('};')
    if mc.icon:
        resource('"META:ICON") #\'VICN\' array {')
        out.extend(hex_block(schema_dir, mc.icon))
        out.append('};')
    if mc.folder_icon:
        resource('"SEN:ICON:FOLDER") #\'VICN\' array {')
        out.extend(hex_block(schema_dir, mc.folder_icon))
        out.append('};')
    return '\n'.join(out) + '\n'


def render_manifest(sv):
    schema = sv.schema
    stable = boolean(annotation(schema, 'oni_stable', False))
    return '\n'.join([
        f'SCHEMA="{annotation(schema, "oni_schema_url", "")}"',
        f'AUTHOR="{annotation(schema, "oni_author", "")}"',
        f'VERSION="{schema.version}"',
        f'DESCRIPTION="{one_line(schema.description)}"',
        f'STABLE={stable}', ''])


def namespace_of(schema):
    return re.sub(r'^sen_', '', schema.name)


def render_header(sv, classes, schema_file):
    schema = sv.schema
    ns = namespace_of(schema)
    out = ['/*', f' * Generated from {schema_file} by oni_gen.py: do not edit.',
           ' * SPDX-License-Identifier: MIT', ' * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.', ' */',
           '#pragma once', '', '/**', f' * @file SenOnto{camel(ns)}.h',
           f' * @brief Attribute names and MIME types of the ontology "{schema.title}": use these instead of string literals.',
           ' */', f'namespace sen {{', 'namespace onto {', f'namespace {ns} {{', '', 'namespace attr {']
    for slot_name in schema.slots:
        a = read_attribute(sv, slot_name)
        if a.description:
            out.append(f'/** {a.description} */')
        out.append(f'inline constexpr char k{camel(slot_name)}[] = "{a.name}";')
    out += ['}\t// namespace attr', '', 'namespace mime {']
    for mc in classes:
        out.append(f'/** {mc.title}: {mc.description} */')
        out.append(f'inline constexpr char k{mc.cls_name}[] = "{mc.mime}";')
    out += ['}\t// namespace mime', '', f'}}\t// namespace {ns}', '}\t// namespace onto', '}\t// namespace sen', '']
    return '\n'.join(out)


def rdef_path(rdef_dir, ontology, mc):
    if mc.supertype:
        return os.path.join(rdef_dir, ontology, mc.mime + '.rdef')
    supertype, leaf = mc.mime.split('/', 1)
    return os.path.join(rdef_dir, ontology, supertype, leaf + '.rdef')


def generate(schema_path, rdef_dir, header_dir):
    """Generate the artifacts of one schema; returns the list of files written."""
    schema_path = os.path.abspath(schema_path)
    schema_dir = os.path.dirname(schema_path)
    schema_file = os.path.relpath(schema_path, os.path.dirname(schema_dir))
    sv = SchemaView(schema_path)
    ontology = namespace_of(sv.schema)
    all_attributes(sv)                       # consistency check across the schema and its imports
    classes = read_classes(sv, schema_dir)
    written = {}
    for mc in classes:
        written[rdef_path(rdef_dir, ontology, mc)] = render_rdef(mc, schema_file, schema_dir)
    written[os.path.join(rdef_dir, ontology, 'manifest.properties')] = render_manifest(sv)
    if header_dir:
        written[os.path.join(header_dir, f'SenOnto{camel(ontology)}.h')] = render_header(sv, classes, schema_file)
    return written


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('schemas', nargs='+')
    parser.add_argument('--rdef-dir', required=True, help='where the ontology folders are written')
    parser.add_argument('--header-dir', help='where the C++ headers are written')
    parser.add_argument('--check', action='store_true', help='write nothing, fail if a file differs from what would be written')
    args = parser.parse_args(argv)
    stale = 0
    for schema in args.schemas:
        try:
            files = generate(schema, args.rdef_dir, args.header_dir)
        except SchemaError as e:
            print(f'{schema}: {e}', file=sys.stderr)
            return 1
        for path, text in sorted(files.items()):
            if args.check:
                current = open(path).read() if os.path.exists(path) else None
                if current != text:
                    print(f'out of date: {path}', file=sys.stderr)
                    stale += 1
            else:
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(path, 'w') as f:
                    f.write(text)
    return 1 if stale else 0


if __name__ == '__main__':
    sys.exit(main())
