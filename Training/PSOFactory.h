#pragma once
#include <unordered_map>
#include "d3dx12.h"
#include "PSO.h"


typedef
enum PSO_FLAGS
{
    PSO_FLAG_NONE = 0,
    PSO_FLAG_TEXTURE = 0x1,
    PSO_FLAGS_FULLCOLOR = 0x2,
} 	PSO_FLAGS;

DEFINE_ENUM_FLAG_OPERATORS(PSO_FLAGS);



class PSOFactory
{
private:
    PSOFactory();
    static PSOFactory* INSTANCE;

public:


    static PSOFactory * GetInstance();

    std::unordered_map<PSO_FLAGS, PSO*> m_psoMap;


    PSO* CreatePSO(PSO_FLAGS a_flag);

    ~PSOFactory();
};

