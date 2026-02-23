# Livefind

> Ncurses-based live directory search tool 
(also a (fzf)[https://github.com/junegunn/fzf/] clone)

## 🖥️ Usage

```livefind [-args] [space-separated directories]```\

Pass in a list of directiories, find and select a specific directory, and 
livefind will output it in stdout. 

> [!NOTE]
> Livefind recognizes args as anything listed in [Flags (-args)](#flags--args), 
even if it is in a middle of a list of directories. Flags are also applied as 
encountered. As such, you should pass flags first to avoid strange behaviour, 
although you can do some cool stuff with this if you know what you are doing.

## ⌨️ Hotkeys

| Hotkey | Action |
| --- | --- |
| <kbd>C-n</kbd> or <kbd>C-j</kbd> or <kbd>↓</kbd> | Next item |
| <kbd>C-p</kbd> or <kbd>C-k</kbd> or <kbd>↑</kbd> | Previous item |
| <kbd>C-c</kbd> or <kbd>C-q</kbd> | Exit |

## 🚩 Flags (-args)

| Flag | Description | 
| ------------- | -------------- |
| -v --verbosity | Select verbosity level | 
| -f --force | Always allow/cancel risky actions | 
| -d --depth | Set max recursion depth | 
| -h --help | Print help message | 

