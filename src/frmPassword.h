// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2010-2018 ANSSI. All Rights Reserved.
/**
 * @file frmPassword.h
 * clip-install-gui encryption password dialog header.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011 ANSSI
 * @n
 * All rights reserved.
 */

#ifndef FRMPASSWORD_H
#define FRMPASSWORD_H

#include <QDialog>

class QString;
class QLineEdit;
class QPushButton;
class QLabel;

class frmPassword : public QDialog
{
  Q_OBJECT

public:
  frmPassword(QString &where);

 private:
  QLineEdit *edtPassword;
  QLineEdit *edtConfirm;

  QLabel *lblMatch;

  QPushButton *btnConfirm;
  QPushButton *btnCancel;

 private:
  QString &pass;

 private slots:
  void update();
  void confirm();
};

#endif // FRMPASSWORD_H

// vi:sw=2:ts=2:et:co=80:
