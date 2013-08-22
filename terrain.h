#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "gameobject.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>

class Terrain : public GameObject
{
// Constructor
public:
	Terrain(World *world, CL_Rect viewPort);
	~Terrain(void);

// Attributes
public:

// Operations
public:
	virtual void draw();
	virtual bool update(int timeElapsed_ms);

	const std::vector<CL_Point>& getWayPoints() const;
	const CL_Sizex<float>& getFieldSize() const;
	CL_Point getFieldLocation(int x, int y) const;
	bool Terrain::isInsideTerrain(const CL_Point& position) const;

	void setViewPort(int width, int height);

	bool isWayField(CL_Point position) const;

// Implementation
private:
	void initTextureMap();
	void initWayPoints();

	std::vector<CL_Point> wayPoints; 
	char textureMap[16][12];

	CL_Texture grassTexture;
	CL_Texture sandTexture;

	CL_Rect viewPort;
	CL_Sizex<float> fieldSize;
};

#endif