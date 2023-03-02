#ifndef EQUATION_H
#define EQUATION_H

#include <QString>
#include <QStringView>

#include "Term.h"

class Equation {

public:
    /* If = is not found in equation, it should error */
    Equation(QStringView eqn);

    QString getCompressedDebug();
//    QString getSymbol();

private:
    std::vector<Term> terms;
};

#endif // EQUATION_H
