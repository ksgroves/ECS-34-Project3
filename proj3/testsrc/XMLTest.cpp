#include <gtest/gtest.h>
#include "XMLReader.h"
#include "StringDataSource.h"
#include "XMLWriter.h"
#include "StringDataSink.h"

TEST(XMLReaderTest, EmptyElement) {
    auto src = std::make_shared<CStringDataSource>("<a/>");
    CXMLReader reader(src);

    SXMLEntity e;

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(e.DNameData, "a");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(e.DNameData, "a");

    EXPECT_FALSE(reader.ReadEntity(e));
}

TEST(XMLReaderTest, TextElement) {
    auto src = std::make_shared<CStringDataSource>("<a>hello</a>");
    CXMLReader reader(src);

    SXMLEntity e;

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(e.DNameData, "a");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(e.DNameData, "hello");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(e.DNameData, "a");
}

TEST(XMLReaderTest, Attributes) {
    auto src = std::make_shared<CStringDataSource>(
        "<person age=\"20\" name=\"Alice\"></person>"
    );
    CXMLReader reader(src);

    SXMLEntity e;

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(e.DNameData, "person");

    EXPECT_TRUE(e.AttributeExists("age"));
    EXPECT_TRUE(e.AttributeExists("name"));
    EXPECT_EQ(e.AttributeValue("age"), "20");
    EXPECT_EQ(e.AttributeValue("name"), "Alice");
}

TEST(XMLReaderTest, NestedElements) {
    auto src = std::make_shared<CStringDataSource>(
        "<a><b>hi</b></a>"
    );
    CXMLReader reader(src);

    SXMLEntity e;

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DNameData, "a");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DNameData, "b");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(e.DNameData, "hi");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(e.DNameData, "b");

    ASSERT_TRUE(reader.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(e.DNameData, "a");
}

TEST(XMLReaderTest, WhitespaceCharData) {
    auto src = std::make_shared<CStringDataSource>(
        "<a>\n  hi \n</a>"
    );
    CXMLReader reader(src);

    SXMLEntity e;
    bool sawText = false;

    while(reader.ReadEntity(e)){
        if(e.DType == SXMLEntity::EType::CharData){
            sawText = true;
        }
    }

    EXPECT_TRUE(sawText);
}

TEST(XMLReaderTest, SkipCharData) {
    auto src = std::make_shared<CStringDataSource>("<a>text</a>");
    CXMLReader reader(src);

    SXMLEntity e;

    ASSERT_TRUE(reader.ReadEntity(e, true));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);

    ASSERT_TRUE(reader.ReadEntity(e, true));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);

    EXPECT_FALSE(reader.ReadEntity(e, true));
}

//XMLWriter

TEST(XMLWriterTest, SimpleElement){
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity e;

    e.DType = SXMLEntity::EType::StartElement;
    e.DNameData = "a";
    ASSERT_TRUE(writer.WriteEntity(e));

    e.DType = SXMLEntity::EType::CharData;
    e.DNameData = "hi";
    ASSERT_TRUE(writer.WriteEntity(e));

    e.DType = SXMLEntity::EType::EndElement;
    e.DNameData = "a";
    ASSERT_TRUE(writer.WriteEntity(e));

    EXPECT_EQ(sink->String(), "<a>hi</a>");
}

TEST(XMLWriterTest, Attributes){
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity e;
    e.DType = SXMLEntity::EType::CompleteElement;
    e.DNameData = "a";
    e.SetAttribute("x", "1");
    e.SetAttribute("y", "2");

    ASSERT_TRUE(writer.WriteEntity(e));

    EXPECT_EQ(sink->String(), "<a x=\"1\" y=\"2\"/>");
}

TEST(XMLWriterTest, NestedElements){
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity e;

    e.DType = SXMLEntity::EType::StartElement;
    e.DNameData = "a";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::StartElement;
    e.DNameData = "b";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::CharData;
    e.DNameData = "c";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::EndElement;
    e.DNameData = "b";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::EndElement;
    e.DNameData = "a";
    writer.WriteEntity(e);

    EXPECT_EQ(sink->String(), "<a><b>c</b></a>");
}

//Test both Reader an Writer

TEST(XMLFinalTest, WriteRead){
    // write XML
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity e;

    e.DType = SXMLEntity::EType::StartElement;
    e.DNameData = "a";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::CharData;
    e.DNameData = "hello";
    writer.WriteEntity(e);

    e.DType = SXMLEntity::EType::EndElement;
    e.DNameData = "a";
    writer.WriteEntity(e);

    //read XML
    auto src = std::make_shared<CStringDataSource>(sink->String());
    CXMLReader reader(src);

    SXMLEntity r;

    ASSERT_TRUE(reader.ReadEntity(r));
    EXPECT_EQ(r.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(r.DNameData, "a");

    ASSERT_TRUE(reader.ReadEntity(r));
    EXPECT_EQ(r.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(r.DNameData, "hello");

    ASSERT_TRUE(reader.ReadEntity(r));
    EXPECT_EQ(r.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(r.DNameData, "a");
}