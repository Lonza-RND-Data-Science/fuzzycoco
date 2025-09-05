#include "evolution_engine.h"
#include <algorithm>
#include "logging_logger.h"

using namespace fuzzy_coco;
using namespace logging;

double EvolutionFitnessMethod::globalFitness(const vector<double>& fitnesses) {
    return *max_element(fitnesses.begin(), fitnesses.end());
}

EvolutionEngine::EvolutionEngine(const EvolutionParams& params, RandomGenerator& rng) :
        _params(params),
        _crossover_method(rng, params.cx_prob),
        _mutation_method(rng, params.mut_flip_genome, params.mut_flip_bit),
        _elite_selection_method(rng),
        _individuals_selection_method(rng)
{}

pair<Generation, vector<double>> EvolutionEngine::evolve(const Genomes& genomes, EvolutionFitnessMethod& fitness_method, 
    int nb_generations, double maxFit) 
{
  Generation generation(genomes, params().elite_size);
  updateGeneration(generation, fitness_method);

  vector<double> generation_fitnesses;
  generation_fitnesses.reserve(nb_generations);

  for (int i = 0; i < nb_generations; i++) {
      generation = nextGeneration(generation, fitness_method);
      generation_fitnesses.push_back(generation.fitness);
      if (generation.fitness >= maxFit) break; // early return if we reach the max fitness
  }
  return make_pair(generation, generation_fitnesses);
}

// compute fitness for all individuals and select elite based on that fitnesses
// after that --> generation is up-to-date
void EvolutionEngine::updateGeneration(Generation& generation, EvolutionFitnessMethod& fitness_method)
{
    const auto& genos = generation.individuals;
    const int nb = genos.size();
    generation.fitnesses.resize(nb);

    for (int k = 0; k < nb; k++) generation.fitnesses[k] = fitness_method.fitness(genos[k]);

    generation.fitness = fitness_method.globalFitness(generation.fitnesses);
    generation.elite = selectElite(genos, generation.fitnesses);
}

// performs selection and reproduction
Generation EvolutionEngine::nextGeneration(const Generation& generation, EvolutionFitnessMethod& fitness_method)
{   
    const auto& genomes = generation.individuals;
    const size_t nb = genomes.size();
    const size_t nb_elite = _params.elite_size;
    const size_t nb_evolvers = nb - nb_elite;

    assert(generation.elite.size() == nb_elite);
    assert(nb_elite < nb);
    assert(nb_evolvers <= nb);

    Genomes evolvers = selectEvolvers(nb_evolvers, generation.individuals, generation.fitnesses);

    // N.B: modify in-place
    _crossover_method.reproduceAllPairsOf(evolvers);
    _mutation_method.mutate(evolvers);

    // new generation is elite + evolved
    Genomes indiv;
    const auto& elite = generation.elite;
    indiv.reserve(elite.size() + evolvers.size());
    indiv.insert(indiv.end(), elite.begin(), elite.end());
    indiv.insert(indiv.end(), evolvers.begin(), evolvers.end());

    Generation newgen(indiv, _params.elite_size);
    updateGeneration(newgen, fitness_method);
    return newgen;
}
 
// Genomes EvolutionEngine::selectElite(const Genomes& genomes)
// {
//     const int nb_elite = _params.elite_size;
//     assert(nb_elite < genomes.size());

//     Genomes elite(genomes.begin(), genomes.begin() + nb_elite);
//     return elite;
// }

Genomes EvolutionEngine::selectElite(const Genomes& genomes, const vector<double>& fitnesses)
{

  const int nb_elite = _params.elite_size;
  vector<int> indexes; // TODO: put in instance state
  indexes.reserve(nb_elite);
  _elite_selection_method.selectEntities(nb_elite, fitnesses, indexes);
  Genomes elite;
  elite.reserve(nb_elite);
  for (int idx : indexes) elite.push_back(genomes[idx]);
  return elite;
}

Genomes EvolutionEngine::selectEvolvers(int nb, const Genomes& genomes, const vector<double>& fitnesses)
{
    vector<int> indexes; // TODO: put in instance state 
    indexes.reserve(nb);
    indexes.clear();
    _individuals_selection_method.selectEntities(nb, fitnesses, indexes);
    Genomes evolvers;
    evolvers.reserve(nb);
    for (int idx : indexes) evolvers.push_back(genomes[idx]);
    return evolvers;
}

// Genomes EvolutionEngine::selectBest(const Generation& generation) {
//     const int nb = generation.individuals.size();
//     const auto& fitnesses = generation.fitnesses;
//     assert(fitnesses.size() == nb);

//     vector<int> idx(nb);
//     for (int i = 0; i < nb; i++) idx[i] = i;
//     // sort the fitness indexes in decreasing order
//     sort(idx.begin(), idx.end(), [&](int a, int b) { return fitnesses[a] > fitnesses[b]; });

//     double best_fitness = fitnesses[idx.front()];
//     Genomes best;
//     best.reserve(nb);
//     for (int i = 0; i < nb; i++) {
//         if (fitnesses[idx[i]] < best_fitness) break;
//         best.push_back(generation.individuals[idx[i]]);
//     }

//     return best;
// }


// bool EvolutionEngine::computeNextGeneration()
// {
//     _current_generation++;
//     // Select elites from the population
//     // copy selected PopEntity instances from the Population->entityList using this->eliteselection() in this->selectedEntitiesCopy. this->population is unchanged
//     // ==> this->selectedEntitiesCopy
//     selectElites();

//     // Set cooperators (representatives)
//     // delete all PopEntity from this->population->representatives, and copy all instances from this->selectedEntitiesCopy into this->population->representatives
//     // ==>  this->population (->representatives)
//     _population->setRepresentativesCopy(_selectedEntitiesCopy, _nb_representatives);

//     // Select non elite individuals
//     // copy selected PopEntity instances from this->individualsSelection in this->evolvingEntitiesCopy
//     //  ==>  this->evolvingEntitiesCopy
//     selectIndividuals();

//     // Crossover
//     // modify PEs in this->evolvingEntitiesCopy in-place
//     // ==>  this->evolvingEntitiesCopy
//     crossover();

//     // Mutate
//     // modify PEs in this->evolvingEntitiesCopy in-place
//     // ==>  this->evolvingEntitiesCopy
//     mutate();
//     // replace (and delete instances) this->population->entityList by this->evolvingEntitiesCopy
//     // also add this->selectedEntitiesCopy to this->population->entityList
//     // ==> this->population->entityList
//     _population->replace(_selectedEntitiesCopy, _evolvingEntitiesCopy);

//     // Evaluate population
//     bool eval = evaluatePopulation(_population, _current_generation);

//     return eval;
// }
// main evolution loop
// void EvolutionEngine::startEvolution(int generationCount)
// {
//     // TODO: crashes here when run under Qt 5 (but not under Qt 4) because leftLock and rightLock are invalid

//     // set the selection methods
//     // setEntitySelector(eliteSelection,eliteSelectionCount,individualsSelection,individualsSelectionCount);

//     // this->mutateMethod = mutateMethod;
//     // this->crossoverMethod = crossoverMethod;

//     // karl: why do we need to this now since it should be performed at the first iteration?
//     // maybe in case the computation is stopped ?
//     // calls the elite selection and stores it in selectedEntitiesCopy
//     selectElites();
//     // sets the elite as the new representatives of the population
//     _population->setRepresentativesCopy(selectedEntitiesCopy, _nb_representatives);

//     // waitOtherThread(access, standby);

//     // initial evaluation ?
//     if(!evaluatePopulation(_population, 0))
//         return;

//     // loop over all possible generations 
//     _current_generation = 0;
//     for(quint32 i = 1; i <= generationCount; i++)
//     {
//         if (!computeNextGeneration()) {
//             return false;
//         }
//     }
    

//     return evaluatePopulation(_population, 0);
// }

// void EvolutionEngine::setEntitySelector(EntitySelection *eliteSelection, quint32 eliteSelectionCount, EntitySelection *entitySelection, quint32 selectionCount)
// {
//     this->eliteSelection = eliteSelection;
//     this->eliteSelectionCount = eliteSelectionCount;
//     this->individualsSelection = entitySelection;
//     this->individualsSelectionCount = selectionCount;
// }
// void EvolutionEngine::setMutationMethod(Mutate * mutateMethod, quint32 mutationProbability)
// {
//     this->mutateMethod = mutateMethod;
//     this->mutationProbability = mutationProbability;
// }
// void EvolutionEngine::setCrossoverMethod(Crossover * crossoverMethod)
// {
//     this->crossoverMethod = crossoverMethod;
// }


// vector<EntitySelection *> EvolutionEngine::getEntitySelectors()
// {
//     vector<EntitySelection *> availableSelection;
//     for(quint32 i=0; i < entitySelectionMethodList.size(); i++)
//         availableSelection.push_back(entitySelectionMethodList.at(i));
//     return availableSelection;
// }
// vector<Mutate *> EvolutionEngine::getMutationMethods()
// {
//     vector<Mutate *> availableMutation;
//     for(quint32 i=0; i < mutateMethodList.size(); i++)
//         availableMutation.push_back(mutateMethodList.at(i));
//     return availableMutation;
// }
// vector<Crossover *> EvolutionEngine::getCrossoverMethods()
// {
//     vector<Crossover *> availableCrossover;
//     for(quint32 i=0; i < crossoverMethodList.size(); i++)
//         availableCrossover.push_back(crossoverMethodList.at(i));
//     return availableCrossover;
// }

// Private :
// void EvolutionEngine::initializePopulation()
// {
//     _population->randomizePopulation();
// }


// bool EvolutionEngine::isElite(Genome *genome)
// {
//     for(quint32 i=0; i < selectedEntitiesCopy.size(); i++){
//         if(genome->getData()->operator ==(*selectedEntitiesCopy.at(i)->getGenome()->getData()))
//             return true;
//     }
//     return false;
// }

// void EvolutionEngine::selectIndividuals()
// {
// //    for(uint i = 0; i < evolvingEntitiesCopy.size(); i++)
// //        delete evolvingEntitiesCopy[i];
//     evolvingEntitiesCopy.clear();
//     evolvingEntitiesCopy = _population->getSomeEntityCopy(individualsSelection,individualsSelectionCount);
// }
// void EvolutionEngine::crossover()
// {
//     // Missing probability
//     // Random list before crossover (To do if better results and not too processing time consuming)
//     crossoverMethod->reproducePairOf(evolvingEntitiesCopy, crossoverProbability);
// }
// void EvolutionEngine::mutate()
// {

//     vector<PopEntity *>::iterator it;
//     for(it=evolvingEntitiesCopy.begin(); it!=evolvingEntitiesCopy.end(); it++)
//     {
//         qreal entitiyLuck = randomGenerator.randomReal(0,1);
//         //  if goal < chance OK.
//         if(entitiyLuck < mutationProbability){
//             mutateMethod->mutateEntity(*it, mutationPerBitProbability);
//         }
//     }
// }

