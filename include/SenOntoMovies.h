/*
 * Generated from schema/sen-movies.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

/**
 * @file SenOntoMovies.h
 * @brief Attribute names and MIME types of the ontology "SEN Movies": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace movies {

namespace attr {
inline constexpr char kScene[] = "SEN:REL:scene";
inline constexpr char kChapterStart[] = "SEN:REL:chapterStart";
inline constexpr char kChapterEnd[] = "SEN:REL:chapterEnd";
/** Start of the place in the movie, in seconds. */
inline constexpr char kTimeStart[] = "schema:startOffset";
/** End of the place in the movie, in seconds. */
inline constexpr char kTimeEnd[] = "schema:endOffset";
}	// namespace attr

namespace mime {
/** Movie: a movie */
inline constexpr char kMovie[] = "entity/x-vnd.sen-labs.entity.movie";
/** Movie medium: a physical movie medium (VHS,DVD,Bluray) */
inline constexpr char kMovieMedium[] = "entity/x-vnd.sen-labs.entity.movie.medium";
/** Movie Reference: Reference to a Movie */
inline constexpr char kMovieReference[] = "relation/x-vnd.sen-labs.relation.movie.reference";
}	// namespace mime

}	// namespace movies
}	// namespace onto
}	// namespace sen
