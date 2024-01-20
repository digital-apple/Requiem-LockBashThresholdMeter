#include "BashMenu.h"
#include "System.h"

BashMenu::BashMenu()
{
    logs::info("BashMenu :: Creation started!");
    auto menu = static_cast<Super*>(this);

    const auto scaleform = RE::BSScaleformManager::GetSingleton();

    menu->inputContext = Context::kNone;
    menu->depthPriority = 0;

    menu->menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
    menu->menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
    menu->menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);

    scaleform->LoadMovieEx(menu, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
        a_def->SetState(RE::GFxState::StateType::kLog,
            RE::make_gptr<Logger>().get());
    });

    logs::info("BashMenu :: Creation finished!");

    if (menu && menu->uiMovie) {
        logs::info("BashMenu :: UI movie detected. Getting movie clip root...");

        RE::GFxValue root;
        menu->uiMovie->GetVariable(&root, "Menu");
        _root = root;
    }
}

void BashMenu::Register()
{
    const auto ui = RE::UI::GetSingleton();

    if (ui) {
        ui->Register(MENU_NAME, Creator);

        logs::info("Menu::Register :: Registered menu: '{}' on path: '{}'.", MENU_NAME, MENU_PATH);
    }
}

void BashMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime)
{
    BashMenu::Update();
    RE::IMenu::AdvanceMovie(a_interval, a_currentTime);
}

void BashMenu::Hide()
{
    const auto queue = RE::UIMessageQueue::GetSingleton();

    if (queue) {
        queue->AddMessage(BashMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
    }
}

void BashMenu::SetVisibility(bool a_visible)
{
    const auto ui = RE::UI::GetSingleton();

    if (ui) {
        auto menu = ui->GetMenu(BashMenu::MENU_NAME);

        if (menu && menu->uiMovie) {
            menu->uiMovie->SetVisible(a_visible);
        }
    }
}

void BashMenu::Show()
{
    const auto queue = RE::UIMessageQueue::GetSingleton();

    if (queue) {
        queue->AddMessage(BashMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
    }
}

void BashMenu::Update()
{
    const auto crosshair = RE::CrosshairPickData::GetSingleton();

    if (crosshair) {
        const auto target = crosshair->target.get().get();
        const auto system = System::GetSingleton();

        if (target && system->ReferenceHasScript(target)) {
            if (target->IsLocked() && !target->IsActivationBlocked()) {
                const auto lock = target->GetLockLevel();

                _root.Invoke("ApplyLayout", system->GetLayout());

                std::array<RE::GFxValue, 1> percentage{ system->GetMeterPercentage(lock) };
                _root.Invoke("SetMeterPercentage", percentage);

                std::array<RE::GFxValue, 3> strength{ system->GetCounter(), static_cast<std::uint32_t>(system->GetPlayerStrength(false)), system->GetLockThreshold(lock) };
                _root.Invoke("UpdateCounter", strength);

                std::array<RE::GFxValue, 1> color{ system->GetColor(lock) };
                _root.Invoke("UpdateColor", color);

                if (system->GetColor(lock) == system->GetColorByType(System::COLOR_TYPE::Ready)) {
                    _root.Invoke("FlashMeter");
                }
                _root.Invoke("Show");
            } else {
                const auto player = RE::PlayerCharacter::GetSingleton();

                if (player) {
                    player->UpdateCrosshairs();
                    _root.Invoke("Hide");
                }
            }
        } else {
            _root.Invoke("Hide");
        }
    }
}