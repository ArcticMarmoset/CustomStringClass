//
// Created by ArcticMarmoset on 2019-11-30.
//

#include "custom/String.h"

#include "DebugAssert.h"

#include <cstring>
#include <algorithm>

using std::size_t;

namespace Custom
{

    void swap(String &first, String &second) noexcept
    {
        using std::swap;

        swap(first.buffer_, second.buffer_);
        swap(first.active_, second.active_);
        swap(first.capacity_, second.capacity_);
        swap(first.charCount_, second.charCount_);
    }

    String::String()
        : active_(buffer_.heap)
        , capacity_(0)
        , charCount_(1)
    {
    }

    String::String(const String &other)
        : capacity_(other.capacity_)
        , charCount_(other.charCount_)
    {
        if (capacity_ == 0) // other is stored locally
        {
            active_         = buffer_.local;
            const char *str = other.buffer_.local;
            strncpy(buffer_.local, str, charCount_);
            return;
        }

        active_          = buffer_.heap;
        buffer_.heap     = other.buffer_.heap;
        buffer_.refCount = other.buffer_.refCount;

        ++*buffer_.refCount;
    }

    String::String(String &&other) noexcept
    {
        // No point initialising member variables
        swap(*this, other);
        // Let destructor take care of other
    }

    String::String(const char *str)
        : capacity_(0)
        , charCount_(strlen(str) + 1)
    {
        // Drop null char if just 1 over smax
        if (charCount_ - 1 <= smax) // can store locally
        {
            active_      = buffer_.local;
            size_t count = (charCount_ < smax) ? charCount_ : smax;
            strncpy(active_, str, count);
            return;
        }

        active_ = buffer_.heap;
        // Arbitrary capacity
        capacity_        = charCount_ + smax;
        buffer_.heap     = new char[capacity_];
        buffer_.refCount = new size_t(1);
        strncpy(active_, str, charCount_);
    }

    size_t String::Length() const
    {
        return charCount_ - 1;
    }

    const char *String::Value() const
    {
        if (IsEmpty() || capacity_ != 0)
        {
            return buffer_.heap;
        }
        else
        {
            return buffer_.local;
        }
    }

    void String::ToCharArray(const char *out)
    {
        if (IsEmpty() || capacity_ != 0)
        {
            out = buffer_.heap;
            return;
        }

        if (buffer_.local[charCount_ - 1] != '\0') // then need to append null
        {
            Append('\0');
            ToCharArray(out);
            return;
        }

        out = buffer_.local;
    }

    bool String::IsEmpty() const
    {
        return charCount_ == 1;
    }

    size_t String::IndexOf(const char c, int fromIndex) const
    {
        // TODO: Use pointer arithmetic?

        if (charCount_ == 1)
        {
            return npos;
        }

        for (size_t i = fromIndex; i < charCount_; i++)
        {
            if (active_[i] == c)
            {
                return i;
            }
        }
        return npos;
    }

    size_t String::IndexOf(const char *str) const
    {
        // TODO: Use pointer arithmetic?

        size_t strLength = strlen(str);

        if (strLength == 0 || IsEmpty())
        {
            return npos;
        }

        for (size_t i = 0; i < charCount_; i++)
        {
            // Look for a first-char match
            if (active_[i] != str[0])
            {
                continue;
            }

            // There was a match so compare the rest
            for (size_t j = 1; j <= strLength; j++)
            {
                // If we are already at strLength, all chars have matched
                if (j == strLength)
                {
                    // Increment i to match j
                    i++;
                    // Return the index of the first char
                    return i - j;
                }

                // Otherwise, get the next char
                i++;

                // If this pair does not match, look for the next first-char match
                if (active_[i] != str[j])
                {
                    break;
                }
            }
        }

        // A match was not found
        return npos;
    }

    size_t String::IndexOf(const String &str) const
    {
        return IndexOf(str.Value());
    }

    bool String::Contains(const char c) const
    {
        return IndexOf(c) != -1;
    }

    bool String::Contains(const char *str) const
    {
        return IndexOf(str) != -1;
    }

    bool String::Contains(const String &str) const
    {
        return IndexOf(str) != -1;
    }

    String &String::PushBack(char c)
    {
        // TODO: Rework

        size_t cap = (smax > capacity_) ? smax : capacity_;
        bool canFit = charCount_ + 1 <= cap;

        // * Not shared if and only if capacity <= 0 and ref count <= 0
        // * Both < 0 is never true, since values are unsigned
        // Therefore: not shared if and only if both == 0
        // * Both == 0 if and only if capacity OR ref count == 0
        // Therefore: shared if and only if (capacity OR ref count) > 0
        bool isShared = (capacity_ | *buffer_.refCount) > 0;

        // If we can fit 1 more char, and string is local, 
        // just append the char and re-add the null char
        if (canFit && !isShared)
        {
            active_[charCount_ - 1] = c;
            active_[charCount_] = '\0';
            charCount_++;
            return *this;
        }

        // Remaining cases:
        // Local string and cannot fit
        // Shared heap string and cannot fit
        // Unique heap string and cannot fit
        // Shared heap string and can fit

        active_ = buffer_.heap;

        if (!canFit) // increase size
        {
            capacity_ = 2 * capacity_;
        }

        char *newString = new char[capacity_];

        // Drop null char since it will be overwritten
        strncpy(newString, active_, Length());

        // Append
        newString[charCount_ - 1] = c;
        newString[charCount_] = '\0';
        charCount_++;

        if (isShared) // decrement ref count and start anew
        {
            --*buffer_.refCount;
            buffer_.refCount = new size_t(1);
        }
        else // free heap allocation
        {
            delete[] buffer_.heap;
        }

        // Update string
        buffer_.heap = newString;
        return *this;
    }

    char String::PopBack()
    {
        if (charCount_ == 1) // cannot pop
        {
            return '\0';
        }

        --charCount_;
        // Set last char to null
        char c = active_[charCount_ - 1];
        active_[charCount_ - 1] = '\0';
        return c;
    }

    String &String::Append(char c)
    {
        return PushBack(c);
    }

    void String::Split(char delimiter, String *out, size_t *outCount) const
    {
        if (charCount_ == 1) // nothing to split
        {
            out = nullptr;
            *outCount = 0;
            return;
        }

        // Calculate number of chunks
        size_t chunkCount = 1;
        for (size_t i = 0; i < charCount_; i++)
        {
            if (active_[i] == delimiter)
            {
                chunkCount++;
            }
        }

        if (chunkCount == 1) // nothing to split
        {
            out = nullptr;
            *outCount = 0;
            return;
        }

        String *chunks = new String[chunkCount];

        size_t prevIndex = 0;
        size_t delIndex;
        for (int i = 0; i < chunkCount; i++)
        {
            // Get index of delimiter, starting from the prevIndex
            delIndex = IndexOf(delimiter, prevIndex);

            // Length without null char
            size_t chunkSize;
            if (delIndex > prevIndex) // not last chunk
            {
                chunkSize = delIndex - prevIndex;
            }
            else
            {
                chunkSize = charCount_ - prevIndex - 1;
            }

            char *chunk = new char[chunkSize + 1];

            // Copy string starting from prevIndex and ending at next delimiter
            for (int j = 0; j < chunkSize; j++)
            {
                chunk[j] = active_[prevIndex + j];
            }

            // Terminate the string
            chunk[chunkSize] = '\0';

            // Update prevIndex to next delimiter index + 1
            prevIndex = delIndex + 1;

            // Assign this chunk to the array
            chunks[i] = chunk;
        }

        out = chunks;
        *outCount = chunkCount;
    }

    // Pass by value to create copy
    String &String::operator=(String str)
    {
        swap(*this, str);
        return *this;
        // Let destructor handle str
    }

    String::~String()
    {
        // Short-circuit eval checks heap allocation first
        // Decrement refCount then compare with 0
        if (capacity_ > 0 && --*buffer_.refCount == 0)
        {
            delete[] buffer_.heap;
            delete buffer_.refCount;
            return;
        }
    }

    bool operator==(const String &lhs, const String &rhs)
    {
        
        if (lhs.active_ == rhs.active_) // same address
        {
            return true;
        }

        if (lhs.charCount_ != rhs.charCount_)
        {
            return false;
        }

        for (int i = 0; i < lhs.charCount_; i++)
        {
            if (lhs.active_[i] != rhs.active_[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const String &lhs, const String &rhs)
    {
        return !(lhs == rhs);
    }

}
