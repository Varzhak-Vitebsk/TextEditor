#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private slots:
    //---File menu
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionOpen_with_mouse_wheel_triggered();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();
    void on_actionExit_triggered();
    bool changesSaved();
    void onTextChange();
    void nullifyWheelUnits();
    //---Edit menu
    void on_action_Remove_C_C_comments_triggered();    
    //---View menu
    void on_actionPlain_text_triggered();
    void on_actionList_triggered();

private:
    Ui::MainWindow *ui;    
    QString current_file;
    QString window_title;
    bool saveFile(const QString &file_name);
    void openFile(const QString &file_name);
    void setCurrentFile(const QString &file_name);
    void copyFromBufDoc(int wheel_units);
    QTextDocument * buffer_doc;
    int wheel_units;
    QTimer * timer;
    QListView * list_view;
    bool std_open;
};

#endif // MAINWINDOW_H
