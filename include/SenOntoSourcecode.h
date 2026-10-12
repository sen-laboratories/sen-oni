/*
 * Generated from schema/sen-sourcecode.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoSourcecode.h
 * @brief Attribute names and MIME types of the ontology "SEN Source Code": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace sourcecode {

namespace attr {
/** The path of the include as written in the source. */
inline constexpr char kIncludePath[] = "SEN:REL:includePath";
/** A global (system) include (angle brackets), not a local one. */
inline constexpr char kIncludeGlobal[] = "SEN:REL:includeGlobal";
}	// namespace attr

namespace mime {
/** Source Include: include dependency to another sourcecode */
inline constexpr char kSourceInclude[] = "relation/x-vnd.sen-labs.relation.sourcecode.include";
/** Source Code: Source code, the type that Haiku gives to the files of all programming languages. The list of extensions is the one of the system (as dumped from the MIME database) plus those of the languages that the source code plugin of SENSEI understands, so that Tracker types such files as source code and SEN can examine them. The language itself is the semantic type, the attribute META:TYPE. */
inline constexpr char kSourceCodeFile[] = "text/x-source-code";
}	// namespace mime

}	// namespace sourcecode
}	// namespace onto
}	// namespace sen
