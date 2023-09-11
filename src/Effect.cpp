#ifndef __EFFECT_INCLUDED__
#define __EFFECT_INCLUDED__

#include <vector>

#pragma region classDefinitions

class EffectBase {
    uint8_t id;
    std::vector<uint8_t> params;
};

#pragma endregion


#endif // __EFFECT_INCLUDED__