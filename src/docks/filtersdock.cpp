/*
 * Copyright (c) 2013-2014 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 * Author: Brian Matherly <code@brianmatherly.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "filtersdock.h"
#include <QQuickView>
#include <QQuickItem>
#include <QtWidgets/QScrollArea>
#include <QQmlEngine>
#include <QDir>
#include <QUrl>
#include <QQmlContext>
#include <QAction>
#include <QIcon>
#include "qmltypes/qmlfilter.h"
#include "qmltypes/qmlutilities.h"
#include "models/metadatamodel.h"
#include "models/attachedfiltersmodel.h"

FiltersDock::FiltersDock(MetadataModel* metadataModel, AttachedFiltersModel* attachedModel, QWidget *parent) :
    QDockWidget(tr("Filters"), parent),
    m_qview()
{
    qDebug() << "begin";
    setObjectName("FiltersDock");
    QIcon filterIcon = QIcon::fromTheme("view-filter", QIcon(":/icons/oxygen/32x32/actions/view-filter.png"));
    setWindowIcon(filterIcon);
    toggleViewAction()->setIcon(windowIcon());

    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    QmlUtilities::setCommonProperties(&m_qview);
    m_qview.rootContext()->setContextProperty("metadatamodel", metadataModel);
    m_qview.rootContext()->setContextProperty("attachedfiltersmodel", attachedModel);
    setCurrentFilter(NULL, NULL);
    m_qview.setResizeMode(QQuickView::SizeRootObjectToView);
    m_qview.setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("filterview.qml"));
    m_qview.setSource(source);
    QWidget* container = QWidget::createWindowContainer(&m_qview, this);
    container->setFocusPolicy(Qt::TabFocus);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    setWidget(scrollArea);

    // Connect signals from m_qview
    QObject* root = m_qview.rootObject();
    QObject::connect(root, SIGNAL(attachFilterRequested(int)),
                     this, SIGNAL(attachFilterRequested(int)));
    QObject::connect(root, SIGNAL(currentFilterRequested(int)),
                     this, SIGNAL(currentFilterRequested(int)));

    qDebug() << "end";
}

void FiltersDock::setCurrentFilter(QmlFilter* filter, QmlMetadata* meta)
{
    m_qview.rootContext()->setContextProperty("filter", filter);
    m_qview.rootContext()->setContextProperty("metadata", meta);
}
