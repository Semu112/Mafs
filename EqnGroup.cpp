#include "EqnGroup.h"

EqnGroup::EqnGroup(QStringView eqnGroup)
{
    this->loadEqn(eqnGroup);
}

void EqnGroup::operator=(QStringView newEqnGroup)
{
    this->loadEqn(newEqnGroup);
}

QString EqnGroup::getCompressedDebug()
{
    QString totalSymbolString = "";

    for(int i = 0; i < this->equations.size(); i++)
    {
        totalSymbolString += equations.at(i).getCompressedDebug();
    }

    return totalSymbolString;
}

void EqnGroup::loadEqn(QStringView newEqnGroup)
{
    int currEqn = 0;

    try{
        this->equations.clear();

        int eqnStart = 0;

        for(int i = 0; i < newEqnGroup.length(); i++) {

            if(newEqnGroup[i] == '\n'
                    && i != newEqnGroup.length()-1 // If it's not the last character
                    && newEqnGroup[i + 1] != '\n') { // If it's not immediately followed by another \n

                this->equations.at(currEqn) = Equation(newEqnGroup.sliced(eqnStart, i - eqnStart));
                eqnStart = i + 1;
                currEqn++;
            }
        }

        // For the last equation
        this->equations.push_back(Equation(newEqnGroup.sliced(eqnStart, newEqnGroup.length() - eqnStart)));
    }
    catch (std::string errText)
    {
        throw "Equation " + std::to_string(currEqn + 1) + " " + errText;
    }
}
