#!
gcc `pkg-config --cflags gtk+-3.0` projekt.c -o projekt.exe `pkg-config --libs gtk+-3.0`
# change -mwindows to -mconsole to enable console and the change DEBUG const to 1 to enable debbuging prints
