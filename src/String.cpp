//
// Created by ArcticMarmoset on 2019-11-30.
//

#include "String.h"

Custom::String::String()
{
    curLen_ = 0;
    // Initial max length of 19
    maxLen_ = 19;
    // Allocate char array of size 19 + CHAR_COUNT_OFFSET
    string_ = new char[21];
    // Because string_[0] is reserved for the refCount
    string_[0] = 1;
    // And we need to terminate string_ with the null char
    string_[1] = '\0';
}

Custom::String::~String()
{
    // Check for refCount of 1 because 0 is the null char
    if (string_[0] == 1)
    {
        delete [] string_;
        return;
    }

    // Otherwise, decrement refCount
    string_[0]--;
}

Custom::String::String(const char *str) : String()
{
    // arrayLength is the number of chars plus the null char
    int arrayLength = String::LengthOf(str) + 1;

    // If arrayLength is 1, then str is empty
    if (arrayLength == 1)
    {
        return;
    }

    // If arrayLength exceeds 19, we need a new array
    if (arrayLength > maxLen_)
    {
        // Update maxLen_ with buffer
        maxLen_ += arrayLength;
        // Free the old one
        delete [] string_;
        // Allocate the new one
        string_ = new char[maxLen_ + CHAR_COUNT_OFFSET];
    }

    // Update curLen_
    curLen_ = arrayLength - 1;

    // Copy over chars from str with string_ index offset by 1
    for (int i = 0; i < curLen_ + 1; i++)
    {
        string_[i + 1] = str[i];
    }
}

Custom::String::String(const Custom::String &str)
{
    // Copy constructor so point string_ to same memory
    string_ = str.string_;
    // Update lengths
    curLen_ = str.curLen_;
    maxLen_ = str.maxLen_;
    // And increment refCount
    string_[0]++;
}

int Custom::String::LengthOf(const char *str)
{
    if (!str)
    {
        return 0;
    }

    char c = str[0];

    if (!c)
    {
        return 0;
    }

    int i = 1;
    while ((c = str[i]))
    {
        i++;
    }

    return i;
}

int Custom::String::Length() const
{
    return curLen_;
}

char *Custom::String::ToCharArray() const
{
    char *string = new char[curLen_ + 1];
    for (int i = 0; i < curLen_ + 1; i++)
    {
        string[i] = string_[i + 1];
    }
    return string;
}

bool Custom::String::IsEmpty() const
{
    return curLen_ == 0;
}

int Custom::String::IndexOf(const char c, int fromIndex) const
{
    if (curLen_ == 0)
    {
        return -1;
    }

    // Loop from fromIndex with refCount offset to curLen_ + CHAR_COUNT_OFFSET
    for (int i = fromIndex + 1; i < (curLen_ + CHAR_COUNT_OFFSET); i++)
    {
        if (string_[i] == c)
        {
            return i - 1;
        }
    }
    return -1;
}

int Custom::String::IndexOf(const char *str) const
{
    int strLength = String::LengthOf(str);

    if (curLen_ == 0 || strLength == 0)
    {
        return -1;
    }

    // Offset by 1 not 2 because there is no need to compare null character
    for (int i = 1; i < curLen_ + 1; i++)
    {
        // Look for a first-char match
        if (string_[i] != str[0])
        {
            continue;
        }

        // There was a match so compare the rest
        for (int j = 1; j <= strLength; j++)
        {
            // If we are already at strLength, all chars have matched
            if (j == strLength)
            {
                // Return the index of the first char
                return i - 1;
            }

            // Otherwise, get the next char
            i++;

            // If this pair does not match, look for the next first-char match
            if (string_[i] != str[j])
            {
                break;
            }
        }
    }

    // A match was not found
    return -1;
}

int Custom::String::IndexOf(const Custom::String &str) const
{
    if (curLen_ == 0 || str.curLen_ == 0)
    {
        return -1;
    }

    int strLength = str.curLen_ + 1;
    for (int i = 1; i < curLen_ + 1; i++)
    {
        // Look for a first-char match
        if (string_[i] != str.string_[1])
        {
            continue;
        }

        // There was a match so compare the rest
        for (int j = 2; j <= strLength; j++)
        {
            // If we are already at strLength, all chars have matched
            if (j == strLength)
            {
                return i - 1;
            }

            // Otherwise, get the next char
            i++;

            // If this pair does not match, look for the next first-char match
            if (string_[i] != str.string_[j])
            {
                break;
            }
        }
    }

    // A match was not found
    return -1;
}

bool Custom::String::Contains(const char c) const
{
    return IndexOf(c) != -1;
}

bool Custom::String::Contains(const char *str) const
{
    return IndexOf(str) != -1;
}

bool Custom::String::Contains(const Custom::String &str) const
{
    return IndexOf(str) != -1;
}

Custom::String &Custom::String::PushBack(char c)
{
    bool canFit = curLen_ + 1 <= maxLen_;
    bool isShared = string_[0] > 1;

    // If we can fit 1 more char, and the memory at string_ is not shared,
    // then just append the char and re-add the null char
    if (canFit && !isShared)
    {
        string_[curLen_ + 1] = c;
        string_[curLen_ + 2] = '\0';
        curLen_++;
        return *this;
    }

    // In all other cases, we need to dynamically allocate a new array

    // But the size of it must first be determined
    if (!canFit)
    {
        maxLen_ = maxLen_ * 2;
    }

    // Dynamically allocate the new array
    char *newString = new char[maxLen_ + CHAR_COUNT_OFFSET];

    // Initialise refCount to 1
    newString[0] = 1;

    // Copy current string into newString
    for (int i = 1; i < curLen_ + 1; i++)
    {
        newString[i] = string_[i];
    }

    // Add the char
    newString[curLen_ + 1] = c;
    newString[curLen_ + 2] = '\0';
    curLen_++;

    // If memory at string_ was shared, decrement refCount
    if (isShared)
    {
        string_[0]--;
    }
    // Otherwise, free memory at string_
    else
    {
        delete [] string_;
    }

    // Update string_
    string_ = newString;
    return *this;
}

char Custom::String::PopBack()
{
    // Cannot pop if string is empty
    if (curLen_ == 0)
    {
        return '\0';
    }

    // If string_ is shared, create a new string and decrement refCount
    if (string_[0] > 1)
    {
        char *newString = new char[curLen_ + CHAR_COUNT_OFFSET];
        newString[0] = 1;
        for (int i = 1; i < curLen_ + 1; i++)
        {
            newString[i] = string_[i];
        }
        string_[0]--;
        string_ = newString;
    }

    // Set the char at curLen_ + 1 to null
    char c = string_[curLen_ + 1];
    string_[curLen_ + 1] = '\0';
    curLen_--;
    return c;
}

Custom::String &Custom::String::Append(char c)
{
    return PushBack(c);
}

char **Custom::String::Split(char delimiter) const
{
    // Cannot split if string is empty
    if (curLen_ == 0)
    {
        return nullptr;
    }

    // Calculate number of chunks
    int chunkCount = 1;
    for (int i = 1; i < curLen_ + 1; i++)
    {
        if (string_[i] == delimiter)
        {
            chunkCount++;
        }
    }

    // Cannot split with no chunks
    // ? Maybe return array of 1 string
    if (chunkCount == 1)
    {
        return nullptr;
    }

    char **chunks = new char *[chunkCount + 1];

    int prevIndex = 0;
    int delIndex;
    for (int i = 0; i < chunkCount; i++)
    {
        // Get index of delimiter, starting from the prevIndex
        delIndex = IndexOf(delimiter, prevIndex);

        int chunkSize;
        if (delIndex > prevIndex)
        {
            chunkSize = delIndex - prevIndex;
        }
        else
        {
            chunkSize = curLen_ - prevIndex + 1;
        }

        char *chunk = new char[chunkSize + 1];

        // Copy string starting from prevIndex and ending at next delimiter
        for (int j = 0; j < chunkSize; j++)
        {
            chunk[j] = string_[1 + prevIndex + j];
        }

        // Terminate the string
        chunk[chunkSize + 1] = '\0';

        // Update prevIndex to next delimiter index + 1
        prevIndex = delIndex + 1;

        // Assign this chunk to the array
        chunks[i] = chunk;
    }

    chunks[chunkCount] = nullptr;
    return chunks;
}

Custom::String &Custom::String::operator=(const char *str)
{
    // TODO: Finish implementing
    return *this;
}

Custom::String &Custom::String::operator=(const Custom::String &str)
{
    if (this == &str)
    {
        return *this;
    }

    if (string_[0] == 1)
    {
        delete [] string_;
    }

    str.string_[0]++;
    string_ = str.string_;
    curLen_ = str.curLen_;
    maxLen_ = str.maxLen_;
    return *this;
}

bool Custom::String::operator==(const Custom::String &str) const
{
    if (string_ == str.string_)
    {
        return true;
    }

    if (curLen_ != str.curLen_)
    {
        return false;
    }

    for (int i = 1; i < curLen_ + 1; i++)
    {
        if (string_[i] != str.string_[i])
        {
            return false;
        }
    }

    return true;
}
