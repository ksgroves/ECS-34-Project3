#include "DSVWriter.h"

#include <memory>
#include <string>
#include <vector>

struct CDSVWriter::SImplementation{
    std::shared_ptr<CDataSink> Sink;
    char Delimiter;
    bool QuoteAll;
    SImplementation(std::shared_ptr<CDataSink> sink,
                char delimiter,
                bool quoteall) //whether to force quotes around every cell
    {
        // Store the output destination (where data will be written)
        Sink = sink;

        // That prevents a delimiter of " (quote) because that would break CSV quoting rules.
        // So if someone passes ", you replace it with ,
        if (delimiter == '"') {
        Delimiter = ',';
        } else {
            Delimiter = delimiter;
        }

        // Store whether all values should be quoted
    QuoteAll = quoteall;
    }

    bool Write1Char(char ch){ //Writes one character to sink
        return Sink && Sink->Put(ch);  
    }
    bool WriteString(const std::string &s){ //converts a string into vector<char>, calls Sink->Write(buf)
        if(!Sink) return false;
        if(s.empty()) return true;

        std::vector<char> buf(s.begin(), s.end());
        return Sink->Write(buf);
    }

    bool SpecialChar(const std::string &s, char c){ //detect special characters
        return s.find(c) != std::string::npos;
    }
    //EscapeQuotes converts every double quote (") inside a string into two double quotes ("") so the string can be safely written inside a quoted DSV field.
    //"" means a literal quote inside the field.
    std::string EscapeQuotes(const std::string &s){
        std::string out;
        for(char ch : s){ //Loops through every character in the input string s.
            if(ch == '"'){ //Checks whether the character is a double quote.
                out.push_back('"');
                out.push_back('"');
            }
            else{
                out.push_back(ch);//If the character is not a quote, copy original.
            }
        }
        return out;
    }

    bool WriteCell(const std::string &cell){
        //A cell must be quoted if quoteall is ON, 
	    //or it contains “special” characters that could break parsing
        bool quote = QuoteAll || SpecialChar(cell,Delimiter) || SpecialChar(cell, '"') || SpecialChar(cell, '\n'); //See if quoting is needed
        std::string escaped = EscapeQuotes(cell); //it escapes quotes: " becomes ""
        if(quote){ //if quoting is needed:write " -> write escaped content -> write "
           if(!Write1Char('"')) return false;
            if(!WriteString(escaped)) return false;
            if(!Write1Char('"')) return false;
        } //no need quotning
        else{ //otherwise write escaped content directly
           if(!WriteString(escaped)) return false;
        }
         return true;
    }


};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
{
    DImplementation = std::make_unique<SImplementation>(sink,delimiter,quoteall);
};

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string> &row){
    if(row.empty()){ //Empty Row
        return DImplementation->Write1Char('\n'); //Writes only a newline character.
    }
    for(int i = 0; i<row.size(); i++){ //Loop over every element in the row.
        if(i != 0){
            if(!DImplementation->Write1Char(DImplementation->Delimiter)){ //delimiter between every except the first cell
                return false;
            }

        }
                if(!DImplementation->WriteCell(row[i])){ //write cell
                return false;
            }
    }
    return DImplementation->Write1Char('\n'); //end of row
}