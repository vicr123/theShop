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
#include "updatesmodel.h"

#include <metadatarepository.h>
#include <AppStreamQt/component.h>

struct UpdatesModelPrivate {
    QStringList systemUpdates;
    QList<AppStream::Component> components;
    QList<QIcon> componentIcons;
};

UpdatesModel::UpdatesModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new UpdatesModelPrivate();
}

UpdatesModel::~UpdatesModel() {
    delete d;
}

int UpdatesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    // FIXME: Implement me!
    return d->components.count() + (d->systemUpdates.isEmpty() ? 0 : 1);
}

QVariant UpdatesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    if (index.row() == 0 && !d->systemUpdates.isEmpty()) {
        switch (role) {
            case Qt::DisplayRole:
                return tr("%n System Updates", nullptr, d->systemUpdates.count());
        }
    } else {
        int row = index.row() - !d->systemUpdates.isEmpty();
        AppStream::Component component = d->components.at(row);
        switch (role) {
            case Qt::DisplayRole:
                return component.name();
            case Qt::DecorationRole:
                return d->componentIcons.at(row);
        }
    }

    return QVariant();
}

tPromise<void>* UpdatesModel::setPackagekitPackages(QStringList packageIds) {
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        struct Return {
            QStringList systemUpdates;
            QList<AppStream::Component> components;
            QList<QIcon> componentIcons;
        };

        tPromise<Return>::runOnNewThread([ = ](tPromiseFunctions<Return>::SuccessFunction res, tPromiseFunctions<Return>::FailureFunction rej) {
            Return r;
            for (QString package : packageIds) {
                QList<AppStream::Component> components = MetadataRepository::instance()->componentsByPackageName(package.split(";").first());
                if (components.count() == 0) {
                    r.systemUpdates.append(package);
                } else {
                    AppStream::Component component = components.first();
                    r.components.append(component);

                    tPromiseResults<QIcon> results = MetadataRepository::instance()->iconForComponent(component, SC_DPI_T(QSize(32, 32), QSize))->await();
                    r.componentIcons.append(results.result);
                }
            }

            res(r);
        })->then([ = ](Return r) {
            d->systemUpdates = r.systemUpdates;
            d->components = r.components;
            d->componentIcons = r.componentIcons;

            emit dataChanged(index(0), index(rowCount()));
            res();
        });
    });
}
