#include <SFML\Graphics.hpp>

//----------------------------------------------------------------

const int BUTTONS_MAX = 100;

//----------------------------------------------------------------

struct ButtonManager;

//----------------------------------------------------------------

struct Button

{

	double x_;
	double y_;

	double width_;
	double height_;

	sf::Color color_;
	sf::Color strokeColor_;

	ButtonManager* manager_;

	bool stroke_;

	Button (double x, double y, double width, double height, sf::Color color, sf::Color strokeColor);

	virtual void draw ();

	virtual void onTimeTick ();

};

Button::Button (double x, double y, double width, double height, sf::Color color, sf::Color strokeColor) :

	x_           (x),
	y_           (y),

	width_       (width),
	height_      (height),

	color_       (color),
	strokeColor_ (strokeColor),

	stroke_      (false)

{}

//----------------------------------------------------------------

struct ButtonManager

{

	Button * buttons_ [BUTTONS_MAX];

	sf::RenderWindow* window_;

	ButtonManager (sf::RenderWindow * window);

	~ButtonManager ();

	int addButton (Button * button);

	int removeButton (Button* button);

	void drawButtons ();

	void onTimeTick ();

	void run ();

};

ButtonManager::ButtonManager (sf::RenderWindow * window) :

	buttons_ (),
	window_  (window)

{}

ButtonManager::~ButtonManager ()

{

	for (int n = 0; n < BUTTONS_MAX; n++)

	{

		if (!buttons_[n]) continue;

		removeButton (buttons_[n]);

	}

}

//----------------------------------------------------------------

void Button::draw ()

{

	sf::RectangleShape rect;
	rect.setFillColor (color_);
	rect.setSize (sf::Vector2f (width_, height_));

	if (stroke_)

	{

		rect.setSize (sf::Vector2f (width_ - 4, height_ - 4));
		rect.setPosition (x_ + 2, y_ + 2);
		rect.setOutlineColor (strokeColor_);
		rect.setOutlineThickness (2);

	}

	manager_->window_->draw (rect);

}

//----------------------------------------------------------------

void Button::onTimeTick ()

{

	sf::Vector2i mPos = sf::Mouse::getPosition ();
	sf::Vector2i wPos = manager_->window_->getPosition ();

	mPos.x = mPos.x - wPos.x - 8;
	mPos.y = mPos.y - wPos.y - 30;

	if (mPos.x >= x_ && mPos.x < x_ + width_ && mPos.y >= y_ && mPos.y < y_ + height_) stroke_ = true;

	else stroke_ = false;

}

//----------------------------------------------------------------

int ButtonManager::addButton (Button * button)

{

	for (int n = 0; n < BUTTONS_MAX; n++)

	{

		if (!buttons_[n])

		{

			button->manager_ = this;

			buttons_[n] = button;
			return n;

		}

	}

	return -1;

}

//----------------------------------------------------------------

int ButtonManager::removeButton (Button* button)

{

	for (int n = 0; n < BUTTONS_MAX; n++)

	{

		if (!buttons_[n]) continue;

		if (buttons_[n] == button)

		{

			buttons_[n] = nullptr;

			delete (button);

			return n;

		}

	}

	return -1;

}

//----------------------------------------------------------------

void ButtonManager::drawButtons ()

{

	for (int n = 0; n < BUTTONS_MAX; n++)

	{

		if (!buttons_[n]) continue;

		buttons_[n]->draw ();

	}

}

//----------------------------------------------------------------

void ButtonManager::onTimeTick ()

{

	for (int n = 0; n < BUTTONS_MAX; n++)

	{

		if (!buttons_[n]) continue;

		buttons_[n]->onTimeTick ();

	}

}

//----------------------------------------------------------------

void ButtonManager::run ()

{

	drawButtons ();
	onTimeTick ();

}