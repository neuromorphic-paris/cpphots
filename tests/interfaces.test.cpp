#include <cpphots/interfaces.h>
#include <sstream>

#include <gtest/gtest.h>


class StreamableTest : public cpphots::Streamable {

public:

    StreamableTest(bool strict = false)
        :strict(strict) {}

    StreamableTest(int x, int y, bool strict = false)
        :x(x), y(y), strict(strict) {}

    void toStream(std::ostream& out) const override {
        writeMetacommand(out, "streamabletest");
        out << x << " " << y << std::endl;
    }

    void fromStream(std::istream& in) override {
        if (strict)
            matchMetacommandRequired(in, "streamabletest");
        else
            matchMetacommandOptional(in, "streamabletest");
        in >> x;
        in >> y;
    }

    int x, y;
    bool strict;

};

TEST(TestStreamable, Basic) {

    StreamableTest st0(1, 2);

    std::ostringstream out;
    st0.toStream(out);

    // explict calling
    {
        std::istringstream in(out.str());
        StreamableTest st1;
        st1.fromStream(in);
        EXPECT_EQ(st1.x, st0.x);
        EXPECT_EQ(st1.y, st0.y);
    }

    // operators
    out.clear();
    out << st0;
    {
        std::istringstream in(out.str());
        StreamableTest st1;
        in >> st1;
        EXPECT_EQ(st1.x, st0.x);
        EXPECT_EQ(st1.y, st0.y);
    }

    // shorthand
    {
        std::istringstream in(out.str());
        auto st1 = cpphots::createFromStream<StreamableTest>(in);
        EXPECT_EQ(st1.x, st0.x);
        EXPECT_EQ(st1.y, st0.y);
    }

}


TEST(TestStreamable, Metacommands) {

    { // correct metacommand
        std::string str("!STREAMABLETEST\n1 2");
        std::istringstream in(str);
        auto st = cpphots::createFromStream<StreamableTest>(in);
        EXPECT_EQ(st.x, 1);
        EXPECT_EQ(st.y, 2);
    }

    { // no metacommand
        std::string str("1 2");
        std::istringstream in(str);
        auto st = cpphots::createFromStream<StreamableTest>(in);
        EXPECT_EQ(st.x, 1);
        EXPECT_EQ(st.y, 2);
    }

    { // wrong metacommand
        std::string str("!WRONGMETA\n1 2");
        std::istringstream in(str);
        EXPECT_THROW(cpphots::createFromStream<StreamableTest>(in), std::runtime_error);
    }

    { // correct metacommand, strict
        std::string str("!STREAMABLETEST\n1 2");
        std::istringstream in(str);
        StreamableTest st(true);
        st.fromStream(in);
        EXPECT_EQ(st.x, 1);
        EXPECT_EQ(st.y, 2);
    }

    { // no metacommand, strict
        std::string str("1 2");
        std::istringstream in(str);
        StreamableTest st(true);
        EXPECT_THROW(st.fromStream(in);, std::runtime_error);
    }

    { // wrong metacommand, strict
        std::string str("!WRONGMETA\n1 2");
        std::istringstream in(str);
        StreamableTest st(true);
        EXPECT_THROW(st.fromStream(in);, std::runtime_error);
    }
    
    { // correct metacommand, whitespace
        std::string str("\n\n!STREAMABLETEST\n1 2");
        std::istringstream in(str);
        auto st = cpphots::createFromStream<StreamableTest>(in);
        EXPECT_EQ(st.x, 1);
        EXPECT_EQ(st.y, 2);
    }

}