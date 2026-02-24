#include "DSVReader.h"

#include <vector>  
#include <string>
#include <memory>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> Source;
    char Delim;
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter){ //When the reader is constructed, the implementation saves the input source and delimiter for later parsing.
        Source = src;
        Delim = delimiter;
    }
    

    bool GetChar(char &ch){ //reads one character and consumes it from the source, safe wrapper around Source->Get that prevents reading past the end.
        if(!Source || Source -> End()){
            return false;
        }
        return Source->Get(ch);
    }

        bool PeekChar(char &ch){ //This looks at the next character without consuming it
        if(!Source || Source -> End()){
            return false;
        }
        return Source->Peek(ch);
    }

};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter) //Creates the implementation object and stores it in a unique_ptr.
{
    DImplementation = std::make_unique<SImplementation>(src,delimiter);
};

CDSVReader::~CDSVReader() = default; //Uses default destructor

bool CDSVReader::End() const{ //End() is a quick check: if there’s no source or the source reports end, we’re done
    return !DImplementation || !DImplementation->Source || DImplementation->Source->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row){
    row.clear(); //Clears output vector so caller gets a fresh row
    if(!DImplementation || !DImplementation->Source){ //Validates pointers
        return false;
    }

    auto &src = DImplementation->Source; //Stores references for convenience
    const char delim = DImplementation->Delim;

    //stop at end
    if(src->End()){ //If already EOF before reading → no row
        return false;
    }
    std::string current; //current: builds the current field’s text
    bool inQuotes=false; //inQuotes: whether we are inside " ... "
    bool read= false; //read: tracks whether we consumed any characters
    
    while (true) { //Loop keeps consuming characters until it completes one row.
        char ch;

        // If we reach end-of-file after already reading part of a row, you still return that partial final row.
        if (src->End()) {
            // last field
            row.push_back(current);
            return true;
        }

        // Consume one character
        if (!DImplementation->GetChar(ch)) {
            // If Get failed but not End()
            return false;
        }

        read = true;

        if (!inQuotes) { //Case 1: NOT inside quotes
            // Delimiter ends the current field
            if (ch == delim) {
                row.push_back(current); //push it into row
                current.clear(); //clear current for next field
                continue;
            }

            //Newline ends the row
            if (ch == '\n') {
                row.push_back(current); //Push last field and return row
                return true;
            }

            //Handle Windows line ending
            // \r is used to return the cursor to the beginning of the current line
            // Windows systems use both a Carriage Return and a Line Feed(\r\n)
            if (ch == '\r') { //If line ends with \r\n
                char next;
                if (!src->End() && DImplementation->PeekChar(next) && next == '\n') { //peek next char,	if it’s \n, consume it
                    DImplementation->GetChar(next); // consume '\n'
                }
                row.push_back(current);//	return row
                return true;
            }

            // Quote at the start of a field begins a quoted field
            if (ch == '"' && current.empty()) {
                inQuotes = true;
                continue;
            }

            // Otherwise normal character
            current.push_back(ch);
            //quote make multiple char into 1 element and push together
            //Inside quotes, delimiter and newline are treated as normal characters
        } else {
            // Inside quotes
            if (ch == '"') {
                // Could be escaped quote ("") OR end of quoted section
                //nside quotes, when it sees ", it peeks ahead:
                //If the next char is also ", this means an escaped quote → "" becomes ".
                char next;
                if (!src->End() && DImplementation->PeekChar(next) && next == '"') {
                    // Escaped quote: consume second quote, append one quote
                    DImplementation->GetChar(next);
                    current.push_back('"');
                } else {
                    // End of quoted section
                    inQuotes = false;
                }
            } else {
                // Everything else inside quotes is literal (including delimiter and newline)
                current.push_back(ch);
            }
        }
    }
}