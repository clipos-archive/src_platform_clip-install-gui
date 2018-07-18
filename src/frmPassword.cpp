// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2010-2018 ANSSI. All Rights Reserved.
/**
 * @file frmPassword.cpp
 * clip-install-gui encryption password dialog.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2011 ANSSI
 * @n
 * All rights reserved.
 */

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

#include "frmPassword.h"

#define ICONPATH PREFIX"/share/icons/"

frmPassword::frmPassword(QString &where)
  : pass(where)
{
  setWindowTitle("Chiffrement de disque");
  setWindowIcon(QIcon(ICONPATH"preferences-cryptography.png"));

  edtPassword = new QLineEdit("", this);
  edtPassword->setEchoMode(QLineEdit::Password);
  edtConfirm = new QLineEdit("", this);
  edtConfirm->setEchoMode(QLineEdit::Password);

  lblMatch = new QLabel("", this);
  
  btnConfirm = new QPushButton(QIcon(ICONPATH"dialog-ok.png"), 
                                "Confirmer", this);
  btnConfirm->setEnabled(false);
  btnCancel = new QPushButton(QIcon(ICONPATH"dialog-cancel.png"), 
                                "Annuler", this);

  connect(edtPassword, SIGNAL(textChanged(const QString&)),
            this, SLOT(update()));
  connect(edtConfirm, SIGNAL(textChanged(const QString&)),
            this, SLOT(update()));

  connect(btnConfirm, SIGNAL(clicked()), this, SLOT(confirm()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  // Layout
  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(10);

  vb->addWidget(new QLabel("<b>Veuillez saisir le mot de passe de "
                    "chiffrement du disque</b>", this));
  vb->addWidget(new QLabel("<i>Ce mot de passe sera nécessaire au "
                    "démarrage du poste</i>", this));

  QHBoxLayout *hbp = new QHBoxLayout;
  hbp->setSpacing(10);
  hbp->addWidget(new QLabel("Mot de passe :", this));
  hbp->addStretch(2);
  hbp->addWidget(edtPassword);
  vb->addLayout(hbp);

  QHBoxLayout *hbc = new QHBoxLayout;
  hbc->setSpacing(10);
  hbc->addWidget(new QLabel("Confirmation :", this));
  hbc->addStretch(2);
  hbc->addWidget(edtConfirm);
  vb->addLayout(hbc);

  vb->addWidget(lblMatch);

  QHBoxLayout *hbb = new QHBoxLayout;
  hbb->setSpacing(10);
  hbb->addStretch(1);
  hbb->addWidget(btnConfirm);
  hbb->addStretch(2);
  hbb->addWidget(btnCancel);
  hbb->addStretch(1);
  vb->addLayout(hbb);
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void 
frmPassword::update()
{
  if (edtPassword->text().isEmpty() && edtConfirm->text().isEmpty()) {
    btnConfirm->setEnabled(false);
    lblMatch->setText("");
  }

  if (edtPassword->text() == edtConfirm->text()) {
    btnConfirm->setEnabled(true);
    lblMatch->setText("Les deux saisies correspondent");
  } else {
    btnConfirm->setEnabled(false);
    lblMatch->setText("<i>Les deux saisies sont différentes</i>");
  }
}

void
frmPassword::confirm()
{
  pass = edtPassword->text();
  emit(accept());
}

// vi:sw=2:ts=2:et:co=80:
