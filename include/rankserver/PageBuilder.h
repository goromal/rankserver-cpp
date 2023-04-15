#pragma once

#include <optional>
#include <variant>

#include "third_party/HTML.h"

std::optional<std::variant<HTML::Bold, HTML::Image>> renderFile(const std::string& fname);

std::string buildRankserverPage(const bool&                     err,
                                const bool&                     done,
                                const std::string&              msg,
                                const std::vector<std::string>& rlist,
                                const std::string&              l,
                                const std::string&              r);
