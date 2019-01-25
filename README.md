# QtAndroidTools
QtAndroidTools is a library that allows to simplify access to some native Android features from Qt C++ and QML.

# The tools

**ApkExpansionFiles**  
Help in manage the apk expansion files download  
[Documentation](https://falsinsoft.blogspot.com/2019/01/qtandroidtools-apkexpansionfiles.html)

**AppPermissions**  
Allow to request Android app permissions in an easier way  
[Documentation](https://falsinsoft.blogspot.com/2019/01/qtandroidtools-apppermissions.html)

**ApkInfo**  
Return info about the apk containing the executed app  
[Documentation](https://falsinsoft.blogspot.com/2019/01/qtandroidtools-apkinfo.html)

**BatteryState**  
Allow monitoring the battery level and state  
[Documentation](https://falsinsoft.blogspot.com/2019/01/qtandroidtools-batterystate.html)

# How To Integrate
- Place QtAndroidTools lib to your project dir and include it in .pro file
```
include(QtAndroidTools/QtAndroidTools.pri)
```

- Assign in you .pro file varialble ANDROID_PACKAGE_SOURCE_DIR before include QtAndroidTools/QtAndroidTools.pri, for ex.
```
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
```

**IMPORTANT NOTE:** The library .pri check if java sources are already present into android/src folder, if no copy the library files. In case you are updating the library remember to remove these java files to be sure having always the new java versions code.