/*
 * Copyright 2013 Javier Goday Caneda <jgoday@gmail.com>
 * Copyright 2008 Frederik Gladhorn <frederik.gladhorn@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "parleyqmlcards.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

#include <KConfigDialog>
#include <KConfigGroup>
#include <KFontDialog>

#include <Plasma/DeclarativeWidget>
#include <Plasma/Package>


ParleyQmlCards::ParleyQmlCards(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args)
    , m_declarativeWidget(0)
    , m_font(QFont())
{
    setPopupIcon("parley");
    resize(430, 290);
    setHasConfigurationInterface(true);
    //setAspectRatioMode(Plasma::KeepAspectRatio);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(NoBackground);
    setAssociatedApplication("parley");
}

ParleyQmlCards::~ParleyQmlCards()
{
    delete m_declarativeWidget;
}

void ParleyQmlCards::init()
{
    m_engine = dataEngine("parley");
    m_engine->connectSource(m_sourceFile.url(), this, m_updateInterval);

    configChanged();

    PopupApplet::init();
}

QGraphicsWidget* ParleyQmlCards::graphicsWidget()
{
    if (!m_declarativeWidget) {
        m_declarativeWidget = new Plasma::DeclarativeWidget();
        m_declarativeWidget->engine()->rootContext()->setContextProperty("plasmoid", this);

        Plasma::PackageStructure::Ptr structure = Plasma::PackageStructure::load("Plasma/Generic");
        Plasma::Package package(QString(), "parleyqmlcards", structure);
        m_declarativeWidget->setQmlPath(package.filePath("mainscript"));
        m_declarativeWidget->setMinimumSize( 100, 100 );
    }
    return m_declarativeWidget;
}

void ParleyQmlCards::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget();
    ui.setupUi(widget);
    parent->addPage(widget, parent->windowTitle(), icon());
    parent->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));

    connect( ui.fontSelectButton, SIGNAL(clicked()), this, SLOT(showFontSelectDlg()) );

    ui.updateIntervalSpinBox->setValue(m_updateInterval/1000);
    ui.updateIntervalSpinBox->setSuffix(ki18np(" second", " seconds"));
    KConfigGroup cg = config();

    ui.filechooser->setUrl(m_sourceFile);
    ui.filechooser->setFilter(i18n("*.kvtml|Vocabulary Collections"));
    connect(ui.filechooser, SIGNAL(urlSelected (const KUrl &)), this, SLOT(urlSelected (const KUrl &)));
    ui.language1->addItems(m_languages);
    ui.language2->addItems(m_languages);
    ui.language1->setCurrentIndex(m_lang1);
    ui.language2->setCurrentIndex(m_lang2);

    switch (m_solutionType) {
    case Hover:
        ui.solutionOnHover->setChecked(true);
        break;
    case Always:
        ui.solutionAlways->setChecked(true);
        break;
    }

    connect(ui.fontSelectButton, SIGNAL(clicked()), parent, SLOT(settingsModified()));
    connect(ui.solutionOnHover, SIGNAL(toggled(bool)), parent, SLOT(settingsModified()));
    connect(ui.solutionAlways, SIGNAL(toggled(bool)), parent, SLOT(settingsModified()));
    connect(ui.filechooser, SIGNAL(urlSelected (const KUrl &)), parent, SLOT(settingsModified()));
    connect(ui.updateIntervalSpinBox, SIGNAL(valueChanged(int)), parent, SLOT(settingsModified()));
}

void ParleyQmlCards::urlSelected(const KUrl &)
{
    m_engine->disconnectSource(m_sourceFile.url(), this);
    // connect to new file
    m_sourceFile = ui.filechooser->url();
    m_engine->connectSource(m_sourceFile.url(), this, m_updateInterval);
    // get language data
    Plasma::DataEngine::Data data = m_engine->query(m_sourceFile.url());
    m_languages = data.keys();
    // update language selection
    ui.language1->clear();
    ui.language2->clear();
    ui.language1->addItems(m_languages);
    ui.language2->addItems(m_languages);
    // just select the first languages - better than nothing ;)
    ui.language1->setCurrentIndex(0);
    ui.language2->setCurrentIndex(1);
}

void ParleyQmlCards::configAccepted()
{
    prepareGeometryChange();

    KConfigGroup cg = config();
    cg.writeEntry("font", m_font);
    //m_label1->setFont(m_font);
    //m_label2->setFont(m_font);
    m_updateInterval = ui.updateIntervalSpinBox->value()*1000;
    cg.writeEntry("updateInterval", m_updateInterval);

    m_engine->disconnectSource(m_sourceFile.url(), this);
    m_sourceFile = ui.filechooser->url();
    cg.writeEntry("File Name", m_sourceFile);

    m_solutionType = Hover;
    if (ui.solutionAlways->isChecked()) {
        m_solutionType = Always;
        //m_label2->show();
    }
    cg.writeEntry("Solution", m_solutionType);

    m_lang1 = ui.language1->currentIndex();
    m_lang2 = ui.language2->currentIndex();

    cg.writeEntry("Top Language", m_lang1);
    cg.writeEntry("Bottom Language", m_lang2);
    cg.writeEntry("Languages", m_languages);

    m_engine->connectSource(m_sourceFile.url(), this, m_updateInterval);

    emit configNeedsSaving();
    emit configUpdated();
}

void ParleyQmlCards::configChanged()
{
    KConfigGroup cg = config();
    m_updateInterval = cg.readEntry("updateInterval", 10000);
    m_solutionType = cg.readEntry("Solution", 0);

    m_languages = cg.readEntry("Languages", QStringList());
    m_lang1 = cg.readEntry("Top Language", 0);
    m_lang2 = cg.readEntry("Bottom Language", 1);
    m_font = cg.readEntry("font", QFont());

    m_sourceFile = cg.readEntry("File Name", KUrl());
    if (m_sourceFile.isEmpty()) {
        KConfig parleyConfig("parleyrc");
        KConfigGroup recentFilesGroup( &parleyConfig, "Recent Files" );
        // take the last file, but there are File1..n and Name1..n entries..
        m_sourceFile = recentFilesGroup.readEntry(
            recentFilesGroup.keyList().value(recentFilesGroup.keyList().count()/2-1), KUrl() );
    }

    emit configUpdated();
}

void ParleyQmlCards::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    m_languages = data.keys();
    m_engine->disconnectSource(m_sourceFile.url(), this);
}

void ParleyQmlCards::showFontSelectDlg()
{
    KFontDialog::getFont(m_font);
}

#include "parleyqmlcards.moc"
