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
/** What kind of event it is, in a word or two: a birthday, a wedding, a meeting, a phone call. The common kinds are offered (Event Type values, mapped to Wikidata); any other text is allowed. */
inline constexpr char kEventType[] = "SEN:eventType";
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
/** The identifier that the calendar file gives the event (UID), the same in every copy of an invitation and every instance of a series. */
inline constexpr char kUid[] = "dc:identifier";
/** Where the event takes place, as the calendar file says it (an address, a room, a name). The place itself is the relation Location. */
inline constexpr char kLocation[] = "schema:location";
/** Who organizes the event, as Name <address>. The people of an event are the relation Attendee. */
inline constexpr char kOrganizer[] = "schema:organizer";
/** Whether the event is going ahead, as the calendar file says it (STATUS). */
inline constexpr char kEventStatus[] = "schema:eventStatus";
/** Whether the event takes place at a place, online, or both. */
inline constexpr char kAttendanceMode[] = "schema:eventAttendanceMode";
/** The web page of the event (URL of the calendar file). */
inline constexpr char kUrl[] = "schema:url";
/** The link to join an online meeting that the event has, found in the conference properties, the location or the description of the calendar file. */
inline constexpr char kMeetingUrl[] = "SEN:meetingUrl";
/** The service of the online meeting (Zoom, Google Meet, Microsoft Teams...), worked out from its link, so that events can be shown, sorted and searched by it. */
inline constexpr char kMeetingType[] = "SEN:meetingType";
/** How the event repeats, as the recurrence rule of the calendar file (FREQ=WEEKLY;BYDAY=MO). An event that does not repeat has none. */
inline constexpr char kRecurrence[] = "SEN:recurrence";
/** The answer of the person to the invitation, as the calendar file says it (accepted, declined, tentative, needs action, delegated). It belongs to the Attendee relation between the event and the person, not to the person. */
inline constexpr char kResponse[] = "SEN:REL:Response";
/** Which instance of a series the relation is about, as the RECURRENCE-ID of the calendar file (20261123T090000). */
inline constexpr char kRecurrenceId[] = "SEN:REL:RecurrenceId";
/** The relation in Allen's interval algebra as the comparison of the two intervals gave it: before, after, meets, met by, overlaps, overlapped by, during, contains, equals, starts, started by, finishes, finished by. The name in words is the property Kind. */
inline constexpr char kAllen[] = "SEN:REL:Allen";
}	// namespace attr

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"

/** An attribute that is queried and needs a BFS index on every volume (the type of the index is that of the attribute, a time is indexed as the integer that holds it: sen::IndexTypeOf() of SenAttributes.h). */
struct Index {
	const char* name;
	uint32_t    type;
};
/** The indices of this ontology: created by the ontology installer and by the SEN server on every mounted volume. */
inline constexpr Index kIndices[] = {
	{attr::kStartDate, 'TIME'},
	{attr::kEndDate, 'TIME'},
	{attr::kUid, 'CSTR'},
	{attr::kEventStatus, 'CSTR'},
	{attr::kMeetingType, 'CSTR'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace values {
/** A common value: the text that is stored, and the Wikidata item that it stands for (empty if none). */
struct Value {
	const char* text;
	const char* wikidata;
};
/** The common kinds of events, mapped to Wikidata. They are offered to choose from; the type of an event is text, and any other value is allowed. */
inline constexpr Value kEventType[] = {
	{"Anniversary", "Q238907"},
	{"Ceremony", "Q2659904"},
	{"Party", "Q200538"},
	{"Birthday", "Q47223"},
	{"Wedding anniversary", "Q2005510"},
	{"Jubilee", "Q1908864"},
	{"Death anniversary", "Q1208945"},
	{"Wedding", "Q119639"},
	{"Funeral", "Q201559"},
	{"Graduation", "Q1928923"},
	{"Baptism", "Q35856"},
	{"Dinner party", "Q1753443"},
	{"Housewarming", "Q1206109"},
	{"Farewell party", "Q1390022"},
	{"Baby shower", "Q797880"},
	{"Meeting", "Q2761147"},
	{"Conference", "Q114834437"},
	{"Telephone call", "Q2296401"},
	{"Video conference", "Q238354"},
};
inline constexpr unsigned kEventTypeCount = sizeof(kEventType) / sizeof(kEventType[0]);
/** Whether an event is going ahead. */
inline constexpr Value kEventStatus[] = {
	{"Confirmed", ""},
	{"Tentative", ""},
	{"Cancelled", ""},
};
inline constexpr unsigned kEventStatusCount = sizeof(kEventStatus) / sizeof(kEventStatus[0]);
/** How people take part in an event. */
inline constexpr Value kAttendanceMode[] = {
	{"Offline", ""},
	{"Online", ""},
	{"Mixed", ""},
};
inline constexpr unsigned kAttendanceModeCount = sizeof(kAttendanceMode) / sizeof(kAttendanceMode[0]);
/** The services of online meetings that are known by their links. Any other service can be written in. */
inline constexpr Value kMeetingType[] = {
	{"Zoom", ""},
	{"Google Meet", ""},
	{"Microsoft Teams", ""},
	{"Webex", ""},
	{"Jitsi", ""},
	{"GoToMeeting", ""},
	{"Whereby", ""},
	{"Skype", ""},
	{"Other", ""},
};
inline constexpr unsigned kMeetingTypeCount = sizeof(kMeetingType) / sizeof(kMeetingType[0]);
/** The answer of a person to an invitation. */
inline constexpr Value kResponseStatus[] = {
	{"Accepted", ""},
	{"Declined", ""},
	{"Tentative", ""},
	{"Needs action", ""},
	{"Delegated", ""},
};
inline constexpr unsigned kResponseStatusCount = sizeof(kResponseStatus) / sizeof(kResponseStatus[0]);
}	// namespace values

namespace mime {
/** Event: an event: anything that occurs in time (a meeting, a trip, a task, a shooting). Its time is a start and an end, or a start and a duration; each can be exact or fuzzy. Any file can have these properties, the event is the entity that is about nothing else */
inline constexpr char kEvent[] = "entity/x-vnd.sen-labs.entity.event";
/** Between: the thing ends before the other one begins, with a gap between them (Allen's before); seen from the other one it comes after. The relation is called Between, the directions before and after: one gap lies between the two */
inline constexpr char kTemporalBetween[] = "relation/x-vnd.sen-labs.relation.temporal.between";
/** Meets: the thing ends at the very moment that the other one begins (Allen's meets), like two days in a row; seen from the other one it is met by it */
inline constexpr char kTemporalMeets[] = "relation/x-vnd.sen-labs.relation.temporal.meets";
/** Overlaps: the thing begins before the other one and ends inside it (Allen's overlaps), the usual collision of two appointments; seen from the other one it is overlapped by it */
inline constexpr char kTemporalOverlaps[] = "relation/x-vnd.sen-labs.relation.temporal.overlaps";
/** During: the thing lies inside the other one, beginning after it and ending before it (Allen's during); seen from the other one it contains the thing */
inline constexpr char kTemporalDuring[] = "relation/x-vnd.sen-labs.relation.temporal.during";
/** Equals: the thing has the same beginning and the same end as the other one (Allen's equals) */
inline constexpr char kTemporalEquals[] = "relation/x-vnd.sen-labs.relation.temporal.equals";
/** Attendee: a person who is invited to the event or organizes it, as the calendar file says; the role is the Role of the relation (organizer, required, optional, chair, resource) and the answer to the invitation its Response (accepted, declined, tentative, needs action, delegated). Seen from the person, the person attends the event */
inline constexpr char kEventAttendee[] = "relation/x-vnd.sen-labs.relation.event.attendee";
/** Takes place at: the place where the event takes place, as the calendar file says (its location text, with the coordinates if it has them); seen from the place, it hosts the event */
inline constexpr char kEventLocation[] = "relation/x-vnd.sen-labs.relation.event.location";
/** Series: the instance of a repeating event just before or after this one, worked out from its recurrence rule (with the exceptions and moved instances of the file). The instances are places in the same calendar file, as the pages of a document are; each has its title, start and end */
inline constexpr char kEventSeries[] = "relation/x-vnd.sen-labs.relation.event.series";
}	// namespace mime

}	// namespace temporal
}	// namespace onto
}	// namespace sen
