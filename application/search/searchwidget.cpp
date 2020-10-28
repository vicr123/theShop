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
#include "searchwidget.h"
#include "ui_searchwidget.h"

#include "searchmodel.h"
#include "popovers/appinfopopover.h"
#include <tpopover.h>

struct SearchWidgetPrivate {
    SearchModel* model;
};

SearchWidget::SearchWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget) {
    ui->setupUi(this);

    d = new SearchWidgetPrivate();
    d->model = new SearchModel();

    ui->resultList->setFixedWidth(SC_DPI(600));
    ui->searchField->setFixedWidth(SC_DPI(600));
    ui->searchSplash->setPixmap(QIcon::fromTheme("edit-find").pixmap(SC_DPI_T(QSize(128, 128), QSize)));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->resultList->setModel(d->model);
}

SearchWidget::~SearchWidget() {
    d->model->deleteLater();
    delete d;
    delete ui;
}

void SearchWidget::on_searchField_textChanged(const QString& arg1) {
    if (arg1 == "") {
        ui->stackedWidget->setCurrentWidget(ui->splashPage);
    } else {
        //Search!
        ui->stackedWidget->setCurrentWidget(ui->loadingPage);
        d->model->search(arg1)->then([ = ] {
            ui->stackedWidget->setCurrentWidget(ui->resultsPage);
        });
    }
}

void SearchWidget::on_resultList_activated(const QModelIndex& index) {
    AppInfoPopover* appInfo = new AppInfoPopover(index.data(Qt::UserRole).toString());
    tPopover* popover = new tPopover(appInfo);
    popover->setPopoverWidth(SC_DPI(800));
    connect(appInfo, &AppInfoPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, appInfo, &AppInfoPopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
}
