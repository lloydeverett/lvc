//
// Reads a file into a std::string
// Taken from http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
//

#pragma once
#include <string>
#include <fstream>

inline std::string getFileContents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}
