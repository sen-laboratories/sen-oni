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
/** When the event starts (time:hasBeginning), as a time. For a fuzzy start (Start Date EDTF) it is the earliest moment of the value, so that the events sort. */
inline constexpr char kStartDate[] = "schema:startDate";
/** When the event ends (time:hasEnd), as a time. For a fuzzy end (End Date EDTF) it is the latest moment of the value. An event with a start and a duration has no end of its own. */
inline constexpr char kEndDate[] = "schema:endDate";
/** The start as it was said, when it is not exact: 1994? (the year is uncertain), 1994-05~ (about May 1994), 202X (some year of the 2020s). The text is in the Extended Date/Time Format (EDTF, ISO 8601-2). An exact start has none. */
inline constexpr char kStartDateEdtf[] = "SEN:startDateEdtf";
/** The end as it was said, when it is not exact (see Start (EDTF)). An exact end has none. */
inline constexpr char kEndDateEdtf[] = "SEN:endDateEdtf";
/** The length of the event, as an ISO 8601 duration (PT2H, PT1H55M, P3D); for an event that has a start and no end. */
inline constexpr char kDuration[] = "schema:duration";
/** The relation in Allen's interval algebra as the comparison of the two intervals gave it: before, after, meets, met by, overlaps, overlapped by, during, contains, equals, starts, started by, finishes, finished by. The name in words is the property Kind. */
inline constexpr char kAllen[] = "SEN:REL:Allen";
}	// namespace attr

namespace mime {
/** Event: an event: anything that occurs in time (a meeting, a trip, a task, a shooting). Its time is a start and an end, or a start and a duration; each can be exact or fuzzy. Any file can have these properties, the event is the entity that is about nothing else */
inline constexpr char kEvent[] = "entity/x-vnd.sen-labs.entity.event";
/** Before: the thing ends before the other one begins, with a gap between them (Allen's before); seen from the other one it comes after */
inline constexpr char kTemporalBefore[] = "relation/x-vnd.sen-labs.relation.temporal.before";
/** Meets: the thing ends at the very moment that the other one begins (Allen's meets), like two days in a row; seen from the other one it is met by it */
inline constexpr char kTemporalMeets[] = "relation/x-vnd.sen-labs.relation.temporal.meets";
/** Overlaps: the thing begins before the other one and ends inside it (Allen's overlaps), the usual collision of two appointments; seen from the other one it is overlapped by it */
inline constexpr char kTemporalOverlaps[] = "relation/x-vnd.sen-labs.relation.temporal.overlaps";
/** During: the thing lies inside the other one, beginning after it and ending before it (Allen's during); seen from the other one it contains the thing */
inline constexpr char kTemporalDuring[] = "relation/x-vnd.sen-labs.relation.temporal.during";
/** Equals: the thing has the same beginning and the same end as the other one (Allen's equals) */
inline constexpr char kTemporalEquals[] = "relation/x-vnd.sen-labs.relation.temporal.equals";
}	// namespace mime

}	// namespace temporal
}	// namespace onto
}	// namespace sen
