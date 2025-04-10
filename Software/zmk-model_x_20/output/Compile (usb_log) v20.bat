cd %HOMEPATH%\git\zmk\app\
west build --pristine -b nice_nano_v2 -S zmk-usb-logging -S studio-rpc-usb-uart -- -DSHIELD=model_x -DZMK_EXTRA_MODULES="c:/Users/nils/git/zmk-model_x_20/"
copy ..\..\zmk\app\build\zephyr\zmk.uf2 c:\Users\Nils\git\zmk-model_x_20\output\ZMK-Model_X_v20(usb_log).uf2
pause