#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QSet>

typedef QPair<QString, QString> mod_t; // file name, folder path

struct modset_t
{
	QString name;
	QList<mod_t> mods;
};

struct game_t
{
	QString name;
	QString exec_path;
	QString add_commandline;
	int sourceport;
};

struct settings_t
{
	struct
	{
		int window_width = 640;
		int window_height = 480;
	} program;
	QSet<QString> modfolders;
	QList<game_t> games;
	QList<modset_t> modsets;

};

struct settings_t settings_load(void);
void settings_save(struct settings_t);

#endif // SETTINGS_H
