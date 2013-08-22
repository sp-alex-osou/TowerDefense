#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "towertype.h"
#include "towers.h"
#include "world.h"
#include "enemy.h"
#include "missile.h"

Towers::Towers(World *world)
: GameObject(world), fieldSize(world->getFieldSize())
{
	towerList = std::vector<Tower*>();
	towerTypes = mapType();
	preview.visible = false;
}

Towers::~Towers(void)
{
	// f�r alle T�rme -> Turm l�schen
	for (std::vector<Tower*>::iterator it = towerList.begin(); it != towerList.end(); ++it)
		delete (*it);

	// f�r alle Turmtypen -> Turmtyp l�schen
	for (mapType::iterator it = towerTypes.begin(); it != towerTypes.end(); ++it)
		delete it->second;
}

// f�gt einen Turm hinzu
bool Towers::addTower(const CL_Point& position, int type)
{
	// angegebenen Typ in der Hashmap suchen
	mapType::iterator it = towerTypes.find(type);

	// wenn Typ nicht gefunden -> Abbruch
	if (it == towerTypes.end())
		return false;

	// neuen Tower erzeugen und in die Liste der T�rme einf�gen
	towerList.push_back(new Tower(world, position, *(it->second)));
	
	return true;
}

// f�gt einen Turmtyp hinzu
bool Towers::addType(int id, const std::string& name, const std::string& t, const std::string& p, float speed, float damage, float range, float slow, float splash, const CL_Color& color, int price)
{
	// wenn Turmtyp bereits existiert -> Abbruch
	if (towerTypes.find(id) != towerTypes.end())
		return false;

	CL_GraphicContext gc = world->get_gc();

	// Texturen laden
	CL_Texture texture = CL_Texture(t, &world->resources, gc);
	CL_Texture preview = CL_Texture(p, &world->resources, gc);

	// neuen Turmtyp erzeugen
	TowerType* type = new TowerType(id, name, texture, preview, speed, damage, range, slow, splash, color, price);

	// neuen Turmtyp in die Liste der Turmtypen einf�gen
	towerTypes.insert(std::pair<int, TowerType*>(id, type));

	return true;
}

// teilt dem Towers Objekt die aktuelle Gr��e eines Feldes am Spielfeld mit
void Towers::setFieldSize(CL_Sizex<float> fieldSize)
{
	this->fieldSize = fieldSize;
}

// zeigt eine Vorschau des angegeben Turmtypen an
bool Towers::showPreview(const CL_Point& position, int type)
{
	// Turmtyp in Hashmap suchen
	mapType::iterator it = towerTypes.find(type);

	// wenn Turmtyp nicht gefunden -> Abbruch
	if (it == towerTypes.end())
		return false;

	// initialisiert die Turmvorschau (Position, Texture, Reichweite)
	preview.position = position;
	preview.texture = &(it->second->preview);
	preview.range = it->second->range;

	// setzt die Turmvorschau auf sichtbar
	preview.visible = true;

	return true;
}

// setzt die Turmvorschau auf nicht sichtbar
void Towers::hidePreview()
{
	preview.visible = false;
}

// liefert zur�ck, ob ein Feld am Spielfeld mit einem Turm belegt ist
bool Towers::isFieldEngaged(const CL_Point& position) const
{
	CL_Point tmpPosition;

	// f�r alle T�rme
	for (unsigned int i = 0; i < towerList.size(); i++)
	{
		// Position des aktuellen Turms abrufen
		tmpPosition = towerList[i]->getPosition();

		// wenn aktueller Turm auf angegebener Position steht -> Feld ist besetzt
		if (tmpPosition.x == position.x && tmpPosition.y == position.y)
			return true;
	}

	return false;
}

// zeichnet die T�rme, Missiles und die Turmvorschau
void Towers::draw()
{
	CL_Quadf quad;
	CL_Rectf rect;
	CL_Pointx<float> position;

	// f�r alle T�rme
	for (unsigned int i = 0; i < towerList.size(); i++)
	{
		// Missiles des aktuellen Turms abrufen
		const std::vector<Missile*>& missiles = towerList[i]->getMissiles();

		// f�r alle Missiles
		for (std::vector<Missile*>::const_iterator it = missiles.begin(); it != missiles.end(); ++it)
		{
			// Position abrufen
			position = (*it)->getPosition() - CL_Vec2<float>(0.1f, 0.1f);

			// Gr��e der Missile berechnen (aktuelle Feldgr��e ber�cksichtigen)
			rect = CL_Rectf(position.x * fieldSize.width, position.y * fieldSize.height, fieldSize * 0.2f);
			quad = CL_Quadf(rect);

			// Missile zeichnen
			CL_Draw::texture(world->get_gc(), towerList[i]->getType().texture, quad);
		}

		// Position abrufen
		const CL_Point& position = towerList[i]->getPosition();

		// Gr��e des Turms berechnen (aktuelle Feldgr��en ber�cksichtigen)
		rect = CL_Rectf(position.x * fieldSize.width, position.y * fieldSize.height, fieldSize);
		quad = CL_Quadf(rect);

		// Turm zeichnen
		CL_Draw::texture(world->get_gc(), towerList[i]->getType().texture, quad);
	}

	// wenn Turmvorschau sichtbar ist
	if (preview.visible)
	{
		// Position abrufen
		CL_Point& position = preview.position;

		// Gr��e der Turmvorschau berechnen (aktuelle Feldgr��en ber�cksichtigen)
		rect = CL_Rectf(position.x * fieldSize.width, position.y * fieldSize.height, fieldSize);
		quad = CL_Quadf(rect);

		// Turmvorschau zeichnen
		CL_Draw::texture(world->get_gc(), *(preview.texture), quad);


		// Rechteck zur Anzeige der Reichweite berechnen
		CL_Rect rangeRectangle = CL_Rect(
			(int)((position.x - preview.range) * fieldSize.width),
			(int)((position.y - preview.range) * fieldSize.height),
			(int)((position.x + preview.range + 1) * fieldSize.width),
			(int)((position.y + preview.range + 1.0f) * fieldSize.height));

		// Reichweite der Turmtyps der Turmvorschau zeichnen
		CL_Draw::box(world->get_gc(), rangeRectangle, CL_Colorf::white);
	}
}

// aktualisiert die T�rme
bool Towers::update(int timeElapsed_ms)
{
	// f�r alle T�rme
	for (std::vector<Tower*>::iterator it = towerList.begin(); it != towerList.end(); ++it)
		// Turm aktualisieren
		(*it)->update(timeElapsed_ms);

	return true;
}

// liefert einen Vektor aller Turmtypen
std::vector<TowerType*> Towers::getTowerTypes() const
{
	std::vector<TowerType*> tmp;

	// f�r alle Turmtypen
	for (mapType::const_iterator it = towerTypes.begin(); it != towerTypes.end(); ++it)
		// Turmtyp dem Vektor hinzuf�gen
		tmp.push_back(it->second);

	return tmp;
}

// liefert einen bestimmten Turmtypen
const TowerType& Towers::getTowerType(int id) const
{
	// Turmtyp in Hashmap suchen und zur�ckliefern
	return *(towerTypes.find(id)->second);
}