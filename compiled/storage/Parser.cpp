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

#include "Parser.h"
#include "../subscription/DownloadableSubscription.h"
#include "../subscription/UserDefinedSubscription.h"
#include "../filter/Filter.h"

namespace
{
  bool IsSection(const String& value, const String& expectedSectionName)
  {
    auto valueLength = value.length();
    // fast check whether it's a section and if its length is expected
    if (!(valueLength == expectedSectionName.length() + 2 && value[0] == u'[' && value[valueLength - 1] == u']'))
      return false;
    OwnedString sectionName(DependentString(value, 1, valueLength - 2));
    sectionName.toLower();
    return sectionName == expectedSectionName;
  }

  bool IsSubscriptionSection(const String& value)
  {
    return IsSection(value, u"subscription"_str);
  }

  bool IsSubscriptionFiltersSection(const String& value)
  {
    return ::IsSection(value, u"subscription filters"_str);
  }

  Subscription::KeyValue CreateKeyValue(const std::pair<DependentString, DependentString>& pair)
  {
    Subscription::KeyValue retValue;
    retValue.first = TrimSpaces(pair.first);
    retValue.second = TrimSpaces(pair.second);
    return retValue;
  }

  void DecodeOpeningBracket(DependentString& value)
  {
    String::size_type skippedChars = 0;
    for (String::size_type i = 1; i < value.length(); ++i)
    {
      if (value[i - 1] == u'\\' && value[i] == u'[')
        ++skippedChars;
      value[i - skippedChars] = value[i];
    }
    if (skippedChars > 0)
      value.reset(value, 0, value.length() - skippedChars);
  }
}

Parser::Parser()
  : mCurrentState{State::Initial}
{
}

void Parser::Process(const String& untrimmedLine)
{
  auto line = TrimSpaces(untrimmedLine);
  // skip empty lines
  if (line.length() == 0)
    return;

  if (IsSubscriptionSection(line))
  {
    Finalize();
    mCurrentState = State::SubscriptionSection;
    return;
  }

  switch (mCurrentState)
  {
  case State::Initial:
    Initial_processLine(line);
    break;
  case State::SubscriptionSection:
    SubscriptionSection_processLine(line);
    break;
  case State::SubscriptionFiltersSection:
    SubscriptionFiltersSection_processLine(line);
    break;
  default:
    ;
  }
}

void Parser::Finalize()
{
  switch (mCurrentState)
  {
  case State::Initial:
    // don't clear file properties because they are also the parser member
    break;
  case State::SubscriptionSection:
    onSubscription(createSubscriptionFromProperties());
    mSubscriptionProperties.clear();
    break;
  case State::SubscriptionFiltersSection:
    onSubscription(std::move(mSubscription));
    break;
  default:
    ;
  }
}

Parser::Subscriptions::size_type Parser::GetSubscriptionCount() const
{
  return mSubscriptions.size();
}

Subscription* Parser::SubscriptionAt(Subscriptions::size_type index)
{
  if (index >= mSubscriptions.size())
    return nullptr;
  return SubscriptionPtr(mSubscriptions[index]).release();
}

SubscriptionPtr Parser::createSubscriptionFromProperties() const
{
  return Subscription::FromProperties(mSubscriptionProperties);
}

void Parser::Initial_processLine(const String& line)
{
  // skip # Adblock Plus preferences
  if (line[0] == u'#')
    return;

  String::size_type assignSignPos = line.find(u'=');
  if (assignSignPos != String::npos)
  {
    mFileProperties.emplace_back(CreateKeyValue(SplitString(line, assignSignPos)));
    return;
  }
  onFail("Unexpected line value, it should be either a file property or the [Subscription] section");
}

void Parser::SubscriptionSection_processLine(const String& line)
{
  String::size_type assignSignPos = line.find(u'=');
  if (assignSignPos != String::npos)
  {
    mSubscriptionProperties.emplace_back(CreateKeyValue(SplitString(line, assignSignPos)));
    return;
  }
  if (IsSubscriptionFiltersSection(line))
  {
    mSubscription = createSubscriptionFromProperties();
    mCurrentState = State::SubscriptionFiltersSection;
    return;
  }
  onFail("Unexpected line value, it should be either a subscription property, the [Subscription filters] section or the [Subscription] section");
}

void Parser::SubscriptionFiltersSection_processLine(const String& line)
{
  OwnedString lineCopy{line};
  DependentString lineForFilter{lineCopy};
  DecodeOpeningBracket(lineForFilter);
  mSubscription->AddFilter(*FilterPtr(Filter::FromText(lineForFilter)));
  // any line is considered as a filter, there can be no error
}

void Parser::onSubscription(SubscriptionPtr subscription)
{
  mSubscriptions.emplace_back(std::move(subscription));
}
