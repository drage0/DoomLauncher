#include "dialogrename.h"
#include "ui_dialogrename.h"

DialogRename::DialogRename(QString currentname, QWidget *parent) :
	QDialog(parent),
	confirmed(false),
	finalname(currentname),
	ui(new Ui::DialogRename)
{
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	ui->setupUi(this);
	ui->le_previous->setText(currentname);
	ui->le_new->setText(currentname);
}

DialogRename::~DialogRename()
{
	delete ui;
}

void DialogRename::on_pushButton_2_clicked()
{
	confirmed = true;
	finalname = ui->le_new->text();
	close();
}


void DialogRename::on_btn_discard_clicked()
{
	confirmed = false;
	close();
}

