#include "player.h"

Player::Player(int lifes, int money)
: lifes(lifes), money(money)
{
}

// liefert die Anzahl der Leben
int Player::getLifes() const
{
	return lifes;
}

// liefert die Geldmenge
int Player::getMoney() const
{
	return money;
}

// gibt an, ob der Spieler lebt
bool Player::isAlive() const
{
	return lifes > 0;
}

// l�sst den Spieler Geld ausgeben
bool Player::spendMoney(int amount)
{
	if (money < amount)
		return false;

	money -= amount;
	return true;
}

// l�sst den Spieler Geld verdienen
void Player::gainMoney(int amount)
{
	money += amount;
}

// zieht dem Spieler ein Leben ab
void Player::looseLife()
{
	--lifes;
}

// t�tet den Spieler
void Player::die()
{
	lifes = 0;
}
