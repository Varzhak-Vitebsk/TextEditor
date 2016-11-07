#include "mainwindow.h"
#include "ui_mainwindow.h"
#

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    text_changed = false;
    current_file = "";
    ui->setupUi(this);
    ui->textEdit->setFocus();
    connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(on_actionExit_triggered()));
    connect(ui->textEdit, SIGNAL(texttext_changed()), this, SLOT(onTextChange()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            current_file = fileName;
            QTextStream in(&file);
            ui->textEdit->setPlainText(in.readAll());
            file.close();
        }
}

void MainWindow::on_actionSave_triggered()
{
    if(current_file.isEmpty())
    {
        on_actionSave_As_triggered();
    } else
    {
        QFile file(current_file);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        QTextStream stream(&file);
        stream << ui->textEdit->toPlainText();
        stream.flush();
        file.close();
        text_changed = false;
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(text_changed)
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret)
        {
          case QMessageBox::Save:
              // Save was clicked
              break;
          case QMessageBox::Discard:
              // Don't Save was clicked
              break;
          case QMessageBox::Cancel:
              // Cancel was clicked
              break;
          default:
              // should never be reached
              break;
        }
    }
    //QDebug() << QString(text_changed);
    qApp->quit();
}

void MainWindow::onTextChange()
{
    text_changed = true;
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
               tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));

       if (!fileName.isEmpty()) {
           QFile file(fileName);
           if (!file.open(QIODevice::WriteOnly)) {
               QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
               return;
           }
           current_file = fileName;
           QTextStream stream(&file);
           stream << ui->textEdit->toPlainText();
           stream.flush();
           file.close();
       }
}
