#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QString IP, int Port, QWidget* parent) : QDialog(parent),
                                                                        _mUi(new Ui::SettingsDialog),
                                                                        _mIp(IP),
                                                                        _mPort(Port)
{
    this->_mUi->setupUi(this);

    this->_mUi->lineEdit_IP->setText(IP);
    this->_mUi->spinBox_Port->setValue(Port);
}

SettingsDialog::~SettingsDialog()
{
    delete this->_mUi;
}

QString SettingsDialog::ip(void) const
{
    return this->_mIp;
}

int SettingsDialog::port(void) const
{
    return this->_mPort;
}

void SettingsDialog::on_buttonBox_Close_clicked(QAbstractButton* button)
{
    if(button == this->_mUi->buttonBox_Close->button(QDialogButtonBox::Save))
    {
        this->accept();
    }
    else if(button == this->_mUi->buttonBox_Close->button(QDialogButtonBox::Discard))
    {
        this->reject();
    }
}

void SettingsDialog::on_lineEdit_IP_textChanged(const QString& arg1)
{
    this->_mIp = arg1;
}

void SettingsDialog::on_spinBox_Port_valueChanged(int arg1)
{
    this->_mPort = arg1;
}
