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
#ifndef METADATAREPOSITORY_H
#define METADATAREPOSITORY_H

#include <QObject>
#include <tpromise.h>
#include <AppStreamQt/component.h>

struct MetadataRepositoryPrivate;
class MetadataRepository : public QObject {
        Q_OBJECT
    public:
        static MetadataRepository* instance();

        QList<AppStream::Component> componentById(QString id);
        QList<AppStream::Component> componentsByPackageName(QString packageName);
        QList<AppStream::Component> componentsBySearch(QString packageName);
        QList<AppStream::Component> componentsByName(QString name);
        QList<AppStream::Component> componentsByCategory(QString category);

        tPromise<QIcon>* iconForComponent(AppStream::Component component, QSize size);

        void launch(QString desktopEntry);

    signals:

    private:
        explicit MetadataRepository(QObject* parent = nullptr);
        MetadataRepositoryPrivate* d;

        void fetchMetadata();
};

#endif // METADATAREPOSITORY_H
