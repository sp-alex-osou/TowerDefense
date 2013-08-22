#ifndef _PLAYER_H_
#define _PLAYER_H_

class Player
{
public:
	Player(int lifes, int money);

	int getLifes() const;
	int getMoney() const;
	bool isAlive() const;

	bool spendMoney(int amount);
	void gainMoney(int amount);
	void looseLife();
	void die();

private:
	int lifes;
	int money;
};

#endif
