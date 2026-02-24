#include "StringUtils.h"
#include <cctype>
#include <vector>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{
    if(str.empty()){ //handle empty input string
        return "";
    }
    int size = str.length();

    if(start<0){ // change neg to pos index
        start = start + size;
    }

    if(end<0){ // change neg to pos index
        end = end + size;
    }
    if(end == 0){
        end = size;

    }
    else{
        if(start==end){
            return "";
        }
    }

    if(start > size){
        start = size;
    }
    if(start < 0){
        start = 0;
    }
    if(end<0){
        end = 0;
    }
    if(end>size){
        end = size;
    }

    if(start>=end){
        return "";
    }
    


    std::string result = "";
    for(int i = start; i<end; i++){
        result = result + str[i];
    }


    return result;
}

std::string Capitalize(const std::string &str) noexcept{
    if(str.empty()){ //handle empty input string
        return "";
    }
    std::string result = str;
    result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0]))); 

    return result;
}

std::string Upper(const std::string &str) noexcept{
    if(str.empty()){ //handle empty input string
        return "";
    }
    std::string result = str;
    for(int i = 0; i<result.length(); i++){
        result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
    }

    return result;
}

std::string Lower(const std::string &str) noexcept{
    if(str.empty()){ //handle empty input string
        return "";
    }
    std::string result = str;
    for(int i = 0; i<result.length(); i++){
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }

    return result;
}

std::string LStrip(const std::string &str) noexcept{
    int start = 0;
    for(int i = 0; i<str.length(); i++){
        if(std::isspace(str[i])){
            if(i == str.length()-1){ //return empty string if still no character occur until the last element(all of the element are space)
                return ""; 
            }
        }
        else{
            start = i;
            break;
        }
    }

    return StringUtils::Slice(str,start); // slice from the first non-space character to the end
}

std::string RStrip(const std::string &str) noexcept{
    int start = 0;
    for(int i = str.length()-1; i>=0; i--){
        if(std::isspace(str[i])){
            if(i == 0){ //return empty string if still no character occur until the first element(all of the element are space)
                return ""; 
            }
        }
        else{
            start = i;
            break;
        }
    }
    if(start == str.length()-1){
        return str; //return original string if nothing needs to be removed to solve the index out of range case
    }

    return StringUtils::Slice(str,0,start+1); 
}

std::string Strip(const std::string &str) noexcept{
    
    return StringUtils::RStrip(StringUtils::LStrip(str));
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    while(result.length()<width){
        result.push_back(fill);
        if(result.length()<width){
            result.insert(0,1,fill);
        }
    }
    return result;
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    while(result.length()<width){
        result.push_back(fill);
    }
    return result;
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    while(result.length()<width){
        result.insert(0,1,fill);
    }
    return result;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    if (old.empty()) {
        return str; // return original string id old is empty string
    }
    int oldlen = old.length();
    std::string result;
    int cur = 0; //current finding start index
    int found;
    while((found = str.find(old,cur))!= std::string::npos){

        result.append(str, cur, found-cur); //append substring before the replace part
        result.append(rep); // append the replace part
        cur = found+oldlen; //update the starting searching position 
    }
    result.append(str,cur,std::string::npos); //append the final part of the original string 

    return result;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    std::vector<std::string> result;
    std::string temp; //store single substring needs to be pushed into the vector
    int cur = 0;
    int found = 0;
    int spltlen = splt.length();
    if(splt == ""){  //empty seperator
        for(int i = 0; i<str.length(); i++){
            if(isspace(str[i])){
                if(temp != ""){
                    result.push_back(temp);
                    temp = "";
                }
            }
            else{
                temp.push_back(str[i]);
            }
        }
        if(temp != ""){
            result.push_back(temp); //append final string
        }
        return result;
    }
    else{
        while((found = str.find(splt,cur))!= std::string::npos){ 
            result.push_back(str.substr(cur, found - cur)); //append substring before the split
            cur = found+spltlen; //update the starting searching position 
        }
        result.push_back(str.substr(cur)); //append final string
        return result; 
    }
    
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    std::string result;
    for(int i = 0; i<vect.size(); i++){
        result.append(vect[i]);
        if(i != vect.size()-1){
            result.append(str);
        }
    }
    return result;
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{
    std::string result;
    int pos = 0;
    for(int i = 0; i<str.length(); i++){
        if(str[i] == '\t'){
            if(tabsize == 0){
                continue;
            }
            int spaces = tabsize - (pos % tabsize); //calculate how much space needed to reach the next tab
            result.append(spaces, ' ');
            pos = pos+spaces;
        }
        else{
            result.push_back(str[i]);//append normal char to the result
            if(str[i] == '\n'){
                pos = 0; //reset position if newline appear
            }
            else{
                pos = pos+1;
            }
        }
    }

    return result;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    int l = left.length();
    int r = right.length();
    std::vector<std::vector<int>> DP(l + 1, std::vector<int>(r + 1, 0)); //Create DP table

     //Initialize base case of the DP table
    for(int i = 0; i<=l; ++i){
        DP[i][0] = i;
    }
    for(int j = 0; j<=r; ++j){
        DP[0][j] = j;
    }

    //FIll the DP table
    for(int i = 1; i<=l; ++i){
        for(int j = 1; j<=r; ++j){
            char a = left[i-1];
            char b = right[j-1];
            
            if(ignorecase == true){ //change all the char to lower case to ignore cases
                a = std::tolower(a);
                b = std::tolower(b);
            }
            int cost;
            if(a == b){
                cost = 0;
            }
            else{
                cost = 1;
            }
            int findmin[3] = {DP[i-1][j]+1,DP[i][j-1]+1,DP[i-1][j-1]+cost}; //find minimum step among deletion, insertion and substitution
            int min = findmin[0];
            for(int m = 0; m<3; m++){
                if(findmin[m]<=min){
                    min = findmin[m];
                }
            }
            DP[i][j] = min;
        }
    }

    return DP[l][r];
}

};