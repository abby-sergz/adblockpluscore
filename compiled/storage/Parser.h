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

#pragma once
#include "../subscription/Subscription.h"

class Parser : public ref_counted
{
  enum class State
  {
    Initial, SubscriptionSection, SubscriptionFiltersSection
  };
  ~Parser() {}
public:
  typedef std::vector<SubscriptionPtr> Subscriptions;
  Parser();
  static Parser* BINDINGS_EXPORTED Create()
  {
    return new Parser();
  }
  void BINDINGS_EXPORTED Process(const String& untrimmedLine);
  void BINDINGS_EXPORTED Finalize();

  Subscriptions::size_type BINDINGS_EXPORTED GetSubscriptionCount() const;
  Subscription* BINDINGS_EXPORTED SubscriptionAt(Subscriptions::size_type index);
private:
  SubscriptionPtr createSubscriptionFromProperties() const;

  void Initial_processLine(const String& line);
  void SubscriptionSection_processLine(const String& line);
  void SubscriptionFiltersSection_processLine(const String& line);

  void onSubscription(SubscriptionPtr subscription);

  void onFail(const char* error)
  {
  }
private:
  // parser members:
  State mCurrentState;
  Subscriptions mSubscriptions;

  // State memebers:
  // Initial State
  // it's also the parser member
  KeyValues mFileProperties;
  // [Subscription] section
  KeyValues mSubscriptionProperties;
  // [Subscription filters] section
  SubscriptionPtr mSubscription;
};
