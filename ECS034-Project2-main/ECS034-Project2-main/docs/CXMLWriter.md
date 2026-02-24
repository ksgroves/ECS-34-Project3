# CXMLWriter

## Purpose
`CXMLWriter` writes XML formatted output to a `CDataSink`. Each call to
`WriteEntity` outputs one XML entity. The writer supports start elements,
end elements, complete elements, and character data.

---

## Constructor

```cpp
CXMLWriter(std::shared_ptr<CDataSink> sink);
```

## Destructor
```cpp
~CXMLWriter();
```

## Methods
```cpp
bool WriteEntity(const SXMLEntity &entity)
```
### Parameters
- `entity`: XML entity to be written to the output stream
### Returns
- TRUE if the entity was written successfully
- FALSE if writing fails

```cpp
bool Flush()
```
### Parameters
None
### Returns
- TRUE if all open XML elements were successfully closed
- 	FALSE if writing fails

## Use Example
```cpp
#include "XMLWriter.h"
#include "StringDataSink.h"
#include <memory>
#include <iostream>

int main(){
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);

    SXMLEntity entity;

    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "a";
    writer.WriteEntity(entity);

    entity.DType = SXMLEntity::EType::CharData;
    entity.DNameData = "hello";
    writer.WriteEntity(entity);

    entity.DType = SXMLEntity::EType::EndElement;
    entity.DNameData = "a";
    writer.WriteEntity(entity);

    std::cout << sink->String() << std::endl;
}

```