#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "terrain.h"
#include "world.h"

Terrain::Terrain(World *world, CL_Rect viewPort)
	: GameObject(world), viewPort(viewPort)
{
	CL_GraphicContext graphicContext = world->get_gc();

	//Laden der Texturen
	grassTexture = CL_Texture("grassTexture", &world->resources, graphicContext);
	sandTexture = CL_Texture("sandTexture", &world->resources, graphicContext);

	initTextureMap();
	initWayPoints();
}

Terrain::~Terrain(void)
{
}

//Initalisieren der Wegpunkte
//Jeder Wegpunkt ist ein Eckpunkt des Weges und jeweils die Mitte des Feldes
void Terrain::initWayPoints()
{
	wayPoints.push_back(CL_Point(0, -1));
	wayPoints.push_back(CL_Point(0, 2));
	wayPoints.push_back(CL_Point(2, 2));
	wayPoints.push_back(CL_Point(2, 5));
	wayPoints.push_back(CL_Point(1, 5));
	wayPoints.push_back(CL_Point(1, 10));
	wayPoints.push_back(CL_Point(14, 10));
	wayPoints.push_back(CL_Point(14, 6));
	wayPoints.push_back(CL_Point(11, 6));
	wayPoints.push_back(CL_Point(11, 8));
	wayPoints.push_back(CL_Point(8, 8));
	wayPoints.push_back(CL_Point(8, 4));
	wayPoints.push_back(CL_Point(6, 4));
	wayPoints.push_back(CL_Point(6, 7));
	wayPoints.push_back(CL_Point(4, 7));
	wayPoints.push_back(CL_Point(4, 1));
	wayPoints.push_back(CL_Point(12, 1));
	wayPoints.push_back(CL_Point(12, 3));
	wayPoints.push_back(CL_Point(16, 3));
}

//Liefert die Wegpunkte zurück
const std::vector<CL_Point>& Terrain::getWayPoints() const
{
	return wayPoints;
}

//Liefert die Größe (Breite und Höhe) eines Feldes zurück
const CL_Sizex<float>& Terrain::getFieldSize() const
{
	return fieldSize;
}

//Liefert die absolute Feldnummer als Punkt an der übergebenen Stelle
CL_Point Terrain::getFieldLocation(int x, int y) const
{
	float xPosition;
	float yPosition;

	xPosition = x / fieldSize.width;
	yPosition = y / fieldSize.height;

	return CL_Point((int)xPosition, (int)yPosition);
}

//Üperprüft ob an der angegebenen Position ein Wegfeld ist
bool Terrain::isWayField(CL_Point position) const
{
	return textureMap[position.x][position.y] == 'W';
}

//Setzt die Größen des Terrains nach der Fenstergröße
void Terrain::setViewPort(int width, int height)
{
	viewPort.set_width(width);
	viewPort.set_height(height);
	fieldSize.width = width / 16.0f;
	fieldSize.height = height / 12.0f;
}

//Initialisiert die Map für das Terrain
void Terrain::initTextureMap()
{
	char tmp[12][16] = 
	{
		{'W','G','G','G','G','G','G','G','G','G','G','G','G','G','G','G'},
		{'W','G','G','G','W','W','W','W','W','W','W','W','W','G','G','G'},
		{'W','W','W','G','W','G','G','G','G','G','G','G','W','G','G','G'},
		{'G','G','W','G','W','G','G','G','G','G','G','G','W','W','W','W'},
		{'G','G','W','G','W','G','W','W','W','G','G','G','G','G','G','G'},
		{'G','W','W','G','W','G','W','G','W','G','G','G','G','G','G','G'},
		{'G','W','G','G','W','G','W','G','W','G','G','W','W','W','W','G'},
		{'G','W','G','G','W','W','W','G','W','G','G','W','G','G','W','G'},
		{'G','W','G','G','G','G','G','G','W','W','W','W','G','G','W','G'},
		{'G','W','G','G','G','G','G','G','G','G','G','G','G','G','W','G'},
		{'G','W','W','W','W','W','W','W','W','W','W','W','W','W','W','G'},
		{'G','G','G','G','G','G','G','G','G','G','G','G','G','G','G','G'}
	};

	char tmpTextureMap[16][12];

	//Oben ist das Array umgehkehrt aufgebaut, damit es im Code gut lesbar ist
	//--> Umdrehen des Arrays, damit x und y Koordinaten stimmen
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 12; j++)
			tmpTextureMap[i][j] = tmp[j][i];

	//Kopieren der Map in das globale Array
	memcpy(textureMap, tmpTextureMap, sizeof(textureMap));

	//Größe eines Feldes berechnen
	fieldSize = CL_Sizex<float>(viewPort.get_width() / 16.0f, viewPort.get_height() / 12.0f); 
}

//Zeichnen des Terrains
void Terrain::draw()
{
	CL_Quadf textureQuad;
	CL_Rectf textureRectangle;
	CL_Texture activeTexture;
	CL_GraphicContext graphicContext;
	int unitIndex = 0;

	graphicContext = world->get_gc();

	//Durchlaufen der Breite
	for(int i=0;i<16;i++)
	{
		//Durchlaufen der Höhe
		for(int j=0; j<12; j++)
		{
			//Zuweisen der aktuellen Textur
			switch(textureMap[i][j])
			{
				case 'G':
					activeTexture = grassTexture;
					break;
				case 'W':
					activeTexture = sandTexture;
				default:
					break;
			}
			//Zeichnen eines Feldes
			textureRectangle = CL_Rectf(i*fieldSize.width, j*fieldSize.height, fieldSize);
			textureQuad = CL_Quadf(textureRectangle);
			CL_Draw::texture(graphicContext, activeTexture, textureQuad);
		}
	}
}

bool Terrain::update(int timeElapsed_ms)
{
	return true;
}

//Prüfen ob die übergebene Position im Terrain liegt
bool Terrain::isInsideTerrain(const CL_Point& position) const
{
	return position.x < 16 && position.y < 12;
}
