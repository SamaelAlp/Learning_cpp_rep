#include "chat_core/owning_buffer.hpp"

#include <gtest/gtest.h>

#include <utility>

using namespace chat;

TEST(OwningBuffer, DefaultIsEmpty) {
    OwningBuffer b;
    EXPECT_EQ(b.size(), 0u);
    EXPECT_EQ(b.data(), nullptr);
}

TEST(OwningBuffer, ConstructWithSize) {
    OwningBuffer b(10);
    EXPECT_EQ(b.size(), 10u);
    EXPECT_NE(b.data(), nullptr);

    for (std::size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(b.get(i), '\0');
    }
}

TEST(OwningBuffer, CopyProducesIndependentMemory) {
    OwningBuffer a(5);
    a.set(0, 'x');
    a.set(1, 'y');

    OwningBuffer b(a);
    EXPECT_EQ(b.size(), 5u);
    EXPECT_EQ(b.get(0), 'x');
    EXPECT_EQ(b.get(1), 'y');

    b.set(0, 'Z');
    EXPECT_EQ(a.get(0), 'x');
    EXPECT_EQ(b.get(0), 'Z');
    EXPECT_NE(a.data(), b.data());
}

TEST(OwningBuffer, CopyAssignmentProducesIndependentMemory) {
    OwningBuffer a(3);
    a.set(0, 'a');
    OwningBuffer b(2);
    b = a;
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.get(0), 'a');
    b.set(0, 'Q');
    EXPECT_EQ(a.get(0), 'a');
}

TEST(OwningBuffer, MoveLeavesSourceInValidState) {
    OwningBuffer a(4);
    a.set(0, 'm');

    OwningBuffer b(std::move(a));
    EXPECT_EQ(b.size(), 4u);
    EXPECT_EQ(b.get(0), 'm');

    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.data(), nullptr);

    a = OwningBuffer(2);
    EXPECT_EQ(a.size(), 2u);
}

TEST(OwningBuffer, MoveAssignment) {
    OwningBuffer a(3);
    a.set(0, 'z');
    OwningBuffer b(1);

    b = std::move(a);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.get(0), 'z');
    EXPECT_EQ(a.size(), 0u);
}

TEST(OwningBuffer, SelfAssignmentDoesNotBreakObject) {
    OwningBuffer a(5);
    a.set(0, 's');
    a.set(4, 'e');

    a = a;

    EXPECT_EQ(a.size(), 5u);
    EXPECT_EQ(a.get(0), 's');
    EXPECT_EQ(a.get(4), 'e');
    EXPECT_NE(a.data(), nullptr);
}

TEST(OwningBuffer, SwapWorks) {
    OwningBuffer a(2); a.set(0, 'A');
    OwningBuffer b(3); b.set(0, 'B');

    swap(a, b);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a.get(0), 'B');
    EXPECT_EQ(b.size(), 2u);
    EXPECT_EQ(b.get(0), 'A');
}