//          Copyright Adam Lach 2016
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// test
#include <gmock/gmock.h>

// ddomain
#include <basic_type.hpp>
#include <name_mixin.hpp>

using namespace di;
using namespace testing;

namespace {

struct SomeTypeTag {};

TEST(BasicTypeShould, beDefaultConstructibleByDefault)
{
    using SomeType = BasicType<int, SomeTypeTag>;

    SomeType instance;
//    EXPECT_FALSE(instance.initialized());
}

TEST(BasicTypeShould, retrieveTheValueThatWasStoredAsOptional)
{
    using SomeType = BasicType<int, Optional, SomeTypeTag>;

    SomeType instance(1);
    EXPECT_EQ(1, instance.get());
    EXPECT_TRUE(instance.initialized());
}

TEST(BasicTypeShould, retrieveTheValueThatWasStoredAsRequired)
{
    using SomeType = BasicType<int, Required, SomeTypeTag>;

    SomeType instance(1);
    EXPECT_EQ(1, instance.get());
    EXPECT_TRUE(instance.initialized());
}

TEST(BasicTypeShould, correctlyPassOptionalState)
{
    using SomeType = BasicType<int, Optional, SomeTypeTag>;

    SomeType instance(1);
    SomeType instance2;
    instance = instance2;
    EXPECT_FALSE(instance.initialized());
}

TEST(BasicTypeShould, beCopyConstructible)
{
    using OptSomeType = BasicType<int, Optional, SomeTypeTag>;
    using ReqSomeType = BasicType<int, Required, SomeTypeTag>;

    OptSomeType opt = OptSomeType(10);
    ReqSomeType req = ReqSomeType(21);

    EXPECT_EQ(10, opt.get());
    EXPECT_EQ(21, req.get());

    opt = OptSomeType(37);
    req = ReqSomeType(41);

    EXPECT_EQ(37, opt.get());
    EXPECT_EQ(41, req.get());
}

TEST(BasicTypeShould, beMovable)
{
    using OptSomeType = BasicType<std::string, Optional>;
    using ReqSomeType = BasicType<std::string, Required>;

    std::string text1("text1");
    std::string text2("text2");

    OptSomeType opt(std::move(text1));
    ReqSomeType req(std::move(text2));

    EXPECT_EQ("text1", opt.get());
    EXPECT_EQ("text2", req.get());

    OptSomeType opt2(std::string("text3"));
    ReqSomeType req2(std::string("text4"));

    opt = std::move(opt2);
    req = std::move(req2);

    EXPECT_EQ("text3", opt.get());
    EXPECT_EQ("text4", req.get());
}

TEST(BasicTypeShould, beMutable)
{
    using OptSomeType = BasicType<std::string, Optional>;
    using ReqSomeType = BasicType<std::string, Mutable, Required>;

    std::string text1("text1");
    std::string text2("text2");

    OptSomeType opt(std::move(text1));
    ReqSomeType req(std::move(text2));

    EXPECT_EQ("text1", opt.get());
    EXPECT_EQ("text2", req.get());

    opt.set(std::string("text3"));
    req.set(std::string("text4"));

    EXPECT_EQ("text3", opt.get());
    EXPECT_EQ("text4", req.get());
}

TEST(BasicTypeShould, handleMutableAndOptionalTogeather)
{
    using OptSomeType = BasicType<std::string, Optional, Mutable>;

    OptSomeType opt;
    opt.set("text1");

    EXPECT_EQ("text1", opt.get());
    EXPECT_TRUE(opt.initialized());
}

TEST(BasicTypeShould, supportCustomMixins)
{
    using OptSomeType = BasicType<std::string, Optional, Mutable, decltype("some_name"_name)>;

    OptSomeType opt("text1");

    EXPECT_EQ("text1", opt.get());
    EXPECT_STREQ("some_name", opt.name());
}

template<typename T>
struct Prefix
{
    std::string prefix() const
    {
        return static_cast<const T*>(this)->get().substr(0,6);
    }
};

TEST(BasicTypeShould, supportValueBasedMixins)
{
    using OptSomeType = BasicType<std::string, Prefix<di::_>>;

    OptSomeType opt("prefix_text");

    EXPECT_EQ("prefix_text", opt.get());
    EXPECT_EQ("prefix", opt.prefix());
}

template<typename T>
struct CachedPrefix
{
    explicit CachedPrefix(const std::string& v)
        : value(v.substr(0,6))
    {}

    const std::string& prefix() const
    {
        return value;
    }

    void set(const std::string& v)
    {
        value = v.substr(0,6);
    }

private:
    std::string value;
};

TEST(BasicTypeShould, supportCachedMixins)
{
    using OptSomeType = BasicType<std::string, Required, CachedPrefix<di::_>>;

    OptSomeType opt("prefix_text");
    EXPECT_EQ("prefix_text", opt.get());
    EXPECT_EQ("prefix", opt.prefix());

    opt.set("pref12_sth");
    EXPECT_EQ("pref12_sth", opt.get());
    EXPECT_EQ("pref12", opt.prefix());
}

TEST(BasicTypeShould, AsignementOperator)
{
    using OptSomeType = BasicType<std::string, Required, CachedPrefix<di::_>>;

    OptSomeType opt("prefix_text");
    EXPECT_EQ("prefix_text", opt.get());
    EXPECT_EQ("prefix", opt.prefix());

    OptSomeType opt2("pref12_sth");
    opt = opt2;
    EXPECT_EQ("pref12_sth", opt.get());
    EXPECT_EQ("pref12", opt.prefix());
}

} // namespace

