#include <QtTest/QTest>

#include "Functions.h"
#include "EqnGroup.h"
#include "Term.h"

class MafsTesting : public QObject {

    Q_OBJECT

public:
    MafsTesting(){};

private slots:

#pragma region unused {
//    void debugDecompression();
//    void debugDecompresssion_data();

//    void debugCompression();
//    void debugCompression_data();
#pragma endregion }


    void sanitise_test();
    void sanitise_test_data();

    void getOrder_test();
    void getOrder_test_data();

    void parse_test();
    void parse_test_data();

private:
    EqnGroup eqnGroup;

    Term term;
};

#pragma region unused (debugCompression and decompression) {
//void MafsTesting::debugDecompression()
//{
//    QFETCH(QString, string);
//    QFETCH(QString, result);

//    QCOMPARE(decompressDebug)
//}

//void MafsTesting::debugCompression()
//{
//    QFETCH(QString, eqn);
//    QFETCH(QString, compressedDebug);

//    this->eqnGroup.loadEqn(eqn);

//    std::cout << decompressDebug(this->eqnGroup.getCompressedDebug()).toStdString() << std::endl;

//    QCOMPARE(this->eqnGroup.getCompressedDebug(), compressedDebug);
//}

//void MafsTesting::debugCompression_data()
//{
//    QTest::addColumn<QString>("eqn");
//    QTest::addColumn<QString>("compressedDebug");

//    QTest::newRow("simple") << "4x" << "\t4x\?";
//}

#pragma endregion }

void MafsTesting::sanitise_test()
{
    QFETCH(QString, unsanitised);
    QFETCH(QString, sanitised);

    QCOMPARE(sanitise(QStringView(unsanitised)), sanitised);

    return;
}

void MafsTesting::sanitise_test_data()
{
    QTest::addColumn<QString>("unsanitised");
    QTest::addColumn<QString>("sanitised");

    QTest::newRow("leading '+' operator") << "+23" << "23";
    QTest::newRow("leading '+' operator in bracket") << "2*(+3+4)" << "2*(3+4)";
    QTest::newRow("multiple operators #1") << "--23" << "23";
    QTest::newRow("multiple operators #2") << "---23" << "-23";
    QTest::newRow("multiple operators #3") << "+++++23" << "23";
    QTest::newRow("multiple operators #4") << "++--+-23" << "-23";
    QTest::newRow("multiple operators #5") << "++--+-23+-+-42" << "-23+42";
    QTest::newRow("Brackets surrounding full term") << "(1)" << "1";
    QTest::newRow("Multiple brackets surrounding full term") << "(((((1)))))" << "1";
    QTest::newRow("Redundant brackets #1") << "(1+2)-3" << "1+2-3";
    QTest::newRow("Redundant brackets #2") << "12*x*(y/3)" << "12xy/3";
    QTest::newRow("Redundant brackets #3") << "(123)+3" << "123+3";
    QTest::newRow("Redundant brackets #4") << "(123)*3" << "123*3";
    QTest::newRow("Redundant brackets #5") << "(12)^5" << "12^5";
    QTest::newRow("Redundant brackets #6") << "(((((1)+2)+3)+4)+5)" << "1+2+3+4+5";
    QTest::newRow("Redundant brackets #7") << "(((((1)*2)/3)*4)/5)" << "1*2/3*4/5";
    QTest::newRow("Redundant brackets #8") << "(((((1)+2)+3)-4)*5)" << "(1+2+3-4)*5";
    QTest::newRow("Redundant brackets #9") << "3+(2*8)/9" << "3+2*8/9";
    QTest::newRow("Redundant brackets #10") << "2/(2*3)+2" << "2/2*3+2";
    QTest::newRow("Redundant brackets #11") << "2*(((1)2))" << "2*1*2";
    QTest::newRow("Redundant brackets #12") << "2*((1)+2)" << "2*(1+2)";
    QTest::newRow("Redundant brackets #13") << "2*(((1)*(2*4))*3)" << "2*1*2*4*3";
    QTest::newRow("Non-redundant brackets #1") << "(123xyz)^2" << "(123xyz)^2";
    QTest::newRow("Non-redundant brackets #2") << "(12*2)^2" << "(12*2)^2";
    QTest::newRow("Non-redundant brackets #3") << "(1+2)*3" << "(1+2)*3";
    QTest::newRow("Non-redundant brackets #4") << "2/(2+3)" << "2/(2+3)";
    QTest::newRow("Non-redundant brackets #5") << "3*(1+2)" << "3*(1+2)";
    QTest::newRow("Non-redundant brackets #6") << "12(1+2)" << "12*(1+2)";
    QTest::newRow("Non-redundant brackets #7") << "(1+2)x" << "(1+2)*x";
    QTest::newRow("Non-redundant brackets #8") << "2*((23)+1)" << "2*(23+1)";
    QTest::newRow("Add multiply #1") << "4x(2+3)" << "4x*(2+3)";
    QTest::newRow("Add multiply #2") << "4x^2" << "4*x^2";
    QTest::newRow("Add multiply #3") << "a123^2" << "a*123^2";
    QTest::newRow("Don't remove multiply #1") << "4x*(2+3)" << "4x*(2+3)";
    QTest::newRow("Don't remove multiply #2") << "4*2" << "4*2";
    QTest::newRow("Don't remove multiply #3") << "4*a^2" << "4*a^2";
    QTest::newRow("Don't remove multiply #4") << "4*\\ln{2}" << "4*\\ln{2}";
    QTest::newRow("Remove multiply #1") << "4*x*(2*x*y)" << "4x2xy";
    QTest::newRow("Remove multiply #2") << "α*2" << "α2";
    QTest::newRow("Remove multiply #3") << "2*Ψ" << "2Ψ";
    QTest::newRow("Remove multiply #4") << "ρ*σ" << "ρσ";
    QTest::newRow("Remove multiply on greek letters") << "σ*ρ*Ψ*α*β*γ*δ*Γ*Δ*ε*ζ*η*Θ*θ*ι*κ*Λ*λ*μ*ν*Ξ*ξ*Π*π*ρ*σ*ς*τ*υ*Φ*φ*χ*ψ*Ω*ω" << "σρΨαβγδΓΔεζηΘθικΛλμνΞξΠπρσςτυΦφχψΩω";

    return;
}

void MafsTesting::getOrder_test()
{
    QFETCH(QString, term);
    QFETCH(int, order);

    QString sanitised = sanitise(term);
    QString::const_iterator itr = sanitised.constBegin();

    QCOMPARE(getOrder(&itr, sanitised.constEnd()), order);
}

void MafsTesting::getOrder_test_data()
{
    QTest::addColumn<QString>("term");
    QTest::addColumn<int>("order");

    QTest::addRow("order 0 test #1") << "12" << 0;
    QTest::addRow("order 0 test #2") << "(12)" << 0;
    QTest::addRow("order 0 test #3") << "(((12xyz)))" << 0;
    QTest::addRow("order 0 test #4") << "12*x*(y*z)" << 0;
    QTest::addRow("order 0 test #5") << "-12x" << 0;
    QTest::addRow("order 0 test #6") << "+-+12x" << 0;
    QTest::addRow("order 1 test #1") << "12^2" << 1;
    QTest::addRow("order 1 test #2") << "(12)^2" << 1;
    QTest::addRow("order 1 test #3") << "(12*x*y^2)^3" << 1;
    QTest::addRow("order 1 test #4") << "-12^2" << 1;
    QTest::addRow("order 1 test #5") << "(12^x)^y" << 1;
    QTest::addRow("order 2 test #1") << "(12*x*y)/3" << 2;
    QTest::addRow("order 2 test #2") << "2/3" << 2;
    QTest::addRow("order 2 test #3") << "2/(3+1)" << 2;
    QTest::addRow("order 2 test #4") << "(12+3*2^3)*12x*y*z" << 2;
    QTest::addRow("order 2 test #5") << "(12+3)*2^3" << 2;
    QTest::addRow("order 2 test #6") << "(1+2/3^4)xyz" << 2;
    QTest::addRow("order 3 test #1") << "1+2" << 3;
    QTest::addRow("order 3 test #2") << "(1*2)+(3*4)" << 3;
    QTest::addRow("order 3 test #3") << "1+2^3" << 3;

    return;
}

void MafsTesting::parse_test()
{
    QFETCH(QStringView, eqn);
    QFETCH(QString, parentSymbolString);

    this->term.parseTerm(eqn);

    QCOMPARE(this->term.getSymbolString(), parentSymbolString);
}

void MafsTesting::parse_test_data()
{
    QTest::addColumn<QString>("eqn");
    QTest::addColumn<QString>("parentSymbolString");

    QTest::addRow("Carat recursion #1") << "2^x^z" << "0^1: 0-->2, 1-->(0^1: 0-->x, 1-->z)";
    QTest::addRow("Carat recursion #2") << "(2^x)^z" << "0^1: 0-->(0^1: 0-->2; 1-->x); 1-->z";
    QTest::newRow("Special parse #1") << "\\frac{1}{2}" << "\\frac{1}{2}";
    QTest::newRow("Special parse #2") << "" << "";
}

QTEST_MAIN(MafsTesting)
#include "tests.moc"
