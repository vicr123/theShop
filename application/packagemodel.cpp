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
#include "packagemodel.h"

#include <QNetworkAccessManager>
#include <QIcon>
#include <tpromise.h>
#include <AppStreamQt/icon.h>
#include <metadatarepository.h>
#include <the-libs_global.h>

struct PackageModelPrivate {
    struct Package {
        QString id;;
        QString name;
        QIcon icon;
    };

    PackageModel::UnknownComponentBehaviour unknownComponentBehaviour;
    QList<QSharedPointer<Package>> packages;
};

PackageModel::PackageModel(QObject* parent)
    : QAbstractListModel(parent) {
    d = new PackageModelPrivate();
}

PackageModel::~PackageModel() {
    delete d;
}

int PackageModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->packages.count();
}

QVariant PackageModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    QSharedPointer<PackageModelPrivate::Package> p = d->packages.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return p->name;
        case Qt::DecorationRole:
            return p->icon;
    }

    return QVariant();
}

void PackageModel::setUnknownComponentBehaviour(PackageModel::UnknownComponentBehaviour behaviour) {
    d->unknownComponentBehaviour = behaviour;
}

void PackageModel::setPackagekitPackages(QStringList packageIds) {
    d->packages.clear();

    for (QString package : packageIds) {
        QList<AppStream::Component> components = MetadataRepository::instance()->componentsByPackageName(package.split(";").first());
        QSharedPointer<PackageModelPrivate::Package> p(new PackageModelPrivate::Package());
        p->id = package;

        if (components.count() == 0) {
            p->name = package.split(";").first();
        } else {
            AppStream::Component component = components.first();
            p->name = component.name();
            MetadataRepository::instance()->iconForComponent(component, SC_DPI_T(QSize(32, 32), QSize))->then([ = ](QIcon icon) {
                p->icon = icon;
                emit dataChanged(index(0), index(rowCount()));
            });
        }
        d->packages.append(p);
    }

    emit dataChanged(index(0), index(rowCount()));

}
