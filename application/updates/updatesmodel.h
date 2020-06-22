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
#ifndef UPDATESMODEL_H
#define UPDATESMODEL_H

#include <tpromise.h>
#include <QAbstractListModel>

struct UpdatesModelPrivate;
class UpdatesModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit UpdatesModel(QObject* parent = nullptr);
        ~UpdatesModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        tPromise<void>* setPackagekitPackages(QStringList packageIds);

    private:
        UpdatesModelPrivate* d;
};

#endif // UPDATESMODEL_H
