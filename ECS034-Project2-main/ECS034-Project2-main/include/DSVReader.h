#ifndef DSVREADER_H
#define DSVREADER_H

#include <memory>
#include <string>
#include "DataSource.h"

class CDSVReader{ // CDSVReader is a class responsible for reading delimiter-separated value data
    private: //Everything below this keyword is private — only accessible inside the class.
        struct SImplementation;  
        std::unique_ptr<SImplementation> DImplementation; //This is a smart pointer that owns an instance of SImplementation, Automatically deletes the implementation when the reader is destroyed

    public:
        CDSVReader(std::shared_ptr< CDataSource > src, char delimiter); //constructor for CDSVReader.
        ~CDSVReader(); //The destructor releases all internal resources.

        bool End() const;
        bool ReadRow(std::vector<std::string> &row);
};

#endif
