# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 SEN Labs e.U.
"""
Tests of the ONI generator and of the consistency of the ontologies with the rest of SEN.

Run from the repository root:  python3 -m unittest discover -s tests -v

The checks against sento and sensei run when those repositories are siblings of this one (or given by SENTO_DIR and
SENSEI_DIR); they are what catches drift between the schema, the constants of the API and the plugins.
"""
import glob
import os
import re
import sys
import tempfile
import textwrap
import unittest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
sys.path.insert(0, os.path.join(ROOT, 'generator'))
import oni_gen  # noqa: E402
from linkml_runtime.utils.schemaview import SchemaView  # noqa: E402

SCHEMAS = sorted(glob.glob(os.path.join(ROOT, 'schema', 'sen-*.yaml')))
SENTO = os.environ.get('SENTO_DIR', os.path.join(ROOT, '..', 'sento'))
SENSEI = os.environ.get('SENSEI_DIR', os.path.join(ROOT, '..', 'sensei'))


def read(path):
    with open(path) as f:
        return f.read()


def generate_all(rdef_dir, header_dir):
    files = {}
    for schema in SCHEMAS:
        files.update(oni_gen.generate(schema, rdef_dir, header_dir))
    return files


def all_schema_attributes():
    """attribute name -> Attribute over all ontologies, with the cross-ontology consistency check."""
    attributes = {}
    for schema in SCHEMAS:
        for name, attribute in oni_gen.all_attributes(SchemaView(schema)).items():
            known = attributes.get(name)
            if known is not None and (known.type, known.searchable) != (attribute.type, attribute.searchable):
                raise oni_gen.SchemaError(f'{name} is defined differently in {schema}')
            attributes[name] = attribute
    return attributes


class GeneratorTest(unittest.TestCase):
    def test_schemas_exist(self):
        self.assertGreaterEqual(len(SCHEMAS), 6)

    def test_committed_files_are_up_to_date(self):
        files = generate_all(os.path.join(ROOT, 'ontologies'), os.path.join(ROOT, 'include'))
        for path, text in files.items():
            self.assertTrue(os.path.exists(path), f'{path} is missing: run ./generate.sh')
            with open(path) as f:
                self.assertEqual(f.read(), text, f'{path} is out of date: run ./generate.sh')

    def test_generation_is_deterministic(self):
        with tempfile.TemporaryDirectory() as a, tempfile.TemporaryDirectory() as b:
            self.assertEqual({os.path.relpath(k, a): v for k, v in generate_all(a, a).items()},
                             {os.path.relpath(k, b): v for k, v in generate_all(b, b).items()})

    def test_every_attribute_array_is_aligned(self):
        """The installer reads the fields of ATTR_INFO by position: all must have one entry per attribute."""
        with tempfile.TemporaryDirectory() as out:
            for path, text in generate_all(out, None).items():
                if not path.endswith('.rdef') or 'META:ATTR_INFO' not in text:
                    continue
                block = text.split('META:ATTR_INFO')[1].split('};')[0]
                counts = {}
                for key in re.findall(r'"attr:(\w+)" =', block):
                    counts[key] = counts.get(key, 0) + 1
                self.assertEqual(len(set(counts.values())), 1, f'{path}: {counts}')
                self.assertEqual(set(counts), {'name', 'public_name', 'type', 'viewable', 'editable', 'searchable',
                                               'width', 'alignment', 'display_as'}, path)

    def test_mime_types_are_unique_and_well_formed(self):
        seen = {}
        for schema in SCHEMAS:
            sv = SchemaView(schema)
            for mc in oni_gen.read_classes(sv, os.path.dirname(schema)):
                self.assertNotIn(mc.mime, seen, f'{mc.mime} in {schema} and {seen.get(mc.mime)}')
                seen[mc.mime] = schema
                self.assertRegex(mc.mime, r'^[a-z]+(/[a-z0-9.\-]+)?$|^application/x-vnd\.[A-Za-z0-9.\-]+$')

    def test_attribute_definitions_agree_across_ontologies(self):
        attributes = all_schema_attributes()
        self.assertIn('SEN:ID', attributes)
        for name, attribute in attributes.items():
            self.assertLessEqual(len(name), oni_gen.MAX_ATTR_NAME)

    def test_index_attributes_are_the_graph_identity(self):
        searchable = {a.name for a in all_schema_attributes().values() if a.searchable}
        self.assertTrue({'SEN:ID', 'SEN:TO', 'SEN:META'} <= searchable)

    def test_relation_types_have_the_relation_supertype(self):
        for schema in SCHEMAS:
            sv = SchemaView(schema)
            for mc in oni_gen.read_classes(sv, os.path.dirname(schema)):
                if mc.config is not None:
                    self.assertTrue(mc.mime.startswith('relation'), mc.mime)


class ValidationTest(unittest.TestCase):
    """The generator has to refuse what would produce a broken ontology."""

    def schema(self, body):
        directory = tempfile.mkdtemp()
        self.addCleanup(lambda: __import__('shutil').rmtree(directory))
        os.makedirs(os.path.join(directory, 'icons'))
        path = os.path.join(directory, 'sen-t.yaml')
        with open(path, 'w') as f:
            f.write(textwrap.dedent('''\
                id: https://example.org/t
                name: sen_t
                title: T
                version: 0.0.1
                prefixes: {linkml: "https://w3id.org/linkml/", sen: "https://sen-labs.org/ns/sen#", dc: "http://purl.org/dc/elements/1.1/"}
                default_prefix: sen
                default_range: string
                imports: [linkml:types]
                ''') + textwrap.dedent(body))
        return path

    def generate(self, body):
        with tempfile.TemporaryDirectory() as out:
            return oni_gen.generate(self.schema(body), out, None)

    def test_missing_mime_type(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'mime'):
            self.generate('''
                classes:
                  A: {title: A, description: a}
                ''')

    def test_slot_without_attribute_name(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'attribute'):
            self.generate('''
                slots:
                  s: {title: S}
                classes:
                  A: {title: A, slots: [s], annotations: {mime: entity/x-a}}
                ''')

    def test_invalid_attribute_name(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'valid attribute name'):
            self.generate('''
                slots:
                  s: {title: S, annotations: {attribute: "no prefix"}}
                classes:
                  A: {title: A, slots: [s], annotations: {mime: entity/x-a}}
                ''')

    def test_same_attribute_with_different_type(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'defined twice'):
            self.generate('''
                slots:
                  a: {title: A, range: integer, annotations: {attribute: "dc:x"}}
                  b: {title: B, range: string, annotations: {attribute: "dc:x"}}
                classes:
                  A: {title: A, slots: [a, b], annotations: {mime: entity/x-a}}
                ''')

    def test_duplicate_mime_type(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'used by'):
            self.generate('''
                classes:
                  A: {title: A, annotations: {mime: entity/x-a}}
                  B: {title: B, annotations: {mime: entity/x-a}}
                ''')

    def test_bool_attribute_cannot_be_indexed(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'cannot be indexed'):
            self.generate('''
                slots:
                  s: {title: S, range: boolean, annotations: {attribute: "dc:s", searchable: true}}
                classes:
                  A: {title: A, slots: [s], annotations: {mime: entity/x-a}}
                ''')

    def test_missing_icon_file(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'icon file'):
            self.generate('''
                classes:
                  A: {title: A, annotations: {mime: entity/x-a, icon: nothing}}
                ''')

    def test_flavors_only_on_relations(self):
        with self.assertRaisesRegex(oni_gen.SchemaError, 'not a relation'):
            self.generate('''
                classes:
                  A: {title: A, annotations: {mime: entity/x-a, dynamic: true}}
                ''')

    def test_valid_schema_generates_aligned_rdef(self):
        files = self.generate('''
            slots:
              s: {title: S, annotations: {attribute: "dc:s"}}
            classes:
              A: {title: A, description: "An A", slots: [s], annotations: {mime: entity/x-a}}
            ''')
        rdef = next(v for k, v in files.items() if k.endswith('x-a.rdef'))
        self.assertIn('"attr:name" = "dc:s"', rdef)
        self.assertIn('"attr:public_name" = "S"', rdef)


@unittest.skipUnless(os.path.isdir(os.path.join(SENTO, 'src')), 'sento is not next to this repository')
class SentoConsistencyTest(unittest.TestCase):
    """The constants of the API must name attributes that the ontology defines."""

    def constants(self, header, namespace=None):
        text = read(os.path.join(SENTO, 'src', 'cpp', 'include', header))
        return dict(re.findall(r'inline constexpr char (k\w+)\[\]\s*=\s*"([^"]+)";', text))

    def test_identity_and_relation_attributes_are_defined(self):
        attributes = set(all_schema_attributes())
        constants = self.constants('SenAttributes.h')
        # attributes that the ontology must define (the rest are prefixes, resource names and chunk names)
        for name in ('kId', 'kTo', 'kMeta', 'kRelationSource', 'kRelationTarget', 'kRelationSourceRef',
                     'kRelationTargetRef', 'kRelationLabel', 'kOntologyAuthor', 'kOntologySchemaUrl',
                     'kOntologyVersion', 'kOntologyDescription', 'kOntologyStable'):
            self.assertIn(constants[name], attributes, f'{name} = {constants[name]} is not defined in the ontology')

    def test_plugin_features_are_defined(self):
        attributes = set(all_schema_attributes())
        text = read(os.path.join(SENTO, 'src', 'cpp', 'include', 'Sensei.h'))
        features = re.findall(r'namespace feature \{(.*?)\}', text, re.S)[0]
        for feature in re.findall(r'"(\w+)"', features):
            self.assertIn(f'SEN:plugin:{feature}', attributes)

    def test_relation_config_keys_match_the_generated_rdefs(self):
        keys = self.constants('SenMessages.h')
        with tempfile.TemporaryDirectory() as out:
            rdefs = '\n'.join(t for p, t in generate_all(out, None).items() if p.endswith('.rdef'))
        for name in ('kRelation', 'kInverse', 'kBidirectional', 'kDynamic', 'kSelf'):
            self.assertIn(f'"{keys[name]}"', rdefs, f'{name} = {keys[name]} is never produced by the ontologies')

    def test_every_attribute_constant_has_the_sen_prefix_or_a_standard_one(self):
        for name, value in self.constants('SenAttributes.h').items():
            self.assertRegex(value, r'^(SEN|META|schema|dc|dcterms|oa|be):')


@unittest.skipUnless(os.path.isdir(os.path.join(SENSEI, 'src')), 'sensei is not next to this repository')
class SenseiConsistencyTest(unittest.TestCase):
    """What the plugins declare must exist in the ontologies."""

    def rdefs(self):
        return glob.glob(os.path.join(SENSEI, 'src', '**', 'Resources.rdef'), recursive=True)

    def mime_types(self):
        types = set()
        for schema in SCHEMAS:
            types |= {mc.mime for mc in oni_gen.read_classes(SchemaView(schema), os.path.dirname(schema))}
        return types

    def test_attribute_mappings_name_defined_attributes(self):
        attributes = set(all_schema_attributes())
        for path in self.rdefs():
            text = read(path)
            match = re.search(r'"SEN:attrMapping"\) message \{(.*?)\};', text, re.S)
            if not match:
                continue
            for alias, attribute in re.findall(r'"(\w+)"\s*=\s*"([^"]+)"', match.group(1)):
                self.assertIn(attribute, attributes, f'{path}: {alias} -> {attribute}')

    def test_declared_types_are_defined(self):
        types = self.mime_types()
        for path in self.rdefs():
            text = read(path)
            for value in re.findall(r'"(?:types|SEN:default)"\s*=\s*"([^"]+)"', text):
                for t in value.split(','):
                    t = t.strip()
                    if '/' in t and not t.startswith(('text/', 'application/pdf', 'application/x-vnd.Be')):
                        self.assertIn(t, types, f'{path}: {t}')

    def test_plugins_declare_the_current_resource_names(self):
        for path in self.rdefs():
            text = read(path)
            self.assertNotIn('"SEN:TYPE"', text, path)
            self.assertNotIn('type_mapping', text, path)
            self.assertNotIn('attr_mapping', text, path)


if __name__ == '__main__':
    unittest.main()
