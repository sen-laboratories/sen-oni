/*
 * Generated from schema/sen-people.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoPeople.h
 * @brief Attribute names and MIME types of the ontology "SEN People": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace people {

namespace attr {
/** The full name (of a person or an organization; the property is called Full Name so that it is not taken for the name of the file, which a file manager shows as Name). */
inline constexpr char kName[] = "foaf:name";
inline constexpr char kGivenName[] = "foaf:givenName";
inline constexpr char kFamilyName[] = "foaf:familyName";
/** The nickname of the person, the short name it uses on the net and for a login (FOAF says that a nickname covers those, so does a handle); with or without the `@`. A Fediverse handle is `user@server`, one of Bluesky a domain (`alice.bsky.social`, `sen-labs.org`), one of GitHub the login. */
inline constexpr char kNickname[] = "foaf:nick";
/** The e-mail address. */
inline constexpr char kMbox[] = "foaf:mbox";
inline constexpr char kPhone[] = "foaf:phone";
inline constexpr char kHomepage[] = "foaf:homepage";
inline constexpr char kBirthDate[] = "schema:birthDate";
inline constexpr char kJobTitle[] = "schema:jobTitle";
inline constexpr char kDescription[] = "dc:description";
/** Where the person lives or says to be (the place as the person writes it). */
inline constexpr char kLocation[] = "schema:homeLocation";
/** The company or organization that the person works for. */
inline constexpr char kWorksFor[] = "schema:worksFor";
/** The ORCID iD of a researcher (0000-0002-1825-0097). */
inline constexpr char kOrcid[] = "vivo:orcidId";
/** Where else the person is known (ORCID, Wikidata, OpenAlex...), as a list of IRIs. */
inline constexpr char kSameAs[] = "schema:sameAs";
/** The ROR id of the institution (Research Organization Registry, ror.org), bare (04d836q62). It is indexed, so that the institution that a paper names is found on the computer; OpenAlex and the other identifiers (Wikidata, ISNI, GRID) are Same As. */
inline constexpr char kRor[] = "dc:identifier";
/** The other names of the institution (the acronym, names in other languages, older names), as a list. */
inline constexpr char kAlternateName[] = "schema:alternateName";
/** What kind of institution it is, as the Research Organization Registry and OpenAlex say (the registry can name several: a university that funds research is Education and Funder), as a list. */
inline constexpr char kInstitutionType[] = "SEN:institutionType";
/** The city, town or village that the institution is in. */
inline constexpr char kLocality[] = "schema:addressLocality";
/** The country, as its two-letter code (ISO 3166-1). */
inline constexpr char kCountry[] = "schema:addressCountry";
/** The topics that the institution works on most (OpenAlex), the most frequent first, as a list. */
inline constexpr char kSubject[] = "dc:subject";
/** How many works OpenAlex has of the institution, when it was enriched. */
inline constexpr char kWorksCount[] = "SEN:worksCount";
/** How often the works of the institution are cited, when it was enriched. */
inline constexpr char kCitationCount[] = "SEN:citationCount";
/** The page of the photo of the person (the thumbnail is the photo itself). */
inline constexpr char kImage[] = "schema:image";
/** The license of the photo. */
inline constexpr char kPhotoLicense[] = "schema:license";
/** Who made the photo, as its license asks to say. */
inline constexpr char kPhotoCredit[] = "schema:creditText";
/** The groups that a person is in (Family, Friends, ...), as a list. */
inline constexpr char kGroups[] = "vcard:category";
/** The latest status message or microfeed note of the person. */
inline constexpr char kStatus[] = "foaf:status";
/** When the latest status message was posted. */
inline constexpr char kStatusDate[] = "SEN:statusDate";
/** The feed of the person's posts (RSS, Atom, the outbox of an ActivityPub actor or the RSS feed of a Bluesky profile). foaf:weblog is "a weblog of some thing". */
inline constexpr char kFeed[] = "foaf:weblog";
inline constexpr char kBeName[] = "META:name";
inline constexpr char kBeCompany[] = "META:company";
inline constexpr char kBeAddress[] = "META:address";
inline constexpr char kBeCity[] = "META:city";
inline constexpr char kBeState[] = "META:state";
inline constexpr char kBeZip[] = "META:zip";
inline constexpr char kBeCountry[] = "META:country";
inline constexpr char kBeEmail[] = "META:email";
inline constexpr char kBeHphone[] = "META:hphone";
inline constexpr char kBeMphone[] = "META:mphone";
inline constexpr char kBeWphone[] = "META:wphone";
inline constexpr char kBeFax[] = "META:fax";
inline constexpr char kBeUrl[] = "META:url";
inline constexpr char kBeGroup[] = "META:group";
inline constexpr char kBeNickname[] = "META:nickname";
/** When the relationship began. */
inline constexpr char kRelSince[] = "SEN:REL:since";
/** What there is to say about the relationship. */
inline constexpr char kRelNote[] = "SEN:REL:note";
/** When the relationship ended; empty while it goes on. */
inline constexpr char kRelUntil[] = "SEN:REL:until";
/** How many works of the institution are about the topic (OpenAlex), which says how much it is a focus of the institution. */
inline constexpr char kRelWorksCount[] = "SEN:REL:worksCount";
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
	{attr::kRor, 'CSTR'},
};
inline constexpr unsigned kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);

#pragma GCC diagnostic pop

namespace values {
/** A common value: the text that is stored, and the Wikidata item that it stands for (empty if none). */
struct Value {
	const char* text;
	const char* wikidata;
};
/** The kinds of institutions of the Research Organization Registry (ROR), which OpenAlex uses, too. */
inline constexpr Value kInstitutionType[] = {
	{"Education", ""},
	{"Facility", ""},
	{"Government", ""},
	{"Nonprofit", ""},
	{"Company", ""},
	{"Healthcare", ""},
	{"Funder", ""},
	{"Archive", ""},
	{"Other", ""},
};
inline constexpr unsigned kInstitutionTypeCount = sizeof(kInstitutionType) / sizeof(kInstitutionType[0]);
}	// namespace values

namespace mime {
/** Person: a person */
inline constexpr char kPerson[] = "entity/x-vnd.sen-labs.entity.person";
/** Person: a person of the People application of Haiku */
inline constexpr char kPeoplePerson[] = "application/x-person";
/** Organization: a company, club or any other group of people with a name (a university or an institute is an Institution) */
inline constexpr char kOrganization[] = "entity/x-vnd.sen-labs.entity.organization";
/** Institution: a university, research institute, laboratory, library, museum or any other institution of research, education and culture: an organization, kept apart for the research use (the affiliation of an author, the publisher of a work, the home of a collection). It is what the Research Organization Registry (ROR) and OpenAlex call an institution. */
inline constexpr char kInstitution[] = "entity/x-vnd.sen-labs.entity.institution";
/** Personal Relation: what people are to one another, in no particular way ("knows"), the relation that all the other kinds of personal relations are special cases of */
inline constexpr char kPersonalRelation[] = "relation/x-vnd.sen-labs.relation.person";
/** Acquaintance: people who have met and know each other a little */
inline constexpr char kAcquaintance[] = "relation/x-vnd.sen-labs.relation.person.acquaintance";
/** Friend: people who like and trust each other */
inline constexpr char kFriend[] = "relation/x-vnd.sen-labs.relation.person.friend";
/** Close Friend: people who are very close, who share what matters to them */
inline constexpr char kCloseFriend[] = "relation/x-vnd.sen-labs.relation.person.close-friend";
/** Partner: people who live their lives together (life partners, spouses) */
inline constexpr char kPartner[] = "relation/x-vnd.sen-labs.relation.person.partner";
/** Parent: a person is the parent of another one */
inline constexpr char kParent[] = "relation/x-vnd.sen-labs.relation.person.parent";
/** Sibling: people who have a parent in common */
inline constexpr char kSibling[] = "relation/x-vnd.sen-labs.relation.person.sibling";
/** Relative: people who are related by blood or marriage, in another way than parent, child or sibling */
inline constexpr char kRelative[] = "relation/x-vnd.sen-labs.relation.person.relative";
/** Colleague: people who work in the same organization, or in the same field, and none is the superior of the other */
inline constexpr char kColleague[] = "relation/x-vnd.sen-labs.relation.person.colleague";
/** Superior: a person is the superior (manager, supervisor, lead) of another one */
inline constexpr char kSuperior[] = "relation/x-vnd.sen-labs.relation.person.superior";
/** Mentor: a person guides the professional or personal growth of another one */
inline constexpr char kMentor[] = "relation/x-vnd.sen-labs.relation.person.mentor";
/** Collaborator: people who work together on something, without one of them being the superior of the other */
inline constexpr char kCollaborator[] = "relation/x-vnd.sen-labs.relation.person.collaborator";
/** Client: a person is the client (customer, patient, student) of another one */
inline constexpr char kClient[] = "relation/x-vnd.sen-labs.relation.person.client";
/** Employment: a person works for an organization */
inline constexpr char kEmployment[] = "relation/x-vnd.sen-labs.relation.person.employment";
/** Membership: a person is a member of an organization or group */
inline constexpr char kMembership[] = "relation/x-vnd.sen-labs.relation.person.membership";
/** Affiliation: a person is affiliated with an institution for a time (the institution that a researcher names on a paper, a position, a visit): since when and until when (empty while it goes on), and in which role (Postdoctoral Researcher, Professor, Visiting Scholar). Employment is the job, Membership the belonging; this is what a paper says */
inline constexpr char kAffiliation[] = "relation/x-vnd.sen-labs.relation.person.affiliation";
/** Part Of: an institution is a part of a larger one (a faculty, a department, an institute or a laboratory of a university) */
inline constexpr char kPartOfInstitution[] = "relation/x-vnd.sen-labs.relation.institution.part-of";
/** Research Focus: a topic that an institution works on; the number of works says how much (the topics of an institution in OpenAlex, the most frequent first) */
inline constexpr char kResearchFocus[] = "relation/x-vnd.sen-labs.relation.institution.research-focus";
/** Status: recent status messages and microfeed posts of a person */
inline constexpr char kStatusRelation[] = "relation/x-vnd.sen-labs.relation.person.status";
}	// namespace mime

}	// namespace people
}	// namespace onto
}	// namespace sen
