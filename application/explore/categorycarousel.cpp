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
#include "categorycarousel.h"
#include "ui_categorycarousel.h"

#include "metadatarepository.h"
#include "explorepage.h"
#include <QToolButton>
#include <QRandomGenerator>
#include "popovers/appinfopopover.h"
#include <tpopover.h>

struct CategoryCarouselPrivate {

};

CategoryCarousel::CategoryCarousel(QString category, ExplorePage* parent) :
    QWidget(parent),
    ui(new Ui::CategoryCarousel) {
    ui->setupUi(this);
    d = new CategoryCarouselPrivate();

    ui->categoryLabel->setText(category.toUpper());

    QList<AppStream::Component> components = MetadataRepository::instance()->componentsByCategory(category);
    for (int i = 0; i < 32; i++) {
        AppStream::Component component = components.takeAt(QRandomGenerator::global()->bounded(components.count()));
        QString componentId = component.id();

        QSize iconSize = SC_DPI_T(QSize(64, 64), QSize);

        QToolButton* button = new QToolButton(this);
        button->setText(component.name());
        button->setIcon(QIcon::fromTheme("generic-app"));
        button->setIconSize(iconSize);
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//        button->setFixedSize(SC_DPI_T(QSize(128, 128), QSize));
        MetadataRepository::instance()->iconForComponent(component, iconSize)->then([ = ](QIcon icon) {
            button->setIcon(icon);
        });
        connect(button, &QToolButton::clicked, this, [ = ] {
            AppInfoPopover* appInfo = new AppInfoPopover(componentId);
            tPopover* popover = new tPopover(appInfo);
            popover->setPopoverWidth(SC_DPI(800));
            connect(appInfo, &AppInfoPopover::done, popover, &tPopover::dismiss);
            connect(popover, &tPopover::dismissed, appInfo, &AppInfoPopover::deleteLater);
            connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
            popover->show(parent);
        });
        ui->componentsLayout->addWidget(button);

        ui->scrollArea->setFixedHeight(button->sizeHint().height() + QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent));
    }

}

CategoryCarousel::~CategoryCarousel() {
    delete d;
    delete ui;
}
