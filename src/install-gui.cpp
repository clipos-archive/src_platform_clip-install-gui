// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2010-2018 ANSSI. All Rights Reserved.
/**
 * @file install-gui.cpp
 * clip-install-gui main.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2009 SGDN
 * Copyright (C) 2010 ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QTranslator>

#include <QtSingleApplication>

#include <stdlib.h>

#include "frmInstMain.h"

int 
main(int argc, char** argv) 
{
  QString appId;

  QtSingleApplication app(appId, argc, argv);
  if (app.sendMessage("Hi there") || app.isRunning())
    return 0;

  QTranslator qt(0);
  qt.load ("qt_fr.qm", PREFIX"/share/qt4/translations");
  app.installTranslator(&qt);
    
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  QTextCodec::setCodecForCStrings(codec);

  // Création et affichage de la fenêtre principale
  QWidget* window = new frmInstMain();

  app.setActivationWindow(window);
  QObject::connect(&app, SIGNAL(messageReceived(const QString &)), 
                                  &app, SLOT(activateWindow()));
  window->show();
  exit(app.exec());
}

// vi:sw=2:ts=2:et:co=80:
