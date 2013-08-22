#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include <cmath>

#include "enemy.h"
#include "world.h"

#define PI 3.141592

Enemy::Enemy(World *world, const std::vector<CL_Point>& wp, float speed, float hitpoints, int value, bool boss, bool alive)
: GameObject(world), wayPoints(wp), speed(speed), defaultSpeed(speed), hitpoints(hitpoints), value(value), boss(boss), alive(alive)
{
	// wenn weniger als zwei Wegpunkte übergeben werden -> Abbruch
	if (wayPoints.size() < 2)
		return;

	// Position auf ersten Wegpunkt setzen
	setPosition(wayPoints[0]);
	wayPoints.erase(wayPoints.begin());

	// Ziel auf zweiten Wegpunkt setzen
	setTarget(wayPoints[0]);
	wayPoints.erase(wayPoints.begin());	
}

// liefert den Winkel der Blickrichtung
const CL_Angle& Enemy::getAngle() const
{
	return angle;
}

// gibt an, ob Gegner lebt
bool Enemy::isAlive() const
{
	return alive;
}

// gibt an, ob Gegner verlangsamt ist
bool Enemy::isSlowed() const
{
	return speed != defaultSpeed;
}

// gibt an, ob Gegner Boss ist
bool Enemy::isBoss() const
{
	return boss;
}

// liefert aktuelle Position
const CL_Pointx<float>& Enemy::getPosition() const
{
	return position;
}

// liefert aktuelle Farbe
const CL_Color& Enemy::getColor() const
{
	return color;
}

// setzt die Bewegungsgeschwindigkeit
void Enemy::setSpeed(float speed)
{
	this->speed = speed;
}

// Gegner zum Leben erwecken oder töten
void Enemy::setAlive(bool alive)
{
	this->alive = alive;
}

// teilt dem Gegner mit, dass er getroffen wurde
void Enemy::hit(float damage, float slow, const CL_Color& hitColor)
{
	// wenn bereits tot -> Abbruch
	if (!alive)
		return;

	// Lebenspunkte um Schaden des Treffers verringern
	this->hitpoints -= damage;

	// Interval der Trefferanzeige auf 0,1 Sekunden setzen
	hitInterval = 0.1f;

	// wenn Treffer verlangsamt
	if (slow > 0.0f)
	{
		// Dauer der Verlangsamung festlegen (normal: 3 Sekunden, Boss: 0,5 Sekunden)
		duration = (boss) ? 0.5f : 3.0f;

		// verlangsamte Bewegungsgeschwindigkeit berechnen
		float slowSpeed = defaultSpeed / (1.0f + slow);

		// wenn verlangsamte Bewegungsgeschwindigkeit kleiner als aktuelle
		if (slowSpeed < speed)
		{
			// aktuelle Bewegungsgeschwindigkeit anpassen
			speed = slowSpeed;

			// Farbe auf Farbe des Treffers setzen
			color = hitColor;
		}
	}

	// wenn nicht verlangsamt
	if (!isSlowed())
		// Farbe auf Farbe des Treffers setzen
		color = hitColor;

	// wenn Lebenspunkte kleiner gleich 0
	if (hitpoints <= 0.0f)
	{
		// Gegner auf "tot" setzen
		alive = false;

		// der Spielwelt mitteilen, dass der Gegner gestorben ist
		world->enemyDied(*this);
	}
}

// aktualisiert den Gegner
bool Enemy::update(int timeElapsed_ms)
{
	float timeElapsed = timeElapsed_ms * 0.001f;

	// wenn Gegner verlangsamt
	if (isSlowed())
		// Dauer der Verlangsamung verringern
		duration -= timeElapsed;

	// wenn Dauer der Verlangsamung kleiner gleich 0
	if (duration <= 0)
		// Bewegungsgeschwindigkeit auf Standardwert setzen
		speed = defaultSpeed;

	// wenn Interval der Trefferanzeige größer 0
	if (hitInterval > 0)
		// Interval der Trefferanzeige verringern
		hitInterval -= timeElapsed;

	// wenn keine Trefferanzeige und nicht verlangsamt
	if (hitInterval <= 0 && !isSlowed())
		// Farbe auf weiß setzen (keine Einfärbung)
		color = CL_Color::white;

	// Gegner bewegen
	move(speed * timeElapsed);

	// Blickrichtung berechnen
	CL_Vec2<float> direction = CL_Vec2<float>::normalize(target - position);
	CL_Vec2<float> up = CL_Vec2<float>(0.0f, -1.0f);

	// Winkel setzen
	angle.set_degrees((float)floor((acos(direction.dot(up)) * 180.0f / PI) + 0.5));

	if (direction.x < 0)
		angle += CL_Angle(180.0f, cl_degrees);

	return true;
}

// bewegt den Gegner um eine angegebene Distanz
void Enemy::move(float distance)
{
	// Vektor von aktueller Position zu aktuellem Ziel berechnen
	CL_Vec2<float> targetVector = target - position;
	float length = (targetVector).length();

	// wenn angegebene Distanz kleiner als Länge des Vektors
	if (distance < length)
		// Position in Richtung des Vektors um die angegebene Distanz verändern
		position += CL_Vec2<float>::normalize(targetVector) * distance;
	// sonst: wenn das aktuelle Ziel das Ende des Weges ist
	else if (wayPoints.size() == 0)
	{
		// Gegner auf "tot" setzen
		alive = false;

		// Welt mitteilen, dass ein Gegner überlebt hat
		world->enemySurvived(*this);
	}
	// sonst
	else	
	{
		// Position auf aktuelles Ziel setzen
		position = target;

		// neues Ziel: nächster Wegpunkt
		setTarget(wayPoints[0]);
		wayPoints.erase(wayPoints.begin());

		// Funktion rekursiv aufrufen (mit verringerter Distanz)
		move(distance - length);
	}
}

// setzt die Position
void Enemy::setPosition(CL_Point wayPoint)
{
	position.x = wayPoint.x + 0.5f;
	position.y = wayPoint.y + 0.5f;
}

// setzt das aktuelle Ziel
void Enemy::setTarget(CL_Point wayPoint)
{
	target.x = wayPoint.x + 0.5f;
	target.y = wayPoint.y + 0.5f;
}

// liefert die Lebenspunkte
float Enemy::getHitpoints() const
{
	return hitpoints;
}

// liefert den Wert
int Enemy::getValue() const
{
	return value;
}
