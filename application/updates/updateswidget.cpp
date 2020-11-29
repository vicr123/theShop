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
#include "updateswidget.h"
#include "ui_updateswidget.h"

#include <Daemon>
#include <Offline>
#include <QDBusConnectionInterface>
#include "metadatarepository.h"
#include "updatesmodel.h"

struct UpdatesWidgetPrivate {
    QStringList updates;
    bool requireRestart = false;

    bool updatesDownloaded = false;

    UpdatesModel* model;

    QMap<QDBusObjectPath, PackageKit::Transaction*> transactions;
    QPointer<PackageKit::Transaction> currentTransaction;
};

UpdatesWidget::UpdatesWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::UpdatesWidget) {
    ui->setupUi(this);
    d = new UpdatesWidgetPrivate();

    ui->mainUpdateWidget->setFixedWidth(SC_DPI(600));
    ui->spinnerWidget->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));

    d->model = new UpdatesModel(this);
    ui->updatesList->setModel(d->model);

    connect(PackageKit::Daemon::global(), &PackageKit::Daemon::transactionListChanged, this, &UpdatesWidget::updatePackagekit);

    checkUpdates();
}

UpdatesWidget::~UpdatesWidget() {
    delete d;
    delete ui;
}

void UpdatesWidget::checkUpdates() {
    ui->stackedWidget->setCurrentWidget(ui->checkingPage);
    d->updates.clear();
    d->requireRestart = false;

    PackageKit::Transaction* tx = PackageKit::Daemon::getUpdates();
    connect(tx, &PackageKit::Transaction::package, this, [ = ](PackageKit::Transaction::Info info, const QString & packageID, const QString & summary) {
        if (info == PackageKit::Transaction::InfoBlocked) return;
        d->updates.append(packageID);
    });
    connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
        if (d->updates.count() == 0) {
            ui->stackedWidget->setCurrentWidget(ui->noUpdatesPage);
            return;
        }

        PackageKit::Transaction* details = PackageKit::Daemon::getUpdatesDetails(d->updates);
        connect(details, &PackageKit::Transaction::updateDetail, this, [ = ](const QString & packageID,
                const QStringList & updates,
                const QStringList & obsoletes,
                const QStringList & vendorUrls,
                const QStringList & bugzillaUrls,
                const QStringList & cveUrls,
                PackageKit::Transaction::Restart restart,
                const QString & updateText,
                const QString & changelog,
                PackageKit::Transaction::UpdateState state,
                const QDateTime & issued,
        const QDateTime & updated) {
            if (restart != PackageKit::Transaction::RestartNone && restart != PackageKit::Transaction::RestartApplication) d->requireRestart = true;
        });
        connect(details, &PackageKit::Transaction::finished, this, [ = ] {
            d->model->setPackagekitPackages(d->updates)->then([ = ] {
                ui->stackedWidget->setCurrentWidget(ui->updatesPage);
            });
        });
    });
}

void UpdatesWidget::updatePackagekit() {
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(PackageKit::Daemon::getTransactionList());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        QDBusError err = watcher->error();

        QDBusArgument arg = watcher->reply().arguments().first().value<QDBusArgument>();
        QList<QDBusObjectPath> transactionIds;
        arg >> transactionIds;
        for (QDBusObjectPath transaction : transactionIds) {
            if (d->transactions.contains(transaction)) continue;

            PackageKit::Transaction* tx = new PackageKit::Transaction(transaction);
            connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
                d->transactions.remove(transaction);
            });
            d->transactions.insert(transaction, tx);
        }

        if (!d->currentTransaction) {
            for (PackageKit::Transaction* tx : d->transactions) {
                if (tx->role() == PackageKit::Transaction::RoleUpdatePackages) {
                    d->currentTransaction = tx;
                    connect(tx, &PackageKit::Transaction::statusChanged, this, [ = ] {
                        QString statusText;
                        switch (tx->status()) {
                            case PackageKit::Transaction::StatusWait:
                            case PackageKit::Transaction::StatusSetup:
                            case PackageKit::Transaction::StatusRunning:
                            case PackageKit::Transaction::StatusDepResolve:
                            case PackageKit::Transaction::StatusTestCommit:
                                statusText = tr("Getting prepared...");
                                break;
                            case PackageKit::Transaction::StatusDownload:
                                statusText = tr("Downloading packages...");
                                break;
                            case PackageKit::Transaction::StatusInstall:
                                statusText = tr("Installing packages...");
                                break;
                            case PackageKit::Transaction::StatusUpdate:
                                statusText = tr("Updating packages...");
                                break;
                            case PackageKit::Transaction::StatusCleanup:
                                statusText = tr("Finishing up...");
                                break;
                            case PackageKit::Transaction::StatusSigCheck:
                                statusText = tr("Reviewing downloaded packages...");
                                break;
                            case PackageKit::Transaction::StatusWaitingForLock:
                                statusText = tr("Waiting for other package managers...");
                                break;
                            case PackageKit::Transaction::StatusWaitingForAuth:
                                statusText = tr("Waiting for authentication...");
                                break;
                            default:
                                statusText = tr("Installing updates...");
                                break;
                        }
                        ui->statusLabel->setText(statusText);
                    });
                    connect(tx, &PackageKit::Transaction::percentageChanged, this, [ = ] {
                        ui->progressBar->setValue(tx->percentage());
                    });
                    ui->progressBar->setValue(tx->percentage());
                }
            }
        }

        if (d->currentTransaction) {
            ui->progressWidget->setVisible(true);
            ui->actionsWidget->setVisible(false);
        } else {
            ui->progressWidget->setVisible(false);
            ui->actionsWidget->setVisible(true);

            if (d->updatesDownloaded) {
                //Reboot to install updates
                ui->installButton->setVisible(false);
                ui->rebootInstallButton->setVisible(true);
            } else {
                ui->installButton->setVisible(true);
                ui->rebootInstallButton->setVisible(false);
            }
        }
    });
}

void UpdatesWidget::on_checkUpdatesNoUpdateButton_clicked() {
    d->updatesDownloaded = false;
    this->checkUpdates();
}

void UpdatesWidget::on_checkUpdatesButton_clicked() {
    checkUpdates();
}

void UpdatesWidget::on_installButton_clicked() {
    d->updatesDownloaded = true;
    PackageKit::Daemon::updatePackages(d->updates, PackageKit::Transaction::TransactionFlagOnlyDownload);
}

void UpdatesWidget::on_rebootInstallButton_clicked() {
    //Ask the DE to reboot
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("com.vicr123.theShop.DesktopSession").value()) {
        QDBusMessage message = QDBusMessage::createMethodCall("com.vicr123.theShop.DesktopSession", "/com/vicr123/theShop/DesktopSession", "com.vicr123.theShop.DesktopSession", "RebootAndUpdate");
        QDBusConnection::sessionBus().call(message);
    } else {
        //TODO: Implement
    }
}
