#include "Gameplay/ArmyHandler.hpp"


using namespace Cookie::Gameplay;


void ArmyHandler::AddElementToArmy(ECS::ComponentGameplay* element)
{
	for (int i = 0; i < livingArmies; ++i)
		if (armies[i].name == element->teamName)
		{
			AddElementToArmy(armies[i], element);
			return;
		}

	assert(livingArmies < MAX_ARMIES && "Too many armies in existence.");

	Army& newArmy = armies[livingArmies];
	newArmy.name = element->teamName;
	AddElementToArmy(newArmy, element);
	livingArmies++;
}
void ArmyHandler::AddElementToArmy(Army& army, ECS::ComponentGameplay* element)
{
	switch (element->type)
	{
	case E_ARMY_TYPE::E_WORKER:
		element->componentWorker.income = &(army.income);
		army.workers.push_back(element);
		break;

	case E_ARMY_TYPE::E_UNIT:
		army.units.push_back(element);
		break;

	case E_ARMY_TYPE::E_BUILDING:
		element->componentProducer.income = &(army.income);
		army.buildings.push_back(element);
		break;

	default:
		break;
	}
}

void ArmyHandler::RemoveElementFromArmy(ECS::ComponentGameplay* element)
{
	for (int i = 0; i < livingArmies; ++i)
		if (armies[i].name == element->teamName)
		{
			RemoveElementFromArmy(armies[i], element);
			return;
		}
}
void ArmyHandler::RemoveElementFromArmy(Army& army, ECS::ComponentGameplay* element)
{
	switch (element->type)
	{
	case E_ARMY_TYPE::E_WORKER:
		RemoveElementFromVector(army.workers, element);
		break;

	case E_ARMY_TYPE::E_UNIT:
		RemoveElementFromVector(army.units, element);
		break;

	case E_ARMY_TYPE::E_BUILDING:
		RemoveElementFromVector(army.buildings, element);
		break;

	default:
		break;
	}
}
void ArmyHandler::RemoveElementFromVector(std::vector<ECS::ComponentGameplay*>& vector, ECS::ComponentGameplay* element)
{
	for (int i = 0; i < vector.size(); ++i)
		if (vector[i] == element)
		{
			vector.erase(vector.begin() + i);
			return;
		}
}