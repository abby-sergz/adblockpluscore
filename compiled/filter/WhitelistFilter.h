/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-present eyeo GmbH
 *
 * Adblock Plus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Adblock Plus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Adblock Plus.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../base.h"
#include "RegExpFilter.h"

ABP_NS_BEGIN

class WhitelistFilter : public RegExpFilter
{
public:
  static constexpr Type classType = Type::WHITELIST;
  explicit WhitelistFilter(const String& text, const RegExpFilterData& data);
};

typedef intrusive_ptr<WhitelistFilter> WhitelistFilterPtr;

ABP_NS_END