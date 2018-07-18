// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2010-2018 ANSSI. All Rights Reserved.
/**
 * @file frmInstMain.cpp
 * clip-install-gui main window.
 * @author Vincent Strubel <clipos@ssi.gouv.fr>
 *
 * Copyright (C) 2010-2012 SGDSN/ANSSI
 * @n
 * All rights reserved.
 */

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QFileDialog>

#include <ClipWidgets/WaitDialog.h>

#include "frmInstMain.h"
#include "frmPassword.h"

#define ICONPATH PREFIX"/share/icons/"

// Uncomment for testing...
//#define TEST 

const QString mirrorsPath("/mnt/cdrom/mirrors");
const QString hwPath("/opt/clip-installer/hw_conf");
const QString listDisksCmd(PREFIX"/bin/clip-list-install-disks");
const QString profiles_default_directory("/mnt/cdrom/config");
const QString termCmd("xterm");
const QString installerCmd("/sbin/full_install.sh");

frmInstMain::frmInstMain()
{
  profilesPath = QString("non configuré"); 
  
  setWindowTitle("Installeur CLIP");
  setWindowIcon(QIcon(ICONPATH"advanced.png"));
  setMinimumWidth(750);

  types = new QButtonGroup(this);
  types->setExclusive(true);
  typeBtns = new QVBoxLayout;
  typeBtns->setSpacing(8);
  
  profiles_layout = new QVBoxLayout;
  profiles_layout->setSpacing(2);  
  profiles = new QListWidget(this);
  profiles->setSelectionMode(QAbstractItemView::SingleSelection);  
  profiles_label = new QLabel(this);
  profiles_button = new QPushButton("Modifier le répertoire", this);
  profiles_button->setToolTip("Permet de choisir le répertoire contenant les profils.");
  
  hardwares = new QTreeWidget(this);
  hardwares->setSelectionMode(QAbstractItemView::SingleSelection);
  hardwares->setHeaderLabel("Fabricant / Modèle");
  
  disks = new QListWidget(this);
  disks->setSelectionMode(QAbstractItemView::ExtendedSelection);

  encryption = new QButtonGroup(this);
  encryption->setExclusive(true);
  QRadioButton *btnEncNone = new QRadioButton("Données seules", this);
  btnEncNone->setToolTip(
    "Dans ce mode, seules les données des utilisateurs sont chiffrées,\n"
    "le reste du système n'est pas protégé en confidentialité.");
  btnEncNone->setIcon(QIcon(ICONPATH"security-low.png"));
  QRadioButton *btnEncCrypt0 = new QRadioButton("Données + système", this);
  btnEncCrypt0->setToolTip(
    "Dans ce mode, outre le chiffrement individuel des données de chaque\n"
    "utilisateur, le système est globalement protégé en confidentialité\n"
    "par un chiffrement intégral. Un mot de passe défini à l'installation\n"
    "est nécessaire pour démarrer le système.");
  btnEncCrypt0->setIcon(QIcon(ICONPATH"security-high.png"));
  QRadioButton *btnEncCrypt1 = new QRadioButton("Données chiffrées, "
                                                "système effaçable", this);
  btnEncCrypt1->setToolTip(
    "Ce mode s'apparente au mode 'données seules' en ce sens que seules les\n"
    "données des utilisateurs sont protégées en confidentialité. Aucun mot\n"
    "de passe n'est nécessaire pour démarrer le poste. En revanche, un\n"
    "chiffrement est mis en oeuvre sur le reste du système, afin d'en\n"
    "faciliter l'effacement a posteriori (sans en garantir la confidentialité\n"
    "en cours de fonctionnement).");
  btnEncCrypt1->setIcon(QIcon(ICONPATH"security-medium.png"));
  encryption->addButton(btnEncNone, ENC_NONE);
  encryption->addButton(btnEncCrypt0, ENC_CRYPT0);
  encryption->addButton(btnEncCrypt1, ENC_CRYPT1);
  btnEncCrypt1->setChecked(true);
  
  btnInstall = new QPushButton(QIcon(ICONPATH"dialog-ok.png"), 
                                        "Lancer l'installation", this);
  btnInstall->setDefault(false);
  btnInstall->setAutoDefault(false);
  btnInstall->setEnabled(false);

  btnQuit = new QPushButton(QIcon(ICONPATH"dialog-close.png"), "Quitter", this);
  btnQuit->setDefault(false);
  btnQuit->setAutoDefault(false);

  chkKeepData = new QCheckBox("Conserver les données", this);
  chkKeepData->setChecked(false);
  chkKeepData->setToolTip("Conserver les données utilisateur d'une "
    "installation CLIP précédente");

  chkVerbose = new QCheckBox("Mode verbeux", this);
  chkVerbose->setChecked(false);
  chkVerbose->setToolTip("Active la génération de messages plus verbeux en "
    "cours d'installation");

  connect(btnQuit, SIGNAL(clicked()), this, SLOT(quit()));
  connect(btnInstall, SIGNAL(clicked()), this, SLOT(install()));

  connect(profiles_button, SIGNAL(clicked()), this, SLOT(select_profiles_directory()));
  
  connect(types, SIGNAL(buttonClicked(int)), this, SLOT(update()));
  connect(profiles, SIGNAL(itemSelectionChanged()), this, SLOT(update()));
  connect(hardwares, SIGNAL(itemSelectionChanged()), this, SLOT(update()));
  connect(disks, SIGNAL(itemSelectionChanged()), this, SLOT(update()));

  /****** Tabs ******/

  /****** Layout ******/
  QVBoxLayout *vb = new QVBoxLayout(this);
  vb->setSpacing(6);
  
  profiles_layout->addWidget(profiles_label);
  profiles_layout->addWidget(profiles);
  profiles_layout->addWidget(profiles_button);  
    
  QGridLayout *gl = new QGridLayout;
  gl->addWidget(new QLabel("<b>Type d'installation</b>", this), 0, 0);
  gl->addWidget(new QLabel("<b>Profil de configuration</b>", this), 0, 1);
  gl->addWidget(new QLabel("<b>Profil matériel</b>", this), 0, 2);
  gl->addWidget(new QLabel("<b>Disque(s)</b>", this), 0, 3);
  gl->addLayout(typeBtns, 1, 0);
  gl->addLayout(profiles_layout, 1, 1);
  gl->addWidget(hardwares, 1, 2);
  gl->addWidget(disks, 1, 3);

  gl->setColumnStretch(0, 1);
  gl->setColumnStretch(1, 2);
  gl->setColumnStretch(2, 2);
  gl->setColumnStretch(3, 1);

  gl->setColumnMinimumWidth(1, 100);
  gl->setColumnMinimumWidth(2, 120);

  vb->addLayout(gl);
  vb->addSpacing(10);

  vb->addWidget(new QLabel("<b><u>Chiffrement du disque</u> :</b>", this));
  vb->addSpacing(5);

  QHBoxLayout *hbe = new QHBoxLayout;
  hbe->setSpacing(6);
  hbe->addSpacing(15);
  hbe->addWidget(btnEncNone);
  hbe->addSpacing(10);
  hbe->addWidget(btnEncCrypt0);
  hbe->addSpacing(10);
  hbe->addWidget(btnEncCrypt1);
  hbe->addSpacing(10);
  hbe->addStretch(5);

  vb->addLayout(hbe);
  vb->addSpacing(10);

  vb->addWidget(new QLabel("<b><u>Options</u> :</b>", this));
  vb->addSpacing(5);

  QHBoxLayout *hbo = new QHBoxLayout;
  hbo->setSpacing(6);
  hbo->addSpacing(15);
  hbo->addWidget(chkKeepData);
  hbo->addSpacing(10);
  hbo->addWidget(chkVerbose);
  hbo->addSpacing(10);
  hbo->addStretch(5);

  vb->addLayout(hbo);
  vb->addStretch(1);
  vb->addSpacing(15);

  QHBoxLayout *hbb = new QHBoxLayout;
  hbb->setSpacing(6);
  hbb->addStretch(5);
  hbb->addSpacing(20);
  hbb->addWidget(btnInstall);
  hbb->addSpacing(20);
  hbb->addWidget(btnQuit);
  hbb->addSpacing(20);
  hbb->addStretch(5);

  vb->addLayout(hbb);
  vb->addStretch(1);

  /****** Initialization ******/
  setProfilesPath(profiles_default_directory);
  listProfiles();
  listHardwares();
  listDisks();
  listTypes();
}

/*************************************************************/
/*                    Protected slots                        */
/*************************************************************/

void 
frmInstMain::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape) {
    e->accept();
    quit();
  }
}

/*************************************************************/
/*                      Private slots                        */
/*************************************************************/

void
frmInstMain::quit()
{
  QApplication::exit(0);
}

void
frmInstMain::install()
{
  QProcess installer;
  QString type("invalid");
  switch (types->checkedId()) {
    case TYPE_RM:
      type = "rm";
      break;
    case TYPE_GW:
      type = "gtw";
      break;
    case TYPE_BARE:
      type = "bare";
      break;
  }

  QString profile = profilesPath + "/" + 
                      profiles->selectedItems().first()->text();
  QTreeWidgetItem *item = hardwares->selectedItems().first();
  QTreeWidgetItem *parent = item->parent();
  QString hardware;
  if (!parent || parent->text(0) == "Autres")
    hardware = item->text(0);
  else
    hardware = QString("%1_%2").arg(parent->text(0)).arg(item->text(0));
  QList<QListWidgetItem *> dlist = disks->selectedItems();

  QStringList args;
  args << "-title" << "Installation CLIP";
  args << "-hold" << "-e" << installerCmd;
  args << "-t" << type;
  args << "-c" << profile;
  args << "-H" << hardware;

  QString pass;
  switch (encryption->checkedId()) {
    case ENC_NONE:
      break;
    case ENC_CRYPT0:
    pass = getPassword();
      if (pass.isEmpty())
        return;
      args << "-C" << "crypt0" << "-P" << pass;
      break;
    case ENC_CRYPT1:
      args << "-C" << "crypt1";
      break;
  }

  if (chkKeepData->isChecked())
    args << "-k";
  if (chkVerbose->isChecked())
    args << "-V";

  if (QMessageBox::question (0, "Lancer l'installation ?",
           "<p>Êtes-vous certain de vouloir lancer l'installation ?</p>"
           "<p><b>Attention :</b> le contenu du ou des disques sélectionnés "
           "sera intégralement effacé !</p>",
           QMessageBox::Yes | QMessageBox::Default,
           QMessageBox::No | QMessageBox::Escape) != QMessageBox::Yes)
    return;

  for (QList<QListWidgetItem *>::const_iterator i = dlist.constBegin();
                                            i != dlist.constEnd(); i++) {
    args << (*i)->text();
  }

#ifdef TEST
  QString cmd = args.join(" ");
  qDebug("Running %s", cmd.toStdString().c_str());
  return;
#endif

  btnInstall->setEnabled(false);
  installer.start(termCmd, args);
  if (!installer.waitForStarted()) {
    QMessageBox::warning (0, "Echec de l'installation", 
                  "L'installeur CLIP n'a pas pu être lancé.",
                  QMessageBox::Ok | QMessageBox::Default, 0, 0);
    return;
  }

  WaitDialog waiter("Installation en cours",
      "<p>L'installation CLIP est en cours. Vous pouvez suivre sa "
      "progression dans la fenêtre \"Installation CLIP\".</p><p>A l'issue "
      "de l'installation, vous pourrez fermer cette même fenêtre pour "
      "revenir à l'outil d'installation.</p>", installer);
  waiter.exec();
  btnInstall->setEnabled(true);
}

void
frmInstMain::update()
{
  bool ok;
  ok = ((!profiles->selectedItems().isEmpty()) && 
        (!hardwares->selectedItems().isEmpty()) &&
        (!disks->selectedItems().isEmpty()) &&
        (types->checkedId() != 0));
  btnInstall->setEnabled(ok);
}


void 
frmInstMain::select_profiles_directory() {
  QString dir = QFileDialog::getExistingDirectory(this, 
						  tr("Open Directory"),
						  profilesPath,
						  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  
  if (dir == "") return;
    
  setProfilesPath(dir);
  profiles->clear();
  listProfiles();
}


/*************************************************************/
/*                          Private                          */
/*************************************************************/

void
frmInstMain::listProfiles()
{
  QDir profs(profilesPath);

  QStringList plist = profs.entryList(
    QDir::Dirs|QDir::Readable|QDir::Executable|QDir::NoDotAndDotDot, 
    QDir::Name);

  for (QStringList::const_iterator i = plist.constBegin(); 
                                    i != plist.constEnd(); i++) {
    profiles->addItem(new 
        QListWidgetItem(QIcon(ICONPATH"info.png"), (*i)));
  }
  profiles->clearSelection();
}

void
frmInstMain::listHardwares()
{
  QDir hw(hwPath);
  QString hardware, maker, model;

  QStringList hlist = hw.entryList(
    QDir::Dirs|QDir::Readable|QDir::Executable|QDir::NoDotAndDotDot, 
    QDir::Name);

  for (QStringList::const_iterator i = hlist.constBegin(); 
                                    i != hlist.constEnd(); i++) {
    hardware = *i;
    if (hardware.contains('_')) {
      maker = hardware.left(hardware.indexOf('_'));
      model = hardware.right(hardware.size() - hardware.indexOf('_') - 1);
    } else {
      maker = QString("Autres");
      model = hardware;
    }

    if (maker.isEmpty() || model.isEmpty()) {
      qDebug("Invalid hardware: %s", hardware.toStdString().c_str());
      continue;
    }

    int idx = hwMakers.indexOf(maker);
    
    QTreeWidgetItem *topLevel;
    if (idx == -1) {
      topLevel = new QTreeWidgetItem;
      topLevel->setIcon(0, QIcon(ICONPATH"info.png"));
      topLevel->setText(0, maker);
      topLevel->setFlags(topLevel->flags() & ~Qt::ItemIsSelectable);
      hardwares->addTopLevelItem(topLevel);
      hwMakers.push_back(maker);
    } else {
      topLevel = hardwares->topLevelItem(idx);
      if (!topLevel) {
        qDebug("No parent for maker %s, index %d", 
                      maker.toStdString().c_str(), idx);
        continue;
      }
    }

    QFile type(hwPath + QDir::separator() + hardware 
                          + QDir::separator() + "formfactor");
    QIcon icon; 
    if (type.exists() && type.open(QIODevice::ReadOnly)) {
      QString typestr = type.readLine();
      type.close();
      typestr.replace('\n', "");
      if (typestr == "server/rack")
        icon = QIcon(ICONPATH"computer-server.png");
      else if (typestr == "pc/desktop")
        icon = QIcon(ICONPATH"computer-desktop.png");
      else if (typestr == "pc/tablet")
        icon = QIcon(ICONPATH"computer-tablet.png");
      else 
        icon = QIcon(ICONPATH"computer-laptop.png");
    } else
      icon = QIcon(ICONPATH"computer-laptop.png");

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setIcon(0, icon);
    item->setText(0, model);
  
    topLevel->addChild(item);

  }
  hardwares->clearSelection();
  hardwares->sortItems(0, Qt::AscendingOrder);
}

void
frmInstMain::listDisks()
{
  QProcess lister;
  lister.setReadChannel(QProcess::StandardOutput);
  lister.start(listDisksCmd);
  lister.waitForFinished();

#ifdef TEST
  QStringList dlist = QString("/dev/sde\n/dev/sdf\n").split('\n', 
                                          QString::SkipEmptyParts);
#else 
  QStringList dlist = QString(lister.readAll()).split('\n', 
                                          QString::SkipEmptyParts);
#endif

  for (QStringList::const_iterator i = dlist.constBegin(); 
                                    i != dlist.constEnd(); i++) {
    disks->addItem(new 
        QListWidgetItem(QIcon(ICONPATH"drive-harddisk.png"), (*i)));
  }
  disks->clearSelection();
}

void
frmInstMain::listTypes()
{
  QRadioButton *btn;
  if (QFile::exists(mirrorsPath + "/clip4-rm-dpkg")) {
    btn = new QRadioButton("Client", this);
    types->addButton(btn, TYPE_RM);
    typeBtns->addWidget(btn);
    typeBtns->addSpacing(5);
    btn->setChecked(true);
  }
  if (QFile::exists(mirrorsPath + "/clip4-gtw-dpkg")) {
    btn = new QRadioButton("Passerelle", this);
    types->addButton(btn, TYPE_GW);
    typeBtns->addWidget(btn);
    typeBtns->addSpacing(5);
  }
  if (QFile::exists(mirrorsPath + "/clip4-bare-dpkg")) {
    btn = new QRadioButton("Bare", this);
    types->addButton(btn, TYPE_BARE);
    typeBtns->addWidget(btn);
    typeBtns->addSpacing(5);
  }
  typeBtns->addStretch(5);
}

const QString
frmInstMain::getPassword()
{
  QString pass("");
  frmPassword d(pass);
  if (!d.exec())
    return "";

  return pass;
}

void frmInstMain::setProfilesPath(const QString &profiles_path) {
  profilesPath = profiles_path;
  profiles_label->setText("Répertoire : "+ profilesPath);  
}

// vi:sw=2:ts=2:et:co=80:
