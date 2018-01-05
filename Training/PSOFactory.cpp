#include "PSOFactory.h"


PSOFactory* PSOFactory::INSTANCE = nullptr;


PSOFactory* PSOFactory::GetInstance() {
    if (!INSTANCE) {
        INSTANCE = new PSOFactory();
    }

    return INSTANCE;
}

PSO * PSOFactory::CreatePSO(PSO_FLAGS a_flag)
{
    PSO* output = m_psoMap[a_flag];
    if (!output) {
        output = new PSO();
        m_psoMap[a_flag] =  output;
    }
    return output;
}

PSOFactory::PSOFactory()
{
}


PSOFactory::~PSOFactory()
{
}
