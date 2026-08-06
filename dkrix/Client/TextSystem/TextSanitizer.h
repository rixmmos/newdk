#ifndef __TEXT_SANITIZER_H__
#define __TEXT_SANITIZER_H__

#include <ctype.h>
#include <string>

#include "TextService.h"

namespace TextSystem {

inline bool LooksLikePlaceholderNoise(const std::string& text)
{
	int visible = 0;
	int alphaNum = 0;
	int question = 0;

	for (size_t i = 0; i < text.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(text[i]);
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;

		++visible;
		if (isalnum(c))
			++alphaNum;
		if (c == '?')
			++question;
	}

	if (visible == 0)
		return false;

	if (alphaNum == 0 && question > 0)
		return true;

	return question > 0 && question >= alphaNum && question * 3 >= visible;
}

inline std::string NormalizeLatinOrFallback(const char* text, const char* fallback)
{
	std::string normalized = TextService::NormalizeText(text != NULL ? text : "");
	if (normalized.empty() || LooksLikePlaceholderNoise(normalized))
		return fallback != NULL ? fallback : "";
	return normalized;
}

inline const char* GetNpcEnglishName(int npcID)
{
	switch (npcID)
	{
	case 13: return "Singh";
	case 14: return "Yowa";
	case 15: return "Jan";
	case 16: return "Jack";
	case 17: return "Kaiser";
	case 18: return "Cain";
	case 20: return "Kevin";
	case 21: return "Chris";
	case 22: return "Christine";
	case 23: return "Clive";
	case 24: return "Han";
	case 25: return "Hannibal";
	case 26: return "Tanya";
	case 27: return "David";
	case 92: return "Bricolacas";
	case 93: return "Kapatini";
	case 94: return "Valerie";
	case 95: return "Gruber";
	case 96: return "Griffin";
	case 97: return "Sandra";
	case 98: return "Salamander";
	case 99: return "Sonya";
	case 100: return "Yoshihara";
	case 101: return "Johan";
	case 102: return "Crow";
	case 103: return "Smith";
	case 108: return "Smirovich";
	case 109: return "Zack";
	case 158: return "Frederick";
	case 250: return "Matthew";
	case 302: return "Terry";
	case 303: return "Nancy";
	case 427: return "Griffin";
	case 428: return "Robin";
	case 429: return "Murphy";
	case 430: return "Carmen";
	default: return NULL;
	}
}

inline const char* GetNpcEnglishDescription(int npcID)
{
	switch (npcID)
	{
	case 13: return "Healing, potions and ammunition";
	case 14: return "Blade skills and blades";
	case 15: return "Gun skills and rifles";
	case 20: return "Storage";
	case 22: return "Storage";
	case 23: return "Accessories";
	case 24: return "Sword skills and swords";
	case 25: return "Enchant skills and maces";
	case 27: return "Repair";
	case 93: return "Transformation items, serum, storage and quests";
	case 94: return "Coats";
	case 95: return "Accessories";
	case 96: return "Repair";
	case 98: return "Healing, potions and ammunition";
	case 100: return "Shoes and gloves";
	case 101: return "Healing, potions and ammunition";
	case 102: return "Shoes and gloves";
	case 103: return "Alignment recovery";
	case 108: return "Repair and silver coating";
	case 16: return "Motorcycle dealer";
	case 17: return "Sword and blade trainer";
	case 26: return "Heal and enchant trainer";
	case 109: return "Gun trainer and starter weapons";
	case 158: return "Advancement weapon exchange";
	case 250: return "Travel and battle zones";
	case 302: return "Item trader";
	case 303: return "Item trader";
	case 427: return "Blood serum, supplies, storage and repair";
	case 428: return "Accessories, storage and repair";
	case 429: return "Item trader and castle travel";
	case 430: return "Healing, supplies and battle zone travel";
	default: return NULL;
	}
}

inline std::string NormalizeNpcNameOrFallback(const char* text, int npcID)
{
	const char* englishName = GetNpcEnglishName(npcID);
	if (englishName != NULL)
		return englishName;
	return NormalizeLatinOrFallback(text, "NPC");
}

inline std::string NormalizeNpcDescriptionOrFallback(const char* text, int npcID)
{
	const char* englishDescription = GetNpcEnglishDescription(npcID);
	if (englishDescription != NULL)
		return englishDescription;
	return NormalizeLatinOrFallback(text, "NPC");
}

inline const char* GetNpcSubjectEnglish(int scriptID)
{
	switch (scriptID)
	{
	case 8000:
		return "Welcome. What can I help you with?";
	case 8010:
		return "The free battle zones are Oltenia for same-race duels and Banatune for open duels. Choose your destination.";
	case 8100:
		return "How did you get here? If you followed that road, you must be capable. Dangerous creatures gather out there. What do you need?";
	case 8200:
		return "Heh heh. What brings you all the way here?";
	case 8210:
		return "The free battle zones are Oltenia for same-race duels and Banatune for open duels. Choose your destination.";
	case 8300:
		return "Hello. How did you get here? Be careful on that dreadful road where slayers come from. What brings you here?";
	case 8311:
		return "Thank you for your generosity. Your donation will be recorded and used where it is needed most.";
	case 8312:
		return "You do not seem to be a formal clan member right now. Please participate as an individual instead.";
	case 8313:
		return "Thank you. Your donation will be recorded under your clan name.";
	case 15000:
		return "Put away that weapons permit. I have no way to verify it anyway. What business do you have with me?";
	case 15001:
		return "I cannot explain every detail, but advancement lets a warrior break past old limits and learn stronger techniques.";
	default:
		return NULL;
	}
}

inline const char* GetNpcContentEnglish(int scriptID, int contentID)
{
	switch (scriptID)
	{
	case 8000:
		switch (contentID)
		{
		case 0: return "I am hurt. Please heal me.";
		case 1: return "Show me what you have for sale.";
		case 2: return "I would like to sell some items.";
		case 3: return "Send me to the field command post.";
		case 4: return "Send me to the free battle zone.";
		case 5: return "Set this as my resurrection point.";
		case 6: return "I want to borrow a Blood Bible sign.";
		case 7: return "I want to return a Blood Bible sign.";
		case 8: return "Nothing right now.";
		default: break;
		}
		break;
	case 8010:
	case 8210:
		switch (contentID)
		{
		case 0: return "Go to Oltenia.";
		case 1: return "Go to Banatune.";
		case 2: return "I will decide later.";
		default: break;
		}
		break;
	case 8100:
		switch (contentID)
		{
		case 0: return "I want to rent storage.";
		case 1: return "Show me my stored items.";
		case 2: return "Show me your accessories.";
		case 3: return "Please repair this.";
		case 4: return "I want to retrieve my motorcycle.";
		case 5: return "Open pet storage.";
		case 6: return "Never mind.";
		default: break;
		}
		break;
	case 8200:
		switch (contentID)
		{
		case 0: return "Show me your goods.";
		case 1: return "I would like to sell items.";
		case 2: return "Send me inside Limbo Castle.";
		case 3: return "Send me inside Rasen Castle.";
		case 4: return "Send me to the free battle zone.";
		case 5: return "Set this as my resurrection point.";
		case 6: return "I was just passing by.";
		default: break;
		}
		break;
	case 8300:
		switch (contentID)
		{
		case 0: return "I want to buy blood serums and supplies.";
		case 1: return "I want to rent storage.";
		case 2: return "Show me my stored items.";
		case 3: return "Please repair this.";
		case 4: return "Open pet storage.";
		case 5: return "I want to borrow a Blood Bible sign.";
		case 6: return "I want to return a Blood Bible sign.";
		case 7: return "I was just passing by.";
		default: break;
		}
		break;
	case 8311:
	case 8313:
		switch (contentID)
		{
		case 0: return "Make a donation.";
		case 1: return "Maybe next time.";
		default: break;
		}
		break;
	case 8312:
		switch (contentID)
		{
		case 0: return "Make a personal donation.";
		case 1: return "Maybe next time.";
		default: break;
		}
		break;
	case 15000:
		switch (contentID)
		{
		case 0: return "What is advancement?";
		case 1: return "I heard you exchange advancement items.";
		case 2: return "I will come back later.";
		default: break;
		}
		break;
	case 15001:
		switch (contentID)
		{
		case 0: return "I understand.";
		default: break;
		}
		break;
	default:
		break;
	}

	return NULL;
}

inline std::string NormalizeNpcSubjectOrFallback(int scriptID, const char* text, const char* fallback)
{
	std::string normalized = NormalizeLatinOrFallback(text, fallback);
	if (fallback != NULL && normalized == fallback)
	{
		const char* english = GetNpcSubjectEnglish(scriptID);
		if (english != NULL)
			return english;
	}
	return normalized;
}

inline std::string NormalizeNpcContentOrFallback(int scriptID, int contentID, const char* text, const char* fallback)
{
	std::string normalized = NormalizeLatinOrFallback(text, fallback);
	if (fallback != NULL && normalized == fallback)
	{
		const char* english = GetNpcContentEnglish(scriptID, contentID);
		if (english != NULL)
			return english;
	}
	return normalized;
}

}

#endif
