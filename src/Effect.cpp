#ifndef __EFFECT_INCLUDED__
#define __EFFECT_INCLUDED__

#include <cstdint>
#include <vector>

#pragma region classDefinitions

class EffectBase {
    uint8_t id;
    std::vector<uint8_t> params;

    public:
        const bool operator==(const EffectBase & other) const;
        const bool operator!=(const EffectBase & other) const;
};

#pragma endregion

#pragma region stdInserts

template <>
struct std::hash<EffectBase> {
    size_t operator()(const EffectBase & cell) const noexcept {
        //TODO
        return 0;
    }
};

const bool EffectBase::operator==(const EffectBase & other) const {
    return (
        id == other.id &&
        params == other.params
    );
}

const bool EffectBase::operator!=(const EffectBase & other) const {
    return (
        id != other.id ||
        params != other.params
    );
}

#pragma endregion

#endif // __EFFECT_INCLUDED__