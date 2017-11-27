copy win32\x64\Release\*dll deployment\
copy win32\x64\Release\hexmap_widgets.exe deployment\
D:\Qt\5.7\msvc2015_64\bin\windeployqt.exe --release --dir deployment win32\x64\Release\hexmap_widgets.exe