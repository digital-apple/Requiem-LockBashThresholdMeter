#include "System.h"

auto System::GetSingleton() -> System*
{
    static System singleton;
    return std::addressof(singleton);
}

void System::ParseJson()
{
    std::ifstream path("Data/SKSE/Plugins/Requiem - Lock Bash Threshold Meter.json");
    json config = json::parse(path);
    const json& configurationArray = config["Configurations"];

    for (const auto& configuration : configurationArray.array_range()) {
        script = configuration["ScriptName"].as<std::string>();

        thresholds.try_emplace(System::THRESHOLD::Novice, configuration["Thresholds"]["Novice"].as<std::uint32_t>());
        thresholds.try_emplace(System::THRESHOLD::Apprentice, configuration["Thresholds"]["Apprentice"].as<std::uint32_t>());
        thresholds.try_emplace(System::THRESHOLD::Adept, configuration["Thresholds"]["Adept"].as<std::uint32_t>());

        counter = configuration["Counter"].as<std::uint32_t>();

        layout = {
            configuration["Layout"]["PositionX"].as<std::uint32_t>(),
            configuration["Layout"]["PositionY"].as<std::uint32_t>(),
            configuration["Layout"]["Rotation"].as<std::uint32_t>(),
            configuration["Layout"]["ScaleX"].as<std::uint32_t>(),
            configuration["Layout"]["ScaleY"].as<std::uint32_t>(),
            configuration["Layout"]["Alpha"].as<std::uint32_t>() 
        };

        auto& colorArray = configuration["Colors"];

        for (auto& color : colorArray.array_range()) {
            System::Color instance{ color["Red"].as<std::int16_t>(), color["Green"].as<std::int16_t>(), color["Blue"].as<std::int16_t>() };
            colors.try_emplace(System::GetColorType(color["Type"].as<std::string>()), instance);
        }
    }
}

auto System::GetColor(RE::LOCK_LEVEL a_level) -> std::uint32_t
{
    const auto threshold = GetLockThreshold(a_level);

    if (threshold > 0) {
        if (GetPlayerStrength(false) >= threshold) {
            auto RGB = colors[System::COLOR_TYPE::Ready];
            return RGBToHex(RGB.red, RGB.green, RGB.blue);
        } else if (GetPlayerStrength(true) < threshold) {
            auto RGB = colors[System::COLOR_TYPE::Insufficient];
            return RGBToHex(RGB.red, RGB.green, RGB.blue);
        } else {
            return UpdateGradient(colors[System::COLOR_TYPE::Waiting], colors[System::COLOR_TYPE::Ready], GetPlayerStrengthPercentage(a_level, false));
        }
    }
    return 0xFFFFFF;
}

auto System::GetColorByType(System::COLOR_TYPE a_type) -> std::uint32_t
{
    return RGBToHex(colors[a_type].red, colors[a_type].green, colors[a_type].blue);
}

auto System::GetLayout() -> std::array<RE::GFxValue, 6>
{
    return std::array<RE::GFxValue, 6>{ 
        layout.position_x,
        layout.position_y,
        layout.rotation,
        layout.scale_x,
        layout.scale_y,
        layout.alpha
    };
}

auto System::GetLockThreshold(RE::LOCK_LEVEL a_level) -> std::uint32_t
{
    switch (a_level) {
    case RE::LOCK_LEVEL::kVeryEasy:
        return static_cast<std::uint32_t>(thresholds[System::THRESHOLD::Novice]);
        break;
    case RE::LOCK_LEVEL::kEasy:
        return static_cast<std::uint32_t>(thresholds[System::THRESHOLD::Apprentice]);
        break;
    case RE::LOCK_LEVEL::kAverage:
        return static_cast<std::uint32_t>(thresholds[System::THRESHOLD::Adept]);
        break;
    default:
        return 0;
    }
}

auto System::GetMeterPercentage(RE::LOCK_LEVEL a_level) -> float
{
    const auto threshold = GetLockThreshold(a_level);

    if (threshold > 0) {
        if (GetPlayerStrength(true) < threshold) {
            return GetPlayerStrengthPercentage(a_level, true);
        } else {
            return 100.0F;
        }
    }

    return 0.0F;
}

auto System::GetPlayerStrength(bool a_max) -> float
{
    const auto player = RE::PlayerCharacter::GetSingleton();

    if (player) {
        const auto actorValueOwner = player->AsActorValueOwner();
        if (a_max) {
            return 2 * actorValueOwner->GetPermanentActorValue(RE::ActorValue::kHealth) + actorValueOwner->GetPermanentActorValue(RE::ActorValue::kStamina);
        }
        return 2 * actorValueOwner->GetActorValue(RE::ActorValue::kHealth) + actorValueOwner->GetActorValue(RE::ActorValue::kStamina);
    }
    return 0;
}

auto System::GetPlayerStrengthPercentage(RE::LOCK_LEVEL a_level, bool a_max) -> float
{
    return GetPlayerStrength(a_max) / GetLockThreshold(a_level) * 100.0F;
}

auto System::GetColorType(std::string a_string) -> System::COLOR_TYPE
{
    std::transform(std::begin(a_string), std::end(a_string), std::begin(a_string), [](unsigned char c)->unsigned char { return static_cast<unsigned char>(std::tolower(c)); });

    if (a_string == "ready") {
        return System::COLOR_TYPE::Ready;
    } else if (a_string == "waiting") {
        return System::COLOR_TYPE::Waiting;
    } else if (a_string == "insufficient") {
        return System::COLOR_TYPE::Insufficient;
    } else {
        return System::COLOR_TYPE::None;
    }
}

bool System::GetCounter()
{
    return counter;
}

bool System::ReferenceHasScript(RE::TESObjectREFR* a_reference)
{
    logs::info("System::ReferenceHasScript :: Looking for script: '{}' on reference: '{}' | '0x{:x}'", script, a_reference->GetName(), a_reference->GetFormID());

    using VM = RE::BSScript::Internal::VirtualMachine;
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;

	auto vm = VM::GetSingleton();
	auto policy = vm->GetObjectHandlePolicy();
	RE::VMHandle handle = policy->GetHandleForObject(a_reference->GetFormType(), a_reference);
    std::string name = script;
	ObjectPtr object = nullptr;

	return vm->FindBoundObject(handle, name.c_str(), object);
}

auto System::RGBToHex(std::int16_t a_red, std::int16_t a_green, std::int16_t a_blue) -> std::uint32_t
{
    return ((a_red & 0xFF) << 16) + ((a_green & 0xFF) << 8) + (a_blue & 0xFF);
}

auto System::UpdateGradient(System::Color a_beginning, System::Color a_destination, float a_percentage) -> std::uint32_t
{
    const auto red = std::roundf((a_destination.red - a_beginning.red) * static_cast<float>(std::pow((a_percentage / 100.0F), 6.0)));
    const auto green = std::roundf((a_destination.green - a_beginning.green) * static_cast<float>(std::pow((a_percentage / 100.0F), 6.0)));
    const auto blue = std::roundf((a_destination.blue - a_beginning.blue) * static_cast<float>(std::pow((a_percentage / 100.0F), 6.0)));

    return RGBToHex(static_cast<std::uint16_t>(a_beginning.red + red), static_cast<std::uint16_t>(a_beginning.green + green), static_cast<std::uint16_t>(a_beginning.blue + blue));
}