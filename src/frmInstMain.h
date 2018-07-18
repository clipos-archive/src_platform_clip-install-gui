// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2010-2018 ANSSI. All Rights Reserved.
/**
 * @file frmInstMain.h
 * clip-install-gui main window header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMINSTMAIN_H
#define FRMINSTMAIN_H

#include <QDialog>
#include <QStringList>

class QButtonGroup;
class QCheckBox;
class QKeyEvent;
class QListWidget;
class QPushButton;
class QTreeWidget;
class QVBoxLayout;
class QLabel;

#define TYPE_RM     0x1
#define TYPE_GW     0x2
#define TYPE_BARE   0x3

#define ENC_NONE    0x1
#define ENC_CRYPT0  0x2
#define ENC_CRYPT1  0x4

class frmInstMain : public QDialog
{
  Q_OBJECT

public:
  frmInstMain();

 private:
  QListWidget *profiles;
  QLabel      *profiles_label;
  QVBoxLayout *profiles_layout;
  QPushButton *profiles_button;

  QTreeWidget *hardwares;
  QStringList hwMakers;

  QListWidget *disks;

  QButtonGroup *types;
  QButtonGroup *encryption;

  QVBoxLayout *typeBtns;

  QPushButton *btnInstall;
  QPushButton *btnQuit;

  QCheckBox *chkKeepData;
  QCheckBox *chkVerbose;

  QString profilesPath;
  
 protected:
  virtual void keyPressEvent(QKeyEvent* e);

 private:
  void listProfiles();
  void listHardwares();
  void listDisks();
  void listTypes();
  void setProfilesPath(const QString &profiles_path);
  const QString getPassword();

 private slots:
  void update();
  void install();
  void quit();
  void select_profiles_directory();
};

#endif // FRMINSTMAIN_H

// vi:sw=2:ts=2:et:co=80:
