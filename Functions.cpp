#include "Functions.h"

 #include <QString>
 #include <QStringView>
#include <array>

#pragma region decompressDebug {

#define INCORRECT_START "Compressed string does not start with \\t"

QStringView compression;
int currDepth;
QString::const_iterator itr;
QString currIndent;
QString decompression;

QString parseTerm()
{
  QString term;

  while (*itr != '\t' && *itr != '\?' && *itr != '\r')
  {
    term += *itr;
    itr++;

    if (itr >= compression.end())
    {
      throw std::string("Not enough close terms (\\?) to match open terms (\\t)\n" + std::to_string(currDepth) + " more needed");
    }
  }

  return term;
}

void decompressionHelper()
{
    currIndent = "\n|";

    //Parse first term independently so we can have it flush

    if (*itr != '\t')
    {
      throw INCORRECT_START;
    }

    itr++;

    decompression += parseTerm();
    currDepth = 1;

    while (itr != compression.end())
    {
      if (*itr == '\t') // Open new term
      {
        itr++;
        currDepth++;

        if (currDepth < 0)
        {
          throw INCORRECT_START;
        }

        decompression += currIndent + currIndent + currIndent + currIndent;
        decompression.replace(decompression.length() - 1, 1, "--->" + parseTerm());

        currIndent += "   |";
      }
      else if(*itr == '\?')
      {
        while (*itr == '\?')
        {
          currDepth--;
          itr++;
        }

        if (currDepth < 0)
        {
          throw std::string("Not enough open terms (\\t) to match close terms (\\?)\n" + std::to_string(0-currDepth) + " more needed");
        }

        if (currDepth == 0)
        {
            decompression += '\n';
            return;
        } else {
            /*
            Resizing to a depth of 1 should give "\n|"
            Resizing to a depth of 2 should give "\n|   |"
            (currDepth-1) * 4 + 2*/
            currIndent.resize((currDepth-1) * 4 + 2); // +2 to include the \n|
        }
      }
      else if (*itr == '\r')
      {
        /*
        Replacing should replace at (currDepth-2) * 4 + 1?
        */
       if (currDepth < 2)
       {
        throw std::string("\\r not valid at a depth of " + std::to_string(currDepth));
       }
        currIndent.replace((currDepth-2) * 4 + 1, 1, " ");
        itr++;
      }
    }

    return;
}

QString decompressDebug(QStringView debug)
{
  compression = debug;
  itr = compression.begin();
  decompression = "";

  while (itr != compression.end())
  {
      decompressionHelper();
  }

  return decompression;
}

#pragma endregion }

QString escapeBackslashes(QStringView input)
{
  QString escaped = input.toString();

  escaped.replace('\t', "\\t");
  escaped.replace('\r', "\\r");
  escaped.replace('\?', "\\?");

  return escaped;
}


#pragma region isX {
inline bool isOpenBracket(QChar chaR)
{
    return chaR == '(' || chaR == '[' /*|| chaR == '{'*/;
}
// For now, no sanitising on {} brackets, so \\ln{{{{{{2}}}}}} would stay like that, unless maybe you only parsed the parent {}'s as {} and treated everything else as ()'s or []
inline bool isCloseBracket(QChar chaR)
{
    return chaR == ')' || chaR == ']' /*|| chaR == '}'*/;
}

inline bool isOperator(QChar chaR)
{
    return chaR == '+' || chaR == '-' || chaR == '*' || chaR == '/' || chaR == '^' || chaR == '\\';
}

// isDelimiter changed to only isDelimitingOperator so that expressions like 4(32) get parse as a single term, not multiple like they were before. E.g. new way of doing it parses 4(32) as 0, old way would have parsed it as (0)1, same with carats
inline bool isDelimiter(QChar chaR)
{
    return isOperator(chaR) || isCloseBracket(chaR) || isOpenBracket(chaR) ;
}

inline bool isOrder1Operator(QChar chaR)
{
    return chaR == '^';
}

inline bool isOrder2Operator(QChar chaR)
{
    return chaR == '/' || chaR == '*';
}

inline bool isOrder3Operator(QChar chaR)
{
    return chaR == '+' || chaR == '-';
}

#pragma endregion }

int getOperatorOrder(QChar chaR)
{
    if (isOrder1Operator(chaR))
    {
        return 1;
    }
    else if(isOrder3Operator(chaR))
    {
        return 3;
    }
    else // nothing in between terms is a multiply, which has order 2
    {
        return 2;
    }
}

/**
 * @brief getOrder Gets the order of the term to be passed in. Order is defined by what's in the term.
 * @note I think = should be parsed only when parsed inside of a {} block, defined when a special operator (starts with \) is parsed
 *  --BASICS--
 *
     * @interface Order 0: A term with no delimiting operators (+, -, ^, /, * and \)
     *  @example 12, this would be parsed as 0
     *  @example 12xyz would also be parsed as 0
     *
     * @interface Order 1: A term with an ^
     *  @example 12xy^2, parsed as two terms, 0*1, 0 ---> 12x, 1 ---> 0^1, 0 ---> y, 1 ---> 2

     * @interface Order 2: A term with a / or an *, or a non-digit
     *  @example 12xy/3z, would be parsed as 0/1 ---> 0 == 12yx, 1 == 3z
     *  @example (1+2)*(3+4) would be considered order 0

     * @interface Order 3: A term with a + or -
     *  @example 12xy+3z, would be parsed as 0+1 ---> 0 == 12xyz, 1 == 3z

     * @interface The order of a term is not taken into account if it is surrounded in brackets
     *  @example (12xy+3z)^2, would be parsed as order 1

 * @param itr starts at the start of the QString, and is encremented until @param end
 * @param end iterator pointing to the end of the string for which we'd like to get the order for
 * @return
 */
int getOrder(QString::const_iterator* itr, QString::const_iterator end)
{
    int bracketWeight = 0;
    int order = 0;

    bool hasDigit = false;
    bool hasNonDigit = false;

    while(isOperator(**itr)) // Skip any signs in front of the terms
    {
        (*itr)++;
    }

    // If bracketWeight is less than 0, return, because it's likely that we'd want to check the order of a substring
    // For example, in the case of (((1)+2)+3), if we passed an iterator to the third bracket in, we would get the order of (1)+2
    while (*itr != end)
    {
        if (isOpenBracket(**itr))
        {
            bracketWeight++;
        }
        else if(isCloseBracket(**itr))
        {
            bracketWeight--;

            if(bracketWeight < 0)
            {
                break;
            }
        }
        else if(bracketWeight == 0 && order != 3) // If order is 3, we don't need to check if it's of a higher order, order 3 is the highest. I don't return 3 so that the iterator can go until the end of the term
        {
            switch (order) // Note that there is no breaks after the case statements, so the code just rolls over
            {
                case 0:
                    if (isOrder1Operator(**itr))
                    {
                        order = 1;
                        break;
                    }
                case 1:
                    if (isOrder2Operator(**itr))
                    {
                        order = 2;
                        break;
                    }
                case 2:
                    if(isOrder3Operator(**itr))
                    {
                        order = 3;
                        break;
                    }
                    else if(!(**itr).isDigit())
                    {
                        hasNonDigit = true;
                    }
                    else if((**itr).isDigit())
                    {
                        hasDigit = true;
                    }

                    if((hasDigit && hasNonDigit) || (hasNonDigit && !(**itr).isDigit()) )
                    {
                        order = 2;
                    }
                break;
            }
        }

        (*itr)++;
    }

    return order;
}
