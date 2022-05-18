#include "doomlauncher.h"
#include "ui_doomlauncher.h"
#include "dialogrename.h"
#include "dialogabout.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QCommandLineParser>

DoomLauncher::DoomLauncher(settings_t settings, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::DoomLauncher),
	  settings(settings)
{
	ui->setupUi(this);

	resize(settings.program.window_width, settings.program.window_height);

	modfolders = settings.modfolders;
	games = settings.games;
	setmods = settings.modsets;

	refresh_games();
	refresh_modfolders();
	refresh_mods();
	refresh_sets();
	refresh_setmods();

	ui->tv_mods->setStyleSheet("QTreeWidget::item:selected {color: black; background-color : #f5ea1d; border: solid 2px red;}");
}

DoomLauncher::~DoomLauncher()
{
	settings.program.window_width = width();
	settings.program.window_height = height();
	settings.games = games;
	settings.modfolders = modfolders;
	settings.modsets = setmods;
	settings_save(settings);
	delete ui;
}

void
DoomLauncher::button_execbrowse(void)
{
	QString path = QFileDialog::getOpenFileName(this, tr("Find target exectuable"), "", "Exectuable (*.exe)");

	if (!path.isEmpty())
		ui->le_execpath->setText(path);
}

void
DoomLauncher::refresh_modfolders(void)
{
	ui->lw_modfolders->clear();
	ui->tv_mods->clear();
	for (const QString &folder : qAsConst(modfolders))
	{
		ui->lw_modfolders->addItem(folder);

		QTreeWidgetItem *top = new QTreeWidgetItem();
		QDir directory(folder);
		QStringList mods = directory.entryList(QStringList() << "*.wad" << "*.WAD" << "*.zip" << "*.ZIP" << "*.pk3" << "*.PK3", QDir::Files);
		foreach(QString filename, mods)
		{
			QTreeWidgetItem *mod = new QTreeWidgetItem();

			mod->setText(0, filename);
			top->addChild(mod);
		}
		top->setText(0, folder);
		ui->tv_mods->addTopLevelItem(top);
	}
}

void
DoomLauncher::button_browsemodfolder(void)
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Find mod folder"), "");

	if (!path.isEmpty())
	{
		modfolders.insert(path);

		ui->lw_modfolders->clear();
		ui->tv_mods->clear();

		settings.modfolders = modfolders;
		refresh_modfolders();
	}
}

void
DoomLauncher::on_lw_modfolders_currentRowChanged(int row)
{
	ui->btn_modfolder_remove->setEnabled((row >= 0 && row < ui->lw_modfolders->count()));
}

void
DoomLauncher::on_btn_modfolder_remove_clicked()
{
	const int row = ui->lw_modfolders->currentRow();

	if (row >= 0 && row < ui->lw_modfolders->count())
	{
		modfolders.remove(ui->lw_modfolders->currentItem()->text());
		refresh_modfolders();
	}
}

void
DoomLauncher::refresh_sets(void)
{
	ui->cb_sets->clear();
	ui->cb_launch_modset->clear();

	for (int i = 0; i < setmods.count(); i++)
	{
		ui->cb_sets->addItem(setmods.at(i).name);
		ui->cb_launch_modset->addItem(setmods.at(i).name);
	}
}

void
DoomLauncher::button_setadd(void)
{
	modset_t m;

	m.name = tr("unnamed set");
	setmods.push_back(m); // First add new set in the internal structure, or crash!!! (because of index change...)
	refresh_sets();
	ui->cb_sets->setCurrentIndex(ui->cb_sets->count()-1);
}

void
DoomLauncher::combobox_setchanged(int i)
{
	const bool validindex = (ui->cb_sets->count() > 0 && i >= 0 && i <= ui->cb_sets->count());
	ui->f_setedit->setEnabled(validindex);
	ui->btn_setrename->setEnabled(validindex);
	ui->btn_setremove->setEnabled(validindex);

	refresh_mods();
	refresh_setmods();
}

void
DoomLauncher::combobox_gamechanged(int i)
{
	const bool validindex = (ui->cb_games->count() > 0 && i >= 0 && i <= ui->cb_games->count());

	ui->f_game->setEnabled(validindex);
}

void
DoomLauncher::refresh_mods(void)
{
	int set = ui->cb_sets->currentIndex();

	if (set < 0 || ui->cb_sets->count() <= 0)
		return;

	ui->tv_mods->clearSelection();

	int top_count =  ui->tv_mods->topLevelItemCount();
	for (int i = 0; i < top_count; i++)
	{
		QTreeWidgetItem * top = ui->tv_mods->topLevelItem(i);

		top->setFlags(top->flags() & ~Qt::ItemIsSelectable);

		for (int j = 0; j < top->childCount(); j++)
		{
			for (int k = 0; k < setmods[set].mods.count(); k++)
			{
				QTreeWidgetItem *child = top->child(j);

				if (setmods[set].mods.at(k).first == top->text(0) && setmods[set].mods.at(k).second == child->text(0))
				{
					child->setSelected(true);
					break;
				}
			}
		}
	}
}

void
DoomLauncher::button_addwad(void)
{
	QList<QTreeWidgetItem*> items_selected = ui->tv_mods->selectedItems();
	QList<mod_t> mods;

	if (items_selected.count() <= 0)
		return;

	for(int i = 0; i < items_selected.count(); i++)
	{
		QTreeWidgetItem *parent = items_selected.at(i)->parent();

		if (parent != nullptr)
			mods.push_back(mod_t(parent->text(0), items_selected.at(i)->text(0)));
	}

	const int set = ui->cb_sets->currentIndex();
	if (set < 0 || set >= ui->cb_sets->count() || ui->cb_sets->count() <= 0)
		return;

	// Remove existing mods from the set if they are unselected.
	for (auto it = setmods[set].mods.begin(); it != setmods[set].mods.end();)
	{
		bool found = false;

		for (const mod_t &test : mods)
			if (test == *it)
			{
				found = true;
				break;
			}

		if (!found)
			it = setmods[set].mods.erase(it);
		else
			it++;
	}
	// Add mods to the set if they are selected.
	for (const mod_t &m : mods)
	{
		bool found = false;

		for (const mod_t &test : setmods[set].mods)
			if (m == test)
			{
				found = true;
				break;
			}

		if (!found)
			setmods[set].mods.push_back(m);
	}
	refresh_setmods();
}

void DoomLauncher::on_btn_setrename_clicked()
{
	DialogRename rename(ui->cb_sets->currentText());
	rename.exec();
	if (rename.confirmed)
	{
		int selected = ui->cb_sets->currentIndex();
		setmods[selected].name = rename.finalname;
		refresh_sets();
		ui->cb_sets->setCurrentIndex(selected);
	}
}

void
DoomLauncher::refresh_setmods(void)
{
	const int set = ui->cb_sets->currentIndex();

	if (set < 0 || set >= ui->cb_sets->count() || ui->cb_sets->count() <= 0)
		return;

	ui->lw_setmods->clear();
	for (const mod_t &mod : setmods[set].mods)
		ui->lw_setmods->addItem(mod.second + " ("+ mod.first + ")");
}

void
DoomLauncher::on_btn_setmodup_clicked()
{
	const int cur = ui->lw_setmods->currentRow();
	const int set = ui->cb_sets->currentIndex();

	if (set < 0 || set >= ui->cb_sets->count() || ui->cb_sets->count() <= 0)
		return;

	if (cur >= 1 && cur < ui->lw_setmods->count() && ui->lw_setmods->count() > 0)
	{
		mod_t temp = setmods[set].mods.takeAt(cur);
		setmods[set].mods.insert(cur-1, temp);
		refresh_setmods();
		ui->lw_setmods->setCurrentRow(cur-1);
	}
}


void
DoomLauncher::on_btn_setmoddown_clicked()
{
	const int cur = ui->lw_setmods->currentRow();
	const int set = ui->cb_sets->currentIndex();

	if (set < 0 || set >= ui->cb_sets->count() || ui->cb_sets->count() <= 0)
		return;

	if (cur >= 0 && cur < ui->lw_setmods->count()-1 && ui->lw_setmods->count() > 0)
	{
		mod_t temp = setmods[set].mods.takeAt(cur);
		setmods[set].mods.insert(cur+1, temp);
		refresh_setmods();
		ui->lw_setmods->setCurrentRow(cur+1);
	}
}


void
DoomLauncher::on_actionabout_Qt_triggered()
{
	QMessageBox::aboutQt(this);
}


void
DoomLauncher::on_actionabout_program_triggered()
{
	DialogAbout da;

	da.exec();
}


void
DoomLauncher::on_lw_setmods_currentRowChanged(int currentRow)
{
	ui->btn_setmodup->setEnabled((currentRow > 0));
	ui->btn_setmoddown->setEnabled((currentRow >= 0 && currentRow < ui->lw_setmods->count() - 1));
}

void
DoomLauncher::on_btn_shortcut_game_clicked()
{
	ui->tabwidget->setCurrentIndex(1);
}


void
DoomLauncher::on_btn_shortcut_modset_clicked()
{
	ui->tabwidget->setCurrentIndex(2);
}

void
DoomLauncher::refresh_games(void)
{
	ui->cb_games->clear();
	ui->cb_launch_game->clear();
	for (const game_t &g : games)
	{
		ui->cb_games->addItem(g.name);
		ui->cb_launch_game->addItem(g.name);
	}
	settings.games = games;
}

void
DoomLauncher::on_btn_gameadd_clicked()
{
	game_t newgame;

	newgame.name = "unnamed";
	games.push_back(newgame);
	refresh_games();
	ui->cb_games->setCurrentIndex(games.size()-1);
}

void DoomLauncher::on_btn_gameremove_clicked()
{
	const int i = ui->cb_games->currentIndex();

	if (i < 0 || i >= ui->cb_games->count())
		return;

	games.removeAt(i);
	refresh_games();
}

void
DoomLauncher::on_btn_gamerename_clicked()
{
	DialogRename rename(ui->cb_games->currentText());
	rename.exec();
	if (rename.confirmed)
		games[ui->cb_games->currentIndex()].name = rename.finalname;
	int selected = ui->cb_games->currentIndex();
	refresh_games();
	ui->cb_games->setCurrentIndex(selected);
}

void
DoomLauncher::on_btn_startgame_clicked()
{
	QStringList arg;
	game_t game;
	QProcess p;

	game = games.at(ui->cb_launch_game->currentIndex());

	//QObject::connect(&p, SIGNAL(readyReadStdError()), this, SLOT(program_output_error(&p)));
	//QObject::connect(&p, SIGNAL(readyReadStdOutput()), this, SLOT(program_output_standard(&p)));

	switch (game.sourceport)
	{
	default:
		for (const mod_t &mod : setmods.at(ui->cb_launch_modset->currentIndex()).mods)
			arg << "-file" << mod.first + '/' + mod.second;
	}

	for (const QString &add_arg : QProcess::splitCommand(game.add_commandline))
		arg << add_arg;

	bool ok = p.startDetached(game.exec_path, arg);

	if (!ok)
	{
		QMessageBox msg;
		msg.critical(0,"Error","Process failed to start.");
		msg.show();
		return;
	}
}

void
DoomLauncher::program_output_error(QProcess *p)
{
	QByteArray data = p->readAllStandardError();
	qDebug() << QString(data);
}

void
DoomLauncher::program_output_standard(QProcess *p)
{
	QByteArray data = p->readAll();
	qDebug() << QString(data);
}

void
DoomLauncher::on_pte_additionalargs_textChanged()
{
	games[ui->cb_games->currentIndex()].add_commandline = ui->pte_additionalargs->document()->toPlainText();
}


void
DoomLauncher::on_cb_launch_game_currentIndexChanged(int index)
{
	const bool game_ok = (ui->cb_launch_game->currentIndex() >= 0 && ui->cb_launch_game->currentIndex() < ui->cb_launch_game->count() && ui->cb_launch_game->count() > 0);
	const bool modset_ok = (ui->cb_launch_modset->currentIndex() >= 0 && ui->cb_launch_modset->currentIndex() < ui->cb_launch_modset->count() && ui->cb_launch_modset->count() > 0);

	ui->f_gameplay->setEnabled(game_ok && modset_ok);
}


void
DoomLauncher::on_cb_launch_modset_currentIndexChanged(int index)
{
	const bool game_ok = (ui->cb_launch_game->currentIndex() >= 0 && ui->cb_launch_game->currentIndex() < ui->cb_launch_game->count() && ui->cb_launch_game->count() > 0);
	const bool modset_ok = (ui->cb_launch_modset->currentIndex() >= 0 && ui->cb_launch_modset->currentIndex() < ui->cb_launch_modset->count() && ui->cb_launch_modset->count() > 0);

	ui->f_gameplay->setEnabled(game_ok && modset_ok);
}


void
DoomLauncher::on_le_execpath_textChanged(const QString &arg1)
{
	games[ui->cb_games->currentIndex()].exec_path = arg1;
}


void
DoomLauncher::on_cb_games_currentIndexChanged(int index)
{
	if (index >= 0 && index < ui->cb_games->count() && ui->cb_games->count() > 0)
	{
		ui->btn_gameremove->setEnabled(true);
		ui->btn_gamerename->setEnabled(true);

		ui->le_execpath->setText(games.at(index).exec_path);
		ui->pte_additionalargs->document()->setPlainText(games.at(index).add_commandline);

		int sourceport = games.at(index).sourceport;
		if (sourceport >= 0 && sourceport < 3)
			ui->cb_gamesourceport->setCurrentIndex(sourceport);
	}
	else
	{
		ui->btn_gameremove->setEnabled(false);
		ui->btn_gamerename->setEnabled(false);
	}
}


void
DoomLauncher::on_btn_quickadd_cheats_clicked()
{
	ui->pte_additionalargs->appendPlainText("+sv_cheats 1 ");
}

void
DoomLauncher::on_btn_quickadd_fastmon_clicked()
{
	ui->pte_additionalargs->appendPlainText("-fast");
}

void
DoomLauncher::on_btn_quickadd_nomo_clicked()
{
	ui->pte_additionalargs->appendPlainText("-nomonsters");
}

