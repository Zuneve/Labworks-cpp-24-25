#include "CityInput.h"

void GetCityInput(std::string& city1, std::string& city2) {
    auto input_city1 =
        ftxui::Input(&city1, "Enter the name of the first city...");
    auto input_city2 =
        ftxui::Input(&city2, "Enter the name of the second city...");

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    auto button_confirm = ftxui::Button("Confirm", [&]() { 
        screen.ExitLoopClosure()();
    });

    auto container = ftxui::Container::Vertical({
        input_city1, input_city2, button_confirm
    });

    auto renderer = ftxui::Renderer(container, [&] {
        return ftxui::vbox(
                   {ftxui::hbox(ftxui::text(" First city: "),
                                input_city1->Render()),
                    ftxui::hbox(ftxui::text(" Second city: "),
                                input_city2->Render()),
                    ftxui::hbox(button_confirm->Render()),
                    ftxui::text("You entered: " + city1 + " and " + city2)}) 
            | ftxui::border;
    });

    screen.Loop(renderer);

}
