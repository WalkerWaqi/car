#include "Tools.h"
#include <boost/lexical_cast.hpp>
#include <json.hpp>
using json = nlohmann::json;


template <typename ElemT>
struct HexTo {
    ElemT value;
    operator ElemT() const {return value;}
    friend std::istream& operator>>(std::istream& in, HexTo& out) {
        in >> std::hex >> out.value;
        return in;
    }
};

template <class T>
void AddJson(std::string& strBuf, json j)
{
    T tmp = 0;

    if (j.is_number())
    {
        tmp = j.get<T>();
    }
    else
    {
        tmp = boost::lexical_cast<HexTo<unsigned int>>(j.get<std::string>());
    }

    strBuf.append((char*)&tmp, sizeof(tmp));
}

int main(int argc, char* agv[]) {
    if (argc > 1) {
        std::ifstream iJson(agv[1]);
        json jData;
        iJson >> jData;

        for (json::iterator it = jData.begin(); it != jData.end(); ++it) {
            if (it.value().is_array())
            {
                std::vector<json> jData = it.value().get<std::vector<json>>();
                std::string strData;

                for(std::vector<json>::iterator it = jData.begin(); it!= jData.end(); ++it)
                {
                    if (it->is_array())
                    {
                        if (it->size() > 1)
                        {
                            if (it->at(0) == 1)
                            {
                                AddJson<unsigned char>(strData, it->at(1));
                            }
                            else if (it->at(0) == 2)
                            {
                                AddJson<unsigned short>(strData, it->at(1));
                            }
                            else if (it->at(0) == 4)
                            {
                                AddJson<unsigned int>(strData, it->at(1));
                            }
                        }
                    }
                }

                if (!strData.empty())
                {
                    std::string strFile = it.key();
                    Tools::write_file(strFile, strData);
                }
            }
        }
    }

    return 0;
}