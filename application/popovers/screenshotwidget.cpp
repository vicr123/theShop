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
#include "screenshotwidget.h"
#include "ui_screenshotwidget.h"

#include <QNetworkAccessManager>
#include <AppStreamQt/image.h>
#include <QNetworkReply>

struct ScreenshotWidgetPrivate {
    AppStream::Screenshot screenshot;
    QPixmap image;

    QNetworkAccessManager mgr;
};

ScreenshotWidget::ScreenshotWidget(AppStream::Screenshot screenshot, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ScreenshotWidget) {
    ui->setupUi(this);

    d = new ScreenshotWidgetPrivate;
    d->screenshot = screenshot;
    ui->imagePage->installEventFilter(this);

    if (screenshot.mediaKind() == AppStream::Screenshot::MediaKindImage) {
        //Find the source image
        AppStream::Image sourceImage;
        for (AppStream::Image image : screenshot.images()) {
            if (image.kind() == AppStream::Image::KindSource) {
                sourceImage = image;
            }
        }

        QNetworkReply* reply = d->mgr.get(QNetworkRequest(sourceImage.url()));
        connect(reply, &QNetworkReply::finished, this, [ = ] {
            d->image.loadFromData(reply->readAll());
            ui->stackedWidget->setCurrentWidget(ui->imagePage);
        });
    } else {

    }
}

ScreenshotWidget::~ScreenshotWidget() {
    delete d;
    delete ui;
}


bool ScreenshotWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->imagePage) {
        QPainter painter(ui->imagePage);

        QRect imageRect;
        imageRect.setSize(d->image.size().scaled(this->width(), this->height(), Qt::KeepAspectRatio));
        imageRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));
        painter.drawPixmap(imageRect, d->image);
    }
    return false;
}
