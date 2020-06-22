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
#ifndef TRANSACTIONPOOL_H
#define TRANSACTIONPOOL_H

#include <Transaction>

struct TransactionPoolPrivate;
class TransactionPool : public QObject {
        Q_OBJECT
    public:
        static TransactionPool* instance();

        void track(QString appstreamId, PackageKit::Transaction* tx);
        QList<PackageKit::Transaction*> transactionsForPackage(QString package);

    signals:
        void transactionAdded(QString appstreamId, PackageKit::Transaction* tx);

    private:
        explicit TransactionPool(QObject* parent = nullptr);
        TransactionPoolPrivate* d;
};

#endif // TRANSACTIONPOOL_H
