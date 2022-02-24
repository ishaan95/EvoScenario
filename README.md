# EvoScenario
Procedurally generating scenarios on [Cruzway](https://github.com/AugmentedDesignLab/CruzWay) using evolutionary algorithms. 

## Dependencies

This project uses [Unreal Engine python programming utilities](https://docs.unrealengine.com/en-US/ProductionPipelines/ScriptingAndAutomation/Python/index.html) and [DEAP](https://deap.readthedocs.io/en/master/) evolutionary algorithms library. Further, there is an assumption that [IntGen](https://github.com/AugmentedDesignLab/intgen) and our version of [Netgenerate](https://github.com/AugmentedDesignLab/sumo-mirror/tree/sumoIntgen) is able to be run on the command line.

## Changes made to Cruzway
This is the Unreal Engine project used for the [Sumo2Unreal importer](https://github.com/AugmentedDesignLab/Sumo2Unreal) which is contained in Cruzway. The most significant change is to the [ParseXML.cpp file in the ParseXML plugin](https://github.com/ishaan95/EvoScenario/blob/main/Plugins/ParseXML/Source/ParseXML/Private/ParseXML.cpp). This file runs 2 python scripts contained in Content/Python folder (it's assumed we have the Unreal Engine python plugins installed). The python plugins contain the evolutionary algorithm code. 
