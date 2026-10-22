#include <gtest/gtest.h>

#include <memory>
#include <string>

struct BadRoom;
struct BadUser {
    std::string name;
    std::shared_ptr<BadRoom> room;

    BadUser(std::string n) : name(std::move(n)) {}
    ~BadUser() = default;
};

struct BadRoom {
    std::string name;
    std::shared_ptr<BadUser> user;

    BadRoom(std::string n) : name(std::move(n)) {}
    ~BadRoom() = default;
};

struct GoodRoom;
struct GoodUser {
    std::string name;
    std::weak_ptr<GoodRoom> room;

    GoodUser(std::string n) : name(std::move(n)) {}
    ~GoodUser() = default;
};

struct GoodRoom {
    std::string name;
    std::shared_ptr<GoodUser> user;

    GoodRoom(std::string n) : name(std::move(n)) {}
    ~GoodRoom() = default;
};

TEST(Cycle, BadCycleCausesMemoryLeak) {
    std::shared_ptr<BadUser> user;
    std::shared_ptr<BadRoom> room;

    {
        user = std::make_shared<BadUser>("alice");
        room = std::make_shared<BadRoom>("general");

        user->room = room;
        room->user = user;
    }

    EXPECT_EQ(user.use_count(), 2);
    EXPECT_EQ(room.use_count(), 2);

    user->room.reset();
    room->user.reset();

    EXPECT_EQ(user.use_count(), 1);
    EXPECT_EQ(room.use_count(), 1);
}

TEST(Cycle, GoodCycleNoLeak) {
    std::shared_ptr<GoodUser> user;
    std::shared_ptr<GoodRoom> room;

    {
        user = std::make_shared<GoodUser>("alice");
        room = std::make_shared<GoodRoom>("general");

        user->room = room;
        room->user = user;

    }

    user.reset();
    room.reset();

}
