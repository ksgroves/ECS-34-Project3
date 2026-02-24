# Project 2 README

Student 1: Mohai Sun (923236184)

Student 2: Kyle Groves  (922241439)

## Project Status
Complete

## Known Issues
No known Issue

## Code References
* [Expat Introduction](http://xml.com/pub/1999/09/expat/index.htmldifferent-ways-to-initialize-variables)
* [C++ Destructors](https://www.geeksforgeeks.org/cpp/destructors-c/)
* [C++ auto](https://www.w3schools.com/cpp/cpp_auto.asp)
* [DSV](https://en.wikipedia.org/wiki/Delimiter-separated_values)
* [Makefile Tutorial](https://makefiletutorial.com/#getting-started)



## Generative AI Use
I used ChatGPT for Prompt 1, and ZZZZZ for Prompt 2....

### Prompt 1
// Constructor for DSV reader, src specifies the data source and delimiter
// specifies the delimiting character
CDSVReader(std::shared_ptr< CDataSource > src, char delimiter);
// Destructor for DSV reader
~CDSVReader();
// Returns true if all rows have been read from the DSV
bool End() const;
// Returns true if the row is successfully read, one string will be put in
// the row per column
bool ReadRow(std::vector< std::string > &row);
// Constructor for DSV writer, sink specifies the data destination, delimiter
Project 2
1 of 6
ECS34 WQ26 January 17, 2026
// specifies the delimiting character, and quoteall specifies if all values
// should be quoted or only those that contain the delimiter, a double quote,
// or a newline
CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter,
bool quoteall = false);
// Destructor for DSV writer
~CDSVWriter();
// Returns true if the row is successfully written, one string per column
// should be put in the row vector
bool WriteRow(const std::vector< std::string > &row);

I don't quite understand How can I get input and output for the project. How can I use the datasource or datasink

### Response 1
Think of CDataSource / CDataSink as the project’s “pluggable I/O layer”.
	•	A data source is where bytes come from (file, string, stdin, network…).
	•	A data sink is where bytes go to (file, string, stdout…).
	•	CDSVReader and CDSVWriter sit on top of that and deal with parsing/formatting rows.

So you don’t “get input” by calling cin in CDSVReader. Instead you construct a suitable CDataSource (e.g., file source), pass it into CDSVReader, and then repeatedly call ReadRow.

Same idea for output: construct a CDataSink (e.g., file sink), pass it into CDSVWriter, and call WriteRow.

The wiring pattern you’ll use
### Reading
```cpp
std::shared_ptr<CDataSource> src = /* something that provides bytes */;
CDSVReader reader(src, ',');  // delimiter ',' or '\t' etc.

std::vector<std::string> row;
while (reader.ReadRow(row)) {
    // use row[0], row[1], ...
}
```

### Writing
```cpp
std::shared_ptr<CDataSink> sink = /* something that consumes bytes */;
CDSVWriter writer(sink, ',', /*quoteall=*/false);

std::vector<std::string> out = {"A", "B", "C"};
writer.WriteRow(out);
```
Where do the source/sink objects come from?

In most ECS34-style projects, the starter code gives you concrete classes like:
	•	CFileDataSource(filename) (reads bytes from a file)
	•	CFileDataSink(filename) (writes bytes to a file)
	•	sometimes CStringDataSource(string) / CStringDataSink(string&) (for testing)

Names vary a bit, but the idea is the same: you never instantiate CDataSource directly because it’s usually an abstract interface.


### Prompt 2
How can I use Expat for parsing XML

### Response 2
Here’s the practical “how to use Expat” pattern (the exact style your snippet is already following): create a parser → register callbacks → feed it bytes in chunks.

1) Core idea: Expat is event-driven (SAX-style)

Expat doesn’t “return a DOM tree.” Instead, it calls your functions when it sees things like:
	•	start tag: <tag ...>
	•	end tag: </tag>
	•	text between tags: hello

So your job is to implement handlers and store the events (or build your own tree).

### 2  Minimal Expat setup (C/C++)

Create parser + set handlers
```cpp
#include <expat.h>

struct ParserState {
    // put whatever you need here (queue, current text buffer, etc.)
};

static void Start(void* userData, const XML_Char* name, const XML_Char** atts) {
    auto* st = static_cast<ParserState*>(userData);

    // atts is a null-terminated list: key0, val0, key1, val1, ..., nullptr
    for (int i = 0; atts[i]; i += 2) {
        const char* key = atts[i];
        const char* val = atts[i+1];
        // store attribute
    }
    // store start-element event
}

static void End(void* userData, const XML_Char* name) {
    auto* st = static_cast<ParserState*>(userData);
    // store end-element event
}

static void Text(void* userData, const XML_Char* s, int len) {
    auto* st = static_cast<ParserState*>(userData);
    // NOTE: text may arrive in multiple chunks; append, don’t overwrite
    // store/append character data
}

int main() {
    ParserState state;

    XML_Parser p = XML_ParserCreate(nullptr);          // nullptr = default encoding
    XML_SetUserData(p, &state);
    XML_SetElementHandler(p, Start, End);
    XML_SetCharacterDataHandler(p, Text);

    // ... feed bytes using XML_Parse (see next section)

    XML_ParserFree(p);
}
```
### How this matches your CXMLReader design

In your snippet you push SXMLEntity into a queue inside handlers. That’s a great approach.

Typical structure:
	•	StartElementHandler → push StartElement(name, attributes)
	•	EndElementHandler → push EndElement(name)
	•	CharacterDataHandler → push CharData(text) or accumulate into a buffer until the next tag boundary (your choice)

One big gotcha:

Character data can be split

Expat may call Text() multiple times for what looks like one continuous string in the XML. So you must either:
	•	push multiple text events and let consumer join them, or
	•	append into a “current text” buffer and flush at the right times.

### Prompt 3
Can you explain briefly what does DSVReader and Writer require us todo

What is a DSV?

DSV = Delimiter-Separated Values
Examples:
	•	CSV → ,
	•	TSV → \t
	•	|-separated, etc.

Each row: value1<delim>value2<delim>value3\n
With quotes allowed: "hello, world",42,"line\nbreak"
CDSVReader — what you must implement

Goal

Read raw bytes from a CDataSource and return one parsed row at a time as vector<string>.

⸻

Your responsibilities

When ReadRow(row) is called:
	1. Read characters from the CDataSource
	•	Possibly one byte at a time
	•	Possibly buffered (your choice)
	2.	Split fields using the delimiter
	•	delimiter separates columns
	•	newline (\n) ends a row
	3.	Handle quoted fields
	•	Fields may start with "
	•	Inside quotes:
	•	delimiter does not end the field
	•	newline does not end the row
	•	"" means a literal "
	4.	Stop exactly at one row
	•	Do not read past the end of the row
	•	Save extra characters for the next call
	5.	Return value
	•	true → row successfully read
	•	false → no more rows (EOF)

⸻

End()

Returns true only after all input has been consumed and no partial row remains.

⸻

CDSVWriter — what you must implement

Goal

Take a vector<string> and write a correctly-formatted DSV row to a CDataSink.

Your responsibilities

When WriteRow(row) is called:
	1.	Write fields in order
    field0<delim>field1<delim>field2\n
	2.	Quote fields when necessary
A field must be quoted if:
	•	it contains the delimiter
	•	it contains a "
	•	it contains a newline
	•	OR quoteall == true
	3.	Escape quotes
	•	Every " inside a quoted field becomes ""
	4.	End the row with a newline
	5.	Return value
	•	true → row written successfully
	•	false → write failure
