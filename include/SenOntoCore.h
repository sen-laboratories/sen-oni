/*
 * Generated from schema/sen-core.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoCore.h
 * @brief Attribute names and MIME types of the ontology "SEN Core": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace core {

namespace attr {
/** Unique identifier of the object, a TSID. */
inline constexpr char kSenId[] = "SEN:ID";
/** IDs of the targets of the normal relations of the object (chunked, see SEN:TO:<n>). */
inline constexpr char kSenTo[] = "SEN:TO";
/** IDs of the targets of the meta relations (classification and context) of the object. */
inline constexpr char kSenMeta[] = "SEN:META";
/** Semantic type of a file, e.g. document/scientific-paper, set by identify plugins (Haiku itself only knows the technical MIME type). Plugins carry the plugin type here. */
inline constexpr char kSemanticType[] = "META:TYPE";
/** The entity is fictional. */
inline constexpr char kFictional[] = "SEN:fictional";
/** ID of the source of the relation. */
inline constexpr char kRelSource[] = "SEN:REL:ID";
/** ID of the target of the relation. */
inline constexpr char kRelTarget[] = "SEN:REL:TO";
/** entry_ref of the source, for sources without a SEN:ID. */
inline constexpr char kRelSourceRef[] = "SEN:REL:SRC";
/** entry_ref of the target. */
inline constexpr char kRelTargetRef[] = "SEN:REL:TRG";
/** Identifier of the relation, only present where several relations of one type exist between two files. */
inline constexpr char kRelId[] = "SEN:REL:relationId";
/** The target of the relation does not exist (any more). */
inline constexpr char kRelMissing[] = "SEN:REL:missing";
/** The type of the relation where it is not the one of the folder (e.g. the type of what a type contains). */
inline constexpr char kRelType[] = "SEN:relationType";
/** The relation cannot be changed or removed by users (e.g. the relations of an ontology to its types). */
inline constexpr char kRelReadonly[] = "SEN:REL:readonly";
/** Label of the relation. */
inline constexpr char kRelLabel[] = "SEN:REL:Label";
inline constexpr char kRelKind[] = "SEN:REL:Kind";
inline constexpr char kRelDesc[] = "SEN:REL:Desc";
inline constexpr char kRelRole[] = "SEN:REL:Role";
/** Name of the attribute, e.g. dc:title. */
inline constexpr char kAttrName[] = "SEN:attr:name";
/** Type of the value of the attribute (string, int32, bool, time, ...). */
inline constexpr char kAttrType[] = "SEN:attr:type";
/** The attribute is displayed by default, e.g. as a column in views. */
inline constexpr char kAttrViewable[] = "SEN:attr:viewable";
/** The value of the attribute can be edited in views. */
inline constexpr char kAttrEditable[] = "SEN:attr:editable";
/** The attribute is indexed (on every volume) so that it can be queried. A SEN extension of the attribute info. */
inline constexpr char kAttrSearchable[] = "SEN:attr:searchable";
/** Default width of the column of the attribute. */
inline constexpr char kAttrWidth[] = "SEN:attr:width";
/** First page of the place (the number of the physical page; roman page numbers are mapped). */
inline constexpr char kPageStart[] = "schema:pageStart";
/** Last page of the place. */
inline constexpr char kPageEnd[] = "schema:pageEnd";
inline constexpr char kSection[] = "SEN:REL:section";
inline constexpr char kParagraph[] = "SEN:REL:paragraph";
inline constexpr char kAnchor[] = "SEN:REL:anchor";
inline constexpr char kHeading[] = "SEN:REL:heading";
/** Offset of the first character of the selection (oa:TextPositionSelector). */
inline constexpr char kTextStart[] = "oa:start";
/** Offset after the last character of the selection. */
inline constexpr char kTextEnd[] = "oa:end";
inline constexpr char kLineStart[] = "SEN:REL:lineStart";
inline constexpr char kLineEnd[] = "SEN:REL:lineEnd";
/** Line of the place (the attribute that Haiku's editors understand). */
inline constexpr char kLine[] = "be:line";
inline constexpr char kColumn[] = "be:column";
inline constexpr char kPluginExtract[] = "SEN:plugin:extract";
inline constexpr char kPluginEnrich[] = "SEN:plugin:enrich";
inline constexpr char kPluginIdentify[] = "SEN:plugin:identify";
inline constexpr char kPluginNavigate[] = "SEN:plugin:navigate";
inline constexpr char kPluginSearch[] = "SEN:plugin:search";
inline constexpr char kOntoSchemaUrl[] = "SEN:onto:schema_url";
inline constexpr char kOntoAuthor[] = "SEN:onto:author";
inline constexpr char kOntoVersion[] = "SEN:onto:version";
inline constexpr char kOntoDescription[] = "SEN:onto:description";
inline constexpr char kOntoStable[] = "SEN:onto:stable";
}	// namespace attr

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"

/** An attribute that is queried and needs a BFS index on every volume (the type of the index is that of the attribute). */
struct Index {
	const char* name;
	uint32_t    type;
};
/** The indices of this ontology: created by the ontology installer and by the SEN server on every mounted volume. */
inline constexpr Index kIndices[] = {
	{attr::kSenId, 'CSTR'},
	{attr::kSenTo, 'CSTR'},
	{"SEN:TO:1", 'CSTR'},
	{"SEN:TO:2", 'CSTR'},
	{"SEN:TO:3", 'CSTR'},
	{"SEN:TO:4", 'CSTR'},
	{"SEN:TO:5", 'CSTR'},
	{"SEN:TO:6", 'CSTR'},
	{"SEN:TO:7", 'CSTR'},
	{attr::kSenMeta, 'CSTR'},
	{"SEN:META:1", 'CSTR'},
	{"SEN:META:2", 'CSTR'},
	{"SEN:META:3", 'CSTR'},
	{"SEN:META:4", 'CSTR'},
	{"SEN:META:5", 'CSTR'},
	{"SEN:META:6", 'CSTR'},
	{"SEN:META:7", 'CSTR'},
	{attr::kSemanticType, 'CSTR'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace mime {
/** Abstract Entity: A semantic entity representing anything. */
inline constexpr char kEntity[] = "entity";
/** native types used for classification: Used for labelling and grouping entities in SEN. */
inline constexpr char kClassification[] = "classification";
/** SEN Relation: a semantic relationship between files and entities in SEN. */
inline constexpr char kRelation[] = "relation";
/** SEN Meta: Describes what other types consist of, e.g. the attributes of a MIME type. */
inline constexpr char kMeta[] = "meta";
/** Object: a general object or Thing in SEN */
inline constexpr char kObject[] = "entity/x-vnd.sen-labs.entity.object";
/** Document Outline: a structural component in a document, e.g. bookmark or label. */
inline constexpr char kDocumentOutlineItem[] = "entity/x-vnd.sen-labs.entity.document.outline-item";
/** Attribute: an attribute of a MIME type (its attribute info), contained in the type. */
inline constexpr char kMimeAttribute[] = "meta/x-vnd.sen-labs.meta-mime-attribute";
/** Label: a more generic label or tag used for simple classification */
inline constexpr char kLabel[] = "classification/x-vnd.sen-labs.entity.label";
/** Topic: a subject or theme associated with an entity */
inline constexpr char kTopic[] = "classification/x-vnd.sen-labs.entity.topic";
/** Concept: an abstract idea */
inline constexpr char kConcept[] = "classification/x-vnd.sen-labs.entity.concept";
/** Collection: a group of entities */
inline constexpr char kCollection[] = "classification/x-vnd.sen-labs.entity.collection";
/** Context: top-level container for grouping things */
inline constexpr char kContext[] = "classification/x-vnd.sen-labs.entity.context";
/** Association: special relation used for classification. */
inline constexpr char kAssociation[] = "relation/x-vnd.sen-labs.relation.association";
/** Generic Reference: generic reference to another entity. */
inline constexpr char kReference[] = "relation/x-vnd.sen-labs.relation.reference";
/** Contains: relates to self contained components or parts. */
inline constexpr char kContains[] = "relation/x-vnd.sen-labs.relation.contains";
/** Document Reference: structural reference to a document outline element. */
inline constexpr char kDocumentReference[] = "relation/x-vnd.sen-labs.relation.docref";
/** Dependency: a dependency between two things, like those between Haiku packages. The property Kind says what it is: provides, requires, uses, supplements, conflicts, freshens or replaces (see DependencyKind). The label is the one of the kind ("provides"), and the opposite direction says it the other way ("provided by"). */
inline constexpr char kDependency[] = "relation/x-vnd.sen-labs.relation.dependency";
/** Textual Reference: reference to a plaintext document. */
inline constexpr char kTextReference[] = "relation/x-vnd.sen-labs.relation.textref";
/** SEN Plugin: a SEN plugin for handling various semantic tasks on entities or relations. */
inline constexpr char kPlugin[] = "application/x-vnd.sen-labs.plugin";
/** Ontology: defines a set of file types for Entities and Relations in SEN. */
inline constexpr char kOntology[] = "application/x-vnd.sen-labs.ontology";
}	// namespace mime

}	// namespace core
}	// namespace onto
}	// namespace sen
