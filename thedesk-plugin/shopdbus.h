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
#ifndef SHOPDBUS_H
#define SHOPDBUS_H

#include <QObject>

class ShopDbus : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.theShop.DesktopSession")
    public:
        explicit ShopDbus(QObject* parent = nullptr);
        ~ShopDbus();

    public Q_SLOTS:
        Q_SCRIPTABLE void RebootAndUpdate();
        Q_SCRIPTABLE void LaunchApplication(QString desktopEntry);

    signals:

};

#endif // SHOPDBUS_H
