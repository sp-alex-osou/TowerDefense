#include "missile.h"
#include "world.h"

Missile::Missile(World *world, const CL_Pointx<float>& position, Enemy& target, float damage, float slow, float splash, const CL_Color& color)
: GameObject(world), position(position), target(target), damage(damage), slow(slow), splash(splash), color(color)
{
}

// aktualisiert die Missile
bool Missile::update(int timeElapsed_ms)
{
	// Vektor von aktueller Position zum Ziel berechnen
	CL_Vec2<float> distance = target.getPosition() - position;

	// zurückzulegende Distanz berechnen
	float movement = timeElapsed_ms * 0.01f;

	// wenn zurückzulegende Distanz größer als Länge des Vektors
	if (movement > distance.length())
	{
		// Gegner treffen
		hitTargets();

		// Missile zerstören
		return false;
	}
	// sonst: Position entlang des Vektors um zurückzulegende Distanz anpassen
	else
		position += CL_Vec2<float>::normalize(distance) * movement;

	return true;
}

// liefert die aktuelle Position der Missile
const CL_Pointx<float>& Missile::getPosition() const
{
	return position;
}

// lässt die Missile Gegner treffen
void Missile::hitTargets()
{
	// das Primärziel treffen
	target.hit(damage, slow, color);

	// wenn kein Flächenschaden -> Abbruch
	if (splash == 0)
		return;

	// Vektor aller Gegner aus der Spielwelt beziehen
	std::vector<Enemy*> enemies = world->getEnemies();

	// für alle Gegner
	for (std::vector<Enemy*>::const_iterator it = enemies.begin(); it != enemies.end(); ++it)
	{
		// wenn Genger tot oder Gegner ist Primärziel -> Gegner überspringen
		if (!(*it)->isAlive() || (*it) == &target)
			continue;

		// Position von Primärziel und aktuellem Gegner berechnen
		CL_Pointx<float> enemyPosition = (*it)->getPosition();
		CL_Pointx<float> targetPosition = target.getPosition();

		// Entfernung zwischen Primärziel und aktuellem Gegner berechnen
		float distance = (enemyPosition - targetPosition).length();

		// wenn Entfernung kleiner gleich dem Radius des Flächeschadens
		if (distance <= splash)
		{
			// Verhältnis von Entfernung und Radius berechnen
			float ratio = 1.0f - distance / splash;

			// aktuellen Gegner mit angepasstem Schaden treffen
			(*it)->hit(damage * ratio, slow * ratio, color);
		}
	}
}