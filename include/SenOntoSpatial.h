/*
 * Generated from schema/sen-spatial.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoSpatial.h
 * @brief Attribute names and MIME types of the ontology "SEN Spatial": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace spatial {

namespace attr {
/** The name of the place or location. */
inline constexpr char kTitle[] = "dc:title";
/** The other names of the place (in other languages, older names), as a list. */
inline constexpr char kAlternateName[] = "schema:alternateName";
/** What there is to say about the place. */
inline constexpr char kDescription[] = "dc:description";
/** What kind of place it is, in a word (the name of its type): place of worship, city, country, district. It is what is shown; the IRI of the type is in Place Type IRI. */
inline constexpr char kPlaceType[] = "SEN:placeType";
/** What kind of place it is, as an IRI: the tag of OpenStreetMap that the place was found with (https://wiki.openstreetmap.org/wiki/Tag:amenity=place_of_worship; for an administrative boundary the tag place with the kind of the area, Tag:place=city, which says more than boundary=administrative), or the class of Wikidata (http://www.wikidata.org/entity/Q515) for a place that Wikidata told. */
inline constexpr char kPlaceTypeUri[] = "schema:additionalType";
/** The web site of the place. */
inline constexpr char kUrl[] = "schema:url";
/** Where else the place is known (Wikidata, OpenStreetMap, Wikipedia, GeoNames), as a list of IRIs. */
inline constexpr char kSameAs[] = "schema:sameAs";
/** The page of the picture of the place (the thumbnail is the picture itself; a photo, the coat of arms or the flag, or a map). */
inline constexpr char kImage[] = "schema:image";
/** The license of the picture. */
inline constexpr char kImageLicense[] = "schema:license";
/** Who made the picture, as its license asks to say. */
inline constexpr char kImageCredit[] = "schema:creditText";
/** The latitude in degrees (WGS 84), north is positive. */
inline constexpr char kLatitude[] = "schema:latitude";
/** The longitude in degrees (WGS 84), east is positive. */
inline constexpr char kLongitude[] = "schema:longitude";
/** The height above sea level in metres. */
inline constexpr char kElevation[] = "schema:elevation";
/** The south edge of the bounding box in degrees of latitude. The four edges are separate numbers so that the places in a region are found with a query. */
inline constexpr char kBoxSouth[] = "SEN:boxSouth";
/** The north edge of the bounding box in degrees of latitude. */
inline constexpr char kBoxNorth[] = "SEN:boxNorth";
/** The west edge of the bounding box in degrees of longitude. */
inline constexpr char kBoxWest[] = "SEN:boxWest";
/** The east edge of the bounding box in degrees of longitude. */
inline constexpr char kBoxEast[] = "SEN:boxEast";
/** The boundary of the place as Well-Known Text (POLYGON or MULTIPOLYGON, WGS 84, longitude first), simplified: it is meant to tell where places are in relation to each other, not to draw a map. */
inline constexpr char kGeometry[] = "geo:asWKT";
/** The postal address in one line. */
inline constexpr char kAddress[] = "schema:address";
inline constexpr char kPostalCode[] = "schema:postalCode";
/** The city, town or village. */
inline constexpr char kLocality[] = "schema:addressLocality";
/** The state or province. */
inline constexpr char kRegion[] = "schema:addressRegion";
/** The country, as its two-letter code (ISO 3166-1). */
inline constexpr char kCountry[] = "schema:addressCountry";
/** What there is to say about the relation (where in the place, how it is located). */
inline constexpr char kRelNote[] = "SEN:REL:note";
/** The code of the relation in the Region Connection Calculus (RCC-8), as the comparison of the shapes of the two places gave it: NTPP (within, not touching the border), NTPPi (contains), TPP (covered by, touching the border), TPPi (covers), EC (touch), PO (overlap), EQ (equal), DC (disconnected). The name in words is the property Kind. */
inline constexpr char kRcc8[] = "SEN:REL:Rcc8";
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
	{attr::kLatitude, 'DBLE'},
	{attr::kLongitude, 'DBLE'},
	{attr::kBoxSouth, 'DBLE'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace mime {
/** Place: a place: a named thing with an identity, a meaning or a function (a city, a building, a café, a mountain), the same place even when its boundary shifts or its coordinates are not known */
inline constexpr char kPlace[] = "entity/x-vnd.sen-labs.entity.place";
/** Location: a location: where something is on Earth (a point, a bounding box, a polygon, a postal address), without more identity than that: the position of a photo, a waypoint, a meeting point */
inline constexpr char kLocation[] = "entity/x-vnd.sen-labs.entity.location";
/** Located At: a file or an entity is located at a place or a location (where a photo was taken, what a paper is about, where an event is) */
inline constexpr char kLocatedAt[] = "relation/x-vnd.sen-labs.relation.spatial.located-at";
/** Within: the place lies inside the other one, without touching its border (RCC-8 NTPP); seen from the other one it contains the place */
inline constexpr char kWithin[] = "relation/x-vnd.sen-labs.relation.spatial.within";
/** Covered By: the place lies inside the other one and touches its border (RCC-8 TPP); seen from the other one it covers the place */
inline constexpr char kCoveredBy[] = "relation/x-vnd.sen-labs.relation.spatial.covered-by";
/** Touch: the borders of two places meet and the places do not overlap (RCC-8 EC), like two neighbouring countries */
inline constexpr char kTouch[] = "relation/x-vnd.sen-labs.relation.spatial.touch";
/** Overlap: two places have a part in common and each has a part of its own (RCC-8 PO), like an island in two districts */
inline constexpr char kOverlap[] = "relation/x-vnd.sen-labs.relation.spatial.overlap";
/** Equal: two places cover exactly the same area (RCC-8 EQ), like two files for the same parcel */
inline constexpr char kEqual[] = "relation/x-vnd.sen-labs.relation.spatial.equal";
}	// namespace mime

}	// namespace spatial
}	// namespace onto
}	// namespace sen
