#include "ppMacro.h"

std::string preProcess(std::string filename)
{
    std::ifstream fin(filename);
    std::ofstream fout(filename + "pp");
    std::string fileContent;
    std::vector<macroTable> Macros;
    // std::stringstream processtream;

    while (getline(fin, fileContent))
    {
        std::stringstream ss;
        std::string tmp;
        ss << fileContent;
        ss >> tmp;
        if (tmp.size() != 0)
        {
            if (tmp == "#define")
            {
                std::vector<std::string> lineTokens;
                while (ss >> tmp)
                    lineTokens.push_back(tmp);

                int rec = 0;
                std::string firstPart, secondPart;
                if (lineTokens[0].find('(') != lineTokens[0].npos)
                {
                    for (int i = 0;; i++)
                    {
                        firstPart += lineTokens[i] + " ";
                        if (lineTokens[i].find(')') != lineTokens[i].npos)
                        {
                            rec = i;
                            break;
                        }
                    }
                }
                else
                    firstPart = lineTokens[0];
                for (int i = rec + 1; i < lineTokens.size(); i++)
                {
                    secondPart += lineTokens[i];
                }
                Macros.push_back(macroTable(firstPart, secondPart));
            }
            else
            {
                for (auto macro : Macros)
                {
                    if (macro.match(fileContent))
                    {
                        fileContent = macro.replace(fileContent);
                    }
                }
                fout << fileContent << std::endl;
                // processtream << fileContent << std::endl;
            }
        }
    }
    fin.close();

    return filename + "pp";
}