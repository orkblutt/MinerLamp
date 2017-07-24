#ifndef NVOCDIALOG_H
#define NVOCDIALOG_H

#include <QDialog>

#include "nvidianvml.h"

namespace Ui {
class nvOCDialog;
}

class nvOCDialog : public QDialog
{
    Q_OBJECT

public:
    explicit nvOCDialog(QWidget *parent = 0);
    ~nvOCDialog();

private:
    Ui::nvOCDialog *ui;

    nvidiaNVML* _nvml;
};

#endif // NVOCDIALOG_H
