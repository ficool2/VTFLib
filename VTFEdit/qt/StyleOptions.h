#pragma once

#include <QDialog>

class QComboBox;

namespace VTFEdit {


    class StyleOptions : public QDialog
    {

        enum class VTFEditTheme
        {
            System = 0,
            Light,
            Dark,
            VGUI,
        };

        QPalette m_systemPalette;
        QString m_systemStyle;

        QPalette m_CurrentTheme;
        QString m_CurrentStyle;

        QComboBox *m_pStyleOption;
        QComboBox *m_pThemeOption;

        [[nodiscard]] QPalette getTheme(VTFEditTheme theme) const;

    public:
        explicit StyleOptions(QWidget *parent);

        int exec() override;

        VTFEditTheme getTheme();
        QString getStyle();

    private slots:
        void updateThemeAndStyle();

    public slots:
        void setTheme(int theme);
        void setStyle(const QString& style);

    };

}