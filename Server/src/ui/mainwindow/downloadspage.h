#ifndef DOWNLOADSPAGE_H
#define DOWNLOADSPAGE_H

#include <QWidget>

namespace Ui {
class DownloadsPage;
}

class DownloadsPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit DownloadsPage(QWidget *parent = 0);
    ~DownloadsPage();
    
private:
    Ui::DownloadsPage *ui;
};

#endif // DOWNLOADSPAGE_H
