#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "world.h"
#include "terrain.h"
#include "enemies.h"
#include "towers.h"
#include "menu.h"

World::World(CL_DisplayWindow &displayWindow) : window(displayWindow), quit(false)
{
	//Input Ereignisse einhängen
	CL_Slot slotQuit = window.sig_window_close().connect(this, &World::onWindowClose);
	CL_Slot slotResize = window.sig_resize().connect(this, &World::onWindowResize);
	CL_Slot slotMouseDown = window.get_ic().get_mouse().sig_key_down().connect(this, &World::onMouseDown);
	CL_Slot slotMouseMove = window.get_ic().get_mouse().sig_pointer_move().connect(this, &World::onMouseMove);
	CL_Slot slotKeyDown = window.get_ic().get_keyboard().sig_key_down().connect(this, &World::onKeyDown);

	graphicContext = window.get_gc();

	// Setup resources
	resources = CL_ResourceManager("resources.xml");

	//Laden und Abspielen der Hintergrundmusik
	CL_SoundBuffer buffer("Sounds/theme.wav");
	themeSession = buffer.prepare(true);

	themeSession.set_volume(0.25f);

	themeSession.play();

	towers = 0;

	//Erstellen der einzelnen GameObjects
	CL_Rect viewport = window.get_viewport();
	menu = new Menu(this, viewport);
	viewport.set_height(viewport.get_height() - menu->getMenuHeight());

	terrain = new Terrain(this, viewport);
	player = new Player(20, 200);
	towers = new Towers(this);

	//Turmtypen definieren
	//				id	name		texture			previewTexture			speed		damage		range		slow		splash		hitColor			price
	towers->addType(0,	"fire",		"fireTower",	"fireTowerPreview",		5.0f,		10.0f,		2.0f,		0.0f,		0.0f,		CL_Color::red,			100);
	towers->addType(1,	"stone",		"stoneTower",	"stoneTowerPreview",		1.0f,		70.0f,		3.0f,		0.0f,		2.0f,		CL_Color::grey,		300);
	towers->addType(2,	"ice",		"iceTower",		"iceTowerPreview",		0.5f,		0.0f,			2.0f,		1.0f,		2.0f,		CL_Color::blue,		300);
	towers->addType(3,	"magic",		"magicTower",	"magicTowerPreview",		0.5f,		2000.0f,		1000.0f,	0.0f,		0.0f,		CL_Color::purple,		1000);

	//Gegenerwellen initalisieren
	waves.push_back(Enemies::getInstance(this, "DogondoYellow"));
	waves.push_back(Enemies::getInstance(this, "DogondoGreen"));
	waves.push_back(Enemies::getInstance(this, "DogondoBlue"));
	waves.push_back(Enemies::getInstance(this, "DogondoRed"));
	waves.push_back(Enemies::getInstance(this, "DogondoBoss"));

	currentTowerType = -1;
	currentWave = 0;
	countdown = 10.0f;

	addObject(terrain);
	addObject(towers);
	addObject(menu);

	quit = false;
	buildTower = false;
	gameOver = false;
	win = false;

	// run the main loop
	run();
}

World::~World(void)
{
	//Löschen aller GameObjects
	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); ++it)
		delete (*it);
}

//Hinzufügen eines GameObject zur Collection
void World::addObject(GameObject *object)
{
	objects.push_back(object);
}

//Programmschleife
void World::run()
{
	//Bei Escape Schleife abbrechen und Programm beenden
	while(!window.get_ic().get_keyboard().get_keycode(CL_KEY_ESCAPE))
	{
		if (quit)
			break;

		update();
		draw();

		window.flip(1);
		CL_KeepAlive::process();
	};
}

//Updates ausführen
void World::update()
{	
	//Spiel vorbei
	if (gameOver)
		return;

	//Vergangene Zeit berechnen
	int timeElapsed_ms = calcTimeElapsed();

	float timeElapsed = timeElapsed_ms * 0.001f;

	if (currentWave < waves.size())
	{
		countdown -= timeElapsed;

		if (countdown < 0)
		{
			addObject(waves[currentWave++]);
		
			if (currentWave < waves.size())
				countdown = 60;
		}
	}

	//Update aller GameObjects aufrufen
	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); )
	{
		//Liefert ein Objekt "false" zurück, wird dieses gelöscht
		if((*it)->update(timeElapsed_ms) == false)
		{
			delete (*it);
			it = objects.erase(it);
		}
		else
			++it;
	}
}

//Vergangene Zeit berechnen
int World::calcTimeElapsed()
{
	static unsigned int lastTime = 0;

	unsigned int newTime = CL_System::get_time();
	if(lastTime == 0)
		lastTime = newTime;

	int deltaTime = (newTime - lastTime);
	lastTime = newTime;

	return deltaTime;
}

//Zeichnen aller Komponenten
void World::draw()
{
	CL_Draw::fill(graphicContext, window.get_viewport(), CL_Colorf::black);

	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); ++it)
		(*it)->draw();
}

//Bei Größenänderung des Fensters
void World::onWindowResize(int width, int height)
{
	//In allen relevanten Komponenten muss das Fensterverhältnis neu gesetzt werden,
	//damit die Größen angepasst werden
	terrain->setViewPort(width, height - menu->getMenuHeight());
	menu->setViewPort(width, height);
	towers->setFieldSize(terrain->getFieldSize());

	for (std::vector<Enemies*>::iterator it = waves.begin(); it != waves.end(); it++)
		(*it)->setFieldSize(terrain->getFieldSize());
}

//Beim Schließen des Fensters
void World::onWindowClose()
{
	quit = true;
}

//Mausklicks behandeln
void World::onMouseDown(const CL_InputEvent &key, const CL_InputState &state)
{
	//Linker Mausklick
	if (key.id == CL_MOUSE_LEFT)
	{
		//Wenn der Klick innerhalb des Terrains ist
		if (key.mouse_pos.y <= window.get_viewport().get_height() - menu->getMenuHeight())
		{
			//Postion im Terrain ermitteln
			CL_Point position = terrain->getFieldLocation(key.mouse_pos.x, key.mouse_pos.y);

			//Wenn auf ein freies Baufeld geklickt wurde
			if(currentTowerType >= 0 && !terrain->isWayField(position) && 
				!towers->isFieldEngaged(position) && terrain->isInsideTerrain(position))
			{
				TowerType towerType = towers->getTowerType(currentTowerType);

				//Turm setzen
				towers->addTower(position, currentTowerType);
				//Kosten vom Spielergeld abziehen
				player->spendMoney(towerType.price);

				CL_SoundBuffer buffer("Sounds/setTower.wav");
				buffer.play();
				
				//Wenn Spieler sich keinen weiteren Turm leisten kann
				if (player->getMoney() < towerType.price)
					currentTowerType = -1;
			}
			else
			{
				CL_SoundBuffer buffer("Sounds/burp.wav");
				buffer.play();
			}
		}
		//Ansonsten Klick im Menü
		else
		{
			//Feld ermitteln und evtl. Baumodus aktivieren
			int field = menu->getMenuField(key.mouse_pos.x, key.mouse_pos.y);
			if(field != -1)
				setCurrentTowerType(field);
		}
	}
	//Rechter Mausklick --> Baumodus deaktivieren
	else if (key.id == CL_MOUSE_RIGHT)
		currentTowerType = -1;

	//Turmvorschau de- bzw. aktivieren
	showPreview(key.mouse_pos.x, key.mouse_pos.y);
}

//Beim Bewegen der Maus
void World::onMouseMove(const CL_InputEvent &key, const CL_InputState &state)
{
	//Turmvorschau de- bzw. aktivieren
	showPreview(key.mouse_pos.x, key.mouse_pos.y);
}

//Turmart für den Baumodus setzen
void World::setCurrentTowerType(int id)
{
	TowerType towerType = towers->getTowerType(id);

	if (player->getMoney() >= towerType.price)
		currentTowerType = id;
	else
		currentTowerType = -1;
}

//Beim Drücken einer Taste auf der Tastatur
void World::onKeyDown(const CL_InputEvent &key, const CL_InputState &state)
{
	//Turmart für Baumodus setzen
	if(key.id == CL_KEY_1)
		setCurrentTowerType(0);
	else if (key.id == CL_KEY_2)
		setCurrentTowerType(1);
	else if (key.id == CL_KEY_3)
		setCurrentTowerType(2);
	else if (key.id == CL_KEY_4)
		setCurrentTowerType(3);

	//Turmvorschau de- bzw. aktivieren
	showPreview(key.mouse_pos.x, key.mouse_pos.y);

	//Programm beenden
	if(key.id == CL_KEY_ESCAPE)
		quit = true;
}

//Turmvorschau de- bzw. aktivieren
void World::showPreview(int x, int y)
{
	CL_Point position = terrain->getFieldLocation(x, y);

	//Maus muss auf einem Baufeld sein, das Feld darf nicht besetzt sein und ein Turmtyp muss aktiviert sein
	//-->Vorschau anzeigen, sonst nicht
	if	(currentTowerType >= 0 && terrain->isInsideTerrain(position) && !terrain->isWayField(position) && !towers->isFieldEngaged(position))
		towers->showPreview(position, currentTowerType);
	else
		towers->hidePreview();
}

//Liefert eine Auflistung der Gegner zurück
std::vector<Enemy*> World::getEnemies() const
{
	std::vector<Enemy*> enemies;

	//Gegner der Waves durchlaufen
	for (std::vector<Enemies*>::const_iterator it = waves.begin(); it != waves.end(); ++it)
	{
		std::vector<Enemy*> wave = (*it)->getEnemies();

		//Gegner der aktuellen Wave durchlaufen
		for (std::vector<Enemy*>::iterator itWave = wave.begin(); itWave != wave.end(); ++itWave)
			//Noch lebende Gegner dem Vektor hinzufügen
			if ((*itWave)->isAlive())
				enemies.push_back(*itWave);
	}

	return enemies;
}

//Liefert die Größe eines Terrainfeldes
const CL_Sizex<float>& World::getFieldSize() const
{
	return terrain->getFieldSize();
}

//Liefert die Wegpunkt aus dem Terrain
const std::vector<CL_Point>& World::getWayPoints() const
{
	return terrain->getWayPoints();
}

//Behandelt die Logik für die Leben des Spielers
void World::enemySurvived(const Enemy& enemy)
{
	//Spiel ist verloren
	if (gameOver)
		return;

	//Ist der Boss durch, stirbt der Spiel egal wieviele Leben er hat
	if (enemy.isBoss())
		player->die();
	//Ansonsten verliert der Spieler ein Leben pro Gegener
	else
	{
		player->looseLife();

		CL_SoundBuffer buffer("Sounds/playerHit.wav");
		buffer.play();
	}

	//Hat der Spieler keine Leben mehr, ist das Spiel verloren
	if (!player->isAlive())
	{
		gameOver = true;

		themeSession.stop();
		CL_SoundBuffer buffer("Sounds/loosing.wav");
		buffer.set_volume(3.0f);
		buffer.play();
	}
}

//Spiellogik für besiegt Gegner
void World::enemyDied(const Enemy& enemy)
{
	//Ist der Boss besiegt, ist das Spiel gewonnen
	if (enemy.isBoss())
	{
		win = true;
		themeSession.stop();
		CL_SoundBuffer buffer("Sounds/winning.wav");
		buffer.play();
	}
	//Ansonsten bekommt der Spieler Geld für den besiegten Gegner zugesprochen
	else
		player->gainMoney(enemy.getValue());
}

//Liefert die Turmtypen zurück
std::vector<TowerType*> World::getTowerTypes() const
{
	std::vector<TowerType*> empty; 

	if (towers == 0)
		return empty;

	return towers->getTowerTypes();
}

//Liefert den Geldbestand des Spielers
int World::getMoney() const
{
	return player->getMoney();
}

//Liefert den aktuellen Countdown
int World::getCountdown() const
{
	return (int)countdown;
}

//Liefert den akutellen Lebensstand
int World::getLifes() const
{
	return player->getLifes();
}