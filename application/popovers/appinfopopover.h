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
#ifndef APPINFOPOPOVER_H
#define APPINFOPOPOVER_H

#include <QDBusObjectPath>
#include <QWidget>
#include <AppStreamQt/component.h>

namespace Ui {
    class AppInfoPopover;
}

struct AppInfoPopoverPrivate;
class AppInfoPopover : public QWidget {
        Q_OBJECT

    public:
        explicit AppInfoPopover(QString componentId, QWidget* parent = nullptr);
        ~AppInfoPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_installButton_clicked();

        void on_launchButton_clicked();

        void on_removeButton_clicked();

        void on_homepageButton_clicked();

        void on_donateButton_clicked();

    private:
        Ui::AppInfoPopover* ui;
        AppInfoPopoverPrivate* d;

        void loadComponent(int index);
        void updateState();
        void updatePackagekit();
};

#endif // APPINFOPOPOVER_H
