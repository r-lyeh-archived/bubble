/* Bubble is a simple dialog library built on a text-based API.
 * Copyright (c) 2014, 2015, Mario 'rlyeh' Rodriguez, zlib/libpng licensed.

 * Callstack code is based on code by Magnus Norddahl (See http://goo.gl/LM5JB)
 * Mem/CPU OS code is based on code by David Robert Nadeau (See http://goo.gl/8P5Jqv)

 * - rlyeh
 */

// [ref] http://msdn.microsoft.com/en-us/library/windows/desktop/bb760441(v=vs.85).aspx
// [ref] http://msdn.microsoft.com/en-us/library/windows/desktop/bb760540(v=vs.85).aspx
// [ref] http://www.codeproject.com/Articles/16806/Vista-Goodies-in-C-Using-TaskDialogIndirect-to-Bui
// [ref] http://msdn.microsoft.com/en-us/library/vstudio/dd234915.aspx
// [ref] http://www.win7dll.info/imageres_dll_icons.png (on win32, try icons in [-4..-1] and [1..255] range)

#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include "bubble.hpp"

#ifdef _WIN32
#   define UNICODE
#   define _UNICODE
#   include <windows.h>
#   include <commctrl.h>
#   pragma comment(lib, "comctl32.lib")
#   pragma comment(lib, "user32.lib")
#   pragma comment(lib, "shell32.lib")
#   if defined(_M_IX86)
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   elif defined(_M_IA64)
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   elif defined(_M_X64)
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   else
#       pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#   endif
#   ifndef TD_SHIELD_ICON
#       define TD_SHIELD_ICON MAKEINTRESOURCEW(-4)
#   endif
#endif

#ifdef _WIN32
#define $win32 $yes
#define $welse $no
#else
#define $win32 $no
#define $welse $yes
#endif

#define $yes(...) __VA_ARGS__
#define $no(...)

namespace {
    using namespace bubble;

    bubble::vars& getDialog()  {
        static std::map< std::thread::id, bubble::vars > all;
        std::thread::id self = std::this_thread::get_id();
        return ( all[ self ] = all[ self ] );
    }

    struct extra {
        int dummy = 0;
        std::function<void(bubble::vars&)> cb;
	std::vector<std::wstring> options;
        bubble::vars copy;

        $win32(
        int progress_style = 0;
        )
    };

    extra& getExtra()  {
        static std::map< std::thread::id, extra > all;
        std::thread::id self = std::this_thread::get_id();
        return ( all[ self ] = all[ self ] );
    }
}


$win32(
    HRESULT CALLBACK TDCallback (
    HWND hwnd, UINT uNotification, WPARAM wParam,
    LPARAM lParam, LONG_PTR dwRefData )
    {
        switch ( uNotification )
        {
            case TDN_DIALOG_CONSTRUCTED:

                // SendMessage( hwnd, TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, IDOK, 1 );       // UAC, SHIELD

                SendMessage( hwnd, TDM_SET_MARQUEE_PROGRESS_BAR, 0 /*off*/, 0 );                // marquee off
                SendMessage( hwnd, TDM_SET_PROGRESS_BAR_MARQUEE, 1 /*on*/, 0 /*30ms*/ );        // marquee speed

                SendMessage( hwnd, TDM_SET_PROGRESS_BAR_RANGE, 0, MAKELPARAM(0, 100) );         // progress range
                SendMessage( hwnd, TDM_SET_PROGRESS_BAR_POS, int(0), 0 );                       // progress at

                {
                    bubble::vars &ui = getDialog();
                    if( ui["style.ontop"] ) {
                        SetWindowPos(hwnd,       // handle to window
                                    HWND_TOPMOST,  // placement-order handle
                                    0,     // horizontal position
                                    0,      // vertical position
                                    0,  // width
                                    0, // height
                                    SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE// window-positioning options
                                    );
                        SetWindowPos(hwnd,       // handle to window
                                    HWND_TOP,
                                    0,     // horizontal position
                                    0,      // vertical position
                                    0,  // width
                                    0, // height
                                    SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE// window-positioning options
                                    );
                    }
                    else
                    if( ui["style.minimized"] ) {
                        SetWindowPos(hwnd,       // handle to window
                                    HWND_BOTTOM,  // placement-order handle
                                    0,     // horizontal position
                                    0,      // vertical position
                                    0,  // width
                                    0, // height
                                    SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE// window-positioning options
                                    );
                    }
                }

                // refresh ui by simulating timer
                TDCallback( hwnd, TDN_TIMER, 0, lParam, dwRefData );
                return S_OK;

            case TDN_DESTROYED:
                break;

            case TDN_HYPERLINK_CLICKED:
                ShellExecuteW( hwnd, L"open", (LPCWSTR) lParam, NULL, NULL, SW_SHOW );

                // you can also elevate a process by using explorer. ie,
                // system("explorer.exe path/to/cmd.exe");
                /*
                ShellExecute(
                    NULL,
                    L"runas",    // Trick for requesting elevation, this is a verb not listed in the documentation above.
                    L"cmd.exe",
                    NULL,        // params
                    NULL,        // directory
                    SW_HIDE);
                */

                break;

            case TDN_TIMER: {
                // reconstruct progress bar if needed
                bubble::vars &ui = getDialog();
                extra &ex = getExtra();
                bubble::vars &copy = ex.copy;

                if( ex.cb ) {
                    ex.cb(ui);
                }

                if( ui["timeout"] > 200 ) {
                    ui["timeout"] = ui["timeout"] - 200;
                } else {
                    ui["timeout"] = 0;
                }

                enum { PERCENT, MARQUEE };
                int progress_style = ( ui["progress"] >= 0 && ui["progress"] <= 100 ? PERCENT : MARQUEE );
                if( ex.progress_style != progress_style ) {
                    ex.progress_style = progress_style;

                    if( progress_style == PERCENT ) {
                        SendMessage( hwnd, TDM_SET_MARQUEE_PROGRESS_BAR, 0 /*show*/, 0 );           // in range; marquee off
                    } else {
                        SendMessage( hwnd, TDM_SET_MARQUEE_PROGRESS_BAR, 1 /*show*/, 0 );           // out of range; marquee on
                    }
                }

                if( progress_style == PERCENT ) {
                    if( copy["progress"] != ui["progress"] )
                    SendMessage( hwnd, TDM_SET_PROGRESS_BAR_POS, ui["progress"], 0 );     // at
                }

                // window title
                if( copy["title.text"] != ui["title.text"] )
                SetWindowTextW( hwnd, ui["title.text"].c_str() );

                // head
                if( copy["head.text"] != ui["head.text"] )
                SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, (WPARAM)TDE_MAIN_INSTRUCTION, (LPARAM)( ui["head.text"].c_str() ));

                // body
                if( copy["body.text"] != ui["body.text"] )
                SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, (WPARAM)TDE_CONTENT, (LPARAM)( ui["body.text"].c_str() ));

                // footer
                if( copy["footer.text"] != ui["footer.text"] )
                SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, (WPARAM)TDE_FOOTER, (LPARAM)( ui["footer.text"].c_str() ));

                // expanded
                if( copy["footer.extra"] != ui["footer.extra"] )
                SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, (WPARAM)TDE_EXPANDED_INFORMATION, (LPARAM)( ui["footer.extra"].c_str() ));

                // update big icon
                if( copy["body.icon"] != ui["body.icon"] ) {
                    SendMessage(hwnd, TDM_UPDATE_ICON, (WPARAM)TDIE_ICON_MAIN, (LPARAM)MAKEINTRESOURCEW( ui["body.icon"] ) );
                }

                // update footer icon
                if( copy["footer.icon"] != ui["footer.icon"] ) {
                    SendMessage(hwnd, TDM_UPDATE_ICON, (WPARAM)TDIE_ICON_FOOTER, (LPARAM)MAKEINTRESOURCEW( ui["footer.icon"] ) );
                }

                copy = ui;

                if( getDialog()["exit"] >= 0 ) {
                    getDialog()["cancel_close"] = true;
                    EndDialog(hwnd,getDialog()["exit"]);
                    // cancel timer
                    return S_OK;
                }

                return S_FALSE;
            }

            default:
                break;
        }

        return !getDialog()["cancel_close"] ? S_OK : S_FALSE;
    }
)

int bubble::show( const bubble::vars &d_, const std::function<void(bubble::vars &)> &cb2 )
{
    auto &dialog = ( getDialog() = d_ );

    auto &ex = getExtra();
    ex = extra();
    ex.copy = bubble::vars();
    ex.cb = cb2;
    auto &options = getExtra().options;

    {
        int each = 0;
        for( auto it = dialog.begin(); it = dialog.find( std::to_wstring(each) + L".text" ), it != dialog.end(); ++each ) {
            options.push_back( it->second );
            dialog[ std::to_wstring(each) + L".icon" ] = dialog[ std::to_wstring(each) + L".icon" ];
        }
    }

    $win32(
    std::map< int, int > choices;
    std::vector<TASKDIALOG_BUTTON> buttons( 16 );

    for( auto i = 0u; i < options.size(); ++i ) {
        buttons[i] = {0};
        buttons[i].pszButtonText = options[i].c_str();
        buttons[i].nButtonID = ( dialog[ bubble::string(i) + L".icon" ].as<int>() != -4 ? 100 + i : 1 );
        choices[ buttons[i].nButtonID ] = i;
    }

    int nButtonPressed                  = 0;
    TASKDIALOGCONFIG config             = {0};

    config.pButtons                     = &buttons[0];
    config.cButtons                     = (UINT)options.size();

    config.cbSize                       = sizeof(config);
    config.hInstance                    = 0; //hInst;
    config.dwCommonButtons              = 0; //TDCBF_OK_BUTTON; //TDCBF_CLOSE_BUTTON; //TDCBF_CANCEL_BUTTON;

    if( dialog.find("headialog.text") != dialog.end() ) {
        config.pszMainInstruction           = L" ";
    }

    if( dialog.find("body.text") != dialog.end() || dialog.find("body.icon") != dialog.end() ) {
        config.pszContent                   = L" ";
    }

    if( dialog.find("footer.text") != dialog.end() || dialog.find("footer.icon") != dialog.end() ) {
        config.pszFooter                    = L" ";
    }

    if( dialog.find("footer.extra") != dialog.end() ) {
        config.pszExpandedInformation       = L" ";
    }

    config.pszMainIcon                  = 0; //TD_INFORMATION_ICON; //MAKEINTRESOURCE(TD_SHIELD_ICON);
    config.pszFooterIcon                = 0;

    if( dialog.find("progress") != dialog.end() ) {
        config.dwFlags |= TDF_SHOW_PROGRESS_BAR;
    }
    /*
    if( dialog.find("style.skippable") == dialog.end() ) {
        config.dwFlags |= TDF_ALLOW_DIALOG_CANCELLATION;
    } else {
        config.dwFlags &= ~TDF_ALLOW_DIALOG_CANCELLATION;
    }*/
    if( dialog.find("style.minimizable") != dialog.end() ) {
        config.dwFlags |= TDF_CAN_BE_MINIMIZED;
    }
    if( dialog.find("style.command_links") != dialog.end() ) {
        config.dwFlags |= TDF_USE_COMMAND_LINKS;
    }
    config.dwFlags |= TDF_ENABLE_HYPERLINKS;
    //config.dwFlags |= 0x10000000ULL; // |= TDIF_SIZE_TO_CONTENT; // resize is pszContent is larger than 48 bytes (hidden flag)

    config.pfCallback = TDCallback;
    config.lpCallbackData = (LONG_PTR) (0);
    config.dwFlags |= TDF_CALLBACK_TIMER;

    dialog["exit"] = -1;
    HRESULT hr = TaskDialogIndirect(&config, &nButtonPressed, NULL, NULL);

    if( SUCCEEDED(hr) ) {
        if( choices.empty() ) { // a task dialog seems to have always an OK button (?) (can we remove it?)
            return nButtonPressed == -1 ? -1 : 0;
        }
        if( choices.find(nButtonPressed) != choices.end() ) {
            return choices[nButtonPressed];
        }
    }
    )

    return ~0;
}

int bubble::show( const bubble::string &input, const std::function<void(bubble::vars &)> &cb2 ) {
    bubble::vars v;

    // parse input text into map. format is key=value;key=value; [...]
    std::wstringstream split(input + L';');
    for( bubble::string each; std::getline(split, each, L';'); ) {
        std::wstringstream token(each);
        for( bubble::string prev, key; std::getline(token, key, L'='); prev = key ) {
            v[ v.find(prev) == v.end() ? key : prev ] = key;
        }
    }

    for( auto &in : v ) {
        for( auto &ch : in.second ) {
            /**/ if( ch == '\a' ) ch = '=';
            else if( ch == '\b' ) ch = ';';
        }
    }

    return show( v, cb2 );
}

// vars
// progress (i), command_links (b)
//

#undef $yes
#undef $no
#undef $win32
#undef $welse
