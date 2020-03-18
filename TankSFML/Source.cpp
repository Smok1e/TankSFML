#include "Config.h"
#define _USE_MATH_DEFINES
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define _RESOURCES "Resources\\"
#define NOMINMAX
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <math.h>
#include "ResourceManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "C:\Lib\ExtraUtilits\ExtraUtilits.h"

using eu::rnd;

//----------------------------------------------------------------

int StartTime = GetTickCount ();

//----------------------------------------------------------------

enum ObjectType

{

	TypeNone,

	TypePlayer,
	TypeEnemy,
	TypeBullet,
	TypeEnemyBullet,
	TypeExplosion,
	TypeCoin,
	TypeFood,
	TypeMedkit,
	TypeLevelUpText,

	TypeAmount

};

//----------------------------------------------------------------

ResourceManager* ResManager;

FontManager* FManager;

SoundManager* SManager;

struct ObjectManager;

//----------------------------------------------------------------

struct HealthBar

{

	double x_;
	double y_;

	double width_;
	double height_;

	sf::Color background_;
	sf::Color foreground1_;
	sf::Color foreground2_;

	float animation_progress_;

	float animation_speed_;

	float animation_start_, animation_end_;

	float health_;

	HealthBar (double x, double y, double width, double height, sf::Color background, sf::Color foreground1, sf::Color foreground2, int value);

	void setHealth (float health);

	float getHealth ();

	void draw (sf::RenderWindow* window);

	void animate ();

	void start_animation (float start, float end, float speed);

};

HealthBar::HealthBar (double x, double y, double width, double height, sf::Color background, sf::Color foreground1, sf::Color foreground2, int value) :

	x_           (x),
	y_           (y),

	width_       (width),
	height_      (height),

	background_  (background),
	foreground1_ (foreground1),
	foreground2_ (foreground2),

	health_      (value),

	animation_progress_ (0),
	animation_speed_    (0),
	animation_start_    (0),
	animation_end_      (0)

{}

//----------------------------------------------------------------

struct AbstractObject

{

	ObjectManager* manager_;

	double x_;
	double y_;

	double vx_;
	double vy_;

	double r_;

	sf::Color color_;

	int type_;

	AbstractObject (int type, double x, double y, double vx, double vy, double r, sf::Color color);

	virtual void draw ();

	virtual void move ();

	virtual void control ();

	virtual void update ();

	virtual void processEvent (sf::Event event);

	virtual void hit (AbstractObject* object);

	void movePosition (double destX, double destY);

	void setPosition (double x, double y);

	virtual void remove ();

	int getType ();

};

AbstractObject::AbstractObject (int type, double x, double y, double vx, double vy, double r, sf::Color color) :

	type_   (type),
	x_      (x),
	y_      (y),
	vx_     (vx),
	vy_     (vy),
	r_      (r),

	color_  (color),

	manager_ (NULL)

{}

//----------------------------------------------------------------

struct Animation

{

	sf::Image spritesheet_;

	double width_;
	double height_;

	int frame_n_;
	int frame_;

	bool smooth_;

	Animation (int image);

	~Animation ();

	void draw (double x, double y, sf::RenderWindow* window);

	void draw (double x, double y, sf::RenderWindow* window, sf::Color color);

	void setFrame (int frame);

	void advanceFrame ();

};

Animation::Animation (int image) :

	spritesheet_ (ResManager->getImage (image)),

	width_       (0),
	height_      (0),

	frame_n_     (ResManager -> getFrames (image)),
	frame_       (0),

	smooth_      (ResManager -> getSmooth (image))

{

	width_ = spritesheet_.getSize ().x / frame_n_;
	height_ = spritesheet_.getSize ().y;

}

Animation::~Animation ()

{



}

//----------------------------------------------------------------

struct GameObject : AbstractObject

{

	Animation animation_;

	GameObject (int type, double x, double y, double r, int image);

	virtual void draw ();

};

GameObject::GameObject (int type, double x, double y, double r, int image) :

	AbstractObject (type, x, y, 0, 0, r, sf::Color::Transparent),

	animation_ (image)

{}

//----------------------------------------------------------------

struct Player : GameObject

{

	int shoot_cooldown;
	int shoot_cooldown_counter;

	double A_;

	int health_;

	HealthBar healthBar_;

	Player (double x, double y);

	virtual void draw ();

	virtual void move ();

	virtual void control ();

	virtual void update ();

	virtual void processEvent (sf::Event event);

	void addHealth (int health);

};

Player::Player (double x, double y) :

	GameObject (TypePlayer, x, y, 30, ImageTank),

	shoot_cooldown (10),

	shoot_cooldown_counter (shoot_cooldown),

	A_ (0),

	health_ (100),

	healthBar_ (5, wHeight - 25, 300, 20, sf::Color::Black, sf::Color (190, 0, 0), sf::Color (255, 90, 90), health_)

{}

//----------------------------------------------------------------

struct Bullet : AbstractObject

{

	Bullet (double x, double y, double vx, double vy);

	virtual void move ();

	virtual void remove ();

};

Bullet::Bullet (double x, double y, double vx, double vy) :

	AbstractObject (TypeBullet, x, y, vx, vy, 3, sf::Color::White)

{}

//----------------------------------------------------------------

struct Explosion : GameObject

{

	int counter_;

	Explosion (double x, double y);

	virtual void update ();

};

Explosion::Explosion (double x, double y) :

	GameObject (TypeExplosion, x, y, 10, ImageExplosion),

	counter_   (0)

{}

//----------------------------------------------------------------

struct Enemy : GameObject

{

	int health_;

	bool moving_;

	double moving_vx;
	double moving_vy;

	int moving_length;

	int moving_time;

	Enemy ();

	virtual void draw ();

	virtual void move ();

	virtual void control ();

	virtual void update ();

	void reset ();

};

Enemy::Enemy () :

	GameObject (TypeEnemy, rnd (50, wWidth - 50), rnd (50, wHeight - 50), 20, ImageEnemy),

	health_       (100),
	moving_       (false),
	moving_vx     (0),
	moving_vy     (0),
	moving_length (0),
	moving_time   (0)

{}

//----------------------------------------------------------------

struct EnemyBullet : AbstractObject

{

	int damage_;

	double speed_;

	EnemyBullet (double x, double y, double vx, double vy);

	virtual void move () override;

	virtual void remove () override;

};

EnemyBullet::EnemyBullet (double x, double y, double vx, double vy) :

	AbstractObject (TypeEnemyBullet, x, y, vx, vy, 3, sf::Color (0, 179, 255)),

	damage_  (25),
	speed_   (rnd (3, 4))

{}

//----------------------------------------------------------------

struct AbstractBuffItem : GameObject

{

	int counter_limit;
	int counter_;

	AbstractBuffItem (int type, int image, double x, double y, double r, int counter_limit_);

	virtual void update ();

};

AbstractBuffItem::AbstractBuffItem (int type, int image, double x, double y, double r, int counter_limit_) :

	GameObject (type, x, y, r, image),

	counter_limit (counter_limit_),
	counter_      (0)

{}

//----------------------------------------------------------------

struct Coin : AbstractBuffItem

{

	Coin (double x, double y);

};

Coin::Coin (double x, double y) :

	AbstractBuffItem (TypeCoin, ImageCoin, x, y, 15, 10)

{}

//----------------------------------------------------------------

struct Food : AbstractBuffItem

{

	Food (double x, double y);

};

Food::Food (double x, double y) :

	AbstractBuffItem (TypeFood, ImageFood, x, y, 15, 10)

{}

//----------------------------------------------------------------

struct Medkit : AbstractBuffItem

{

	Medkit (double x, double y);

};

Medkit::Medkit (double x, double y) :

	AbstractBuffItem (TypeMedkit, ImageMedkit, x, y, 15, 10)

{}

//----------------------------------------------------------------

struct LevelUpText : GameObject

{

	double alpha_;

	LevelUpText ();

	virtual void draw ();

	virtual void move ();

};

LevelUpText::LevelUpText () :

	GameObject (TypeLevelUpText, wWidth / 2, wHeight / 2, 0, ImageLevelUpText),

	alpha_ (1)

{}

//----------------------------------------------------------------

struct ObjectManager

{

	AbstractObject* objects_[OBJECTS_MAX];

	sf::RenderWindow* window_;

	sf::Color background_;

	int score_;

	int level_;

	sf::Image cursor_;

	ObjectManager (sf::RenderWindow* window);

	~ObjectManager ();

	int addObject (AbstractObject* object);

	int removeObject (AbstractObject* object);

	void drawObjects ();

	void moveObjects ();

	void controlObjects ();

	void UpdateObjects ();

	void processEvents (sf::Event event);

	void checkCollision ();

	int objectsAmount ();

	void update ();

	void run ();

	void addScore ();

	void addScore (int score);

	void setBackground (sf::Color background);

	void setCursor (sf::Image * cursor);

	void remove ();

};

ObjectManager::ObjectManager (sf::RenderWindow* window) :

	objects_ (),
	window_  (window),

	score_   (0),
	level_   (0),

	cursor_  (ResManager -> getImage (ImageCursor))

{}

ObjectManager::~ObjectManager ()

{

	remove ();

}

//----------------------------------------------------------------

int run (sf::RenderWindow * window);

double angle (double x0, double y0, double x1, double y1);

template <typename T>

T* getObject (ObjectManager* manager);

template <typename T>

T* checkType (AbstractObject* object);

double sqrDistance (double x, double y, double x1, double y1);

bool collisionDetection (const AbstractObject* obj1, const AbstractObject* obj2);

double sqr (double d);

void pause (ObjectManager* manager);

void debugMode (ObjectManager * manager);

void Hit (AbstractObject* obj1, AbstractObject* obj2);

//----------------------------------------------------------------

#include "HitTable.h"

//----------------------------------------------------------------

int main ()

{

	sf::RenderWindow window (sf::VideoMode (wWidth, wHeight), "Tank");

	ResourceManager resManager (Resources, _RESOURCES"Images");
	ResManager = &resManager;

	FontManager fontManager (Fonts, _RESOURCES"Fonts");
	FManager = &fontManager;

	SoundManager soundManager (Sounds, _RESOURCES"Sound");
	SManager = &soundManager;

	run (&window);

}

//----------------------------------------------------------------

int run (sf::RenderWindow * window)

{

	ObjectManager manager (window);
	manager.setBackground (sf::Color (40, 40, 40));

	window -> setFramerateLimit (FPS_LIMIT);

	sf::Image icon = ResManager->getImage (ImageIcon);

	window -> setIcon (icon.getSize ().x, icon.getSize ().y, icon.getPixelsPtr ());

	Player* player = new Player{ wWidth / 2, wHeight / 2 };

	manager.addObject (player);

	for (int n = 0; n < ENEMY_N; n++) manager.addObject (new Enemy);

	while (window -> isOpen ())

	{

		sf::Event event;

		while (window -> pollEvent (event))

		{

			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed (sf::Keyboard::Escape)) window -> close ();

			if (event.type == sf::Event::KeyPressed)

			{

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::P))

				{

					pause (&manager);

				}

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::K))

				{

					manager.addObject (new Coin{ rnd (15, wWidth - 15), rnd (15, wHeight - 15) });

				}

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::Tab))

				{

					debugMode (&manager);

				}

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::E))

				{

					manager.addObject (new Enemy ());

				}

			}

			manager.processEvents (event);

		}

		if (window -> isOpen () && GetForegroundWindow () != window -> getSystemHandle ()) pause (&manager);

		Player* player = getObject <Player> (&manager);

		if (player->health_ <= 0) window -> close ();

		manager.run ();

		window -> display ();

	}

	return 0;

}

//----------------------------------------------------------------

void AbstractObject::draw ()

{

	sf::CircleShape circle;

	circle.setPosition (sf::Vector2f (x_ - r_, y_ - r_));
	circle.setRadius (r_);
	circle.setFillColor (color_);
	manager_->window_->draw (circle);

}

//----------------------------------------------------------------

void AbstractObject::move ()

{

	x_ += vx_;
	y_ += vy_;

}

//----------------------------------------------------------------

void AbstractObject::remove ()

{

	manager_->removeObject (this);

}

//----------------------------------------------------------------

void AbstractObject::control ()

{

}

//----------------------------------------------------------------

void AbstractObject::update ()

{


}

//----------------------------------------------------------------

void AbstractObject::processEvent (sf::Event event)

{

}

//----------------------------------------------------------------

void AbstractObject::hit (AbstractObject* object)

{

	::Hit (this, object);

}

//----------------------------------------------------------------

void AbstractObject::movePosition (double destX, double destY)

{

	if (x_ + destX <= wWidth - r_ && x_ + destX >= r_) x_ += destX;

	else if (x_ + destX > wWidth - r_) x_ = wWidth - r_;

	else if (x_ + destX < r_) x_ = r_;

	if (y_ + destY <= wHeight - r_ && y_ + destY >= r_) y_ += destY;

	else if (y_ + destY > wHeight - r_) y_ = wHeight - r_;

	else if (y_ + destY < r_) y_ = r_;

}

//----------------------------------------------------------------

void AbstractObject::setPosition (double x, double y)

{

	if (x <= wWidth - r_ && x_ >= r_) x_ = x;

	else if (x > wWidth - r_) x_ = wWidth - r_;

	else if (x < r_) x_ = r_;

	if (y <= wHeight - r_ && y_ >= r_) y_ = y;

	else if (y > wHeight - r_) y_ = wHeight - r_;

	else if (y < r_) y_ = r_;

}

//----------------------------------------------------------------

int AbstractObject::getType ()

{

	return type_;

}

//----------------------------------------------------------------

void Animation::draw (double x, double y, sf::RenderWindow* window)

{

	sf::IntRect bounds (width_ * frame_, 0, width_, height_);

	//sf::IntRect bounds (0, 0, width_, height_);

	sf::Texture texture;

	texture.loadFromImage (spritesheet_, bounds);
	texture.setSmooth (smooth_);

	sf::Sprite sprite;
	sprite.setTexture (texture);
	sprite.setPosition (x, y);
	//sprite.setOrigin (width_ / 2, height_ / 2);
	//sprite.setRotation (frame_ / 72.0 * 360);

	window->draw (sprite);

}

//----------------------------------------------------------------

void Animation::draw (double x, double y, sf::RenderWindow* window, sf::Color color)

{

	sf::IntRect bounds (width_ * frame_, 0, width_, height_);

	sf::Texture texture;

	texture.loadFromImage (spritesheet_, bounds);
	texture.setSmooth (smooth_);

	sf::Sprite sprite;
	sprite.setTexture (texture);
	sprite.setPosition (x, y);

	sprite.setColor (color);

	window->draw (sprite);

}

//----------------------------------------------------------------

void Animation::setFrame (int frame)

{

	if (frame < frame_n_) frame_ = frame;

}

//----------------------------------------------------------------

void Animation::advanceFrame ()

{

	frame_++;

	if (frame_ >= frame_n_) frame_ = 0;

}

//----------------------------------------------------------------

void GameObject::draw ()

{

	animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2, manager_->window_);

}

//----------------------------------------------------------------

void Player::draw ()

{

	healthBar_.draw (manager_->window_);

	sf::Texture texture;

	texture.loadFromImage (animation_.spritesheet_);

	sf::Sprite sprite;
	sprite.setTexture (texture);
	sprite.setOrigin (animation_.width_ / 2, animation_.height_ / 2);
	sprite.setPosition (x_, y_);
	sprite.setRotation (A_);

	manager_->window_->draw (sprite);

	sf::Font font = FManager->getFont (FontVAGWorld);

	char str[100] = "";

	sprintf_s (str, "Level: %d", manager_->level_);

	sf::Text text;
	text.setCharacterSize (18);
	text.setFont (font);
	text.setString (str);
	text.setPosition (5, wHeight - 45);
	text.setFillColor (sf::Color (150, 150, 150));

	manager_->window_->draw (text);

	sprintf_s (str, "Score: %d", manager_->score_);

	text.setString (str);
	text.setPosition (5, wHeight - 65);
	text.setFillColor (sf::Color::White);

	manager_->window_->draw (text);

}

//----------------------------------------------------------------

void Player::control ()

{

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::W)) movePosition (0, -PLAYER_SPEED);

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::A)) movePosition (-PLAYER_SPEED, 0);

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::S)) movePosition (0, PLAYER_SPEED);

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::D)) movePosition (PLAYER_SPEED, 0);

	sf::Vector2i mPos = sf::Mouse::getPosition (*manager_->window_);

	if (sf::Mouse::isButtonPressed (sf::Mouse::Left))

	{

		if (shoot_cooldown_counter >= shoot_cooldown)

		{

			double a = mPos.x - x_;
			double b = mPos.y - y_;

			double c = sqrt (a * a + b * b);

			double vx = a / c;
			double vy = b / c;

			manager_->addObject (new Bullet{ x_ + vx * 45, y_ + vy * 45, vx * 10 + rnd (-0.5, 0.5), vy * 10 + rnd (-0.5, 0.5) });

			shoot_cooldown_counter = 0;

		}

	}

	else shoot_cooldown_counter = shoot_cooldown;

	A_ = 360 - angle (x_, y_, mPos.x, mPos.y);

}

//----------------------------------------------------------------

void Player::update ()

{

	int frame = 72.0 / 360.0 * A_;

	animation_.setFrame (frame);

}

//----------------------------------------------------------------

void Player::move ()

{

	shoot_cooldown = 20 - manager_->level_ * 2;

	AbstractObject::move();

	if (shoot_cooldown_counter < shoot_cooldown) shoot_cooldown_counter++;

}

//----------------------------------------------------------------

void Player::processEvent (sf::Event event)

{

	/*if (event.type == sf::Event::MouseWheelMoved)

	{

		int delta = event.mouseWheel.delta;

		if (shoot_cooldown - delta >= 0 && shoot_cooldown - delta <= 100) shoot_cooldown -= delta;

	}*/

}

//----------------------------------------------------------------

void Player::addHealth (int health)

{

	health_ += health;

	if (health_ > 100)

	{

		health_ = 100;

	}

	else if (health_ < 0)

	{

		health_ = 0;

	}

	healthBar_.setHealth (health_);

}

//----------------------------------------------------------------

void Bullet::move ()

{

	x_ += vx_;
	y_ += vy_;

	//if (x_ > wWidth - r_ || x_ < r_) vx_ = -vx_;
	//if (y_ > wHeight - r_ || y_ < r_) vy_ = -vy_;

	if (x_ >= wWidth - r_ || x_ < r_ || y_ >= wHeight - r_ || y_ < r_) remove ();

}

//----------------------------------------------------------------

void Bullet::remove ()

{

	manager_->addObject (new Explosion{ x_, y_ });

	AbstractObject::remove ();

}

//----------------------------------------------------------------

void Explosion::update ()

{

	counter_++;

	if (counter_ % 4 == 0)

	{

		animation_.advanceFrame ();

	}

	if (counter_ >= animation_.frame_n_ * 4) remove ();

}

//----------------------------------------------------------------

void Enemy::draw ()

{

	GameObject::draw ();

	double w = 30;
	double h = 7;

	double x = x_ - w / 2;
	double y = y_ + r_ + 5;

	sf::RectangleShape rect;
	rect.setFillColor (sf::Color::Black);
	rect.setPosition (x, y);
	rect.setSize (sf::Vector2f (w, h));

	manager_->window_->draw (rect);

	rect.setFillColor (sf::Color (255, 48, 48));
	rect.setPosition (x, y);
	rect.setSize (sf::Vector2f (w / 100 * health_, h));

	manager_->window_->draw (rect);

}

//----------------------------------------------------------------

void Enemy::move ()

{

	if (!moving_)

	{

		if (floor (rnd (1, 300)) == 1)

		{

			moving_ = true;
			moving_time = 0;

			moving_vx = rnd (-1, 1) / 20;
			moving_vy = rnd (-1, 1) / 20;
			moving_length = 50;

		}

	}

	else

	{

		if (moving_time <= moving_length)

		{

			setPosition (x_ + moving_vx * moving_time, y_ + moving_vy * moving_time);

			moving_time++;

		}

		else

		{

			moving_ = false;

		}

	}

}

//----------------------------------------------------------------

void Enemy::control ()

{

	if (floor (rnd (1, ENEMY_SHOOTING_FREQ)) == 1)

	{

		Player* player = getObject <Player> (manager_);

		double a = x_ - (player->x_);
		double b = y_ - (player->y_);

		double c = sqrt (a * a + b * b);

		double vx = a / c;
		double vy = b / c;

		EnemyBullet* bullet = new EnemyBullet{ x_ + vx * 24, y_ + vy * 24, vx, -vy };

		manager_->addObject (bullet);

	}

	if (health_ <= 0)

	{

		if (floor (rnd (0, COIN_SPAWNING_FREQ)) == 1)

		{

			manager_->addObject (new Coin{ x_, y_ });

			SoundPlayer* sound = SManager->getSound (SoundItemSpawn);
			sound->play ();

		}

		else if (floor (rnd (0, FOOD_SPAWNING_FREQ)) == 1)

		{

			manager_->addObject (new Food{ x_, y_ });

			SoundPlayer* sound = SManager->getSound (SoundItemSpawn);
			sound->play ();

		}

		else if (floor (rnd (0, MEDKIT_SPAWNING_FREQ)) == 1)

		{

			manager_->addObject (new Medkit{ x_, y_ });

			SoundPlayer* sound = SManager->getSound (SoundItemSpawn);
			sound->play ();

		}

		reset ();

		manager_->addScore ();

	}

}

//----------------------------------------------------------------

void Enemy::update ()

{

	animation_.advanceFrame ();

}

//----------------------------------------------------------------

void Enemy::reset ()

{

	x_ = rnd (50, wWidth - 50);
	y_ = rnd (50, wHeight - 50);

	health_ = 100;

}

//----------------------------------------------------------------

void EnemyBullet::move ()

{

	x_ -= speed_ * vx_;
	y_ += speed_ * vy_;

	if (x_ - r_ < 0 || x_ + r_ > wWidth || y_ - r_ < 0 || y_ + r_ > wHeight)

	{

		remove ();

	}

}

//----------------------------------------------------------------

void EnemyBullet::remove ()

{

	manager_->addObject (new Explosion (x_, y_));

	AbstractObject::remove ();

}

//----------------------------------------------------------------

void AbstractBuffItem::update ()

{

	if (counter_ >= counter_limit)

	{

		animation_.advanceFrame ();

		counter_ = 0;

	}

	counter_++;

}

//----------------------------------------------------------------

void LevelUpText::draw ()

{

	animation_.draw (x_ - animation_.width_ / 2, y_ - animation_.height_ / 2, manager_->window_, sf::Color (255, 255, 255, 255.0 * alpha_));

}

//----------------------------------------------------------------

void LevelUpText::move ()

{

	alpha_ -= 0.01;
	y_ -= 0.5;

	if (alpha_ <= 0) remove ();

}

//----------------------------------------------------------------

void ObjectManager::drawObjects ()

{

	window_->clear (background_);

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		objects_[n]->draw ();

	}

	sf::Font font = FManager->getFont (FontVAGWorld);

	sf::Text text;
	text.setString ("Press p to pause");
	text.setCharacterSize (20);
	text.setFillColor (sf::Color::White);
	text.setFont (font);

	sf::FloatRect bounds = text.getGlobalBounds ();

	text.setPosition (wWidth - bounds.width - 5, wHeight - bounds.height - 5);

	window_->draw (text);

}

//----------------------------------------------------------------

void ObjectManager::moveObjects ()

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		objects_[n]->move ();

	}

}

//----------------------------------------------------------------

void ObjectManager::controlObjects ()

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		objects_[n]->control ();

	}

}

//----------------------------------------------------------------

void ObjectManager::UpdateObjects ()

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		objects_[n]->update ();

	}

}

//----------------------------------------------------------------

void ObjectManager::update ()

{

	sf::Cursor cursor;
	cursor.loadFromPixels (cursor_.getPixelsPtr (), sf::Vector2u (cursor_.getSize ().x, cursor_.getSize ().y), sf::Vector2u (cursor_.getSize ().x / 2, cursor_.getSize ().y / 2));

	window_->setMouseCursor (cursor);

}

//----------------------------------------------------------------

void ObjectManager::run ()

{

	update ();
	drawObjects ();
	moveObjects ();
	controlObjects ();
	UpdateObjects ();
	checkCollision ();

}

//----------------------------------------------------------------

int ObjectManager::addObject (AbstractObject* object)

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n])

		{

			objects_[n] = object;

			object->manager_ = this;

			printf ("%s: Object added %p, object type = %d\n", __PRETTY_FUNCTION__, object, objects_[n]->getType ());

			return n;

		}

	}

	return -1;

}

//----------------------------------------------------------------

int ObjectManager::removeObject (AbstractObject* object)

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (objects_[n] == object)

		{

			printf ("%s: Object removed %p, object type = %d\n", __PRETTY_FUNCTION__, objects_[n], objects_[n]->getType ());

			delete (objects_[n]);

			objects_[n] = nullptr;

			return n;

		}

	}

	return -1;

}

//----------------------------------------------------------------

void ObjectManager::processEvents (sf::Event event)

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		objects_[n]->processEvent (event);

	}

}
//----------------------------------------------------------------

void ObjectManager::checkCollision ()

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!objects_[n]) continue;

		for (int i = n + 1; i < OBJECTS_MAX; i++)

		{

			if (!objects_[i]) continue;
			if (!objects_[n]) break;

			if (collisionDetection (objects_[n], objects_[i]))

			{

				objects_[n]->hit (objects_[i]);

			}

		}

	}

}

//----------------------------------------------------------------

int ObjectManager::objectsAmount ()

{

	int n = 0;

	for (int i = 0; i < OBJECTS_MAX; i++)

	{

		if (!objects_[i]) continue;

		n++;

	}

	return n;

}

//----------------------------------------------------------------

void ObjectManager::addScore ()

{

	score_++;

	if (score_ % 10 == 0)

	{

		level_++;

		addObject (new LevelUpText);

	}

}

//----------------------------------------------------------------

void ObjectManager::addScore (int score)

{

	for (int i = 0; i < score; i++)

	{

		score_++;

		if (score_ % 10 == 0)

		{

			level_++;

			addObject (new LevelUpText);

		}

	}

}

//----------------------------------------------------------------

void ObjectManager::setBackground (sf::Color background)

{

	background_ = background;

}

//----------------------------------------------------------------

void ObjectManager::setCursor (sf::Image * cursor)

{

	cursor_ = *cursor;

}

//----------------------------------------------------------------

void ObjectManager::remove ()

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (objects_[n])

		{

			objects_[n]->remove ();

			delete (objects_[n]);

		}

	}

}

//----------------------------------------------------------------

void HealthBar::setHealth (float health)

{

	start_animation (health_, health, 0.05);

}

//----------------------------------------------------------------

float HealthBar::getHealth ()

{

	return health_;

}

//----------------------------------------------------------------

void HealthBar::draw (sf::RenderWindow* window)

{

	animate ();

	sf::RectangleShape rect;
	rect.setFillColor (background_);
	rect.setPosition (x_, y_);
	rect.setSize (sf::Vector2f (width_, height_));

	window->draw (rect);

	int w = floor (width_ / 100.0 * health_);

	for (int i = 0; i < w; i++)

	{

		sf::Color color;

		color.r = (double)foreground1_.r + (((double)foreground2_.r - (double)foreground1_.r) / width_) * i;
		color.g = (double)foreground1_.g + (((double)foreground2_.g - (double)foreground1_.g) / width_) * i;
		color.b = (double)foreground1_.b + (((double)foreground2_.b - (double)foreground1_.b) / width_) * i;

		int x = floor (x_ + 1 + i);
		int y = floor (y_);

		rect.setFillColor (color);
		rect.setPosition (x, y);
		rect.setSize (sf::Vector2f (1, height_ - 1));

		window->draw (rect);

	}

	window->draw (rect);

	char str[100] = "";

	sprintf_s (str, "%d", (int)health_);

	sf::Font font = FManager->getFont (FontVAGWorld);

	sf::Text text;
	text.setString (str);
	text.setFillColor (sf::Color::White);
	text.setPosition (x_, y_ - 1);
	text.setCharacterSize (18);
	text.setFont (font);

	window->draw (text);

}

//----------------------------------------------------------------

void HealthBar::animate ()

{

	if (animation_speed_ == 0) return;


	animation_progress_ += animation_speed_;

	if (animation_progress_ > 1)

	{

		health_ = animation_end_;
		animation_speed_ = 0;
		return;

	}

	float scale = 1 - pow (1 - animation_progress_, 2);

	health_ = animation_start_ + (animation_end_ - animation_start_) * scale;

}

//----------------------------------------------------------------

void HealthBar::start_animation (float start, float end, float speed)

{

	animation_progress_ = 0;

	animation_speed_ = speed;
	animation_start_ = start;
	animation_end_ = end;

}

//----------------------------------------------------------------

double angle (double x0, double y0, double x1, double y1)

{

	double dx = x1 - x0;
	double dy = y1 - y0;

	if (fabs (dx) < EPSILON) dx = 1;
	if (fabs (dy) < EPSILON) dy = 1;

	double A = atan (-dy / dx);

	A = A / M_PI * 180;

	if (dx < 0) A += 180;

	if (dx > 0 && dy > 0) A += 360;

	return A;

}

//----------------------------------------------------------------

template <typename T>

T* getObject (ObjectManager* manager)

{

	for (int n = 0; n < OBJECTS_MAX; n++)

	{

		if (!manager->objects_[n]) continue;

		T* object = dynamic_cast <T*> (manager->objects_[n]);

		if (object) return object;

	}

	return nullptr;

}

//----------------------------------------------------------------

template <typename T>

T* checkType (AbstractObject* object)

{

	T* ptr = dynamic_cast <T*> (object);

	if (ptr) return ptr;

	return nullptr;

}

//----------------------------------------------------------------

double sqrDistance (double x, double y, double x1, double y1)

{

	double a = x - x1;
	double b = y - y1;

	double c = sqr(a) + sqr (b);

	return c;

}

//-----------------------------------------------------------------------------

bool collisionDetection (const AbstractObject* obj1, const AbstractObject* obj2)

{

	double sqrD = sqrDistance (obj1->x_, obj1->y_, obj2->x_, obj2->y_);

	if (sqrD <= sqr(obj1->r_ + obj2->r_))

	{

		return true;

	}

	return false;

}

//-----------------------------------------------------------------------------

double sqr (double d)

{

	return d * d;

}

//-----------------------------------------------------------------------------

void pause (ObjectManager* manager)

{

	printf ("%s: Paused\n", __PRETTY_FUNCTION__);

	manager->drawObjects ();

	sf::RectangleShape rect;
	rect.setPosition (0, 0);
	rect.setSize (sf::Vector2f (wWidth, wHeight));

	sf::Font font = FManager->getFont (FontVAGWorld);

	sf::Text text;
	text.setString ("Paused");
	text.setCharacterSize (30);
	text.setFont (font);

	sf::FloatRect bounds = text.getGlobalBounds ();

	text.setPosition (wWidth / 2 - bounds.width / 2, wHeight / 2 - bounds.height / 2);

	int a = 0;

	for (a = a; a <= 100; a += 3)

	{

		manager->update ();

		manager->UpdateObjects ();
		manager->drawObjects ();

		rect.setFillColor (sf::Color (0, 0, 0, a));
		text.setFillColor (sf::Color (255, 255, 255, 255.0 / 100.0 * a));

		manager->window_->draw (rect);
		manager->window_->draw (text);
		manager->window_->display ();

	}

	bool done = false;

	while (!done)

	{

		manager->update ();

		manager->UpdateObjects ();
		manager->drawObjects ();

		rect.setFillColor (sf::Color (0, 0, 0, a));
		text.setFillColor (sf::Color (255, 255, 255, 255.0 / 100.0 * a));

		manager->window_->draw (rect);
		manager->window_->draw (text);
		manager->window_->display ();

		sf::Event event;

		while (manager->window_->pollEvent (event))

		{

			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed (sf::Keyboard::Escape)) manager->window_->close ();

			if (event.type == sf::Event::KeyPressed)

			{

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::P)) done = true;

			}

			if (event.type == sf::Event::MouseButtonPressed) done = true;

		}

		manager->window_->display ();

	}

	printf ("%s, Continued\n", __PRETTY_FUNCTION__);

	for (a = a; a >= 0; a -= 3)

	{

		manager->update ();
		manager->drawObjects ();

		rect.setFillColor (sf::Color (0, 0, 0, a));
		text.setFillColor (sf::Color (255, 255, 255, 255.0 / 100.0 * a));

		manager->window_->draw (rect);
		manager->window_->draw (text);
		manager->window_->display ();

	}

}

//-----------------------------------------------------------------------------

void debugMode (ObjectManager* manager)

{

	printf ("%s: Debug mode activated\n", __PRETTY_FUNCTION__);

	manager -> setCursor (&ResManager -> getImage (ImageDebugCursor));

	bool done = false;

	while (!done)

	{

		manager->update ();

		manager->window_->clear (manager -> background_);

		for (int n = 0; n < OBJECTS_MAX; n++)

		{

			if (!manager->objects_[n]) continue;

			manager->objects_[n]->draw ();

			double x = manager->objects_[n]->x_;
			double y = manager->objects_[n]->y_;
			double r = manager->objects_[n]->r_;

			sf::CircleShape circle;
			circle.setFillColor (sf::Color::Transparent);
			circle.setOutlineColor (sf::Color::White);
			circle.setOutlineThickness (1);
			circle.setRadius (r);
			circle.setPosition (x - r, y - r);

			manager->window_->draw (circle);

			sf::RectangleShape rect;
			rect.setPosition (x - r, y);
			rect.setFillColor (sf::Color::White);
			rect.setSize (sf::Vector2f (r * 2, 2));

			manager->window_->draw (rect);

			rect.setPosition (x, y - r);
			rect.setFillColor (sf::Color::White);
			rect.setSize (sf::Vector2f (2, r * 2));

			manager->window_->draw (rect);

		}

		sf::Font font;
		font = FManager->getFont (FontVAGWorld);

		sf::Text text;
		text.setFont (font);
		text.setFillColor (sf::Color (0, 149, 255));
		text.setString ("[Debug mode]");
		text.setCharacterSize (20);
		text.setPosition (2, 2);

		manager->window_->draw (text);

		char str[100] = "";
		sprintf_s (str, "Objects amount: %d", manager->objectsAmount ());

		text.setString (str);
		text.setPosition (2, 22);

		manager->window_->draw (text);

		sprintf_s (str, "Game started %d seconds ago", (GetTickCount () - StartTime) / 1000);

		text.setString (str);
		text.setPosition (2, 44);

		manager->window_->draw (text);

		sf::Event event;

		while (manager->window_->pollEvent (event))

		{

			if (event.type == sf::Event::Closed)

			{

				done = true;
				manager->window_->close ();

			}

			if (event.type == sf::Event::KeyPressed)

			{

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::Tab)) done = true;

				if (sf::Keyboard::isKeyPressed (sf::Keyboard::Escape))

				{

					done = true;
					manager->window_->close ();

				}

			}

		}

		if (sf::Mouse::isButtonPressed (sf::Mouse::Left))

		{

			sf::Vector2i mPos = sf::Mouse::getPosition (*manager->window_);

			AbstractObject obj (TypeNone, mPos.x, mPos.y, 0, 0, 1, sf::Color::Transparent);

			for (int n = 0; n < OBJECTS_MAX; n++)

			{

				if (!manager->objects_[n]) continue;

				if (collisionDetection (&obj, manager->objects_[n]))

				{

					if (manager->objects_[n]->getType () != TypePlayer) manager->objects_[n]->remove ();

					else

					{

						MessageBox (manager->window_->getSystemHandle (), L"You may not remove player.", L"Error", MB_OK | MB_ICONWARNING);

					}

				}

			}

		}

		manager->window_->display ();

	}

	manager -> setCursor (& ResManager->getImage (ImageCursor));

	printf ("%s: Debug mode stopped\n", __PRETTY_FUNCTION__);

}

//-----------------------------------------------------------------------------

void Hit (AbstractObject* obj1, AbstractObject* obj2)

{

	int objectType1 = obj1->getType (), objectType2 = obj2->getType ();

	if (objectType1 > objectType2) std::swap (obj1, obj2);

	hitTable[objectType1][objectType2] (obj1, obj2);

}

//{----------------------------------------------------------------------------

void hit_PlayerBullet (AbstractObject* obj1, AbstractObject* obj2)

{

	Player* player = checkType <Player> (obj1);

	player->addHealth (-(rnd (2, 6)));

	obj2->remove ();

}

void hit_PlayerEnemyBullet (AbstractObject* obj1, AbstractObject* obj2)

{

	Player* player = checkType <Player> (obj1);

	player->addHealth (-15);

	obj2->remove ();

}

void hit_PlayerCoin (AbstractObject* obj1, AbstractObject* obj2)

{

	Player* player = checkType <Player> (obj1);

	player->addHealth (rnd (20, 35));

	player->manager_->addScore (10);

	obj2->remove ();

	SoundPlayer* sound = SManager->getSound (SoundCoin);
	sound->play ();

}

void hit_PlayerFood (AbstractObject* obj1, AbstractObject* obj2)

{

	Player* player = checkType <Player> (obj1);

	player->addHealth (15);

	obj2->remove ();

	SoundPlayer* sound = SManager->getSound (SoundItemTake);
	sound->play ();

}

void hit_PlayerMedkit (AbstractObject* obj1, AbstractObject* obj2)

{

	Player* player = checkType <Player> (obj1);

	player->addHealth (100);

	obj2->remove ();

	SoundPlayer* sound = SManager->getSound (SoundItemTake);
	sound->play ();

}

//-----------------------------------------------------------------------------

void hit_EnemyBullet (AbstractObject* obj1, AbstractObject* obj2)

{

	Enemy* enemy = checkType <Enemy>  (obj1);
	Bullet* bullet = checkType <Bullet> (obj2);
	enemy->health_ -= rnd (5, 15);

	enemy->setPosition (enemy->x_ + bullet->vx_, enemy->y_ + bullet->vy_);

	bullet->remove ();

}

void hit_BulletBullet (AbstractObject* obj1, AbstractObject* obj2)

{

	obj1->remove ();
	obj2->remove ();

}

void hit_BulletObject (AbstractObject* obj1, AbstractObject* obj2)

{

	obj2->movePosition (obj1->vx_, obj1->vy_);

	obj1->vx_ = -obj1->vx_;
	obj1->vy_ = -obj1->vy_;

}

//-----------------------------------------------------------------------------

void hit_Error (AbstractObject* obj1, AbstractObject* obj2)

{

}

//-----------------------------------------------------------------------------

void hit_None (AbstractObject* obj1, AbstractObject* obj2)

{

}

//}----------------------------------------------------------------------------
