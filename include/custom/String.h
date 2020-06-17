//
// Created by ArcticMarmoset on 2019-11-30.
//

#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

namespace Custom
{
    class String
    {
    private:
        int curLen_;
        int maxLen_;
        char *string_;
        int *refCount_;

    public:
        String();
        ~String();

        explicit String(const char str[]);
        String(const String &str);

        int Length() const;
        char *ToCharArray() const;

        bool IsEmpty() const;

        int IndexOf(char c, int fromIndex = 0) const;
        int IndexOf(const char str[]) const;
        int IndexOf(const String &str) const;

        bool Contains(char c) const;
        bool Contains(const char str[]) const;
        bool Contains(const String &str) const;

        String &PushBack(char c);
        char PopBack();

        String &Append(char c);
        String &Append(const char str[]);
        String &Append(const String &str);

        char **Split(char delimiter) const;

        String &operator=(const char str[]);
        String &operator=(const String &str);
        bool operator==(const String &str) const;

    private:
        static int LengthOf(const char str[]);
    };
}

#endif //CUSTOM_STRING_H
