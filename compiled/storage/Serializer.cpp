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
 * along with Adblock Plus. If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>
#include "Serializer.h"
#include "../subscription/Subscription.h"

namespace
{
  // escape [, if nothing is escaped then `second` is false.
  std::pair<OwnedString, bool> EscapeOpeningBracket(const String& value)
  {
    auto valueLength = value.length();
    // there are only few filters with [, so it's faster to not copy the value
    // but quickly scan firstly whether it makes sense to create a new string.
    int bracketsCounter = 0;
    for (String::size_type i = 0; i < valueLength; ++i)
    {
      if (value[i] == u'[')
        ++bracketsCounter;
    }
    if (bracketsCounter == 0)
      return std::make_pair(OwnedString(), false);

    OwnedString escapedString(valueLength + bracketsCounter);
    String::size_type writeOffset = 0;
    for (String::size_type i = 0; i < valueLength; ++i)
    {
      if (value[i] == u'[')
        escapedString[i + writeOffset++] = u'\\';
      escapedString[i + writeOffset] = value[i];
    }
    return std::make_pair(std::move(escapedString), true);
  }
}

Serializer::Serializer()
{
  mStream.append(u"# Adblock Plus preferences\nversion=5\n"_str);
}

void Serializer::Serialize(const Subscription& subscription)
{
  mStream.append(u"[Subscription]\n"_str);
  mStream.append(subscription.SerializeProperties());

  const auto& filters = subscription.GetFilters();
  if (!filters.empty())
  {
    mStream.append(u"[Subscription filters]\n"_str);
    for (const auto& filter : filters)
    {
      auto escapedResult = EscapeOpeningBracket(filter->GetText());
      mStream.append(escapedResult.second ? escapedResult.first : filter->GetText());
      mStream.append(u'\n');
    }
  }
}