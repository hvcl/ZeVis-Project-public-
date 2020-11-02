/********************************************************************************
** Form generated from reading UI file 'ZebrafishAnalytics.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZEBRAFISHANALYTICS_H
#define UI_ZEBRAFISHANALYTICS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZebrafishAnalyticsClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ZebrafishAnalyticsClass)
    {
        if (ZebrafishAnalyticsClass->objectName().isEmpty())
            ZebrafishAnalyticsClass->setObjectName(QStringLiteral("ZebrafishAnalyticsClass"));
        ZebrafishAnalyticsClass->resize(600, 400);
        menuBar = new QMenuBar(ZebrafishAnalyticsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        ZebrafishAnalyticsClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ZebrafishAnalyticsClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ZebrafishAnalyticsClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ZebrafishAnalyticsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ZebrafishAnalyticsClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ZebrafishAnalyticsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ZebrafishAnalyticsClass->setStatusBar(statusBar);

        retranslateUi(ZebrafishAnalyticsClass);

        QMetaObject::connectSlotsByName(ZebrafishAnalyticsClass);
    } // setupUi

    void retranslateUi(QMainWindow *ZebrafishAnalyticsClass)
    {
        ZebrafishAnalyticsClass->setWindowTitle(QApplication::translate("ZebrafishAnalyticsClass", "ZebrafishAnalytics", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZebrafishAnalyticsClass: public Ui_ZebrafishAnalyticsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZEBRAFISHANALYTICS_H
