
# CDSVReader

## Purpose
`CDSVReader` reads delimiter-separated-value (DSV) text  from a `CDataSource`
and converts it into rows of `std::vector<std::string>`.




## Constructor
```cpp
CDSVReader(std::shared_ptr<CDataSource> src, char delimiter);
```
### Parameters
 - sink: Output destination  CStringDataSink in this case
 - delimiter: Character used to separate columns
- quoteall: flag to determine whether all fields need to be quoted

## Methods
```cpp
bool ReadRow(std::vector<std::string> &row)
```
### Parameters
None

### Returns
- TRUE if the all row was read successfully from input source

```cpp
bool End() const
```
### Parameters
`row`: Output vector where each element represents one column

### Returns
- TRUE if a row was successfully read
- FALSE if no more rows are available or error occur

## Use Example
```cpp
#include "DSVReader.h"
#include "StringDataSource.h"
#include <memory>
#include <vector>
#include <iostream>

int main(){
    auto src = std::make_shared<CStringDataSource>("A,B,C\n1,2,3\n");
    CDSVReader reader(src, ',');

    std::vector<std::string> row;
    while(reader.ReadRow(row)){
        for(const auto &col : row){
            std::cout << "[" << col << "] ";
        }
        std::cout << std::endl;
    }
}
```