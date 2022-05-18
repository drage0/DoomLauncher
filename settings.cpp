#include "settings.h"
#include <QSettings>
#include <QFile>

Q_DECLARE_METATYPE(game_t);

QDataStream&
operator<<(QDataStream& out, const game_t& g)
{
	out << g.name;
	out << g.exec_path;
	out << g.add_commandline;
	out << g.sourceport;
	return out;
}

QDataStream&
operator>>(QDataStream& in, game_t& g)
{
	in >> g.name;
	in >> g.exec_path;
	in >> g.add_commandline;
	in >> g.sourceport;
	return in;
}

QDataStream&
operator<<(QDataStream& out, const modset_t& m)
{
	out << m.name;
	out << m.mods;
	return out;
}

QDataStream&
operator>>(QDataStream& in, modset_t& m)
{
	in >> m.name;
	in >> m.mods;
	return in;
}

QDataStream&
operator<<(QDataStream& out, const settings_t& s)
{
	out << s.program.window_width << s.program.window_height;
	out << s.modfolders;
	out << s.games;
	out << s.modsets;
	return out;
}

QDataStream&
operator>>(QDataStream& in, settings_t& s)
{
	in >> s.program.window_width >> s.program.window_height;
	in >> s.modfolders;
	in >> s.games;
	in >> s.modsets;
	return in;
}

struct settings_t
settings_load(void)
{
	struct settings_t settings;

	QFile fin("settings.bin");
	if (fin.open(QIODevice::ReadOnly))
	{
		QDataStream in(&fin);
		in>>settings;
		fin.close();
		qDebug() << settings.program.window_width;
		qDebug() << settings.program.window_height;
	}

	return settings;
}

void
settings_save(struct settings_t settings)
{
	QFile fout("settings.bin");

	if(fout.open(QIODevice::WriteOnly))
	{
		QDataStream out(&fout);
		out << settings;
		fout.close();
	}
}
