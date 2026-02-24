#include <gtest/gtest.h>

#include "DSVWriter.h"
#include "StringDataSink.h"
#include "DSVReader.h"
#include "StringDataSource.h"

//Writer
TEST(DSVWriterTest, SimpleRow) {
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row = {"A", "B", "C"};

    EXPECT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(sink->String(), "A,B,C\n");
}

TEST(DSVWriterTest, DelimiterInside) { //test quoting when delimiter in input
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    writer.WriteRow({"A", "B,C", "D"});

    EXPECT_EQ(sink->String(), "A,\"B,C\",D\n");
}

TEST(DSVWriterTest, QuoteEscape) { //quotes inside input
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    writer.WriteRow({"A", "B\"C"});

    EXPECT_EQ(sink->String(), "A,\"B\"\"C\"\n");
}

TEST(DSVWriterTest, Newline) { //newline indise input
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    writer.WriteRow({"A", "B\nC"});

    EXPECT_EQ(sink->String(), "A,\"B\nC\"\n");
}

TEST(DSVWriterTest, QuoteAll) { //quoteall == true senario
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',', true);

    writer.WriteRow({"A", "B", "C"});

    EXPECT_EQ(sink->String(), "\"A\",\"B\",\"C\"\n");
}

TEST(DSVWriterTest, EmptyRow) { //empty row
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');
    std::vector<std::string> row;
    writer.WriteRow(row);
    EXPECT_EQ(sink->String(), "\n");
}

TEST(DSVWriterTest, MultipleRows) { //multiple row input
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    writer.WriteRow({"A", "B"});
    writer.WriteRow({"1", "2"});

    EXPECT_EQ(sink->String(), "A,B\n1,2\n");
}

//Reader

TEST(DSVReaderTest, SimpleRow) {
    auto src = std::make_shared<CStringDataSource>("A,B,C\n");
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "B");
    EXPECT_EQ(row[2], "C");
}
//Multi rows
TEST(DSVReaderTest, MultipleRows) {
    auto src = std::make_shared<CStringDataSource>(
        "A,B,C\n"
        "D,E,F\n"
    );
    CDSVReader reader(src, ',');

    std::vector<std::string> row;

    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "B");
    EXPECT_EQ(row[2], "C");

    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row[0], "D");
    EXPECT_EQ(row[1], "E");
    EXPECT_EQ(row[2], "F");

    EXPECT_FALSE(reader.ReadRow(row)); // no more rows
}

TEST(DSVReaderTest, NoTrailingNewline) {
    auto src = std::make_shared<CStringDataSource>("A,B,C");
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[2], "C");
}

TEST(DSVReaderTest, QuotedDelimiter) {
    auto src = std::make_shared<CStringDataSource>(
        "A,\"B,C\",D\n"
    );
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "B,C");
    EXPECT_EQ(row[2], "D");
}

TEST(DSVReaderTest, EscapedQuotes) {
    auto src = std::make_shared<CStringDataSource>(
        "A,\"B\"\"C\",D\n"
    );
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row[1], "B\"C");
}

TEST(DSVReaderTest, NewlineInsideQuotes) {
    auto src = std::make_shared<CStringDataSource>(
        "A,\"B\nC\",D\n"
    );
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row[1], "B\nC");
}

TEST(DSVReaderTest, TabDelimiter) {
    auto src = std::make_shared<CStringDataSource>(
        "A\tB\tC\n"
    );
    CDSVReader reader(src, '\t');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row[1], "B");
}

TEST(DSVReaderTest, EmptyFields) {
    auto src = std::make_shared<CStringDataSource>(
        "A,,C\n"
    );
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));

    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "");
    EXPECT_EQ(row[2], "C");
}

//TEst both Reader and writer

TEST(DSVFInalTest, SimpleRow){
    //write
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row = {"A", "B", "C"};
    ASSERT_TRUE(writer.WriteRow(row));
    //read
    auto src = std::make_shared<CStringDataSource>(sink->String());
    CDSVReader reader(src, ',');

    std::vector<std::string> out;
    ASSERT_TRUE(reader.ReadRow(out));
    EXPECT_EQ(out, row);
    EXPECT_TRUE(reader.End());
}

