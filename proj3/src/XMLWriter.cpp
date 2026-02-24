#include "XMLWriter.h"

#include <vector>
#include <string>
//CXMLWriter takes an SXMLEntity (StartElement / EndElement / CharData / CompleteElement) and writes the correct XML text into a CDataSink.
//It also keeps a stack (DOpenTags) of currently-open tags so Flush() can close any unclosed tags.
struct CXMLWriter::SImplementation{
    std::shared_ptr<CDataSink> DSink;
    std::vector<std::string> DOpenTags; //stack,when you write <a> you push "a"
	                                    //when you write </a> you pop "a"

    SImplementation(std::shared_ptr<CDataSink> sink){
        DSink = sink;
    }

    ~SImplementation(){
    }

    bool PutChar(char ch){ //Writes one character to the sink safely (fails if DSink is null).
        return DSink && DSink->Put(ch);
    }

    bool PutString(const std::string &s){ //Writes a whole string by converting to a byte buffer.
        if(!DSink) return false;
        std::vector<char> buf(s.begin(), s.end());
        return DSink->Write(buf);
    }

    //Escape converts special characters to XML entities so the output stays valid XML
    //Example: 5 < 10 & 7 > 2 -> 5 &lt; 10 &amp; 7 &gt; 2
    std::string Escape(const std::string &s){
        std::string out;
        for(char ch : s){
            if(ch == '&') out += "&amp;";
            else if(ch == '<') out += "&lt;";
            else if(ch == '>') out += "&gt;";
            else if(ch == '"') out += "&quot;";
            else out.push_back(ch);
        }
        return out;
    }

    bool WriteAttributes(const TAttributes &attrs){
        for(auto &A : attrs){
            // A = (name, value)
            if(!PutChar(' ')) return false;
            if(!PutString(A.first)) return false;
            if(!PutString("=\"")) return false;
            if(!PutString(Escape(A.second))) return false;
            if(!PutChar('"')) return false;
        }
        return true;
    }

    bool WriteStart(const SXMLEntity &e){
        if(!PutChar('<')) return false;
        if(!PutString(e.DNameData)) return false;
        if(!WriteAttributes(e.DAttributes)) return false;
        if(!PutChar('>')) return false;

        //push tag so Flush can close it later
        DOpenTags.push_back(e.DNameData);
        return true;
    }

    bool WriteEnd(const SXMLEntity &e){ //writes a closing XML tag, Example: </item>
        if(!PutString("</")) return false;
        if(!PutString(e.DNameData)) return false;
        if(!PutChar('>')) return false;

        // pop if it matches the last started
        if(!DOpenTags.empty() && DOpenTags.back() == e.DNameData){
            DOpenTags.pop_back();
        }
        return true;
    }

    bool WriteComplete(const SXMLEntity &e){
        //In XML, a complete element is an element that: Has no content and Opens and closes in one tag
        //Example: <br/>,<img src="a.png"/>,<item id="3"/>
        //Write from left to right
        if(!PutChar('<')) return false;
        if(!PutString(e.DNameData)) return false;
        if(!WriteAttributes(e.DAttributes)) return false;
        if(!PutString("/>")) return false;
        return true;
    }

    bool WriteCharData(const SXMLEntity &e){ //Writes text content between tags, escaped.
        return PutString(Escape(e.DNameData));
    }

    bool WriteEntity(const SXMLEntity &e){ //This is the “one public entry point” used by the class
        switch(e.DType){
            case SXMLEntity::EType::StartElement:
                return WriteStart(e);
            case SXMLEntity::EType::EndElement:
                return WriteEnd(e);
            case SXMLEntity::EType::CompleteElement:
                return WriteComplete(e);
            case SXMLEntity::EType::CharData:
                return WriteCharData(e);
        }
        return false;
    }

    bool Flush(){
        // Close everything still open
        while(!DOpenTags.empty()){
            std::string name = DOpenTags.back();
            DOpenTags.pop_back();

            if(!PutString("</")) return false;
            if(!PutString(name)) return false;
            if(!PutChar('>')) return false;
        }
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink){
    DImplementation = std::make_unique<SImplementation>(sink);
}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush(){
    return DImplementation->Flush();
}

