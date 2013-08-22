#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include <cmath>

#include "enemies.h"
#include "world.h"

#define PI 3.141592

Enemies::Enemies(World *world, const CL_StringRef& spriteName, float speed, float hitpoints, int value, bool boss = false)
: GameObject(world), fieldSize(world->getFieldSize()), hitpoints(hitpoints), boss(boss)
{
	CL_GraphicContext gc = world->get_gc();

	// Sprite laden
	sprite = new CL_Sprite(gc, spriteName, &world->resources);

	// Waypoints laden
	const std::vector<CL_Point>& wayPoints = world->getWayPoints();
	
	// Gegneranzahl setzen (normal 20, Boss 1)
	int numEnemies = (boss) ? 1 : 20;

	// Gegner erzeugen
	for (int i = 0; i < numEnemies; i++)
		enemies.push_back(new Enemy(world, wayPoints, speed, hitpoints, value, boss, false));

	// Anzahl der gestarteten Gegner
	count = 0;

	// Interval bis zum Starten des nächsten Gegners
	interval = 0;
}

Enemies::~Enemies()
{
	// Gegner löschen
	for (std::vector<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		delete (*it);

	delete sprite;
}

// liefert eine Instanz der Klasse
Enemies* Enemies::getInstance(World *world, const CL_StringRef& type)
{
	// je nach angefordertem Typ Instanz erstellen und mit bestimmten Werten initialisieren
	if (type == "DogondoYellow")
		return new Enemies(world, type, 1.5f, 100.0f, 10);
	else if (type == "DogondoGreen")
		return new Enemies(world, type, 1.6f, 200.0f, 20);
	else if (type == "DogondoBlue")
		return new Enemies(world, type, 1.8f, 500.0f, 50);
	else if (type == "DogondoRed")
		return new Enemies(world, type, 2.0f, 900.0f, 100);
	else if (type == "DogondoBoss")
		return new Enemies(world, type, 1.0f, 100000.0f, 100000, true);
	else
		return 0;
}

// teilt einem Enemies Objekt die aktuelle Größe eines Feldes am Spielfeld mit
void Enemies::setFieldSize(CL_Sizex<float> fieldSize)
{
	this->fieldSize = fieldSize;
}

// liefert einen mit Gegnern gefüllten Vektor
const std::vector<Enemy*>& Enemies::getEnemies() const
{
	return enemies;
}

// zeichnet die Gegner
void Enemies::draw()
{
	CL_GraphicContext gc = world->get_gc();

	CL_Vec2<float> position;
	CL_Rectf rect;

	// Größe der Lebensanzeige
	CL_Sizex<float> size1 = CL_Sizex<float>(fieldSize.width * 0.7f, fieldSize.height * 0.05f);
	CL_Sizex<float> size2;

	// für alle Gegner
	for (unsigned int i = 0; i < enemies.size(); i++)
		// wenn Gegner noch am Leben
		if (enemies[i]->isAlive())
		{
			// Position des Gegners am Spielfeld berechnen (aktuelle Feldgröße berücksichtigen)
			position = enemies[i]->getPosition();
			position.x *= fieldSize.width;
			position.y *= fieldSize.height;
			
			// wenn Gegner Boss -> Sprite vergrößern
			if (enemies[i]->isBoss())
				sprite->set_scale(fieldSize.width * 0.03f, fieldSize.height * 0.03f);
			else
				sprite->set_scale(fieldSize.width * 0.01f, fieldSize.height * 0.01f);
			
			// Winkel und Farbe des Sprites setzen
			sprite->set_angle(enemies[i]->getAngle());
			sprite->set_color(enemies[i]->getColor());

			// Sprite an Position des aktuellen Gegners zeichen
			sprite->draw(gc, position.x, position.y);

			// Größe der Lebensanzeige (Hitpoints des Gegners berücksichtigen)
			size2 = CL_Sizex<float>(size1.width * enemies[i]->getHitpoints() / hitpoints, size1.height);

			// Position der Lebensanzige am Spielfeld
			CL_Pointx<float> pos = position - CL_Vec2<float>(0.0f, fieldSize.height * 0.3f);

			// wenn Gegner Boss -> Lebensanzeige vergrößern und verschieben
			if (enemies[i]->isBoss())
			{
				size1 *= 2.0f;
				size2 *= 2.0f;
				pos.y -= fieldSize.height * 0.3f;
			}

			// Position des linken oberen Punktes
			pos.x -= size1.width * 0.5f;
			pos.y -= size1.height * 0.5f;

			// Hintergrund der Lebensanzeige zeichnen (schwarz)
			rect = CL_Rectf(pos, size1);
			CL_Draw::fill(gc, rect, CL_Colorf::black);

			// Lebensanzeige zeichnen (grün)
			rect = CL_Rectf(pos, size2);
			CL_Draw::fill(gc, rect, CL_Colorf::green);
		}
}

// aktualisiert die Gegner und das Sprite
bool Enemies::update(int timeElapsed_ms)
{
	// für alle Gegner
	for (unsigned int i = 0; i < enemies.size(); i++)
		// wenn Gegner am Leben
		if (enemies[i]->isAlive())
			// Gegner aktualisieren
			enemies[i]->update(timeElapsed_ms);

	// wenn noch nicht alle Gegner gestartet
	if (count < enemies.size())
	{
		// Interval zum Starten des nächsten Gegners verkürzen
		interval -= timeElapsed_ms;

		// solange nicht alle Gegner gestartet und Interval kleiner 0
		while (count < enemies.size() && interval < 0)
		{
			// Gegner zum Leben erwecken
			enemies[count]->setAlive(true);

			// Gegner aktualisieren
			enemies[count]->update(-interval);

			// Interval um eine halbe Sekunde erhöhen
			interval += 500;

			// Anzahl der gestarteten Gegner erhöhen
			count++;
		}
	}

	// Sprite aktualisieren
	sprite->update(timeElapsed_ms);

	return true;
}

bool Enemies::isBoss() const
{
	return boss;
}