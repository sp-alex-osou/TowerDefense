#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <sstream>
#include <string>

#include "menu.h"
#include "world.h"

Menu::Menu(World *world, CL_Rect viewPort) 
: GameObject(world), menuHeight(50), menuTowerLength(30), menuCoinsLength(45), menuTimeLength(45), menuLifeLength(40)
{
	//Viewport für die Positionierung des Menüs setzen
	this->viewPort = viewPort;

	//Textures setzen
	coinsTexture = CL_Texture("coinsTexture", &world->resources, world->get_gc());
	clockTexture = CL_Texture("clockTexture", &world->resources, world->get_gc());
	heartTexture = CL_Texture("heartTexture", &world->resources, world->get_gc());

	//Gibt die Breite des Menüinhaltes an
	menuWidth = -1;
}

Menu::~Menu(void)
{
}

//Liefert die Höhe des Menüs
int Menu::getMenuHeight() const
{
	return menuHeight;
}

//Setzt die Größenverhältnisse des Fensters, um das Menü richtig zu setzen
void Menu::setViewPort(int width, int height)
{
	viewPort.set_width(width);
	viewPort.set_height(height);
}

//Liefert die Nummer des Feldes im Baumenü (von links nach rechts)
//an der mitgegebenen Position (bei 0 beginnend)
//Ist die Position außerhalb des Bauemenüs, wird -1 zurückgegeben
int Menu::getMenuField(int x, int y) const
{
	int field = -1;
	int xPosition;

	//Ist eine menuWidth gesetzt, muss der Anfang des Menüs
	//in x-Richtung ermittelt werden, da es zentriert ist
	if(menuWidth == -1)
		xPosition = 0;
	else
		xPosition = (viewPort.get_width() - menuWidth) / 2;

	//Prüfen ob der Klick überhaupt innerhalb des Baumenüs ist
	if(x >= xPosition && x <= (int)towerTypes.size() * menuHeight + xPosition && y >= viewPort.get_height() - menuHeight)
		field = (x - xPosition) / menuHeight;

	return field;
}

//Zeichnet das Menü
void Menu::draw()
{
	CL_Font fontSmall;
	CL_Font fontBig;
	CL_Rect rectangle;
	std::stringstream sstream;
	std::string outputString;
	CL_Quadf textureQuad;
	CL_Rectf textureRectangle;
	CL_GraphicContext graphicContext;
	int space = 30;
	CL_Size textSize;
	int xPosition;

	//Ist die Menübreite gesetzt, wird der Anfang in x-Richtung
	//berechnet, damit das Menü zentriert ist
	if(menuWidth == -1)
		xPosition = 0;
	else
		xPosition = (viewPort.get_width() - menuWidth) / 2;

	graphicContext = world->get_gc();

	//Schriften setzen
	fontSmall = CL_Font(graphicContext, "Comic Sans MS", 10);
	fontBig = CL_Font(graphicContext, "Comic Sans MS", 20);

	//Die unterschiedlichen Turmtypen durchgehen
	for(unsigned int i = 0; i < towerTypes.size(); i++)
	{
		//Ein Rechteck für den Rahmen eines Baufeldes definieren
		rectangle = CL_Rect(
			i*menuHeight + xPosition,
			viewPort.get_height() - menuHeight,
			(i+1)*menuHeight + xPosition,
			viewPort.get_height()-1);
		//Den Rahmen des Baufeldes zeichnen
		CL_Draw::box(graphicContext, rectangle, CL_Colorf::white);
		
		//Ein Rechteck für die Anzeige der Turmtextur innerhalb des Baufeldes definieren
		textureRectangle = CL_Rectf(
			i * menuHeight + ((menuHeight - menuTowerLength) / 2.0f) + xPosition,
			viewPort.get_height() - menuHeight + 5.0f,
			CL_Sizex<float>((float)menuTowerLength, (float)menuTowerLength));
		textureQuad = CL_Quadf(textureRectangle);
		//Turmtextur zeichnen
		CL_Draw::texture(graphicContext, towerTypes[i]->texture, textureRectangle);
		
		//Nummer (=Shortcut auf Tastatur) des Baufeldes in der linken oberen Ecke zeichnen
		sstream << (i+1);
		sstream >> outputString;
		sstream.clear();
		fontSmall.draw_text(graphicContext, rectangle.left + 5, rectangle.top + 10,
			outputString,CL_Colorf::white);

		//Kosten des Turmes unterhalb der Textur und zentriert im Baufeld ausgeben
		sstream << towerTypes[i]->price;
		sstream >> outputString;
		sstream.clear();
		textSize = fontSmall.get_text_size(graphicContext, outputString);
		fontSmall.draw_text(graphicContext, ((menuHeight - textSize.width) / 2) + i * menuHeight + xPosition,
			viewPort.get_height() - textSize.height/2, outputString, CL_Colorf::white);
	}
	
	//x-Position weitersetzen für nächsten Menüeintrag
	xPosition += towerTypes.size()*menuHeight + space;

	//Geldanzeige
	sstream << world->getMoney();
	sstream >> outputString;
	sstream.clear();
	textSize = fontBig.get_text_size(world->get_gc(), outputString);

	Menu::drawMenuItem(outputString, menuCoinsLength, coinsTexture, fontBig, menuHeight/2, xPosition);

	xPosition += menuCoinsLength + textSize.width + space;

	//Zeitanzeige
	sstream << world->getCountdown();
	sstream >> outputString;
	sstream.clear();
	outputString = "Next Wave:\n" + outputString;
	textSize = fontBig.get_text_size(graphicContext, outputString);

	Menu::drawMenuItem(outputString, menuTimeLength, clockTexture, fontBig, menuHeight/2 - 10, xPosition);

	xPosition += menuTimeLength + textSize.width + space;

	//Lebensanzeige
	sstream << world->getLifes();
	sstream >> outputString;
	sstream.clear();
	
	Menu::drawMenuItem(outputString, menuLifeLength, heartTexture, fontBig, menuHeight/2, xPosition);

	//Beim ersten Aufruf wird die Menübreite errechnet
	if(menuWidth == -1)
	{
		textSize = fontBig.get_text_size(graphicContext, outputString);
		menuWidth = xPosition + menuLifeLength + textSize.width;
	}
}

//Zeichnen eines Menüeintrages, z.B.: Lebensanzeige (nicht für Baufelder)
void Menu::drawMenuItem(std::string outputString, int itemLength, CL_Texture texture, CL_Font font,
		int textOffsetY, int xPosition)
{
	CL_Rectf textureRectangle;
	CL_Quadf textureQuad;

	//Definieren eines Rechtecks für Position und Größe der Textur des Menüeintrages
	textureRectangle = CL_Rectf(
		(float)xPosition,
		viewPort.get_height() - menuHeight + ((menuHeight - itemLength) / 2.0f),
		CL_Sizex<float>((float)itemLength, (float)itemLength));
	textureQuad = CL_Quadf(textureRectangle);
	CL_Draw::texture(world->get_gc(), texture, textureRectangle);
	
	//Text zum Menüeintrag schreiben
	font.draw_text(world->get_gc(), textureRectangle.right + 5.0f,
		(float)(viewPort.get_height() - menuHeight + textOffsetY),
		outputString, CL_Colorf::white);
}

bool Menu::update(int timeElapsed_ms)
{
	//Beim ersten Aufruf die Turmtypen holen
	if (towerTypes.empty())
		towerTypes = world->getTowerTypes();

	return true;
}
