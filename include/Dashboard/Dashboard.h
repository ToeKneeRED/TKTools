#pragma once
#include <Component.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QApplication>

struct Dashboard
{
    static void Init(int32_t aArgc, char** apArgv)
    {
        App = new QApplication(aArgc, apArgv);

        Settings = new QSettings("Dashboard.ini", QSettings::IniFormat);

        Window = new QWidget;
        Window->setObjectName("Window");
        Window->setWindowTitle("Dashboard");
        Window->resize(400, 700);
        Window->setFocusPolicy(Qt::ClickFocus);
        Window->setStyleSheet(R"(
        QWidget#Window {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: "Jetbrains Mono NL";
            font-size: 14px;
        })");

        Layout = new QVBoxLayout(Window);

        Window->setLayout(Layout);
        Window->show();
    }

    static int Run() { return QApplication::exec(); }

    static void SetIcon(const QIcon& acIcon) { Window->setWindowIcon(acIcon); }

    template <typename T, typename... Args> static T* CreateComponent(Args&&... aArgs)
    {
        T* component = new T(std::forward<Args>(aArgs)...);
        Components.push_back(component);

        return component;
    }
    template <typename T> static T* FindComponent()
    {
        for (auto* pComponent : Components)
        {
            if (auto* component = reinterpret_cast<T*>(pComponent); component)
                return component;
        }
        return nullptr;
    }
    template<typename T> static std::vector<T*> FindComponents()
    {
        std::vector<T*> foundComponents{};

        for (auto* pComponent : Components)
        {
            if (auto* component = reinterpret_cast<T*>(pComponent); component)
                 foundComponents.push_back(component);
        }

        return foundComponents;
    }

    static inline QApplication* App;
    static inline QWidget* Window;
    static inline QVBoxLayout* Layout;
    static inline QIcon* Icon;
    static inline QSettings* Settings;

    static inline std::string GamePath;
    static inline std::string DllPath;

    static inline std::vector<IComponent*> Components;
};
