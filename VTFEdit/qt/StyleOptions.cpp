#include "StyleOptions.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QStyleFactory>
#include <QDialogButtonBox>
#include <QPushButton>

QPalette VTFEdit::StyleOptions::getTheme(VTFEditTheme theme) const
{
    switch (theme) {
        case VTFEditTheme::System:
            return this->m_systemPalette;
        case VTFEditTheme::Light:
        {
            QPalette palette { };
            palette.setColor( QPalette::Text, Qt::black );
            palette.setColor( QPalette::WindowText, Qt::black );
            palette.setColor( QPalette::ToolTipBase, 0xefefef );
            palette.setColor( QPalette::ToolTipText, Qt::black );
            palette.setColor( QPalette::ButtonText, Qt::black );
            palette.setColor( QPalette::BrightText, Qt::black );
            palette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
            palette.setColor( QPalette::HighlightedText, Qt::black );
            palette.setColor( QPalette::Active, QPalette::BrightText, Qt::black );

            palette.setColor( QPalette::Dark, 0xfbfbfb );
            palette.setColor( QPalette::Light, 0xfbfbfb );

            palette.setColor( QPalette::Base, 0xefefef );
            palette.setColor( QPalette::Window, 0xefefef );
            palette.setColor( QPalette::Button, QColor( 240, 240, 240 ) );
            palette.setColor( QPalette::AlternateBase, 0xefefef );

            palette.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 150, 150, 150 ) );
            palette.setColor( QPalette::Disabled, QPalette::WindowText, QColor( 150, 150, 150 ) );
            palette.setColor( QPalette::Disabled, QPalette::Text, QColor( 150, 150, 150 ) );
            return palette;
        }
        case VTFEditTheme::Dark:
        {
            QPalette palette { };
            palette.setColor( QPalette::Text, Qt::white );
            palette.setColor( QPalette::WindowText, Qt::white );
            palette.setColor( QPalette::ToolTipText, Qt::white );
            palette.setColor( QPalette::ButtonText, Qt::white );
            palette.setColor( QPalette::BrightText, Qt::red );
            palette.setColor( QPalette::HighlightedText, Qt::black );
            palette.setColor( QPalette::Window, QColor( 49, 54, 59 ) );
            palette.setColor( QPalette::Base, QColor( 27, 30, 32 ) );
            palette.setColor( QPalette::AlternateBase, QColor( 49, 54, 59 ) );
            palette.setColor( QPalette::ToolTipBase, Qt::black );
            palette.setColor( QPalette::Button, QColor( 49, 54, 59 ) );
            palette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
            palette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
            palette.setColor( QPalette::Active, QPalette::Button, QColor( 49, 54, 59 ) );
            palette.setColor( QPalette::Disabled, QPalette::ButtonText, Qt::darkGray );
            palette.setColor( QPalette::Disabled, QPalette::WindowText, Qt::darkGray );
            palette.setColor( QPalette::Disabled, QPalette::Text, Qt::darkGray );
            palette.setColor( QPalette::Disabled, QPalette::Light, QColor( 49, 54, 59 ) );

            palette.setColor( QPalette::Dark, QColor( 21, 23, 26 ) );
            palette.setColor( QPalette::Light, QColor( 120, 122, 130 ) );
            palette.setColor( QPalette::Shadow, QColor( 31, 33, 36 ) );

            return palette;
        }
        case VTFEditTheme::VGUI:
        {
            QPalette palette { };
            palette.setColor( QPalette::Text, QColor( 216, 222, 211 ) );
            palette.setColor( QPalette::WindowText, QColor( 216, 222, 211 ) );
            palette.setColor( QPalette::ToolTipText, QColor( 216, 222, 211 ) );
            palette.setColor( QPalette::ButtonText, QColor( 216, 222, 211 ) );
            palette.setColor( QPalette::BrightText, QColor( 255, 255, 255 ) );
            palette.setColor( QPalette::HighlightedText, QColor( 200, 200, 200 ) );
            palette.setColor( QPalette::Window, QColor( 62, 70, 55 ) );
            palette.setColor( QPalette::Base, QColor( 76, 88, 68 ) );
            palette.setColor( QPalette::AlternateBase, QColor( 90, 106, 80 ) );
            palette.setColor( QPalette::ToolTipBase, QColor( 90, 106, 80 ) );
            palette.setColor( QPalette::Button, QColor( 76, 88, 68 ) );
            palette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
            palette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
            palette.setColor( QPalette::Active, QPalette::BrightText, QColor( 196, 181, 80 ) );
            palette.setColor( QPalette::Disabled, QPalette::Text, QColor( 117, 128, 111 ) );
            palette.setColor( QPalette::Disabled, QPalette::WindowText, QColor( 137, 148, 131 ) );
            palette.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 137, 148, 131 ) );

            palette.setColor( QPalette::Dark, QColor( 40, 46, 34 ) );
            palette.setColor( QPalette::Light, QColor( 136, 145, 128 ) );
            palette.setColor( QPalette::Shadow, QColor( 66, 78, 58 ) );

            return palette;
        }
    }

    return m_systemPalette;
}

VTFEdit::StyleOptions::StyleOptions(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Style and Theme Options"));
    this->m_systemPalette = this->palette();
    this->m_systemStyle = QApplication::style()->name();

    auto layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Style:"),this),0,0);
    this->m_pStyleOption = new QComboBox(this);
    this->m_pStyleOption->addItem(tr("Follow system style"), this->m_systemStyle);
    for ( const auto &key : QStyleFactory::keys() )
    {
        this->m_pStyleOption->addItem( key, key );
    }
    layout->addWidget(this->m_pStyleOption,0,1);

    connect(this->m_pStyleOption, &QComboBox::currentTextChanged, this, &StyleOptions::updateThemeAndStyle);

    layout->addWidget(new QLabel(tr("Theme:"),this),1,0);
    this->m_pThemeOption = new QComboBox(this);
    this->m_pThemeOption->addItem(tr("Follow system theme"), static_cast<int>(VTFEditTheme::System));
    this->m_pThemeOption->addItem(tr("Light"), static_cast<int>(VTFEditTheme::Light));
    this->m_pThemeOption->addItem(tr("Dark"), static_cast<int>(VTFEditTheme::Dark));
    this->m_pThemeOption->addItem(tr("VGUI"), static_cast<int>(VTFEditTheme::VGUI));
    this->m_pThemeOption->setToolTip(tr("The colour scheme used by the VTFEdit++. (Does not affect VMT Editor)"));
    layout->addWidget(this->m_pThemeOption,1,1);

    connect(this->m_pThemeOption, &QComboBox::currentTextChanged, this, &StyleOptions::updateThemeAndStyle);

    QDialogButtonBox *pButtons = new QDialogButtonBox(this);
    QPushButton *pApply = pButtons->addButton(tr("&Apply"), QDialogButtonBox::AcceptRole);
    QPushButton *pClose = pButtons->addButton(tr("&Close"), QDialogButtonBox::RejectRole);
    layout->addWidget(pButtons,2,0,1,2);

    connect(pApply, &QPushButton::clicked, this, &StyleOptions::accept);
    connect(pClose, &QPushButton::clicked, this, &StyleOptions::reject);

}

void VTFEdit::StyleOptions::updateThemeAndStyle()
{
    auto selectedPalette = this->getTheme( this->m_pThemeOption->currentData().value<VTFEditTheme>() );
    QApplication::setStyle( this->m_pStyleOption->currentData().toString() );
    QApplication::setPalette( selectedPalette );
}

int VTFEdit::StyleOptions::exec()
{
    this->m_CurrentTheme = this->m_pThemeOption->currentData().toInt();;
    this->m_CurrentStyle = this->m_pStyleOption->currentData().toString();

    int res = QDialog::exec();

    if(res == QDialog::Accepted)
        return res;

    this->setTheme(this->m_CurrentTheme);
    this->setStyle(this->m_CurrentStyle);

    return res;
}

VTFEdit::StyleOptions::VTFEditTheme VTFEdit::StyleOptions::getTheme()
{
    return this->m_pThemeOption->currentData().value<VTFEditTheme>();
}

void VTFEdit::StyleOptions::setTheme(int theme)
{
    int pos = this->m_pThemeOption->findData(theme);

    if(pos == -1)
        pos = 0;

    this->m_pThemeOption->setCurrentIndex(pos);
}

QString VTFEdit::StyleOptions::getStyle()
{
    return this->m_pStyleOption->currentData().toString();
}

void VTFEdit::StyleOptions::setStyle(const QString& style)
{
    int pos = this->m_pStyleOption->findData(style);

    if(pos == -1)
        pos = 0;

    this->m_pStyleOption->setCurrentIndex(pos);
}
