#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>    // ← добавлен для std::strlen
#include <memory>
#include <string>

TEST(SmartPointers, UniquePtrWithCustomDeleter) {
    auto file_deleter = [](std::FILE* f) {
        if (f) std::fclose(f);
    };

    std::unique_ptr<std::FILE, decltype(file_deleter)> file(
        std::tmpfile(), file_deleter);

    ASSERT_NE(file.get(), nullptr);

    const char* data = "test data\n";
    std::fwrite(data, 1, std::strlen(data), file.get());

}

TEST(SmartPointers, SizeofUniquePtrWithFunctionDeleter) {
    using FuncDeleterPtr = std::unique_ptr<std::FILE, void(*)(std::FILE*)>;

    auto lambda_deleter = [](std::FILE* f) { if (f) std::fclose(f); };
    using LambdaDeleterPtr = std::unique_ptr<std::FILE, decltype(lambda_deleter)>;

    struct StructDeleter {
        void operator()(std::FILE* f) const { if (f) std::fclose(f); }
    };
    using StructDeleterPtr = std::unique_ptr<std::FILE, StructDeleter>;

    EXPECT_EQ(sizeof(FuncDeleterPtr), sizeof(void*) + sizeof(void(*)()));
    EXPECT_EQ(sizeof(LambdaDeleterPtr), sizeof(void*));
    EXPECT_EQ(sizeof(StructDeleterPtr), sizeof(void*));
}

TEST(SmartPointers, SharedPtrControlBlock) {
    auto ptr1 = std::make_shared<std::string>("hello");

    auto ptr2 = std::shared_ptr<std::string>(new std::string("world"));

    EXPECT_EQ(*ptr1, "hello");
    EXPECT_EQ(*ptr2, "world");
}

TEST(SmartPointers, SharedPtrReferenceCounting) {
    auto ptr1 = std::make_shared<std::string>("test");
    EXPECT_EQ(ptr1.use_count(), 1);

    {
        auto ptr2 = ptr1;
        EXPECT_EQ(ptr1.use_count(), 2);
        EXPECT_EQ(ptr2.use_count(), 2);
    }

    EXPECT_EQ(ptr1.use_count(), 1);
}