#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

class World;

class GameObject
{
// Construction
public:
	GameObject(World *world) { this->world = world; }

// Attributes
public:
	
// Operations
public:
	virtual void draw() {};
	virtual bool update(int timeElapsed_ms) { return false; }
	
protected:
	World *world;
};

#endif
