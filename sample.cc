#include "bubble.hpp"
#include <iostream>

int main() {
    bubble::notify("notify dialog, using icon #19", "hello world", 19);

    bubble::show(
        "title.text=About;"
        "body.icon=14;"
        "body.text=Your app name here;"
        "progress=0;"
        "footer.icon=-3;"
        "footer.text=Made with Bubble dialog library (built " __DATE__ ")", []( bubble::vars &ui ) {
            ui["progress"] = ui["progress"] + 10;
        } );

    bubble::show( {
            {"title.text", "progress bar"},
            {"body.text", "this is a progress bar"},
            {"body.icon", "warning"},
            {"progress", 0}
        },
        []( bubble::vars &ui ) {
             ui["progress"] = int(ui["progress"]) + 10;
         }
    );

    int choice = bubble::show( bubble::string()
        << "body.text=This is a radio dialog;"
        << "0.text=" << "Say hello;"
        << "1.text=" << "Say goodbye\nsuboption;"
        << "2.icon=" << -4 << ";"
        << "2.text=Say nothing;"
        << "style.command_links=" << false << ";"
        << "footer.icon=40;footer.text=Made with Bubble!" );

    bubble::show( bubble::string() << "body.text=option #" << choice << " selected" );

    bubble::show(
        {
            {"0.text", "cancel!"},
            {"1.text", "continue" },
            {"style.minimizable", true},
            {"style.skippable", true},
            {"style.command_links", true},
            {"progress", 0},
            {"body.icon", 18 },
            {"footer.text", "im here"},
            {"timeout", 5000}
        },
        []( bubble::vars &ui ) {
            static int timer = 0; timer = (++timer)%4;

            ui["progress"] = ui["progress"] + 1;

            ui["title.text"] = L"Modal test";

            ui["head.text"] = L"Change Password - " + std::to_wstring( ui["timeout"] / 1000 );

            ui["body.text"] = bubble::string( L"Update in progress " ) + std::wstring( timer, L'.' );

            ui["footer.text"] = L"This is a footer. <a href=\"https://github.com/r-lyeh/bubble/\">Made with Bubble!</a>";

            ui["footer.icon"] = ( ui["footer.icon"] + 1 ) % 0xFF;

            if( ui["timeout"] <= 0 ) {
                ui["exit"] = 0;
            }
    } );

    // parent icon
    bubble::notify("this is another notify dialog, using parent icon", "hello 2");
}
