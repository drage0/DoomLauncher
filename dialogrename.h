#ifndef DIALOGRENAME_H
#define DIALOGRENAME_H

#include <QDialog>

namespace Ui {
class DialogRename;
}

class DialogRename : public QDialog
{
	Q_OBJECT

public:
	explicit DialogRename(QString currentname, QWidget *parent = nullptr);
	~DialogRename();

	bool confirmed;
	QString finalname;

private slots:
	void on_pushButton_2_clicked();

	void on_btn_discard_clicked();

private:
	Ui::DialogRename *ui;
};

#endif // DIALOGRENAME_H
