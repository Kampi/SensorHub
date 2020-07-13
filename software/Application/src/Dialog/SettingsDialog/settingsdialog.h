#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QString IP, int Port, QWidget* parent = nullptr);
        ~SettingsDialog();

        QString ip(void) const;
        int port(void) const;

    private slots:
        void on_buttonBox_Close_clicked(QAbstractButton* button);
        void on_lineEdit_IP_textChanged(const QString& arg1);
        void on_spinBox_Port_valueChanged(int arg1);

    private:
        Ui::SettingsDialog* _mUi;

        QString _mIp;
        int _mPort;
};

#endif // SETTINGSDIALOG_H
