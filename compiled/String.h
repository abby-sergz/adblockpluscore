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

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <type_traits>
#ifdef INSIDE_TESTS
#include <iostream>
#include <locale>
#include <codecvt>
#endif
#include <utility>
#include <limits>

#include "base.h"
#include "debug.h"
#include "library.h"

ABP_NS_BEGIN

inline void String_assert_writable(bool isWritable);

// hacky because without templates
#ifdef ABP_UTF8_STRING
#define ABP_TEXT(val) val
struct StringTraits
{
  typedef char char_type;
};
#else
#define ABP_TEXT(val) u##val
struct StringTraits
{
  typedef char16_t char_type;
};
#endif

class String
{
  friend class DependentString;
  friend class OwnedString;

public:
  typedef StringTraits::char_type value_type;
  typedef size_t size_type;

  // Type flags, stored in the top 2 bits of the mLen member
  static constexpr size_type INVALID = 0xC0000000;
  static constexpr size_type DELETED = 0x80000000;
  static constexpr size_type READ_ONLY = 0x40000000;
  static constexpr size_type READ_WRITE = 0x00000000;

  static constexpr size_type FLAGS_MASK = 0xC0000000;
  static constexpr size_type LENGTH_MASK = 0x3FFFFFFF;

  static constexpr size_type npos = -1;

protected:
  value_type* mBuf;
  size_type mLen;

  explicit String(value_type* buf, size_type len, size_type flags)
      : mBuf(buf), mLen((len & LENGTH_MASK) | flags)
  {
  }

  ~String()
  {
  }

  void reset(value_type* buf, size_type len, size_type flags)
  {
    mBuf = buf;
    mLen = (len & LENGTH_MASK) | flags;
  }

public:
  size_type length() const
  {
    return mLen & LENGTH_MASK;
  }

  bool empty() const
  {
    return !(mLen & LENGTH_MASK);
  }

  const value_type* data() const
  {
    return mBuf;
  }

  value_type* data()
  {
    String_assert_writable(is_writable());
    return mBuf;
  }

  const value_type& operator[](size_type pos) const
  {
    return mBuf[pos];
  }

  value_type& operator[](size_type pos)
  {
    String_assert_writable(is_writable());
    return mBuf[pos];
  }

  bool is_writable() const
  {
    return (mLen & FLAGS_MASK) == READ_WRITE;
  }

  bool equals(const String& other) const
  {
    if (length() != other.length())
      return false;

    return std::memcmp(mBuf, other.mBuf, sizeof(value_type) * length()) == 0;
  }

  bool operator==(const String& other) const
  {
    return equals(other);
  }

  bool operator!=(const String& other) const
  {
    return !equals(other);
  }

  size_type find(value_type c, size_type pos = 0) const
  {
    for (size_type i = pos; i < length(); ++i)
      if (mBuf[i] == c)
        return i;
    return npos;
  }

  size_type find(const String& str, size_type pos = 0) const
  {
    return find(str.mBuf, pos, str.length());
  }

  size_type find(const value_type* str, size_type pos, size_type count) const
  {
    if (pos > LENGTH_MASK || pos + count > length())
      return npos;

    if (!count)
      return pos;

    for (; pos + count <= length(); ++pos)
    {
      if (mBuf[pos] == str[0] &&
          std::memcmp(mBuf + pos, str, sizeof(value_type) * count) == 0)
      {
        return pos;
      }
    }

    return npos;
  }

  size_type rfind(value_type c, size_type pos = npos) const
  {
    if (length() == 0)
      return npos;

    if (pos >= length())
      pos = length() - 1;

    for (int i = pos; i >= 0; --i)
      if (mBuf[i] == c)
        return i;
    return npos;
  }

  bool is_invalid() const
  {
    return (mLen & FLAGS_MASK) == INVALID;
  }

  bool is_deleted() const
  {
    return (mLen & FLAGS_MASK) == DELETED;
  }

  void toLower()
  {
    size_type len = length();
    for (size_type i = 0; i < len; ++i)
    {
      value_type currChar = mBuf[i];

      // This should be more efficient with a lookup table but I couldn't measure
      // any performance difference.
      if (currChar >= ABP_TEXT('A') && currChar <= ABP_TEXT('Z'))
        mBuf[i] = currChar + ABP_TEXT('a') - ABP_TEXT('A');
      else if (currChar >= 128)
      {
        mBuf[i] = CharToLower(currChar);
      }
    }
  }
};

#ifdef INSIDE_TESTS
inline std::ostream& operator<<(std::ostream& os, const String& str)
{
#ifdef ABP_UTF8_STRING
  os.write(str.data(), str.length());
#else
#if _MSC_VER >= 1900
  std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> converter;
  auto p = reinterpret_cast<const int16_t *>(str.data());
  os << converter.to_bytes(p, p + str.length());
#else
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
  os << converter.to_bytes(str.data(), str.data() + str.length());
#endif // _MSC_VER >= 1900
#endif // ABP_UTF8_STRING
  return os;
}
#endif // INSIDE_TESTS

class DependentString : public String
{
public:
  explicit DependentString()
      : String(nullptr, 0, INVALID)
  {
  }

  explicit DependentString(value_type* buf, size_type len)
      : String(buf, len, READ_WRITE)
  {
  }

  explicit DependentString(const value_type* buf, size_type len)
      : String(const_cast<value_type*>(buf), len, READ_ONLY)
  {
  }

  explicit DependentString(String& str, size_type pos = 0, size_type len = npos)
      : String(
          str.mBuf + std::min(pos, str.length()),
          std::min(len, str.length() - std::min(pos, str.length())),
          str.is_writable() ? READ_WRITE: READ_ONLY
        )
  {
  }

  explicit DependentString(const String& str, size_type pos = 0,
      size_type len = npos)
      : String(
          str.mBuf + std::min(pos, str.length()),
          std::min(len, str.length() - std::min(pos, str.length())),
          READ_ONLY
        )
  {
  }

  void reset(value_type* buf, size_type len)
  {
    *this = DependentString(buf, len);
  }

  void reset(const value_type* buf, size_type len)
  {
    *this = DependentString(buf, len);
  }

  void reset(String& str, size_type pos = 0, size_type len = npos)
  {
    *this = DependentString(str, pos, len);
  }

  void reset(const String& str, size_type pos = 0, size_type len = npos)
  {
    *this = DependentString(str, pos, len);
  }

  void erase()
  {
    *this = DependentString();
    mLen = DELETED;
  }
};

#ifdef INSIDE_TESTS
inline std::ostream& operator<<(std::ostream& os, const DependentString& str)
{
  return os << static_cast<const String&>(str);
}
#endif

inline DependentString operator "" _str(const String::value_type* str,
    String::size_type len)
{
  return DependentString(str, len);
}

inline void String_assert_writable(bool isWritable)
{
  assert2(isWritable, ABP_TEXT("Writing access to a read-only string"_str));
}

class OwnedString : public String
{
private:
  void grow(size_type additionalSize)
  {
    OwnedString newValue(length() + additionalSize);
    if (length() > 0)
      std::memcpy(newValue.mBuf, mBuf, sizeof(value_type) * length());
    *this = std::move(newValue);
  }

public:
  explicit OwnedString(size_type len = 0)
    : String(nullptr, len, len ? READ_WRITE : INVALID)
  {
    if (len)
    {
      mBuf = new value_type[length()];
      annotate_address(mBuf, "String");
    }
    else
      mBuf = nullptr;
  }

  explicit OwnedString(const String& str)
      : OwnedString(str.length())
  {
    if (!str.is_invalid() && !length())
      mLen = length() | READ_WRITE;
    else if (length())
      std::memcpy(mBuf, str.mBuf, sizeof(value_type) * length());
  }

  OwnedString(const OwnedString& str)
      : OwnedString(static_cast<const String&>(str))
  {
  }

  explicit OwnedString(const value_type* str, size_type len)
      : OwnedString(DependentString(str, len))
  {
  }

  explicit OwnedString(OwnedString&& str)
      : OwnedString(0)
  {
    mBuf = str.mBuf;
    mLen = str.mLen;
    str.mBuf = nullptr;
    str.mLen = READ_WRITE | 0;
  }

  ~OwnedString()
  {
    if (mBuf)
      delete[] mBuf;
  }

  void reset(const String& str)
  {
    *this = str;
  }

  void erase()
  {
    if (mBuf)
      delete[] mBuf;
    mBuf = nullptr;
    mLen = DELETED;
  }

  OwnedString& operator=(const String& str)
  {
    *this = OwnedString(str);
    return *this;
  }

  OwnedString& operator=(const OwnedString& str)
  {
    *this = OwnedString(str);
    return *this;
  }

  OwnedString& operator=(OwnedString&& str)
  {
    std::swap(mBuf, str.mBuf);
    std::swap(mLen, str.mLen);
    return *this;
  }

  void append(const value_type* source, size_type sourceLen)
  {
    if (!sourceLen)
      return;

    assert2(source, ABP_TEXT("Null buffer passed to OwnedString.append()"_str));
    size_t oldLength = length();
    grow(sourceLen);
    std::memcpy(mBuf + oldLength, source, sizeof(value_type) * sourceLen);
  }

#ifndef ABP_UTF8_STRING
  void append(const char* source, size_type sourceLen)
  {
    if (!sourceLen)
      return;

    assert2(source, ABP_TEXT("Null buffer passed to OwnedString.append()"_str));
    size_t oldLength = length();
    grow(sourceLen);
    for (size_t i = 0; i < sourceLen; i++)
      mBuf[oldLength + i] = source[i];
  }
#endif // !ABP_UTF8_STRING

  void append(const String& str)
  {
    append(str.mBuf, str.length());
  }

  void append(value_type c)
  {
    append(&c, 1);
  }

  template<typename T,
      typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  void append(T num)
  {
    bool negative = false;
    if (num < 0)
    {
      negative = true;
      num = -num;
    }

    size_type size = 0;
    for (T i = num; i; i /= 10)
      size++;
    size = (size ? size : 1);

    size_type pos = length();
    grow((negative ? 1 : 0) + size);

    if (negative)
      mBuf[pos++] = ABP_TEXT('-');

    for (int i = size - 1; i >= 0; i--)
    {
      mBuf[pos + i] = ABP_TEXT('0') + (num % 10);
      num /= 10;
    }
  }
};

#ifdef INSIDE_TESTS
inline std::ostream& operator<<(std::ostream& os, const OwnedString& str)
{
  return os << static_cast<const String&>(str);
}
#endif

template<typename T>
struct LexicalCastImpl;

/// Performs common conversions of a text represented value.
template<typename T>
inline T lexical_cast(const String& value)
{
  return LexicalCastImpl<T>::Convert(value);
}

template<>
struct LexicalCastImpl<bool>
{
  static bool Convert(const String& value)
  {
    return value == ABP_TEXT("true"_str);
  }
};

template<typename T>
struct LexicalCastImpl
{
  static_assert(std::is_integral<T>::value, "T should be a number");
  static T Convert(const String& value)
  {
    String::size_type len = value.length();
    if (len == 0)
      return 0;
    String::size_type pos = 0;
    bool negative = std::numeric_limits<T>::is_signed && value[0] == ABP_TEXT('-');
    if (negative)
    {
      ++pos;
    }
    T result = 0;
    for (; pos < len; ++pos)
    {
      auto c = value[pos];
      if (c < ABP_TEXT('0') || c > ABP_TEXT('9'))
        return 0;
      // isDangerous is the optimization because there is no need for some checks
      // when the values are far from edge cases.
      // It targets the normal values, when a value is prefixed with several
      // zeros additional checks start to work earlier than the actual value of
      // result reaches an edge case, but it does not affect the result.
      bool isDangerous = pos >= std::numeric_limits<T>::digits10;
      // It also invalidates the parsing of too big numbers in comparison with
      // stopping when it encounters a non numerical character.
      // cast<uint8_t>(u"1230"_str) -> 0
      // cast<uint8_t>(u"123E"_str) -> 123
      if (isDangerous && std::numeric_limits<T>::max() / 10 < result)
      {
        return 0;
      }
      result *= 10;
      uint8_t digit = c - ABP_TEXT('0');
      if (isDangerous && (std::numeric_limits<T>::max() - digit < result - (negative ? 1 : 0)))
      {
        return 0;
      }
      result += digit;
    }
    return negative ? -result : result;
  }
};

template<>
inline OwnedString lexical_cast<OwnedString>(const String& value)
{
  return OwnedString{value};
}

DependentString TrimSpaces(const String& value);

// Splits the `value` string into two `DependentString`s excluding the character staying at `separatorPos`.
// Useful for parsing.
std::pair<DependentString, DependentString> SplitString(const String& value, String::size_type separatorPos);

ABP_NS_END
