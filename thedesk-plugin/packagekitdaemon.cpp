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
#include "packagekitdaemon.h"

#include <tnotification.h>
#include <Daemon>

#include <statemanager.h>
#include <powermanager.h>

struct PackageKitDaemonPrivate {
    QMap<QDBusObjectPath, PackageKit::Transaction*> transactions;
};

PackageKitDaemon::PackageKitDaemon(QObject* parent) : QObject(parent) {
    d = new PackageKitDaemonPrivate();
    connect(PackageKit::Daemon::global(), &PackageKit::Daemon::transactionListChanged, this, &PackageKitDaemon::updatePackageKit);
    updatePackageKit();
}

PackageKitDaemon::~PackageKitDaemon() {
    delete d;
}

void PackageKitDaemon::updatePackageKit() {
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(PackageKit::Daemon::getTransactionList());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QDBusError err = watcher->error();

        QDBusArgument arg = watcher->reply().arguments().first().value<QDBusArgument>();
        QList<QDBusObjectPath> transactionIds;
        arg >> transactionIds;
        for (QDBusObjectPath transaction : transactionIds) {
            if (d->transactions.contains(transaction)) continue;

            PackageKit::Transaction* tx = new PackageKit::Transaction(transaction);
            d->transactions.insert(transaction, tx);

            updateTx(transaction);
        }
    });

}

void PackageKitDaemon::updateTx(QDBusObjectPath path) {
    PackageKit::Transaction* tx = d->transactions.value(path);
    tx->disconnect(this);

    connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
        d->transactions.remove(path);
    });
    connect(tx, &PackageKit::Transaction::roleChanged, this, [ = ] {
        updateTx(path);
    });

    switch (tx->role()) {
        case PackageKit::Transaction::RoleUpdatePackages: {
            connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
                tNotification* notification = new tNotification();
                notification->setAppName(tr("theShop"));
                if (tx->transactionFlags() & PackageKit::Transaction::TransactionFlagOnlyDownload) {
                    //Prompt to reboot
                    notification->setSummary(tr("Updates Downloaded"));
                    notification->setText(tr("Reboot your device to install pending updates."));
                    notification->insertAction("reboot", tr("Reboot Now"));
                } else {
                    notification->setSummary(tr("Updates Complete"));
                    notification->setText(tr("System Updates were installed successfully."));
                }
                connect(notification, &tNotification::actionClicked, this, [ = ](QString key) {
                    if (key == "reboot") {
                        StateManager::powerManager()->showPowerOffConfirmation(PowerManager::Reboot, tr("%1, reboot and install updates? We'll go ahead and reboot to install updates in %n seconds if you don't do anything."), {"update"});
                    }
                });
                notification->post();
            });
            break;
        }
        default:
            break;
    }
}
