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
#include "transactionpool.h"

struct TransactionPoolPrivate {
    QMultiMap<QString, PackageKit::Transaction*> transactions;
};

TransactionPool* TransactionPool::instance() {
    static TransactionPool* instance = new TransactionPool();
    return instance;
}

void TransactionPool::track(QString appstreamId, PackageKit::Transaction* tx) {
    connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
        d->transactions.remove(appstreamId, tx);
    });
    d->transactions.insert(appstreamId, tx);

    emit transactionAdded(appstreamId, tx);
}

QList<PackageKit::Transaction*> TransactionPool::transactionsForPackage(QString package) {
    return d->transactions.values(package);
}

TransactionPool::TransactionPool(QObject* parent) : QObject(parent) {
    d = new TransactionPoolPrivate();
}
