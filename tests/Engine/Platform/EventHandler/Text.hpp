#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include <Nazara/Graphics/TextSprite.hpp>
#include <NDK/Entity.hpp>
#include "StateContext.hpp"

class Text
{
	public:
		Text(StateContext& context);
		~Text();

		void SetContent(const Nz::String& string, unsigned int size = 36);
		void SetVisible(bool shouldBeVisible = true);

	private:
		StateContext& m_context;
		Ndk::EntityHandle m_text;
		Nz::TextSpriteRef m_textSprite;
};

#endif // __TEXT_HPP__