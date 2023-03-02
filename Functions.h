#ifndef FUNCTIONS_H
#define FUNCTIONS_H

 #include <QString>
 #include <QStringView>

// Compressed / decompressed debug
QString decompressDebug(QStringView debug);
QString escapeBackslashes(QStringView input);

// Sanitising input
QString sanitise(QStringView input);

// Used in multiple places
int getOrder(QString::const_iterator* itr, QString::const_iterator end); // ATOW used in mafs.cpp and term.cpp, defined in mafs.cpp

// eh, global i guess
bool isOpenBracket(QChar chaR);

bool isCloseBracket(QChar chaR);

bool isOperator(QChar chaR);

// isDelimiter changed to only isDelimitingOperator so that expressions like 4(32) get parse as a single term, not multiple like they were before. E.g. new way of doing it parses 4(32) as 0, old way would have parsed it as (0)1, same with carats
bool isDelimiter(QChar chaR);

bool isOrder1Operator(QChar chaR);

bool isOrder2Operator(QChar chaR);

bool isOrder3Operator(QChar chaR);

int getOperatorOrder(QChar chaR);

#endif
