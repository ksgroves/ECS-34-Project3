#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <vector>

struct CXMLReader::SImplementation{
    std::shared_ptr<CDataSource> DSource;
    XML_Parser DParser; //Expat parser object
    std::queue<SXMLEntity> DQueue; //events waiting to be returned (StartElement, EndElement, CharData)
    bool DFinished = false; //whether we have finished reading/parsing the source

    static void StartElementHandler(void *userData,const XML_Char *name,const XML_Char **atts){
        auto *This = static_cast<SImplementation*>(userData); //Convert userData back to SImplementation*
        SXMLEntity E; //Create a SXMLEntity E

        E.DType = SXMLEntity::EType::StartElement; 
        E.DNameData = name; 

        for(int i = 0; atts[i]; i += 2){  //Parse attributes: atts is a C array
            E.DAttributes.emplace_back(atts[i], atts[i+1]);
        }

        This->DQueue.push(E); //Push entity into DQueue
    }
    //Example:
    //<a x="1" y="2">
    //Entity produced:
    //Type: StartElement
    //NameData: "a"
    //Attributes: [("x","1"), ("y","2")]

    static void EndElementHandler(void *userData, //get end element
                                  const XML_Char *name){
        auto *This = static_cast<SImplementation*>(userData);
        SXMLEntity E;

        E.DType = SXMLEntity::EType::EndElement;
        E.DNameData = name;

        This->DQueue.push(E);
    }
    //Example:
    //</a>  Type: EndElement    NameData: "a"

    static void CharacterDataHandler(void *userData,
                                      const XML_Char *s,
                                      int len){
        auto *This = static_cast<SImplementation*>(userData);

        SXMLEntity E;
        E.DType = SXMLEntity::EType::CharData;
        E.DNameData.assign(s, len);

        This->DQueue.push(E);
    }
    //Example:
    //<a>hello</a>  Type: CharData  NameData: "hello"

    // Constructor
    SImplementation(std::shared_ptr<CDataSource> src)
        : DSource(src)
    {
        DParser = XML_ParserCreate(nullptr); //Creates parser
        XML_SetUserData(DParser, this); //Stores a pointer to your object inside Expat (userData)
        //Tells Expat which functions to call for events
        XML_SetStartElementHandler(DParser, StartElementHandler);
        XML_SetEndElementHandler(DParser, EndElementHandler);
        XML_SetCharacterDataHandler(DParser, CharacterDataHandler);
    }

    ~SImplementation(){
        XML_ParserFree(DParser);
    }

    //Core logic

    //Done if parsing finished (DFinished==true)AND there are no more events queued
    bool End() const{ 
        return DFinished && DQueue.empty();
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        while(true){
            // If we already have something queued, return it
            if(!DQueue.empty()){
                entity = DQueue.front();
                DQueue.pop();

                if(skipcdata &&
                   entity.DType == SXMLEntity::EType::CharData){
                    continue; //CharData entities get dropped, only Start/End elements are returned
                }

                return true;
            }

            // If finished and queue empty, return false
            if(DFinished){
                return false;
            }

            // Otherwise, read more input and parse it
            std::vector<char> Buffer(512);
            bool ReadOK = DSource->Read(Buffer, Buffer.size());

            if(!ReadOK){
                // Final parse
                XML_Parse(DParser, "", 0, true);
                DFinished = true;
            }
            else{
                XML_Parse(DParser,
                          Buffer.data(),
                          Buffer.size(),
                          false);
            }
        }
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src){
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity, skipcdata);
}