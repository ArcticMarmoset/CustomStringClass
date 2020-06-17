//
// Created by ArcticMarmoset on 2019-11-30.
//

#ifndef CUSTOM_STRING_CONTAINER_H
#define CUSTOM_STRING_CONTAINER_H

#include <cstddef>
#include <iostream>

namespace Custom
{

    class String
    {
        friend void swap(String &first, String &second) noexcept;

        friend bool operator==(const String &lhs, const String &rhs);
        friend bool operator!=(const String &lhs, const String &rhs);

        friend String operator+(const String &lhs, const String &rhs);
        friend String operator+(String &&lhs, const String &rhs);
        friend String operator+(const String &lhs, String &&rhs);
        friend String operator+(String &&lhs, String &&rhs);

    public:
        static constexpr std::size_t npos = -1;

        String();
        String(const String &other);
        String(String &&other) noexcept;
        String(const char *other);
        ~String();

        void Reserve(std::size_t size);

        bool IsEmpty() const;
        std::size_t Length() const;

        const char *Value() const;
        void ToCharArray(const char *out);

        size_t IndexOf(char c, int fromIndex = 0) const;
        size_t IndexOf(const char *str) const;
        size_t IndexOf(const String &str) const;

        bool Contains(char c) const;
        bool Contains(const char *str) const;
        bool Contains(const String &str) const;

        String &PushBack(char c);
        char PopBack();

        String &Append(char c);
        String &Append(const char *str);
        String &Append(const String &str);

        void Split(char delimiter, String *out, std::size_t *outCount) const;

        String &operator=(String other);
        String &operator+=(const char *str);
        String &operator+=(const String &other);

    private:
        /// <summary>
        /// The maximum size of a small string.
        /// </summary>
        static constexpr auto smax = sizeof(void *) + sizeof(std::size_t);
        static inline char null = '\0';

        // TODO: Drop refCount for type of same size
        // - reduces heap allocs but maintains alignment

        union Buffer
        {
            struct
            {
                /// <summary>
                /// Pointer to a heap-allocated string.
                /// </summary>
                char *heap;

                /// <summary>
                /// Pointer to reference counter.
                /// </summary>
                std::size_t *refCount;
            };
            char local[smax];

            Buffer() : heap(&null), refCount(0) {}
        };

        // TODO: Re-evaluate necessity of active

        char *active_;
        std::size_t capacity_;
        std::size_t charCount_;
        Buffer buffer_;
    };

    inline std::ostream &operator<<(std::ostream &os, const String &str)
    {
        return os << str.ToCharArray();
    }

}

#endif // !CUSTOM_STRING_CONTAINER_H
