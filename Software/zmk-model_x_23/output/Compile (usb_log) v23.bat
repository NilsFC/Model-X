cd %HOMEPATH%\git\zmk\app\
west build --pristine -b nice_nano_v2 -S zmk-usb-logging -S studio-rpc-usb-uart -- -DSHIELD=model_x -DZMK_EXTRA_MODULES="c:/Users/nilsc/git/zmk-model_x_23/"
copy ..\..\zmk\app\build\zephyr\zmk.uf2 c:\Users\nilsc\git\zmk-model_x_23\output\ZMK-Model_X_v23(usb_log).uf2
pause