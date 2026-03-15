#include "Universe.h"
#include "Mass.h"

Universe::Universe()
    : m_iIteration(0)
{
}

Universe::Universe(int p_iMasses)
    : m_iIteration(0)
{
    m_masses.resize(static_cast<size_t>(p_iMasses));
}

Universe::Universe(const Universe& src)
    : m_iIteration(src.m_iIteration), m_masses(src.m_masses)
{
}

Universe::~Universe()
{
}

Universe* Universe::GenerateSimulationStep(int p_iStepSize)
{
    Universe* poUniverse = new Universe(static_cast<int>(m_masses.size()));
    poUniverse->m_iIteration = m_iIteration + 1;

    // Copy all masses to the new universe
    poUniverse->m_masses = m_masses;

    // Simulate one step from the current universe
    poUniverse->SimulateFrom(*this, p_iStepSize);

    return poUniverse;
}

void Universe::Randomize()
{
    for (auto& mass : m_masses)
    {
        mass.Randomize();
    }
}

// Simulate gravitational interactions and update positions/velocities
void Universe::SimulateFrom(const Universe& p_roUniverse, int p_iStepSize)
{
    // First, update velocities based on gravitational forces
    // We must do this before position updates
    for (auto& mass : m_masses)
    {
        mass.EffectuerPasChangementVitesse(p_roUniverse, p_iStepSize);
    }

    // Then update positions based on new velocities
    for (auto& mass : m_masses)
    {
        mass.EffectuerPasChangementPosition(p_iStepSize);
    }
}