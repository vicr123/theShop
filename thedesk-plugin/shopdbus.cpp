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
#include "shopdbus.h"

#include "desktopsession_adaptor.h"
#include <QDBusConnection>

#include <Daemon>
#include <Offline>

#include <statemanager.h>
#include <powermanager.h>
#include <application.h>

ShopDbus::ShopDbus(QObject* parent) : QObject(parent) {
    new DesktopSessionAdaptor(this);
    QDBusConnection::sessionBus().registerService("com.vicr123.theShop.DesktopSession");
    QDBusConnection::sessionBus().registerObject("/com/vicr123/theShop/DesktopSession", this);
}

ShopDbus::~ShopDbus() {
    QDBusConnection::sessionBus().unregisterService("com.vicr123.theShop.DesktopSession");
}

void ShopDbus::RebootAndUpdate() {
    if (PackageKit::Daemon::global()->offline()->updatePrepared()) {
        StateManager::powerManager()->showPowerOffConfirmation(PowerManager::RebootInstallUpdates, tr("%1, reboot and install updates? We'll go ahead and reboot to install updates in %n seconds if you don't do anything."));
    }
}

void ShopDbus::LaunchApplication(QString desktopEntry) {
    Application a(desktopEntry);
    if (a.isValid()) a.launch();
}
