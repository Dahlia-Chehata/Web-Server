#ifndef YAWS_STRING_UTILS_H_
#define YAWS_STRING_UTILS_H_

#include <string>
#include <vector>

using namespace std;

namespace yaws {
    static vector<string> split_text (string text, const std::string& delimiter) {

        size_t pos = 0;
        string token;
        std::vector<std::string> splited_text;

        while ((pos = text.find(delimiter)) != string::npos) {
            token = text.substr(0, pos);
            if(token.length() > 0) {
                splited_text.push_back(token);
            }
            text.erase(0, pos + delimiter.length());
        }

        if(text.length() > 0) {
            splited_text.push_back(text);
        }

        return splited_text;
    }
}

#endif // YAWS_STRING_UTILS_H_
