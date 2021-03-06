//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <complex>
#include <cstddef>
#include <cstdint>

#define EACH_MESSAGE_TYPE(F)                    \
    F(RequestAnalyzeFrequency)                  \
    F(NotifyFrequencyAnalysis)

enum class Message_Tag {
    #define DECLARE_MEMBER(x) x,
    EACH_MESSAGE_TYPE(DECLARE_MEMBER)
    #undef DECLARE_MEMBER
};

struct Basic_Message {
    Basic_Message()
        : tag() {}
    explicit Basic_Message(Message_Tag tag)
        : tag(tag) {}
    const Message_Tag tag;
};

template <Message_Tag Tag>
struct Basic_Message_T : public Basic_Message {
    Basic_Message_T()
        : Basic_Message(Tag) {}
};

namespace Messages {
    #define DEFMESSAGE(t)                                   \
        struct t : public Basic_Message_T<Message_Tag::t>

    DEFMESSAGE(RequestAnalyzeFrequency) {
        float frequency;
        int spl;
    };

    DEFMESSAGE(NotifyFrequencyAnalysis) {
        float frequency;
        int spl;
        std::complex<float> response;
    };

    #undef DEFMESSAGE

    size_t size_of(Message_Tag tag);
    uint8_t *allocate_buffer();
}
