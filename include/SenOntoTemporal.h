/*
 * Generated from schema/sen-temporal.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoTemporal.h
 * @brief Attribute names and MIME types of the ontology "SEN Temporal": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace temporal {

namespace attr {
/** The name of the event (the property is called Title so that it is not taken for the name of the file, which a file manager shows as Name). */
inline constexpr char kTitle[] = "dc:title";
/** What there is to say about the event. */
inline constexpr char kDescription[] = "dc:description";
/** When the event starts (time:hasBeginning), as a time. For a fuzzy start (Start Date EDTF) it is the earliest moment of the value, so that the events sort and are found with a query. */
inline constexpr char kStartDate[] = "schema:startDate";
/** When the event ends (time:hasEnd), as a time. For a fuzzy end (End Date EDTF) it is the latest moment of the value. An event with a start and a duration has no end of its own. */
inline constexpr char kEndDate[] = "schema:endDate";
/** The start as it was said, when it is not exact: 1994? (the year is uncertain), 1994-05~ (about May 1994), 202X (some year of the 2020s). The text is in the Extended Date/Time Format (EDTF, ISO 8601-2). An exact start has none. */
inline constexpr char kStartDateEdtf[] = "SEN:startDateEdtf";
/** The end as it was said, when it is not exact (see Start (EDTF)). An exact end has none. */
inline constexpr char kEndDateEdtf[] = "SEN:endDateEdtf";
/** The length of the event, as an ISO 8601 duration (PT2H, PT1H55M, P3D); for an event that has a start and no end. */
inline constexpr char kDuration[] = "schema:duration";
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
	{attr::kStartDate, 'TIME'},
	{attr::kEndDate, 'TIME'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace mime {
/** Event: an event: anything that occurs in time (a meeting, a trip, a task, a shooting). Its time is a start and an end, or a start and a duration; each can be exact or fuzzy. Any file can have these properties, the event is the entity that is about nothing else */
inline constexpr char kEvent[] = "entity/x-vnd.sen-labs.entity.event";
}	// namespace mime

}	// namespace temporal
}	// namespace onto
}	// namespace sen
