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
#include "explorepage.h"
#include "ui_explorepage.h"

#include <QRandomGenerator>
#include "categorycarousel.h"

ExplorePage::ExplorePage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ExplorePage) {
    ui->setupUi(this);

    QStringList categories = {
        "AudioVideo",
        "Audio",
        "Video",
        "Development",
        "Education",
        "Game",
        "Graphics",
        "Network",
        "Office",
        "Science",
        "Settings",
        "System",
        "Utility"
    };

    for (int i = 0; i < 4; i++) {
        QString category = categories.takeAt(QRandomGenerator::global()->bounded(categories.count()));
        CategoryCarousel* carousel = new CategoryCarousel(category, this);
        ui->mainExploreLayout->addWidget(carousel);
    }
}

ExplorePage::~ExplorePage() {
    delete ui;
}
