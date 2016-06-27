call android update project --target "android-18" --path .
call ndk-build
call ant clean
call ant debug
call adb install -r bin/NativeActivity-debug.apk
call run.bat