/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "metadatarepository.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QDir>
#include <QLocale>
#include <QDBusMessage>
#include <QDBusConnection>
#include <AppStreamQt/metadata.h>
#include <AppStreamQt/pool.h>
#include <AppStreamQt/icon.h>
#include <tpromise.h>

struct MetadataRepositoryPrivate {
    AppStream::Pool pool;
};

MetadataRepository* MetadataRepository::instance() {
    static MetadataRepository* instance = new MetadataRepository();
    return instance;
}

QList<AppStream::Component> MetadataRepository::componentById(QString id) {
    return d->pool.componentsById(id);
}

QList<AppStream::Component> MetadataRepository::componentsByPackageName(QString packageName) {
    return QtConcurrent::blockingFiltered(d->pool.components(), [ = ](const AppStream::Component & component) {
        if (component.packageNames().isEmpty()) return false;
        return component.packageNames().contains(packageName);
    });
}

QList<AppStream::Component> MetadataRepository::componentsBySearch(QString packageName) {
    return QtConcurrent::blockingFiltered(d->pool.components(), [ = ](const AppStream::Component & component) {
        if (component.packageNames().isEmpty()) return false;
        return component.name().toLower().contains(packageName.toLower());
    });
}

QList<AppStream::Component> MetadataRepository::componentsByName(QString name) {
    return QtConcurrent::blockingFiltered(d->pool.components(), [ = ](const AppStream::Component & component) {
        if (component.packageNames().isEmpty()) return false;
        return component.name().toLower().contains(name.toLower());
    });
}

QList<AppStream::Component> MetadataRepository::componentsByCategory(QString category) {
    return QtConcurrent::blockingFiltered(d->pool.components(), [ = ](const AppStream::Component & component) {
        if (component.packageNames().isEmpty()) return false;
        return component.hasCategory(category);
    });
}

tPromise<QIcon>* MetadataRepository::iconForComponent(AppStream::Component component, QSize size) {
    QString id = component.id();
    return tPromise<QIcon>::runOnNewThread([ = ](tPromiseFunctions<QIcon>::SuccessFunction res, tPromiseFunctions<QIcon>::FailureFunction rej) {
        AppStream::Component component = d->pool.componentsById(id).first();
        if (component.icons().isEmpty()) {
            res(QIcon::fromTheme("generic-app"));
            return;
        }

        QIcon icon;
        for (AppStream::Icon ic : component.icons()) {
            QUrl url = ic.url();
            if (url.isLocalFile()) {
                QPixmap px(url.toLocalFile());
                icon.addPixmap(px);
            } else {
                QNetworkAccessManager mgr;
                QNetworkReply* reply = mgr.get(QNetworkRequest(url));
                reply->waitForReadyRead(3000);

                QPixmap px(reply->readAll());
                icon.addPixmap(px);
            }
        }
        res(icon);
    });
}

void MetadataRepository::launch(QString desktopEntry) {
    QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.theShop.DesktopSession", "/com/vicr123/theShop/DesktopSession", "com.vicr123.theShop.DesktopSession", "LaunchApplication");
    message.setArguments({desktopEntry.chopped(8)});
    QDBusConnection::sessionBus().call(message);
}

MetadataRepository::MetadataRepository(QObject* parent) : QObject(parent) {
    d = new MetadataRepositoryPrivate();

    fetchMetadata();
}

void MetadataRepository::fetchMetadata() {
    d->pool.setLocale(QLocale().bcp47Name());
    d->pool.load();
}
