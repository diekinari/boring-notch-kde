pragma Singleton
import QtQuick

// Lightweight runtime-switchable translations. `lang` is pushed in from
// Config.language by the root components (a Binding), so this singleton itself
// doesn't need access to context properties. t(s) returns the translation for
// the current language, falling back to the English source string.
//
// Bindings written as `Tr.t("...")` re-evaluate when `lang` changes, because
// t() reads `lang` during evaluation.
QtObject {
    id: root
    property string lang: "en"

    function t(s) {
        if (lang === "en")
            return s;
        var entry = dict[s];
        return (entry && entry[lang]) ? entry[lang] : s;
    }

    readonly property var dict: ({
        // Window / tabs
        "Boring Notch — Settings": { ru: "Boring Notch — Настройки" },
        "Appearance": { ru: "Внешний вид" },
        "Notch": { ru: "Чёлка" },
        "Glass": { ru: "Стекло" },
        // General
        "General": { ru: "Основное" },
        "Language": { ru: "Язык" },
        "Always show tabs": { ru: "Всегда показывать вкладки" },
        "Show settings icon in notch": { ru: "Показывать значок настроек в чёлке" },
        "Show notch on all displays": { ru: "Показывать чёлку на всех дисплеях" },
        "Mirror the notch onto every connected monitor.": { ru: "Дублировать чёлку на каждый подключённый монитор." },
        // Media
        "Media": { ru: "Медиа" },
        "Colored spectrogram": { ru: "Цветной спектр" },
        "Real-time audio waveform": { ru: "Аудиоволна в реальном времени" },
        "Uses the playing app's audio via PipeWire. Slightly more CPU. (Visualizer not yet implemented.)": { ru: "Использует звук играющего приложения через PipeWire. Чуть больше нагрузки на CPU. (Визуализатор ещё не реализован.)" },
        "Player tinting": { ru: "Подкраска под плеер" },
        "Tint the notch with the album art's dominant color.": { ru: "Окрашивать чёлку в доминирующий цвет обложки." },
        "Enable blur effect behind album art": { ru: "Размытие за обложкой" },
        "Slider color": { ru: "Цвет ползунка" },
        // Additional
        "Additional features": { ru: "Дополнительно" },
        "Show cool face animation while inactive": { ru: "Анимация «лица» в простое" },
        // Claude
        "Show Claude Code activity": { ru: "Показывать активность Claude Code" },
        "Shows in the notch when Claude Code is working, finished, or needs input. Requires the Claude Code status hook.": { ru: "Показывает в чёлке, когда Claude Code работает, завершил или ждёт ввода. Требует хук статуса Claude Code." },
        // Notch sizes
        "Closed (collapsed) notch": { ru: "Свёрнутая чёлка" },
        "Open (expanded) notch": { ru: "Раскрытая чёлка" },
        "Width": { ru: "Ширина" },
        "Height": { ru: "Высота" },
        "Corner radius": { ru: "Скругление углов" },
        "Reset to defaults": { ru: "Сбросить к умолчаниям" },
        // Glass
        "Liquid glass": { ru: "Жидкое стекло" },
        "Enable liquid glass": { ru: "Включить жидкое стекло" },
        "Background blur (KWin)": { ru: "Размытие фона (KWin)" },
        "Frosted desktop behind the notch. Needs KWin's Blur desktop effect enabled.": { ru: "Матовый рабочий стол за чёлкой. Требует включённого эффекта Blur в KWin." },
        "Opacity (%)": { ru: "Непрозрачность (%)" },
        "Sheen (%)": { ru: "Блик (%)" },
        "Rim highlight (%)": { ru: "Светящаяся кромка (%)" },
        "Frost strength (%)": { ru: "Сила матовости (%)" },
        "Blur radius is a global KWin setting and can't be set per-window; frost adjusts the background contrast/intensity instead.": { ru: "Радиус размытия — глобальная настройка KWin, его нельзя задать для отдельного окна; «матовость» меняет контраст/яркость фона." },
        "Open KWin blur settings…": { ru: "Открыть настройки блюра KWin…" },
        // Music
        "Nothing playing": { ru: "Ничего не играет" },
        "No player": { ru: "Нет плеера" }
    })
}
