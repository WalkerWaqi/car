//
// Created by walker on 17-2-5.
//

#ifndef CAR_TOOLS_H
#define CAR_TOOLS_H


#include <fstream>
#include <sstream>

class Tools {
public:
    static int read_file(std::string& file, std::string &data)
    {
        int nRet = -1;

        std::ifstream rfile(file, std::ios::binary);
        if (!rfile.fail())
        {
            std::istreambuf_iterator<char> beg(rfile), end;
            data.append(beg, end);

            rfile.close();

            nRet = 0;
        }

        return nRet;
    }

    static int write_file(std::string& file, std::string &data)
    {
        int nRet = -1;

        std::ofstream wfile(file, std::ios::binary);
        if (!wfile.fail())
        {
            wfile << data;
            wfile.flush();
            wfile.close();

            nRet = 0;
        }

        return nRet;
    }

    static std::string char2hex(std::string const &s)
    {
        std::string ret;
        for (unsigned i = 0; i != s.size(); ++i)
        {
            char hex[3];
            sprintf(hex, "%.2x ", (unsigned char)s[i]);
            ret += hex;
        }
        return ret;
    }

    static std::string hex2char(std::string const &s)
    {
        std::string ret;
        std::istringstream iss(s);
        for (std::string buf; std::getline(iss, buf, ' ');)
        {
            unsigned int value;
            sscanf(buf.c_str(), "%x", &value);
            ret += ((char)value);
        }
        return ret;
    }
};


#endif //CAR_TOOLS_H
