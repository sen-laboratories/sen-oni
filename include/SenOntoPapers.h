/*
 * Generated from schema/sen-papers.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoPapers.h
 * @brief Attribute names and MIME types of the ontology "SEN Papers": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace papers {

namespace attr {
/** The DOI of the paper, bare and lower case (10.1038/nature14539). It is indexed, so that the papers that a paper cites can be found on the computer. */
inline constexpr char kIdentifier[] = "dc:identifier";
inline constexpr char kTitle[] = "dc:title";
/** The names of the authors, separated by semicolons. */
inline constexpr char kCreator[] = "dc:creator";
/** Keywords, separated by semicolons. */
inline constexpr char kSubject[] = "dc:subject";
/** The abstract. */
inline constexpr char kAbstractText[] = "dc:description";
inline constexpr char kPublisher[] = "dc:publisher";
inline constexpr char kLanguage[] = "dc:language";
/** Day of publication. */
inline constexpr char kDate[] = "dc:date";
/** The kind of work (article, preprint, book-chapter, ...). */
inline constexpr char kKind[] = "dc:type";
/** The journal, conference or repository that the paper was published in. */
inline constexpr char kVenue[] = "dcterms:isPartOf";
/** Landing page of the best open access copy, else the page of the DOI. */
inline constexpr char kUrl[] = "schema:url";
/** Direct link to the PDF of the best open access copy. */
inline constexpr char kContentUrl[] = "schema:contentUrl";
/** How freely the paper can be read (gold, green, hybrid, bronze, diamond or closed). */
inline constexpr char kOaStatus[] = "SEN:oaStatus";
/** How often the paper is cited. */
inline constexpr char kCitationCount[] = "SEN:citationCount";
/** The year of the work that a citation points to. */
inline constexpr char kRelYear[] = "SEN:REL:year";
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
	{attr::kIdentifier, 'CSTR'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace mime {
/** Document: Documents that SEN knows more about than their file format tells. */
inline constexpr char kDocument[] = "document";
/** Scientific Paper: A scientific paper (an article, a preprint, a chapter) that has a DOI. It is the semantic type of the file (META:TYPE), the file keeps its own type (PDF, EPUB). */
inline constexpr char kScientificPaper[] = "document/scientific-paper";
/** Citation: a work that a paper cites (the generic reference, with what is known about the cited work). */
inline constexpr char kCitation[] = "relation/x-vnd.sen-labs.relation.citation";
}	// namespace mime

}	// namespace papers
}	// namespace onto
}	// namespace sen
