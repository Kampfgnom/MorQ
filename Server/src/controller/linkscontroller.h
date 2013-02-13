#ifndef LINKSCONTROLLER_H
#define LINKSCONTROLLER_H

#include <QObject>

class Download;

class LinksController : public QObject
{
    Q_OBJECT
public:
    explicit LinksController(QObject *parent = 0);

private slots:
    void clipboardChanged();

private:
    Download *createDownload(const QUrl &url);
};

#endif // LINKSCONTROLLER_H
