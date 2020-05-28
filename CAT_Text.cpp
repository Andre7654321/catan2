#include <iostream>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <string.h>
#include <cstdlib>

#include "CAT_Text.h"

//прототип из сетевого раздела
void Sent_Chart_Mess(std::string text);



//=====================================================================
const std::string TextField::getText() const
{
	return m_text;
}

//и переместить его, поместив внутри окна ????
void TextField::setPosition(float x, float  y)
{
	//sf::Transformable::setPosition(x, y);
	m_rect.setPosition(x, y);
}

bool TextField::contains(sf::Vector2f point) const 
    {
	//std::cout << "  test x  " << point.x << std::endl;
	return m_rect.getGlobalBounds().contains(point);
    }

//Установить(или отключить) фокус на TextField :

void TextField::setFocus(bool focus) 
  {
	m_hasfocus = focus;
	if (focus) 
	    {
		m_rect.setOutlineColor(sf::Color::Blue);
	    }
	else {
		  m_rect.setOutlineColor(sf::Color(127, 127, 127)); // Gray color
	     }
}

void TextField::draw(sf::RenderWindow* win)
   {
	win->draw(m_rect);
	sf::Text text("", m_font, 16);
	text.setFillColor(sf::Color::Black);
	text.setString(m_text);
	auto coord = m_rect.getPosition();
	text.setPosition(coord.x + 5, coord.y);

	win->draw(text);
   }

// наш TextField должен вести себя как - то, когда ввод(ака SFML - событие получено :
void TextField::handleInput(sf::Event e) {
	if (!m_hasfocus || e.type != sf::Event::TextEntered)
		return;

	if (e.text.unicode == 8)   // Delete key BACKSPACE
	    {
		m_text = m_text.substr(0, m_text.size() - 1);
		return;
	    }

	if (e.text.unicode == 13)   // ENTER
	    {
		Sent_Chart_Mess(m_text);
		m_text.clear();
		return;
	    }

	if (m_text.size() < m_size) 
	    {
		//std::cout << "  keycode =   " << e.text.unicode << std::endl;
		m_text += e.text.unicode;
	    }
}
