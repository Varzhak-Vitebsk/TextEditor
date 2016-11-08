#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setFocus();
    window_title = "Text Editor[*]";
    setWindowTitle(windowTitle());
    setCurrentFile(QString());
    connect(ui->textEdit->document(), &QTextDocument::contentsChanged,
                  this, &MainWindow::onTextChange);
    //connect(ui->textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(onTextChange()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (changesSaved())
    {
        event->accept();
    } else
    {
        event->ignore();
    }
}

void MainWindow::on_actionNew_triggered()
{
    if (changesSaved())
    {
        ui->textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if(changesSaved())
    {
        QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                    tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));
        openFile(file_name);
    }
}

bool MainWindow::on_actionSave_triggered()
{
    if(current_file.isEmpty())
    {
        return on_actionSave_As_triggered();
    } else
    {
        return saveFile(current_file);
    }
}

bool MainWindow::on_actionSave_As_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
               tr("Text Files (*.txt);;C++ Files (*.cpp *.h)"));
    return saveFile(file_name);
}

void MainWindow::on_actionExit_triggered()
{
    if(changesSaved()) qApp->quit();
}

bool MainWindow::changesSaved()
{
    if(ui->textEdit->document()->isModified())
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
                return on_actionSave_triggered();
                break;
            case QMessageBox::Discard:
                return true;
                break;
            case QMessageBox::Cancel:
                return false;
                break;
            default:
                return false;
                break;
        }
    }
    return true;
}

void MainWindow::onTextChange(){

    setWindowModified(ui->textEdit->document()->isModified());
}

bool MainWindow::saveFile(const QString &file_name)
{
    if (!file_name.isEmpty())
    {
        QFile file(file_name);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return false;
        }
        setCurrentFile(file_name);
        QTextStream stream(&file);
        stream << ui->textEdit->toPlainText();
        stream.flush();
        file.close();
        return true;
    }
    return false;
}

void MainWindow::openFile(const QString &file_name)
{
    if (!file_name.isEmpty())
    {
        QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        setCurrentFile(file_name);
        QTextStream in(&file);
        ui->textEdit->setPlainText(in.readAll());
        file.close();
    }
}

void MainWindow::setCurrentFile(const QString &file_name)
{
    current_file = file_name;
    ui->textEdit->document()->setModified(false);
    setWindowModified(false);
    QString shown_name = current_file;
    if (current_file.isEmpty()) shown_name = "untitled.txt";
    setWindowTitle(shown_name + " - " + window_title);
}

void MainWindow::on_action_Remove_C_C_comments_triggered()
{

}
