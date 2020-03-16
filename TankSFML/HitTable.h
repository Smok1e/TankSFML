//{--Hit funcs-----------------------------------------------------------------

void hit_PlayerBullet      (AbstractObject* obj1, AbstractObject* obj2);
void hit_PlayerEnemyBullet (AbstractObject* obj1, AbstractObject* obj2);
void hit_PlayerCoin        (AbstractObject* obj1, AbstractObject* obj2);
void hit_PlayerFood        (AbstractObject* obj1, AbstractObject* obj2);
void hit_PlayerMedkit      (AbstractObject* obj1, AbstractObject* obj2);

void hit_EnemyBullet       (AbstractObject* obj1, AbstractObject* obj2);

void hit_BulletBullet      (AbstractObject* obj1, AbstractObject* obj2);
void hit_BulletObject      (AbstractObject* obj1, AbstractObject* obj2);

void hit_Error             (AbstractObject* obj1, AbstractObject* obj2);
void hit_None              (AbstractObject* obj1, AbstractObject* obj2);

//----------------------------------------------------------------

typedef void hit_t (AbstractObject* obj1, AbstractObject* obj2);

const hit_t* hitTable[TypeAmount][TypeAmount] =

{
	//   0          1                      2                3                   4                      5		  6                 7                 8                 9
	//   TypeNone   Player                 Enemy            Bullet              EnemyBulet             Explosion  Coin              Food              Medkit            LevelUpText

		{hit_Error, hit_Error,             hit_Error,       hit_Error,          hit_Error,             hit_Error, hit_Error,        hit_None,         hit_None,         hit_None }, // 0 TypeNone
		{hit_Error, hit_Error,             hit_None,        hit_PlayerBullet,   hit_PlayerEnemyBullet, hit_None,  hit_PlayerCoin,   hit_PlayerFood,   hit_PlayerMedkit, hit_None }, // 1 Player
		{hit_Error, hit_None,              hit_None,        hit_EnemyBullet,    hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 2 Enemy
		{hit_Error, hit_PlayerBullet,      hit_EnemyBullet, hit_None,           hit_BulletBullet,      hit_None,  hit_BulletObject, hit_BulletObject, hit_BulletObject, hit_None }, // 3 Bullet
		{hit_Error, hit_PlayerEnemyBullet, hit_None,        hit_BulletBullet,   hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 4 EnemyBullet
		{hit_Error, hit_None,              hit_None,        hit_None,           hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 5 Explosion
		{hit_Error,	hit_PlayerCoin,        hit_None,        hit_BulletObject,   hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 6 Coin
		{hit_Error, hit_PlayerFood,        hit_None,        hit_BulletObject,   hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 7 Food
		{hit_Error, hit_PlayerFood,        hit_None,        hit_BulletObject,   hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }, // 8 Medkit
		{hit_Error, hit_None,              hit_None,        hit_None,           hit_None,              hit_None,  hit_None,         hit_None,         hit_None,         hit_None }  // 8 LevelUpText

};