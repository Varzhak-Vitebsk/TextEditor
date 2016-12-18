#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setFocus();
    window_title = "Text Editor[*]";
    setWindowTitle(windowTitle());
    setCurrentFile(QString());
    wheel_units = 0;
    timer = new QTimer(this);
    buffer_doc = new QTextDocument();
    list_view = new QListView(this);
    list_view->setVisible(false);
    std_open = true;
    // ---------------- connections
    connect(timer, SIGNAL(timeout()), this, SLOT(nullifyWheelUnits()));
    connect(ui->textEdit->document(), &QTextDocument::contentsChanged,
                  this, &MainWindow::onTextChange);        
}

MainWindow::~MainWindow()
{
    delete ui;    
    delete buffer_doc;
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

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(!buffer_doc->isEmpty())
    {
        if(!timer->isActive()) timer->start(100);
        wheel_units += 1;
        //ui->textEdit->document()->setPlainText(QString::number(wheel_units) + " " + QString::number(event->angleDelta().y() ));
        event->accept();
    } else event->ignore();
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

void MainWindow::on_actionOpen_with_mouse_wheel_triggered()
{
    ui->textEdit->setPlaceholderText("Use mouse wheel to uncover text.");
    std_open = false;
    on_actionOpen_triggered();
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

void MainWindow::on_action_Remove_C_C_comments_triggered()
{
    QTextDocument *cur_doc = ui->textEdit->document();
    QString comment_open_1 = "//";
    QString comment_open_2 = "/*";
    QString comment_close_1 = "*/";
    QTextCursor cursor(cur_doc);
    for(;;)
    {
        cursor = cur_doc->find(comment_open_1.left(1), cursor);
        if(cursor.position() > 0)
        {
            if(cur_doc->characterAt(cursor.position()) == comment_open_1[1]
                    && (cur_doc->characterAt(cursor.position() - 2) != comment_open_1[1]
                        && cur_doc->characterAt(cursor.position() + 1) != comment_open_1[1]))
            {
                if(cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor))
                {
                    /*ui->textEdit->setTextCursor(cursor);
                    ui->textEdit->setTextColor(Qt::green);
                    ui->textEdit->setTextCursor(QTextCursor());*/
                    cursor.removeSelectedText();
                }
            } else if(cur_doc->characterAt(cursor.position()) == comment_open_2[1])
            {
                QTextCursor temp = cur_doc->find(comment_close_1, cursor.position() + 1);
                if(temp.position() > 0)
                {
                    cursor.setPosition(temp.position(), QTextCursor::KeepAnchor);
                    /*ui->textEdit->setTextCursor(cursor);
                    ui->textEdit->setTextColor(Qt::green);
                    ui->textEdit->setTextCursor(QTextCursor());*/
                    cursor.removeSelectedText();
                } else
                {
                    QMessageBox::warning(this, tr("Error"), tr("Unterminated comment"));
                    break;
                }
            }
        } else break;
    }
}

void MainWindow::nullifyWheelUnits()
{
    timer->stop();
    copyFromBufDoc(wheel_units);
    wheel_units = 0;
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
        std_open ? ui->textEdit->setPlainText(in.readAll()) : buffer_doc->setPlainText(in.readAll());
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

void MainWindow::copyFromBufDoc(int wheel_units)
{
    QTextCursor buff_cursor(buffer_doc);
    QTextCursor doc_cursor(ui->textEdit->document());
    doc_cursor.movePosition(QTextCursor::End);
    if(wheel_units < 3)
    {
        buff_cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    }
    if(wheel_units >= 3 && wheel_units <= 5)
    {
        buff_cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
    }
    if(wheel_units > 5)
    {
        if(!buff_cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor))
            buff_cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }
    doc_cursor.insertText(buff_cursor.selectedText());
    buff_cursor.removeSelectedText();
    if(buffer_doc->isEmpty()) std_open = true;
}


void MainWindow::on_actionPlain_text_triggered()
{
    if(!ui->textEdit->isVisible())
    {
        list_view->setVisible(false);
        ui->textEdit->show();
    }
}

void MainWindow::on_actionList_triggered()
{
    if(!list_view->isVisible())
    {
        ui->textEdit->setVisible(false);
        list_view->show();
    }

}
