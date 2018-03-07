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

#include <cstdio>
#include <cstdlib>
#include <random>

#include "Subscription.h"
#include "DownloadableSubscription.h"
#include "UserDefinedSubscription.h"
#include "../StringMap.h"

ABP_NS_USING

namespace
{
  StringMap<Subscription*> knownSubscriptions(16);
}

Subscription::Subscription(Type type, const String& id, const KeyValues& properties)
    : mID(id), mType(type), mDisabled(false), mListed(false)
{
  annotate_address(this, "Subscription");
  parseProperty(properties, mTitle, u"title"_str);
  parseProperty(properties, mDisabled, u"disabled"_str);
}

Subscription::~Subscription()
{
  knownSubscriptions.erase(mID);
}

Filter* Subscription::FilterAt(Subscription::Filters::size_type index)
{
  if (index >= mFilters.size())
    return nullptr;

  FilterPtr result(mFilters[index]);
  return result.release();
}

int Subscription::IndexOfFilter(const Filter& filter)
{
  for (Filters::size_type i = 0; i < mFilters.size(); i++)
    if (mFilters[i] == &filter)
      return i;
  return -1;
}

void Subscription::AddFilter(Filter& filter)
{
  mFilters.emplace_back(&filter);
}

OwnedString Subscription::SerializeProperties() const
{
  if (const auto* downcastedSubscription = As<DownloadableSubscription>())
    return downcastedSubscription->SerializeProperties();
  else if (const auto* downcastedSubscription = As<UserDefinedSubscription>())
    return downcastedSubscription->SerializeProperties();

  return DoSerializeProperties();
}


OwnedString Subscription::DoSerializeProperties() const
{
  OwnedString result(u"url="_str);
  result.append(mID);
  result.append(u'\n');
  if (!mTitle.empty())
  {
    result.append(u"title="_str);
    result.append(mTitle);
    result.append(u'\n');
  }
  if (mDisabled)
    result.append(u"disabled=true\n"_str);

  return result;
}

SubscriptionPtr Subscription::FromProperties(const String& id, const KeyValues& keyValues)
{
  if (id.empty())
  {
    // Generate a new random ID
    std::mt19937 gen(knownSubscriptions.size());
    OwnedString randomID(u"~user~000000"_str);
    do
    {
      int number = gen();
      for (String::size_type i = randomID.length() - 6; i < randomID.length(); i++)
      {
        randomID[i] = '0' + (number % 10);
        number /= 10;
      }
    } while (knownSubscriptions.find(randomID));
    return FromProperties(randomID, keyValues);
  }

  auto knownSubscription = knownSubscriptions.find(id);
  if (knownSubscription)
    return SubscriptionPtr(knownSubscription->second);

  SubscriptionPtr subscription;
  if (id[0] == '~')
    subscription.reset(new UserDefinedSubscription(id, keyValues), false);
  else
    subscription.reset(new DownloadableSubscription(id, keyValues), false);

  // This is a hack: we looked up the entry using id but create it using
  // subscription->mID. This works because both are equal at this point.
  // However, id refers to a temporary buffer which will go away.
  enter_context("Adding to known subscriptions");
  knownSubscription.assign(subscription->mID, subscription.get());
  exit_context();

  return subscription;
}

SubscriptionPtr Subscription::FromProperties(const KeyValues& properties)
{
  const String* id = findPropertyValue(properties, u"url"_str);
  if (id == nullptr || id->empty())
    return SubscriptionPtr();
  return FromProperties(*id, properties);
}

const String* Subscription::findPropertyValue(const KeyValues& properties, const String& propertyName)
{
  for (const auto& property : properties)
    if (property.first == propertyName)
      return &property.second;
  return nullptr;
}
