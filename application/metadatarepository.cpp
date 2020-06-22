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
    for (AppStream::Component component : d->pool.components()) {
        if (component.packageNames().contains(packageName)) return {component};
    }
    return {};
}

tPromise<QIcon>* MetadataRepository::iconForComponent(AppStream::Component component, QSize size) {
    return tPromise<QIcon>::runOnNewThread([ = ](tPromiseFunctions<QIcon>::SuccessFunction res, tPromiseFunctions<QIcon>::FailureFunction rej) {
        QUrl url = component.icons().first().url();

        if (url.isLocalFile()) {
            res(QIcon(url.toLocalFile()));
        } else {
            QNetworkAccessManager mgr;
            QNetworkReply* reply = mgr.get(QNetworkRequest(url));
            reply->waitForReadyRead(3000);

            QIcon icon(QPixmap(reply->readAll()));
            res(icon);
        }
    });
}

MetadataRepository::MetadataRepository(QObject* parent) : QObject(parent) {
    d = new MetadataRepositoryPrivate();

    fetchMetadata();
}

void MetadataRepository::fetchMetadata() {
    d->pool.setLocale(QLocale().bcp47Name());
    d->pool.load();
}
