from deap import base, creator, tools
import random
import sys
import pipeline
import json


creator.create("Fitness", base.Fitness, weights=(1.0, 1.0))
creator.create("Individual", list, fitness=creator.Fitness)
toolbox = base.Toolbox()

def generate_netgen_roads(population):
    pipeline.parameter_values = population
    pipeline.main()

def initial_population(number_individuals):
    #register has one argument as an alias and sucessive argument lists are assigned to the preceding argument
    toolbox.register("random_turnlanes", random.randint, 1, 2)
    toolbox.register("random_incoming", random.randint, 2, 6)
    toolbox.register("random_number_of_lanes", random.randrange, 2, 8, 2)
    toolbox.register("individual", tools.initCycle, creator.Individual,
                 (toolbox.random_turnlanes, toolbox.random_incoming, toolbox.random_number_of_lanes), n=1)
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)

    #Create a population
    population = toolbox.population(n=number_individuals)
    generate_netgen_roads(population)
    return population

def createAndWriteInitialPopulation():
    population = initial_population(3)
    population_data = {}
    population_data["initial_population"] = population

    with open("Data/populationData.json", "w") as file:
        json.dump(population_data, file)



def selection():

    #Create placeholder population
    toolbox.register("random_number", random.randint, 20, 25)
    toolbox.register("individual", tools.initCycle, creator.Individual,
                 (toolbox.random_number, toolbox.random_number, toolbox.random_number), n=1)

    toolbox.register("population", tools.initRepeat, list, toolbox.individual )
    
    placeholder_population = toolbox.population(n=3)
    #print(placeholder_population)
    
    #Read some evaluation data and plug into placeholder population data
    with open('Data/populationData.json') as f1:
        file1 = json.load(f1)

    with open('Data/intersectionScores.json') as f2:
        file2 = json.load(f2)
    i=0
    population = file1["population"]
    print(population)
    for ind in placeholder_population:
        ind[0] = population[i][0]
        ind[1] = population[i][1]
        ind[2] = population[i][2]
        ind.fitness.values = (float(file2["intersection" + str(i)]["forking_points"]), 0)
        i += 1
        print("fitness values are"+str(ind.fitness))

    print("-=-=-=-=-=-=-=-=-=-=-=-= Individuals have been selected -=-=-=-=-=-=-=-=-=-=-=-=-")
    selected = tools.selBest(placeholder_population, 2)
    print("selected individuals are"+str(selected))

    #check for termination condition


    #Duplicate to create offsprings
    offspring1, offspring2 = [toolbox.clone(ind) for ind in selected]

    #One point crossover
    crossover_one, crossover_two = tools.cxOnePoint(offspring1, offspring2)
    print("Crossover results are"+str(crossover_one)+"and"+str(crossover_two))
    del offspring1.fitness.values
    del offspring2.fitness.values

    #Mutate the crossover results
    lower_bounds = [1, 2, 2]
    upper_bounds = [3, 6, 8]
    mutated01 = tools.mutUniformInt(crossover_one, lower_bounds, upper_bounds, 0.05)
    mutated02 = tools.mutUniformInt(crossover_two, lower_bounds, upper_bounds, 0.05)
    print("Mutated results are"+str(mutated01)+"and"+str(mutated02))

    new_generation = {
        "generation_number" : 0, 
        "child01" : mutated01,
        "child02" : mutated02
        }
    
    with open('Data/populationData.json') as f1:
        file1 = json.load(f1)
        file1.update(new_generation)
        with open("Data/populationData.json", "w") as file:
            json.dump(file1, file)
            print("written")

    