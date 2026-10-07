/*
 * Generated from schema/sen-books.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoBooks.h
 * @brief Attribute names and MIME types of the ontology "SEN Books": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace books {

namespace attr {
inline constexpr char kTitle[] = "dc:title";
/** The authors. */
inline constexpr char kCreator[] = "dc:creator";
inline constexpr char kSubject[] = "dc:subject";
inline constexpr char kLanguage[] = "dc:language";
inline constexpr char kPublisher[] = "dc:publisher";
/** Date of publication of the edition. */
inline constexpr char kDate[] = "dc:date";
/** Classification code of the library (Dewey, LoC, ...). */
inline constexpr char kBookClass[] = "SEN:classification";
inline constexpr char kBookFormat[] = "schema:bookFormat";
inline constexpr char kIsbn[] = "schema:isbn";
inline constexpr char kNumberOfPages[] = "schema:numberOfPages";
/** The first words of the quote. */
inline constexpr char kQuoteStart[] = "SEN:REL:textStart";
/** The last words of the quote. */
inline constexpr char kQuoteEnd[] = "SEN:REL:textEnd";
inline constexpr char kQuoteSource[] = "dc:source";
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
	{attr::kIsbn, 'CSTR'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace mime {
/** Book: describes a book entity */
inline constexpr char kBook[] = "entity/x-vnd.sen-labs.entity.book";
/** Authorship: authorship of a particular work */
inline constexpr char kAuthorship[] = "relation/x-vnd.sen-labs.relation.authorship";
/** Book Quote: quotes some text in a book */
inline constexpr char kBookQuote[] = "relation/x-vnd.sen-labs.relation.book.quote";
}	// namespace mime

}	// namespace books
}	// namespace onto
}	// namespace sen
