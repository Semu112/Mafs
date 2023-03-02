#include "Term.h"
#include "Functions.h"

#include <functional>

//bool checkIsLeaf(QStringView* term) {
//    QStringView::iterator itr = term->begin();

//    while ( itr != term->end() && !isDelimiter(*itr) )
//    {
//        itr++;
//    }

//    return itr == term->end();
//}

Term::Term(QStringView term) {

    this->parseTerm(term);

}

void Term::parseTerm(QStringView term)
{
    QStringView::const_iterator itr = term.constBegin();

    std::function<bool(QChar)> func;

    int order = getOrder(&itr, term.constEnd());

    itr = term.constBegin();

    switch (order)
    {
    case 0:
        itr = term.constBegin();

        this->isLeaf = true;

        while(itr < term.end())
        {
            if(!(*itr).isDigit()) {
                if (*itr != '*')
                {
                   this->symbolString += *itr;
                }
                itr++;
            } else {
                QString strCoefficient = *itr;

                itr++;

                while((*itr).isDigit()) {
                    strCoefficient += *itr;
                    itr++;
                }

                this->coefficient *= strCoefficient.toInt();
                this->coefficientSet = true;
            }
        }
        break;

    case 1:
        func = isOrder1Operator;
        break;

    case 2:
        func = isOrder2Operator;
        break;

    case 3:
        func = isOrder3Operator;
        break;
    }

    int bracketWeight = 0;
    int termStart = 0;
    int childTermIndex = 0;

    itr = term.constBegin();

    while( (itr < term.constEnd()) && bracketWeight == 0)
    {
        if(isOpenBracket(*itr))
        {
            bracketWeight++;
        }
        else if(isCloseBracket(*itr))
        {
            bracketWeight--;
        }
        else if(func(*itr))
        {
            this->symbolString += QString::number(childTermIndex) + *itr;

            int termLength = itr - term.constBegin() - termStart;
            this->childTerms.push_back(Term(term.sliced(termStart, termLength)));
            termStart += termLength + 1; // + 2 to move past the operator to the start of the next term
        }

        itr++;
    }
}

QString Term::getCompressedDebug()
{
    if(this->isLeaf)
    {
        return '\t' + ( (this->coefficientSet) ? QString::number(this->coefficient) : "" ) + this->symbolString + '\?';
    } else if(this->childTerms.size() > 0)
    {
        QString compressedDebug = '\t' + this->symbolString;

        for(int i = 0; i < this->childTerms.size() - 1; i++)
        {
            compressedDebug += '\t' + QString::number(i) + this->childTerms.at(i).getCompressedDebug() + '\?';
        }

        compressedDebug += '\t' + QString::number(this->childTerms.size() - 1) + '\r' + this->childTerms.at(this->childTerms.size()-1).getCompressedDebug() + '\?';
        compressedDebug += '\?';

        return compressedDebug;
    } else {
        return "";
    }
}
