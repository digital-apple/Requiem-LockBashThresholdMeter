#pragma once

class System
{
public:
    enum class THRESHOLD : std::uint32_t
    {
        Novice = 1,
        Apprentice = 2,
        Adept = 3
    };

    enum class COLOR_TYPE : std::uint32_t
    {
        None = 0,
        Ready = 1,
        Waiting = 2,
        Insufficient = 3,
    };

    struct Color
    {
        std::int16_t red;
        std::int16_t green;
        std::int16_t blue;
    };

    struct Layout
    {
        std::uint32_t position_x;
        std::uint32_t position_y;
        std::uint32_t rotation;
        std::uint32_t scale_x;
        std::uint32_t scale_y;
        std::uint32_t alpha;
    };

    static auto GetSingleton() -> System*;

    void ParseJson();

    auto GetColor(RE::LOCK_LEVEL a_level) -> std::uint32_t;
    auto GetColorByType(System::COLOR_TYPE a_type) -> std::uint32_t;
    bool GetCounter();
    auto GetLayout() -> std::array<RE::GFxValue, 6>;
    auto GetLockThreshold(RE::LOCK_LEVEL a_level) -> std::uint32_t;
    auto GetMeterPercentage(RE::LOCK_LEVEL a_level) -> float;
    auto GetPlayerStrength(bool a_max) -> float;
    auto GetPlayerStrengthPercentage(RE::LOCK_LEVEL a_level, bool a_max) -> float;
    auto GetColorType(std::string a_string) -> System::COLOR_TYPE;
    bool ReferenceHasScript(RE::TESObjectREFR* a_reference);
    auto RGBToHex(std::int16_t a_red, std::int16_t a_green, std::int16_t a_blue) -> std::uint32_t;
    auto UpdateGradient(System::Color a_beginning, System::Color a_destination, float a_percentage) -> std::uint32_t;
private:
    System() = default;
    System(const System&) = delete;
    System(System&&) = delete;

    ~System() = default;

    System& operator=(const System&) = delete;
    System& operator=(System&&) = delete;

    std::string script;
    std::atomic_bool counter;
    std::unordered_map<System::THRESHOLD, std::uint32_t> thresholds;
    std::unordered_map<System::COLOR_TYPE, System::Color> colors;
    System::Layout layout;
};