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
/** The full name (of a person or an organization). */
inline constexpr char kName[] = "foaf:name";
inline constexpr char kGivenName[] = "foaf:givenName";
inline constexpr char kFamilyName[] = "foaf:familyName";
inline constexpr char kNickname[] = "foaf:nick";
/** The e-mail address. */
inline constexpr char kMbox[] = "foaf:mbox";
inline constexpr char kPhone[] = "foaf:phone";
inline constexpr char kHomepage[] = "foaf:homepage";
inline constexpr char kBirthDate[] = "schema:birthDate";
inline constexpr char kJobTitle[] = "schema:jobTitle";
inline constexpr char kDescription[] = "dc:description";
/** Where else the person is known (ORCID, Wikidata, OpenAlex...), as a list of IRIs. */
inline constexpr char kSameAs[] = "schema:sameAs";
/** The page of the photo of the person (the thumbnail is the photo itself). */
inline constexpr char kImage[] = "schema:image";
/** The license of the photo. */
inline constexpr char kPhotoLicense[] = "schema:license";
/** Who made the photo, as its license asks to say. */
inline constexpr char kPhotoCredit[] = "schema:creditText";
/** The groups that a person is in (Family, Friends, ...), as a list. */
inline constexpr char kGroups[] = "vcard:category";
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
}	// namespace attr

namespace mime {
/** Person: a person */
inline constexpr char kPerson[] = "entity/x-vnd.sen-labs.entity.person";
/** Person: a person of the People application of Haiku */
inline constexpr char kPeoplePerson[] = "application/x-person";
/** Organization: a company, institution, club or any other group of people with a name */
inline constexpr char kOrganization[] = "entity/x-vnd.sen-labs.entity.organization";
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
}	// namespace mime

}	// namespace people
}	// namespace onto
}	// namespace sen
