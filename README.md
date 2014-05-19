bubble
======

- Bubble is a simple and lightweight C++11 dialog library.
- Bubble creates dialogs boxes, progress bars and radio buttons.
- Bubble replaces MessageBox() with ease.
- Bubble provides callbacks to update dialogs.
- Bubble has an expressive API based on strings.
- Bubble has no external dependencies. Only native OS calls are used.
- Bubble is MIT licensed.

### samples
```c++
#include "bubble.hpp"

int main() {
    bubble::show(
        "title.text=About;"
        "body.icon=14;"
        "body.text=Your app name here;"
        "progress=0;"
        "footer.icon=-3;"
        "footer.text=Made with Bubble dialog library (built " __DATE__ ")",
        {}( bubble::vars &ui ) {
            ui["progress"] = ui["progress"] + 10;
        } );
}
```

### possible output
![image](https://raw.github.com/r-lyeh/depot/master/bubble-snapshot-1.png)

### more samples
Check [sample.cc](https://github.com/r-lyeh/bubble/sample.cc) for a few examples

![image](https://raw.github.com/r-lyeh/depot/master/bubble-snapshot-2.png)

![image](https://raw.github.com/r-lyeh/depot/master/bubble-snapshot-3.png)

![image](https://raw.github.com/r-lyeh/depot/master/bubble-snapshot-4.png)

### Cons
- Windows only (for now).

### API
- `int bubble::show( string options, [callback] )`
- `int bubble::show( map<string,string> options, [callback] )`

### vars
- `timeout (int ms)` in milliseconds; [0] to stay forever
- `progress (int pct)` [0..100] range; [-1] creates an infinite marquee
- `title.text (string)`
- `head.text (string)`
- `body.icon (int)` see note below
- `body.text (string)`
- `footer.icon (int)` see note below
- `footer.text (string)`
- `style.minimizable (bool)`
- `style.command_links (bool)`
- `style.skippable (bool)`
- `exit (int)` to close dialog and return exit code from callback
- `[number].text (string)`
- `[number].icon (int)` see note below

### win32 icons
- valid icons are in range {-1 = information, -2 = warning, -3 = error, -4 = admin/shield }
- also in range [1..255] which maps to icons in imageres.dll, as follows:

![image](https://raw.github.com/r-lyeh/depot/master/bubble-imageresdll.png)
