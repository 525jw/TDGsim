#pragma once

#include "common.hpp"

// Reads company orders from a JSON file and filters them by side and recipient set.
// - path: file path to JSON containing an "orders" array
// - sideFilter: only accept orders whose "who.side" matches this side (if present)
// - allowedRecipients: if non-empty, only include orders for entity ids in this set
CompanyOrd LoadOrderFromFile(const std::string& path,
                             SideType sideFilter,
                             const std::unordered_set<int>& allowedRecipients);

