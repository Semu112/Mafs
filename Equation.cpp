#include "Equation.h"

Equation::Equation(QStringView eqn)
{
    int termStart = 0;
    int termEnd = 0;

    int currTerm = 1;

    try {
        for(int i = 0; i < eqn.length(); i++) {
            if(eqn[i] == '=') {
                termEnd = i;
                this->terms.push_back(eqn.sliced(termStart, termEnd - termStart));
                termStart = i + 1;
                currTerm++;
            }
        }

        // Last term
        terms.push_back(eqn.sliced(termStart, eqn.length() - termStart));
    }
    catch (const char* err)
    {
        throw "term " + std::to_string(currTerm) + " " + err;
    }
}

//QString Equation::getSymbol()
//{
//    QString symbolString = "";

//    for(int i = 0; i < this->terms.size(); i++)
//    {
//        symbolString += terms.at(i).getSymbol() + ( i == terms.size() - 1 ? "" : "=" );
//    }

//    return symbolString;
//}

QString Equation::getCompressedDebug()
{
    QString compressedDebug = "";

    for(int i = 0; i < this->terms.size(); i++)
    {
        compressedDebug += this->terms.at(i).getCompressedDebug();
    }

    return compressedDebug;
}
