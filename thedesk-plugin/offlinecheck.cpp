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
#include "offlinecheck.h"

#include <QFile>
#include <QSettings>
#include <offlineerrorresults.h>
#include <tnotification.h>

#include <Daemon>
#include <Offline>

void OfflineCheck::checkOfflineStatus() {
    QFile file("/var/lib/PackageKit/offline-update-competed");
    if (!file.exists()) return;

    file.open(QFile::ReadOnly);
    QString firstLine = file.readLine().trimmed();
    if (firstLine != "[PackageKit Offline Update Results]") return;

    QMap<QString, QString> data;

    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        QString key = line.left(line.indexOf("="));
        QString value = line.mid(line.indexOf("=") + 1);

        data.insert(key, value);
    }

    if (data.value("Success") == "true") {
        if (data.value("Role") == "update-packages") {
            //Updates were installed!
            int count = data.value("Packages").split(",").count();

            tNotification* notification = new tNotification();
            notification->setAppName(tr("theShop"));
            notification->setSummary(tr("Updates were installed successfully"));
            notification->setText(tr("%n packages were updated", nullptr, count));
            notification->post();
        } else if (data.value("Role") == "upgrade-system") {
            //Find out what OS we're running

            QString os;

            QFile osrel("/etc/os-release");
            osrel.open(QFile::ReadOnly);
            while (os == "" && !osrel.atEnd()) {
                QString line = osrel.readLine();
                if (line.startsWith("PRETTY_NAME")) {
                    os = line.mid(line.indexOf("=") + 1).trimmed().remove("\"");
                }
            }

            if (os == "") os = tr("your new operating system");

            tNotification* notification = new tNotification();
            notification->setAppName(tr("theShop"));
            notification->setSummary(tr("System Upgrade Complete"));
            notification->setText(tr("Welcome to %1!").arg(os));
            notification->post();
        }
    } else {
        tNotification* notification = new tNotification();
        notification->setAppName(tr("theShop"));
        notification->setSummary(tr("Couldn't install updates"));
        notification->setText(tr("Updates failed to install"));
        notification->insertAction("details", tr("View Details"));
        notification->setTimeout(30000);
        connect(notification, &tNotification::actionClicked, [ = ](QString key) {
            if (key == "details") {
                OfflineErrorResults* results = new OfflineErrorResults(data.value("ErrorDetails"));
                results->show();
            }
        });
        notification->post();
    }

    PackageKit::Daemon::global()->offline()->clearResults();
}
