# CXMLReader

## Purpose
`CXMLReader` reads XML formatted input from a `CDataSource` and parses it into
a sequence of `SXMLEntity` objects. Each call to `ReadEntity` returns the next
XML entity in the input stream.

XML parsing is implemented using the Expat library.

---

## Constructor

```cpp
CXMLReader(std::shared_ptr<CDataSource> src);
```
## Destructor
```cpp
~CXMLReader();
```

## Methods
```cpp
bool End() const
```
### Parameters
None
### Returns
- TRUE if all XML entities have been read
- FALSE otherwaise

```cpp
bool ReadEntity(SXMLEntity &entity, bool skipcdata = false)
```
### Parameters
 - `entity`: output parameter that receives the parsed XML entity
- `skipcdata`:
    - FALSE to return both element and character data entities
    - TRUE to skip character data entities
### Returns
- TRUE if an entity was successfully read
- FALSE if no more entities are available

## Use Example
```cpp
#include "XMLReader.h"
#include "StringDataSource.h"
#include <memory>
#include <iostream>

int main(){
    auto src = std::make_shared<CStringDataSource>("<a x=\"1\">hello</a>");
    CXMLReader reader(src);

    SXMLEntity entity;
    while(reader.ReadEntity(entity)){
        if(entity.DType == SXMLEntity::EType::StartElement){
            std::cout << "Start Element: " << entity.DNameData << std::endl;
        }
        else if(entity.DType == SXMLEntity::EType::CharData){
            std::cout << "Character Data: " << entity.DNameData << std::endl;
        }
        else if(entity.DType == SXMLEntity::EType::EndElement){
            std::cout << "End Element: " << entity.DNameData << std::endl;
        }
    }
}

```