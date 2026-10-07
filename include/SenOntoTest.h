/*
 * Generated from schema/sen-test.yaml by oni_gen.py: do not edit.
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024-2026 SEN Labs e.U.
 */
#pragma once

#include <stdint.h>

/**
 * @file SenOntoTest.h
 * @brief Attribute names and MIME types of the ontology "SEN Test": use these instead of string literals.
 */
namespace sen {
namespace onto {
namespace test {

namespace attr {
}	// namespace attr

namespace mime {
/** SEN: a custom supertype for testing */
inline constexpr char kSenTestSupertype[] = "sen";
/** Test: a custom test type */
inline constexpr char kSenTest[] = "sen/x-vnd.sen-labs.sen.test";
}	// namespace mime

}	// namespace test
}	// namespace onto
}	// namespace sen
