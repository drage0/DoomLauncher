#include "doomlauncher.h"
#include "settings.h"
#include <QApplication>


int
main(int argc, char *argv[])
{
	const settings_t settings = settings_load();
	QApplication a(argc, argv);
	DoomLauncher w(settings);
	w.show();
	return a.exec();
}
