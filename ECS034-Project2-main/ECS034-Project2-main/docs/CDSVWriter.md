# CDSVWriter

## Purpose
`CDSVWriter` writes delimiter-separated-value (DSV) formatted output 
to a `CDataSink`. Each call to `WriteRow` outputs one row of data.

---

## Constructor

```cpp
CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall = false);
## Destructor
```cpp
~CDSVWriter();
```

## Methods
```cpp
bool WriteRow(const std::vector<std::string> &row)
```
### Parameters
- row: vector containing one string per column
### Returns
- TRUE if the row was written successfully
- FALSE if writing fails

## Use Example
```cpp
#include "DSVWriter.h"
#include "StringDataSink.h"
#include <memory>
#include <vector>
#include <iostream>

int main(){
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',', false);

    writer.WriteRow({"A", "B", "C"});
    writer.WriteRow({"1", "2", "3"});

    std::cout << sink->String() << std::endl;
}
```