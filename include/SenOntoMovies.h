/*
 * Generated from schema/sen-movies.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

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
inline constexpr char kTitle[] = "dc:title";
/** The title in the original language, if it is another one. */
inline constexpr char kOriginalTitle[] = "schema:alternateName";
/** The date of the first release. */
inline constexpr char kReleaseDate[] = "schema:datePublished";
/** The length of the movie, as an ISO 8601 duration (PT2H28M). */
inline constexpr char kDuration[] = "schema:duration";
/** The names of the directors, when the people are not entities of their own (otherwise they are the people with a Contribution to the movie). */
inline constexpr char kDirector[] = "schema:director";
/** The names of the main actors, when the people are not entities of their own. */
inline constexpr char kActor[] = "schema:actor";
/** What the movie is about. */
inline constexpr char kDescription[] = "dc:description";
inline constexpr char kGenre[] = "schema:genre";
/** The languages of the movie (the original one first), as language codes. */
inline constexpr char kInLanguage[] = "schema:inLanguage";
inline constexpr char kCountryOfOrigin[] = "schema:countryOfOrigin";
/** The page of the poster (the thumbnail is the poster itself). */
inline constexpr char kImage[] = "schema:image";
/** The license of the poster, or where it is from. */
inline constexpr char kPosterLicense[] = "schema:license";
/** Where else the movie is known (Wikidata, IMDb, TMDB, Letterboxd), as a list of IRIs. */
inline constexpr char kSameAs[] = "schema:sameAs";
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
