#include "mafs.h"
#include "./ui_mafs.h"

#include "EqnGroup.h"
#include "Functions.h"

#include <functional>
#include <QString>

Mafs::Mafs(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Mafs)
//    , debuggingOutputTester("C:/Users/seamu/Documents/QT projects/Mafs/debugOutputTesting.tst")
{
    ui->setupUi(this);
}

Mafs::~Mafs()
{
    delete ui;
}

/**
 * @brief Should remove redundant brackets
    * @note I want redundant brackets not to be parsed, for performance, because otherwise ((((1)))) would be a cesspool of child terms
    *  @def Redundant bracket:
    *      @memberof Any brackets that surround the entire term
    *      @memberof Any brackets that do not affect the order of operations
    *          @def brackets that surround terms that have an order greater than or equal to the order immediately outside the bracket
    *              @example (1+2)-3
    *              @example 12*x*(y/3)
    *              @example (1*3x/123z)+2
    *              @example (123)+3
    *              @example (123)*3
    *              @example (12)^2
    *              @omit (123xyz)^2
    *              @omit (12*2)^2
    *              @omit (1+2)*3
    * @example I want (((1+2)+3)) to be parsed as 0+1+2, 0 ---> 1, 1--->2, 2--->3
    * @example I want (((12x)(2xy + 3)) to be parsed as 0*1, 0 ---> 12x, 1 ---> 0 + 1, 0 ---> 2xy, 1 ---> 3
*/
QString removeRedundantBrackets(QStringView input)
{
    QString sanitised = "";

    QStringView::const_iterator itr = input.constBegin();

    while(itr < input.constEnd()) {

        if( isOpenBracket(*itr) ) //Remove redundant brackets
        {
            bool needsMultiplyBefore = false;

            // Check if the character before the open bracket is not an operator, if not, store in needsMultiply to add later
            if (itr > input.constBegin() && !isOperator(*(itr-1)))
            {
                needsMultiplyBefore = true;
            }

            int outerOrderL = 0;
            if ( (itr - 1) > input.constBegin())
            {
                outerOrderL = getOperatorOrder(*(itr-1));
            }

            itr++; // to move away from the open bracket
            if (itr > input.constEnd())
            {
                throw "Not enough close brackets found ot match opening ones";
            }
            qsizetype termBegin = itr - input.constBegin();

            int innerOrder = getOrder(&itr, input.constEnd()); // iterator is moved to the end character in term

            QString substr = removeRedundantBrackets(input.sliced(termBegin, itr - input.constBegin() - termBegin));

            itr++;
            if (itr > input.constEnd())
            {
                throw "Not enough close brackets found to match opening brackets";
            }

            bool needsMultiplyAfter = false;

            if( ( itr < input.end() ) && !isOperator(*itr) )
            {
                needsMultiplyAfter = true;
            }

            int outerOrderR = ( itr != input.end() ) ? getOperatorOrder(*itr) : 0;

            int maxOuterOrder = std::max(outerOrderL, outerOrderR);

            if ( (innerOrder <= maxOuterOrder) || ( (outerOrderL == outerOrderR) && (outerOrderL == 0))) // Not really sure why outerOrderL and outerOrderR can be 0
            {
                sanitised += ( needsMultiplyBefore ? "*" : "" ) + substr + ( needsMultiplyAfter ? "*" : "");
            }
            else {
                sanitised += ( needsMultiplyBefore ? "*(" : "(" ) + substr + ( needsMultiplyAfter ? ")*" : ")");
            }
        }
        else
        {
            sanitised += *itr;
            itr++;
        }
    }


    return sanitised;
}


/**
 * @brief Should remove redundant spaces and operators (multiple +'s, -'s, and some *'s)
*  @section *'s
*   *'s should be added if the term before a special term (\), a bracket, or the term before a ^ is not an operator
*   otherwise *'s should be removed, unless it's between two numbers
*/
QString resolveSyntaxRedundancies(QStringView input)
{
    QString sanitised = "";

    QStringView::const_iterator itr = input.constBegin();

    while(itr < input.constEnd()) {

        if ( itr < input.end() - 1 && (*itr == '\\' && *(itr+1) == ' ') ) { // Spaces
            itr += 2;
        }
        else if( *itr == '+' || *itr == '-') /* repeated operators */
        {
            bool operatorIsNegative = false;

            bool isTermStart = ( itr == input.constBegin() ) || ( (*(itr-1)) == '(' );

            while( itr < input.end() && (*itr == '+'|| *itr == '-') ) {

                if(*itr == '-') {
                    operatorIsNegative = !operatorIsNegative;
                }

                itr++;
            }

            if(operatorIsNegative) {
                sanitised += '-';
            } else {
                if (!isTermStart)
                {
                    sanitised += '+';
                }
            }

            if( itr == input.end() )
            {
                break;
            }
        }
       /* If '*' operator is not already there checks, open and close bracket '*' terms handled by removeRedundantBrackets */
        else if(*itr == '\\' && ( itr > input.constBegin() ) && !isOperator(*(itr-1))) // '\' characters not preceeded by an operator
        {
            sanitised += "*\\";
        }
        else if( *itr == '^' ) // Carat bases not preceeded by an operator
        {
            int insertPos = sanitised.size();

            if (insertPos <= 1)
            {
                throw "Exponent with no base";
            }

            if(!isCloseBracket(sanitised[insertPos-1]))
            {
                if(sanitised[insertPos-1].isLetter())
                {
                    insertPos--;
                }
                else
                {
                    while( ( insertPos > 0 ) && ( sanitised[insertPos-1].isDigit() ) )
                    {
                        insertPos--;
                    }
                }

                if(insertPos != 0 && !isOperator(sanitised[insertPos-1]))
                {
                    sanitised.insert(insertPos, '*');
                }
            }

            sanitised += *itr;
            itr++;
        }
        else {
            if ( *itr != '*')
            {
                sanitised += *itr;
            }
            else if ( itr > input.constBegin() ) // If * operator is already there checks
            {
                if ( isCloseBracket( *(itr-1) ) ) // If * operator is preceeded by a close bracket
                {
                    sanitised += *itr;
                }
                else if( ( (itr < input.constEnd() - 2 ) && *(itr+2) == '^' ) ) // If there is a carat term coming up
                {
                    sanitised += *itr;
                }
                else if (itr < input.constEnd()-1)
                {
                    if ( isOpenBracket(*(itr+1)) ) // If * operator is followed by an open bracket or a '\\'
                    {
                        sanitised += *itr;
                    }
                    else if ((*(itr+1) == '\\'))
                    {
                        sanitised += *itr;
                    }
                    else if((*(itr-1)).isDigit() && (*(itr+1)).isDigit()) // If is a digit timesed by another digit i.e. 4 * 2
                    {
                        sanitised += *itr;
                    }
                }
            }
            itr++;
        }
    }

    return sanitised;
}


QString sanitise(QStringView input) {

    return removeRedundantBrackets(resolveSyntaxRedundancies(input));
}


//QString viewDebugCompression(QStringView debugCompression)
//{
//    QString view;

//    QStringView::const_iterator itr = debugCompression.constBegin();

//    while (itr != debugCompression.constEnd())
//    {

//    }

//}

void Mafs::on_eqnInput_textChanged()
{
    try {
        this->eqnGroup = QStringView( sanitise( ui->eqnInput->toPlainText() ) );
        ui->console->setText( escapeBackslashes(QStringView(eqnGroup.getCompressedDebug()) ) );
//        ui->console->setText( decompressDebug( QStringView(eqnGroup.getCompressedDebug()) ) );
    }
    catch (std::string errMessage) {
        ui->console->setText(QString::fromStdString(errMessage));
    }
    catch (...)
    {
        ui->console->setText(QString("Sorry, looks like there's an error in my programming - Seamus"));
    }
}

//void Mafs::commitTest()
//{
//    this->debuggingOutputTester.commit( this->ui->eqnInput->toPlainText().toStdString(), this->eqnGroup.getCompressedDebug().toStdString() );
//}

//void Mafs::on_commitTestButton_clicked()
//{
//    this->commitTest();
//}


//void Mafs::on_writeToFile_clicked()
//{
//    this->debuggingOutputTester.writeMapToFile();
//}

//void Mafs::on_printMap_clicked()
//{
//    this->ui->console->setPlainText( QString::fromStdString(this->debuggingOutputTester.printMap()) );
//}

