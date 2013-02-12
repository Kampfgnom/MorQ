#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QMainWindow>

namespace Ui {
class PreferencesWindow;
}

class PreferencesWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PreferencesWindow(QWidget *parent = 0);
    ~PreferencesWindow();
    
private slots:
    void on_lineEditUserName_editingFinished();

private:
    Ui::PreferencesWindow *ui;
};

#endif // PREFERENCESWINDOW_H
