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
#include "searchmodel.h"

#include "metadatarepository.h"

struct SearchModelPrivate {
    QList<AppStream::Component> components;
    uint incNum = 0;
};

SearchModel::SearchModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new SearchModelPrivate();
}

SearchModel::~SearchModel() {
    delete d;
}

int SearchModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->components.count();
}

QVariant SearchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    AppStream::Component component = d->components.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return component.name();
        case Qt::UserRole:
            return component.id();
    }

    return QVariant();
}

tPromise<void>* SearchModel::search(QString query) {
    uint num = ++d->incNum;
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        tPromise<QList<AppStream::Component>>::runOnNewThread([ = ](tPromiseFunctions<QList<AppStream::Component>>::SuccessFunction res, tPromiseFunctions<QList<AppStream::Component>>::FailureFunction rej) {
            QList<AppStream::Component> components;
            if (!query.isEmpty()) {
                components = MetadataRepository::instance()->componentsBySearch(query);

//                std::sort(components.begin(), components.end(), [ = ](const AppStream::Component & first, const AppStream::Component & second) {
//                    return first.name().toLower().localeAwareCompare(second.name().toLower()) > 0;
//                });
            }

            res(components);
        })->then([ = ](QList<AppStream::Component> components) {
            //Wait for the fade animation in tStackedWidget
            QTimer::singleShot(500, [ = ] {
                if (num != d->incNum) {
                    rej("Another search was performed");
                    return;
                }

                d->components = components;
                emit dataChanged(index(0), index(rowCount()));
                res();
            });
        });
    });
}
