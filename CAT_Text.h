#pragma once
#include <SFML/Graphics.hpp>
#include <string.h>



/*
class TextField : public sf::Transformable, public sf::Drawable{
private:
unsigned int m_size;
sf::Font m_font;
std::string m_text;
sf::RectangleShape m_rect;
bool m_hasfocus;
};
*/


//графические текстовые поля  для ввода информации
class TextField   // : public sf::Transformable, public sf::Drawable
{
public:
	//конструктор
	TextField(unsigned int maxChars) :
		m_size(maxChars),
		m_rect(sf::Vector2f(15 * m_size, 20)),
		m_hasfocus(false)
	    {
		m_font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
		m_rect.setOutlineThickness(2);
		m_rect.setOutlineColor(sf::Color(127, 127, 127));
		//m_rect.setPosition(this->getPosition());
		std::cout << "  Edit text CONSTRUCT  " << std::endl;
	    }

	void setPosition(float x, float  y);
	const std::string getText() const;
	void setFocus(bool focus);
	void handleInput(sf::Event e);
	bool contains(sf::Vector2f point) const;
	void draw(sf::RenderWindow* win);

private:
	unsigned int m_size;
	sf::Font m_font;
	std::string m_text;
	sf::RectangleShape m_rect;
	bool m_hasfocus;
};


