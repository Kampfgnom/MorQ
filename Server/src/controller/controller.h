#ifndef CONTROLLER_H
#define CONTROLLER_H

class DownloadController;

class Controller
{
public:
    static bool initialize();

    static DownloadController *downloads();

private:
    static bool initlializeDatabase();
};

#endif // CONTROLLER_H
