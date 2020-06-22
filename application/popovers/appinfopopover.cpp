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
#include "appinfopopover.h"
#include "ui_appinfopopover.h"

#include "metadatarepository.h"
#include <Daemon>
#include <Details>
#include <tmessagebox.h>
#include <tnotification.h>
#include "transactionpool.h"
#include <QDesktopServices>

struct AppInfoPopoverPrivate {
    QList<AppStream::Component> components;
    AppStream::Component currentComponent;
    QString currentPackageId;

    QMap<QDBusObjectPath, PackageKit::Transaction*> transactions;
    QList<PackageKit::Transaction*> relatedTransactions;

    int currentComponentIndex;
};

AppInfoPopover::AppInfoPopover(QString componentId, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AppInfoPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    d = new AppInfoPopoverPrivate();

    ui->progressWidget->setExpanded(false);

    ui->iconLabel->setPixmap(QIcon::fromTheme("generic-app").pixmap(SC_DPI_T(QSize(64, 64), QSize)));

    d->components = MetadataRepository::instance()->componentById(componentId);
    loadComponent(0);

    connect(TransactionPool::instance(), &TransactionPool::transactionAdded, this, [ = ](QString appstreamId, PackageKit::Transaction * tx) {
        if (d->currentComponent.isValid() && d->currentComponent.id() == appstreamId) updatePackagekit();
    });
    updatePackagekit();
}

AppInfoPopover::~AppInfoPopover() {
    delete d;
    delete ui;
}

void AppInfoPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void AppInfoPopover::loadComponent(int index) {
    AppStream::Component component = d->components.at(index);
    d->currentComponent = component;
    d->currentComponentIndex = index;

    ui->titleLabel->setText(component.name());
    ui->appNameLabel->setText(component.name());
    ui->summaryLabel->setText(component.summary());
    ui->descriptionLabel->setText(component.description());
    ui->screenshotBrowser->setScreenshots(component.screenshots());

    ui->homepageButton->setVisible(d->currentComponent.url(AppStream::Component::UrlKindHomepage).isValid());
    ui->donateButton->setVisible(d->currentComponent.url(AppStream::Component::UrlKindDonation).isValid());

    QSize iconSize = SC_DPI_T(QSize(64, 64), QSize);
    MetadataRepository::instance()->iconForComponent(component, iconSize)->then([ = ](QIcon icon) {
        ui->iconLabel->setPixmap(icon.pixmap(iconSize));
    })->error([ = ](QString error) {
        ui->iconLabel->setPixmap(QIcon::fromTheme("generic-app").pixmap(iconSize));
    });

    updateState();
}

void AppInfoPopover::updateState() {
    ui->installButton->setVisible(false);
    ui->removeButton->setVisible(false);
    ui->launchButton->setVisible(false);
    ui->actionSpinner->setVisible(true);

    PackageKit::Transaction* tx = PackageKit::Daemon::resolve(d->currentComponent.packageNames().first());
    connect(tx, &PackageKit::Transaction::package, this, [ = ](PackageKit::Transaction::Info info, const QString & packageID, const QString & summary) {
        if (info == PackageKit::Transaction::InfoInstalled) {
            AppStream::Launchable launchable = d->currentComponent.launchable(AppStream::Launchable::KindDesktopId);

            ui->installButton->setVisible(false);
            ui->removeButton->setVisible(true);
            ui->launchButton->setVisible(!launchable.entries().isEmpty());
        } else {
            ui->installButton->setVisible(true);
            ui->removeButton->setVisible(false);
            ui->launchButton->setVisible(false);
        }

        d->currentPackageId = packageID;
        ui->actionSpinner->setVisible(false);
    });
}

void AppInfoPopover::updatePackagekit() {
    QList<PackageKit::Transaction*> transactions = TransactionPool::instance()->transactionsForPackage(d->currentComponent.id());
    if (transactions.isEmpty()) {
        ui->actionsWidget->setVisible(true);
        ui->progressWidget->collapse();
    } else {
        PackageKit::Transaction* tx = transactions.first();

        auto updateStatus = [ = ] {
            QString status;
            switch (tx->status()) {
                case PackageKit::Transaction::StatusUnknown:
                case PackageKit::Transaction::StatusWait:
                case PackageKit::Transaction::StatusSetup:
                case PackageKit::Transaction::StatusRunning:
                case PackageKit::Transaction::StatusQuery:
                case PackageKit::Transaction::StatusInfo:
                case PackageKit::Transaction::StatusRefreshCache:
                case PackageKit::Transaction::StatusDepResolve:
                case PackageKit::Transaction::StatusTestCommit:
                    status = tr("Getting ready...");
                    break;
                case PackageKit::Transaction::StatusRemove:
                    status = tr("Removing...");
                    break;
                case PackageKit::Transaction::StatusDownload:
                    status = tr("Downloading...");
                    break;
                case PackageKit::Transaction::StatusInstall:
                    status = tr("Installing...");
                    break;
                case PackageKit::Transaction::StatusUpdate:
                    status = tr("Updating...");
                    break;
                case PackageKit::Transaction::StatusCleanup:
                case PackageKit::Transaction::StatusSigCheck:
                case PackageKit::Transaction::StatusCommit:
                case PackageKit::Transaction::StatusRequest:
                case PackageKit::Transaction::StatusFinished:
                case PackageKit::Transaction::StatusCancel:
                    status = tr("Finishing up...");
                    break;
                case PackageKit::Transaction::StatusWaitingForLock:
                    status = tr("Waiting for package manager to become available...");
                    break;
                case PackageKit::Transaction::StatusWaitingForAuth:
                    status = tr("Waiting for authorisation...");
                    break;
                default:
                    status = tr("Processing...");
                    break;
            }
            ui->statusLabel->setText(status);
        };
        connect(tx, &PackageKit::Transaction::statusChanged, this, updateStatus);
        updateStatus();

        connect(tx, &PackageKit::Transaction::percentageChanged, this, [ = ] {
            ui->progressBar->setValue(tx->percentage());
            ui->progressBar->setMaximum(100);
        });
        connect(tx, &PackageKit::Transaction::finished, this, [ = ] {
            updatePackagekit();
            updateState();

            ui->progressBar->setMaximum(0);
        });

        ui->actionsWidget->setVisible(false);
        ui->progressWidget->expand();
    }
}

void AppInfoPopover::on_installButton_clicked() {
    AppStream::Launchable launchable = d->currentComponent.launchable(AppStream::Launchable::KindDesktopId);
    QString name = d->currentComponent.name();
    QString desktopEntry;
    if (!launchable.entries().isEmpty()) desktopEntry = launchable.entries().first();

    PackageKit::Transaction* tx = PackageKit::Daemon::installPackage(d->currentPackageId);
    connect(tx, &PackageKit::Transaction::errorCode, tx, [ name, desktopEntry ](PackageKit::Transaction::Error error, QString details) {
        tNotification* notification = new tNotification();
        notification->setSummary(name);
        notification->setText(tr("Failed to install"));
        notification->insertAction("details", tr("View Details"));
        connect(notification, &tNotification::actionClicked, [ = ](QString key) {
            if (key == "details") {
                tMessageBox* box = new tMessageBox();
                box->setWindowTitle(tr("Failure Information"));
                box->setText(tr("%1 failed to install.").arg(name));
                box->setDetailedText(details);
                box->setStandardButtons(tMessageBox::Ok);
                connect(box, &tMessageBox::finished, [ = ](int result) {
                    box->deleteLater();
                });
                box->show();
            }
        });
        notification->post();
    });
    connect(tx, &PackageKit::Transaction::finished, tx, [ name, desktopEntry ](PackageKit::Transaction::Exit status, uint runtime) {
        if (status == PackageKit::Transaction::ExitSuccess) {

            tNotification* notification = new tNotification();
            notification->setSummary(name);
            notification->setText(tr("Installed Successfully"));
            if (!desktopEntry.isEmpty()) {
                notification->insertAction("launch", tr("Launch"));
            }
            connect(notification, &tNotification::actionClicked, [ = ](QString key) {
                if (key == "launch") {
                    MetadataRepository::instance()->launch(desktopEntry);
                }
            });
            notification->post();
        }
    });
    TransactionPool::instance()->track(d->currentComponent.id(), tx);
}

void AppInfoPopover::on_launchButton_clicked() {
    AppStream::Launchable launchable = d->currentComponent.launchable(AppStream::Launchable::KindDesktopId);
    MetadataRepository::instance()->launch(launchable.entries().first());
}

void AppInfoPopover::on_removeButton_clicked() {
    QString name = d->currentComponent.name();

    PackageKit::Transaction* tx = PackageKit::Daemon::removePackage(d->currentPackageId, false, true);
    connect(tx, &PackageKit::Transaction::errorCode, tx, [ name ](PackageKit::Transaction::Error error, QString details) {
        tNotification* notification = new tNotification();
        notification->setSummary(name);
        notification->setText(tr("Failed to remove"));
        notification->insertAction("details", tr("View Details"));
        connect(notification, &tNotification::actionClicked, [ = ](QString key) {
            if (key == "details") {
                tMessageBox* box = new tMessageBox();
                box->setWindowTitle(tr("Failure Information"));
                box->setText(tr("Failed to remove %1.").arg(name));
                box->setDetailedText(details);
                box->setStandardButtons(tMessageBox::Ok);
                connect(box, &tMessageBox::finished, [ = ](int result) {
                    box->deleteLater();
                });
                box->show();
            }
        });
        notification->post();
    });
    TransactionPool::instance()->track(d->currentComponent.id(), tx);
}

void AppInfoPopover::on_homepageButton_clicked() {
    QDesktopServices::openUrl(d->currentComponent.url(AppStream::Component::UrlKindHomepage));
}

void AppInfoPopover::on_donateButton_clicked() {
    QDesktopServices::openUrl(d->currentComponent.url(AppStream::Component::UrlKindDonation));
}
