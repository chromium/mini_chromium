// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/string_number_conversions.h"

#include <ctype.h>
#include <stdint.h>

#include <limits>

namespace base {

namespace {

template<typename CHAR, int BASE, bool BASE_LTE_10>
class BaseCharToDigit {
};

template<typename CHAR, int BASE>
class BaseCharToDigit<CHAR, BASE, true> {
 public:
  static bool Convert(CHAR c, uint8_t* digit) {
    if (c >= '0' && c < '0' + BASE) {
      *digit = c - '0';
      return true;
    }
    return false;
  }
};

template<typename CHAR, int BASE>
class BaseCharToDigit<CHAR, BASE, false> {
 public:
  static bool Convert(CHAR c, uint8_t* digit) {
    if (c >= '0' && c <= '9') {
      *digit = c - '0';
    } else if (c >= 'a' && c < 'a' + BASE - 10) {
      *digit = c - 'a' + 10;
    } else if (c >= 'A' && c < 'A' + BASE - 10) {
      *digit = c - 'A' + 10;
    } else {
      return false;
    }
    return true;
  }
};

template<int BASE, typename CHAR>
bool CharToDigit(CHAR c, uint8_t* digit) {
  return BaseCharToDigit<CHAR, BASE, BASE <= 10>::Convert(c, digit);
}

template<typename CHAR>
class WhitespaceHelper {
};

template<>
class WhitespaceHelper<char> {
 public:
  static bool Invoke(char c) {
    return 0 != isspace(static_cast<unsigned char>(c));
  }
};

template<typename CHAR>
bool LocalIsWhitespace(CHAR c) {
  return WhitespaceHelper<CHAR>::Invoke(c);
}

template<typename IteratorRangeToNumberTraits>
class IteratorRangeToNumber {
 public:
  typedef IteratorRangeToNumberTraits traits;
  typedef typename traits::iterator_type const_iterator;
  typedef typename traits::value_type value_type;

  static bool Invoke(const_iterator begin,
                     const_iterator end,
                     value_type* output) {
    bool valid = true;

    while (begin != end && LocalIsWhitespace(*begin)) {
      valid = false;
      ++begin;
    }

    if (begin != end && *begin == '-') {
      if (!Negative::Invoke(begin + 1, end, output)) {
        valid = false;
      }
    } else {
      if (begin != end && *begin == '+') {
        ++begin;
      }
      if (!Positive::Invoke(begin, end, output)) {
        valid = false;
      }
    }

    return valid;
  }

 private:
  template<typename Sign>
  class Base {
   public:
    static bool Invoke(const_iterator begin, const_iterator end,
                       typename traits::value_type* output) {
      *output = 0;

      if (begin == end) {
        return false;
      }

      if (traits::kBase == 16 && end - begin >= 2 && *begin == '0' &&
          (*(begin + 1) == 'x' || *(begin + 1) == 'X')) {
        begin += 2;
      }

      for (const_iterator current = begin; current != end; ++current) {
        uint8_t new_digit = 0;

        if (!CharToDigit<traits::kBase>(*current, &new_digit)) {
          return false;
        }

        if (current != begin) {
          if (!Sign::CheckBounds(output, new_digit)) {
            return false;
          }
          *output *= traits::kBase;
        }

        Sign::Increment(new_digit, output);
      }
      return true;
    }
  };

  class Positive : public Base<Positive> {
   public:
    static bool CheckBounds(value_type* output, uint8_t new_digit) {
      if (*output > static_cast<value_type>(traits::max() / traits::kBase) ||
          (*output == static_cast<value_type>(traits::max() / traits::kBase) &&
           new_digit > traits::max() % traits::kBase)) {
        *output = traits::max();
        return false;
      }
      return true;
    }
    static void Increment(uint8_t increment, value_type* output) {
      *output += increment;
    }
  };

  class Negative : public Base<Negative> {
   public:
    static bool CheckBounds(value_type* output, uint8_t new_digit) {
      if (*output < traits::min() / traits::kBase ||
          (*output == traits::min() / traits::kBase &&
           new_digit > 0 - traits::min() % traits::kBase)) {
        *output = traits::min();
        return false;
      }
      return true;
    }
    static void Increment(uint8_t increment, value_type* output) {
      *output -= increment;
    }
  };
};

template<typename ITERATOR, typename VALUE, int BASE>
class BaseIteratorRangeToNumberTraits {
 public:
  typedef ITERATOR iterator_type;
  typedef VALUE value_type;
  static value_type min() {
    return std::numeric_limits<value_type>::min();
  }
  static value_type max() {
    return std::numeric_limits<value_type>::max();
  }
  static const int kBase = BASE;
};

typedef BaseIteratorRangeToNumberTraits<std::string::const_iterator, int, 10>
    IteratorRangeToIntTraits;
typedef BaseIteratorRangeToNumberTraits<const char*, int, 10>
    CharBufferToIntTraits;

typedef BaseIteratorRangeToNumberTraits<std::string::const_iterator,
                                       unsigned int,
                                       10>
    IteratorRangeToUintTraits;
typedef BaseIteratorRangeToNumberTraits<const char*, unsigned int, 10>
    CharBufferToUintTraits;

typedef BaseIteratorRangeToNumberTraits<std::string::const_iterator,
                                        int64_t,
                                        10>
    IteratorRangeToInt64Traits;
typedef BaseIteratorRangeToNumberTraits<const char*, int64_t, 10>
    CharBufferToInt64Traits;

typedef BaseIteratorRangeToNumberTraits<std::string::const_iterator,
                                        uint64_t,
                                        10>
    IteratorRangeToUint64Traits;
typedef BaseIteratorRangeToNumberTraits<const char*, uint64_t, 10>
    CharBufferToUint64Traits;

typedef BaseIteratorRangeToNumberTraits<std::string::const_iterator,
                                        size_t,
                                        10>
    IteratorRangeToSizeTTraits;
typedef BaseIteratorRangeToNumberTraits<const char*, size_t, 10>
    CharBufferToSizeTTraits;

template<typename ITERATOR>
class BaseHexIteratorRangeToIntTraits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, int, 16> {
 public:
  static unsigned int max() {
    return std::numeric_limits<unsigned int>::max();
  }
};

typedef BaseHexIteratorRangeToIntTraits<std::string::const_iterator>
    HexIteratorRangeToIntTraits;

}  // namespace

bool StringToInt(const std::string& input, int* output) {
  return IteratorRangeToNumber<IteratorRangeToIntTraits>::Invoke(input.begin(),
                                                                 input.end(),
                                                                 output);
}

bool StringToInt(const char* begin, const char* end, int* output) {
  return IteratorRangeToNumber<CharBufferToIntTraits>::Invoke(begin,
                                                              end,
                                                              output);
}

bool StringToUint(const std::string& input, unsigned int* output) {
  return IteratorRangeToNumber<IteratorRangeToUintTraits>::Invoke(input.begin(),
                                                                  input.end(),
                                                                  output);
}

bool StringToUint(const char* begin, const char* end, unsigned int* output) {
  return IteratorRangeToNumber<CharBufferToUintTraits>::Invoke(begin,
                                                               end,
                                                               output);
}

bool StringToInt64(const std::string& input, int64_t* output) {
  return IteratorRangeToNumber<IteratorRangeToInt64Traits>::Invoke(
      input.begin(), input.end(), output);
}

bool StringToInt64(const char* begin, const char* end, int64_t* output) {
  return IteratorRangeToNumber<CharBufferToInt64Traits>::Invoke(begin,
                                                                end,
                                                                output);
}

bool StringToUint64(const std::string& input, uint64_t* output) {
  return IteratorRangeToNumber<IteratorRangeToUint64Traits>::Invoke(
      input.begin(), input.end(), output);
}

bool StringToUint64(const char* begin, const char* end, uint64_t* output) {
  return IteratorRangeToNumber<CharBufferToUint64Traits>::Invoke(begin,
                                                                 end,
                                                                 output);
}

bool StringToSizeT(const std::string& input, size_t* output) {
  return IteratorRangeToNumber<IteratorRangeToSizeTTraits>::Invoke(
      input.begin(), input.end(), output);
}

bool StringToSizeT(const char* begin, const char* end, size_t* output) {
  return IteratorRangeToNumber<CharBufferToSizeTTraits>::Invoke(begin,
                                                                end,
                                                                output);
}

bool HexStringToInt(const std::string& input, int* output) {
  return IteratorRangeToNumber<HexIteratorRangeToIntTraits>::Invoke(
      input.begin(), input.end(), output);
}

bool HexStringToBytes(const std::string& input, std::vector<uint8_t>* output) {
  size_t count = input.size();
  if (count == 0 || (count % 2) != 0) {
    return false;
  }
  for (size_t i = 0; i < count; i += 2) {
    uint8_t msb = 0;
    uint8_t lsb = 0;
    if (!CharToDigit<16>(input[i], &msb) ||
        !CharToDigit<16>(input[i + 1], &lsb)) {
      return false;
    }
    output->push_back((msb << 4) | lsb);
  }
  return true;
}

}  // namespace base
