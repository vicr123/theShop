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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <tcsdtools.h>

struct MainWindowPrivate {
    tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->csdLayoutLeft->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->csdLayoutRight->addWidget(d->csd.csdBoxForWidget(this));
    }

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_exploreButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->explorePage);
    }
}

void MainWindow::on_updatesButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->updatesPage);
    }
}
