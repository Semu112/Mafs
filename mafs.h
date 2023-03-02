#ifndef MAFS_H
#define MAFS_H

#include <QMainWindow>
#include <map>

#include "EqnGroup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Mafs; }
QT_END_NAMESPACE

class Mafs : public QMainWindow
{
    Q_OBJECT

public:
    Mafs(QWidget *parent = nullptr);
    ~Mafs();

private slots:
    void on_eqnInput_textChanged();

//    void on_commitTestButton_clicked();

//    void on_writeToFile_clicked();

//    void on_printMap_clicked();

private:
    Ui::Mafs *ui;
//    TestObject debuggingOutputTester;
    EqnGroup eqnGroup;

    void commitTest();
};
#endif // MAFS_H
