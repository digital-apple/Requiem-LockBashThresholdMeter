#pragma once

class BashMenu : RE::IMenu
{
private:
    using Super = RE::IMenu;
public:
    constexpr static std::string_view MENU_NAME {"BashMenu"};
    constexpr static std::string_view MENU_PATH {"Requiem - Lock Bash Threshold Meter/BashMenu"}; // Change path

    BashMenu();

    static void Register();

    void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;

    static void Hide();
    static void SetVisibility(bool a_visible);
    static void Show();
    void Update();
    
    static auto Creator() -> RE::stl::owner<RE::IMenu*> { return new BashMenu(); }
private:
    class Logger : public RE::GFxLog
    {
    public:
        void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
        {
            std::string fmt(a_fmt ? a_fmt : "");
            while (!fmt.empty() && fmt.back() == '\n') {
                fmt.pop_back();
            }

            std::va_list args;
            va_copy(args, a_argList);
            std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
            std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
            va_end(args);

            logs::info("{}"sv, buf.data());
        }
    };

    RE::GFxValue _root;
};