QHal 0.1
========

QHal is a Qt wrapper for [HAL](http://stateless.co/hal_specification.html) documents.


Project setup
=============

QHal comes with some helpers to include it in you project.

You can simply add the whole project to your qmake based project and add the following lines to your application's project file:

````
QHAL_PATH = relative/or/absolute/path/to/QHal
include($$QHAL_PATH/QHal.pri)
LIBS            += $$QHAL_LIBS
INCLUDEPATH     += $$QHAL_INCLUDEPATH
`````

You may of course setup everything as you like :wink:


Usage
=====

Parsing
-------

Include header, parse JSON, parse HAL:

```` C++
#include <qhalresource.h>
...

QByteArray data = "... some JSON data ...";

QJsonDocument document = QJsonDocument::fromJson(data);
QHalResource collectionDocument = QHalResource::fromQVariant(document.toVariant().toMap());
````

**Important**: Note that QHalResource and QHalLink are both either resources (or links) or **lists** of resources (or links).

You can ask the link/resource if it is a list with `isList()`.

If you have the information, whether a link/resource is a list, you can use `count()` and the `operator[]` to access the individual links/resources.

Writing
-------
```` C++
    QHalResource resource;
    resource.setHref(QUrl("http://example.com/friends"));
    resource.setProperty("count", QVariant(2));
    resource.addLink(QHalLink("blogs", QUrl("http://example.com/blogs{/name}"), true));
    resource.addLink(QHalLink("feeds", QList<QHalLink>()
                              << QHalLink("", QUrl("http://example.com/friends?atom"))
                              << QHalLink("", QUrl("http://example.com/friends?rss"))));

    QHalResource friends;

    QHalResource friend1;
    friend1.setProperty("name", "Niklas");
    friend1.addLink(QHalLink("blog", QUrl("http://example.com/blogs/Niklas")));
    friends.append(friend1);

    QHalResource friend2;
    friend2.setProperty("name", "Isa");
    friend2.addLink(QHalLink("blog", QUrl("http://example.com/blogs/Isa")));
    friends.append(friend2);

    resource.embed("friends", friends);

    QVariant documentVariant = resource.toVariant();
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(documentVariant);
    qDebug() << jsonDoc.toJson();
````

Note that once a link/resource has been determined to be a list (by calling the QList constructor or `append()`), all other information will be ignored, when calling `toVariant()`.

Please have a look at the example for further details.

Requirements and building
=========================

QHal has been developed and tested only on Qt 5.0 on a Mac. The library itself should be compatible to older versions of Qt, but the example uses Qt 5's QJson* classes.

The project can be build in Qt Creator or by issuing the following commands.

````
qmake
make
````

Contributing
============
Since this library is currently not in use, it is not well tested.

If you find errors, simply file an issue in GitHub, or even better:

If you want to contribute to QHal, you can do the usual Fork-Patch-FilePullRequest dance. I might even tranfser the project in someone else's hands!

License
=======

QHal is licensed under the LGPLv3.0. See LICENSE for details.
