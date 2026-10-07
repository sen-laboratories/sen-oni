/*
 * Generated from schema/sen-music.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

/**
 * @file SenOntoMusic.h
 * @brief Attribute names and MIME types of the ontology "SEN Music": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace music {

namespace attr {
}	// namespace attr

namespace mime {
/** Music work: a musical work or piece of music (general) */
inline constexpr char kMusicWork[] = "entity/x-vnd.sen-labs.entity.music.work";
/** Song: an individual piece of music */
inline constexpr char kSong[] = "entity/x-vnd.sen-labs.entity.music.song";
/** Movement: a movement of a musical work */
inline constexpr char kMovement[] = "entity/x-vnd.sen-labs.entity.music.movement";
/** Music album: a music album containing songs or pieces */
inline constexpr char kMusicAlbum[] = "entity/x-vnd.sen-labs.entity.music.album";
/** Music medium: a physical music medium (CD,DVD,Bluray etc.) */
inline constexpr char kMusicMedium[] = "entity/x-vnd.sen-labs.entity.music.medium";
/** Music transition to: suitable as a transition from this song. */
inline constexpr char kTransitionTo[] = "relation/x-vnd.sen-labs.relation.music.transition.to";
/** Music transition from: suitable as a transition to this song */
inline constexpr char kTransitionFrom[] = "relation/x-vnd.sen-labs.relation.music.transition.from";
}	// namespace mime

}	// namespace music
}	// namespace onto
}	// namespace sen
