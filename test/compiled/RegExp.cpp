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

#include <gtest/gtest.h>
#include "compiled/String.h"

ABP_NS_USING

TEST(TestRegexp, RegExp)
{
  auto id = GenerateRegExp(ABP_TEXT("[0-9]*"_str), false);
  EXPECT_EQ(0u, id);
  EXPECT_FALSE(TestRegExp(id, ABP_TEXT("abcd"_str)));
  EXPECT_TRUE(TestRegExp(id, ABP_TEXT("1234"_str)));

  DeleteRegExp(id);
  // RegExp has been delete: all is false
  EXPECT_FALSE(TestRegExp(id, ABP_TEXT("abcd"_str)));
  EXPECT_FALSE(TestRegExp(id, ABP_TEXT("1234"_str)));
}
