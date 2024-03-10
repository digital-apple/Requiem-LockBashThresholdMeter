#include "Events.h"
#include "BashMenu.h"

auto Events::GetSingleton() -> Events*
{
    static Events singleton;
    return std::addressof(singleton);
}

void Events::Register()
{
    if (const auto UI = RE::UI::GetSingleton()) {
        UI->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());

        logs::info("Events :: Registered for MenuOpenCloseEvent.");
    }
}

auto Events::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (a_event->menuName == RE::LoadingMenu::MENU_NAME && !a_event->opening) {
        BashMenu::Show();
    }

    const auto controls = RE::ControlMap::GetSingleton();

    if (controls) {
        const auto stack = (REL::Module::IsAE() && REL::Module::get().version().patch() >= 1130) ? *REL::Relocation<RE::BSTArray<CONTEXT>*>(reinterpret_cast<std::uintptr_t>(controls) + 0x108).get() : controls->contextPriorityStack;

        if (stack.empty() || (stack.back() != CONTEXT::kGameplay && stack.back() != CONTEXT::kFavorites)) {
            BashMenu::SetVisibility(false);
        } else {
            BashMenu::SetVisibility(true);
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}