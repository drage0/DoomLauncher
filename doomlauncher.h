#ifndef DOOMLAUNCHER_H
#define DOOMLAUNCHER_H

#include <QMainWindow>
#include "settings.h"
#include "checklistmodel.h"
#include "checktreeitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DoomLauncher; }
QT_END_NAMESPACE

class QProcess;

class DoomLauncher : public QMainWindow
{
	Q_OBJECT

public:
	DoomLauncher(settings_t settings, QWidget *parent = nullptr);
	~DoomLauncher();

public slots:
	void button_execbrowse(void);
	void button_browsemodfolder(void);
	void button_setadd(void);
	void button_addwad(void);

	void combobox_setchanged(int i);
	void combobox_gamechanged(int i);

private slots:
	void on_btn_setrename_clicked();

	void on_btn_setmodup_clicked();

	void on_btn_setmoddown_clicked();

	void on_actionabout_Qt_triggered();

	void on_actionabout_program_triggered();

	void on_lw_setmods_currentRowChanged(int currentRow);

	void on_btn_modfolder_remove_clicked();

	void on_lw_modfolders_currentRowChanged(int currentRow);

	void on_btn_shortcut_game_clicked();

	void on_btn_shortcut_modset_clicked();

	void on_btn_gameadd_clicked();

	void on_btn_gameremove_clicked();

	void on_btn_gamerename_clicked();

	void on_btn_startgame_clicked();

	void on_pte_additionalargs_textChanged();

	void on_cb_launch_game_currentIndexChanged(int index);

	void on_cb_launch_modset_currentIndexChanged(int index);

	void on_le_execpath_textChanged(const QString &arg1);
	void program_output_error(QProcess *p);
	void program_output_standard(QProcess *p);

	void on_cb_games_currentIndexChanged(int index);

	void on_btn_quickadd_cheats_clicked();

	void on_btn_quickadd_nomo_clicked();

	void on_btn_quickadd_fastmon_clicked();

private:
	Ui::DoomLauncher *ui;
	settings_t settings;

	QSet<QString> modfolders;
	QList<modset_t> setmods;
	QList<game_t> games;

	void refresh_modfolders(void);
	void refresh_mods(void);
	void refresh_setmods(void);
	void refresh_games(void);
	void refresh_sets(void);
};
#endif // DOOMLAUNCHER_H
