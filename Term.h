#ifndef TERM_H
#define TERM_H

#include <QStringView>
#include <QString>
#include <vector>

class Term
{

    public:
    Term(){};
        Term(QStringView term);

//        QString getSymbol();
        QString getCompressedDebug();

        void parseTerm(QStringView term);

        QStringView getSymbolString() {return QStringView(this->symbolString);}

    private:
        bool isLeaf;

        bool coefficientSet = false;
        int coefficient = 1;
        QString symbolString;

        std::vector<Term> childTerms;
};

#endif // EXPRESSION_H
