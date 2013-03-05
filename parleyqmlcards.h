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

#ifndef PARLEYQMLCARDS_HEADER
#define PARLEYQMLCARDS_HEADER

#include <Plasma/PopupApplet>
#include <Plasma/DataEngine>

#include "ui_config.h"

class QFont;

namespace Plasma {
    class DeclarativeWidget;
}

class ParleyQmlCards : public Plasma::PopupApplet {
    Q_OBJECT
    Q_PROPERTY(QVariant fileName READ fileName)
    Q_PROPERTY(QVariant lang1 READ lang1)
    Q_PROPERTY(QVariant lang2 READ lang2)
    Q_PROPERTY(QVariant updateInterval READ updateInterval)
    Q_PROPERTY(QVariant alwaysRevealSolution READ alwaysRevealSolution)
    Q_PROPERTY(QVariant font READ font)
public:
    ParleyQmlCards(QObject *parent, const QVariantList &args);
    ~ParleyQmlCards();

    virtual void init();
    QGraphicsWidget* graphicsWidget();
    virtual void createConfigurationInterface(KConfigDialog *parent);

    QString fileName() const { return m_sourceFile.url(); };
    QString lang1() const {
        if (m_languages.size() > m_lang1) return m_languages.at(m_lang1);
        else return QString("");
    }
    QString lang2() const {
        if (m_languages.size() > m_lang2) return m_languages.at(m_lang2);
        else return QString("");
    }
    int updateInterval() const { return m_updateInterval; }
    bool alwaysRevealSolution() const { return m_solutionType == Always; }
    QString font() const { return m_font.family(); }

public slots:
    virtual void configAccepted();
    void configChanged();
    void urlSelected(const KUrl &);
    void dataUpdated(const QString &name, const Plasma::DataEngine::Data &data);
    void showFontSelectDlg();

signals:
    void configUpdated();

protected:
private:
    enum ShowSolution {
        Hover = 0,
        Always
    };
    Plasma::DeclarativeWidget *m_declarativeWidget;
    Ui::config ui;

    QFont m_font;

    Plasma::DataEngine* m_engine;
    KUrl m_sourceFile;
    QStringList m_languages;
    int m_solutionType;
    int m_lang1;
    int m_lang2;
    int m_updateInterval;
};

K_EXPORT_PLASMA_APPLET(parleyqmlcards, ParleyQmlCards)

#endif
