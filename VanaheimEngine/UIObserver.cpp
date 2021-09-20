#include "pch.h"
#include "UIObserver.h"

#include "Locator.h"
#include "GeneratorManager.h"
#include "NoiseGenerator.h"
#include "ProceduralLandscapeGenerator.h"

void UIObserver::onNotify(ObserverEvent /*event*/)
{
	/*GeneratorManager* pGenManager{ Locator::GetGeneratorManagerService() };
	NoiseGenerator* pNoiseGen{ pGenManager->GetGenerator<NoiseGenerator>() };
	ProceduralLandscapeGenerator* pProcGen{ pGenManager->GetGenerator<ProceduralLandscapeGenerator>() };
	if (event == ObserverEvent::INCREASE_LACUNARITY)
	{
		pNoiseGen->IncreaseLacunarity();
	}
	else if (event == ObserverEvent::DECREASE_LACUNARITY)
	{
		pNoiseGen->DecreaseLacunarity();
	}
	else if (event == ObserverEvent::INCREASE_LACUNARITY)
	{
		pNoiseGen->IncreaseLacunarity();
	}
	else if (event == ObserverEvent::DECREASE_LACUNARITY)
	{
		pNoiseGen->DecreaseLacunarity();
	}*/
}