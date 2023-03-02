#ifndef EQNGROUP_H
#define EQNGROUP_H

#include "Equation.h"

#include <QString>
#include <vector>
#include <array>

class EqnGroup
{
public:
    EqnGroup(QStringView totalInput);
    EqnGroup() {}

    bool compare(int eqn1, int eqn2);

    void operator=(QStringView newEqnGroup);

    // Debug
    QString getCompressedDebug();

    void loadEqn(QStringView newEqnGroup); // For parsing a new equation (AOWT (As of writing this), only thing constructor does is call this method)

private:
    std::vector<Equation> equations;
};

#endif // EQNGROUP_H
