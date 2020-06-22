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
#include "screenshotbrowser.h"
#include "ui_screenshotbrowser.h"

#include <QToolButton>
#include "screenshotwidget.h"

struct ScreenshotBrowserPrivate {
    QList<ScreenshotWidget*> screenshots;
    QToolButton* leftButton, *rightButton;
};

ScreenshotBrowser::ScreenshotBrowser(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ScreenshotBrowser) {
    ui->setupUi(this);

    d = new ScreenshotBrowserPrivate();
    d->leftButton = new QToolButton(this);
    d->leftButton->setIcon(QIcon::fromTheme("arrow-left"));
    d->leftButton->setFixedSize(SC_DPI_T(QSize(30, 60), QSize));
    connect(d->leftButton, &QToolButton::clicked, this, [ = ] {
        int newIndex = ui->stackedWidget->currentIndex() - 1;
        if (newIndex < 0) newIndex = ui->stackedWidget->count() - 1;
        ui->stackedWidget->setCurrentIndex(newIndex);
    });
    d->leftButton->setVisible(true);

    d->rightButton = new QToolButton(this);
    d->rightButton->setIcon(QIcon::fromTheme("arrow-right"));
    d->rightButton->setFixedSize(SC_DPI_T(QSize(30, 60), QSize));
    connect(d->rightButton, &QToolButton::clicked, this, [ = ] {
        int newIndex = ui->stackedWidget->currentIndex() + 1;
        if (newIndex >= ui->stackedWidget->count()) newIndex = 0;
        ui->stackedWidget->setCurrentIndex(newIndex);
    });
    d->rightButton->setVisible(true);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

ScreenshotBrowser::~ScreenshotBrowser() {
    setScreenshots({});
    delete d;
    delete ui;
}

void ScreenshotBrowser::setScreenshots(QList<AppStream::Screenshot> screenshots) {
    for (ScreenshotWidget* widget : d->screenshots) {
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }
    d->screenshots.clear();

    if (screenshots.isEmpty()) {
        this->setVisible(false);
    } else {
        for (AppStream::Screenshot screenshot : screenshots) {
            ScreenshotWidget* w = new ScreenshotWidget(screenshot, this);
            ui->stackedWidget->addWidget(w);
        }

        if (screenshots.count() == 1) {
            d->leftButton->setVisible(false);
            d->rightButton->setVisible(false);
        } else {
            d->leftButton->setVisible(true);
            d->rightButton->setVisible(true);
        }

        this->setVisible(true);
    }
}


void ScreenshotBrowser::resizeEvent(QResizeEvent* event) {
    QSize s(16, 9);
    s.scale(this->width(), this->height(), Qt::KeepAspectRatioByExpanding);
    this->setFixedHeight(s.height());

    d->leftButton->move(0, this->height() / 2 - d->leftButton->height() / 2);
    d->rightButton->move(this->width() - d->rightButton->width(), this->height() / 2 - d->rightButton->height() / 2);
}
