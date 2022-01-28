#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"

#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libFileA.so")

#include "Includes/Macros.h"

// fancy struct for patches for kittyMemory
struct My_Patches {
    // let's assume we have patches for these functions for whatever game
    // like show in miniMap boolean function
    MemoryPatch GodMode, GodMode2, SliderExample;
    // etc...
} hexPatches;

bool feature1, feature2, featureHookToggle, Health;
int sliderValue = 1, level = 0;
void *instanceBtn;

// Hooking examples. Assuming you know how to write hook
void (*AddMoneyExample)(void *instance, int amount);

bool (*old_get_BoolExample)(void *instance);
bool get_BoolExample(void *instance) {
    if (instance != NULL && featureHookToggle) {
        return true;
    }
    return old_get_BoolExample(instance);
}

float (*old_get_FloatExample)(void *instance);
float get_FloatExample(void *instance) {
    if (instance != NULL && sliderValue > 1) {
        return (float) sliderValue;
    }
    return old_get_FloatExample(instance);
}

int (*old_Level)(void *instance);
int Level(void *instance) {
    if (instance != NULL && level) {
        return (int) level;
    }
    return old_Level(instance);
}

void (*old_FunctionExample)(void *instance);
void FunctionExample(void *instance) {
    instanceBtn = instance;
    if (instance != NULL) {
        if (Health) {
            *(int *) ((uint64_t) instance + 0x48) = 999;
        }
    }
    return old_FunctionExample(instance);
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it still works
    // New way to patch hex via KittyMemory without need to  specify len. Spaces or without spaces are fine
    // ARM64 assembly example
    // MOV X0, #0x0 = 00 00 80 D2
    // RET = C0 03 5F D6
    hexPatches.GodMode = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x123456")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
    //You can also specify target lib like this
    hexPatches.GodMode2 = MemoryPatch::createWithHex("libtargetLibHere.so",
                                                     string2Offset(OBFUSCATE("0x222222")),
                                                     OBFUSCATE("20 00 80 D2 C0 03 5F D6"));

    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("str", FunctionExample, old_FunctionExample);
    HOOK_LIB("libFileB.so", "0x123456", FunctionExample, old_FunctionExample);
    HOOK_NO_ORIG("0x123456", FunctionExample);
    HOOK_LIB_NO_ORIG("libFileC.so", "0x123456", FunctionExample);
    HOOKSYM("__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_LIB("libFileB.so", "__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_NO_ORIG("__SymbolNameExample", FunctionExample);
    HOOKSYM_LIB_NO_ORIG("libFileB.so", "__SymbolNameExample", FunctionExample);

    // Patching offsets directly. Strings are automatically obfuscated too!
    PATCHOFFSET("0x20D3A8", "00 00 A0 E3 1E FF 2F E1");
    PATCHOFFSET_LIB("libFileB.so", "0x20D3A8", "00 00 A0 E3 1E FF 2F E1");

    AddMoneyExample = (void(*)(void *,int))getAbsoluteAddress(targetLibName, 0x123456);

#else //To compile this code for armv7 lib only.
    // New way to patch hex via KittyMemory without need to specify len. Spaces or without spaces are fine
    // ARMv7 assembly example
    // MOV R0, #0x0 = 00 00 A0 E3
    // BX LR = 1E FF 2F E1
    hexPatches.GodMode = MemoryPatch::createWithHex(targetLibName, //Normal obfuscate
                                                    string2Offset(OBFUSCATE("0x123456")),
                                                    OBFUSCATE("00 00 A0 E3 1E FF 2F E1"));
    //You can also specify target lib like this
    hexPatches.GodMode2 = MemoryPatch::createWithHex("libtargetLibHere.so",
                                                     string2Offset(OBFUSCATE("0x222222")),
                                                     OBFUSCATE("01 00 A0 E3 1E FF 2F E1"));

    // Hook example. Comment out if you don't use hook
    // Strings in macros are automatically obfuscated. No need to obfuscate!
    HOOK("str", FunctionExample, old_FunctionExample);
    HOOK_LIB("libFileB.so", "0x123456", FunctionExample, old_FunctionExample);
    HOOK_NO_ORIG("0x123456", FunctionExample);
    HOOK_LIB_NO_ORIG("libFileC.so", "0x123456", FunctionExample);
    HOOKSYM("__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_LIB("libFileB.so", "__SymbolNameExample", FunctionExample, old_FunctionExample);
    HOOKSYM_NO_ORIG("__SymbolNameExample", FunctionExample);
    HOOKSYM_LIB_NO_ORIG("libFileB.so", "__SymbolNameExample", FunctionExample);

    // Patching offsets directly. Strings are automatically obfuscated too!
    PATCHOFFSET("0x20D3A8", "00 00 A0 E3 1E FF 2F E1");
    PATCHOFFSET_LIB("libFileB.so", "0x20D3A8", "00 00 A0 E3 1E FF 2F E1");

    AddMoneyExample = (void (*)(void *, int)) getAbsoluteAddress(targetLibName, 0x123456);

    LOGI(OBFUSCATE("Done"));
#endif

    // Anti Leech
    sleep(20);
    if (!titleValid || !headingValid || !iconValid || !settingsValid) {
        int *p = 0;
        *p = 0;
    }

    return NULL;
}


jobjectArray getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Raunak Menu"), Toast::LENGTH_LONG);

    const char *features[] = {
        OBFUSCATE("Category_BUTTON LINK EXAMPLE"),
           OBFUSCATE("ButtonLink_SUBSCRIBE_https://youtube.com/c/RaunakMods"),
           OBFUSCATE("Category_Category Example"),
           OBFUSCATE("Text_New Feature"),
           OBFUSCATE("Rainbow_New Feature"),
           OBFUSCATE("Category_TOGGLE Example"),
            OBFUSCATE("Toggle_The toggle"),
            OBFUSCATE("TG_The toggle"),
            OBFUSCATE("Tg_The toggle"),
            OBFUSCATE("-11_Button_SETTING"),
            
    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 1:
            feature2 = boolean;
            if (feature2) {
           
                hexPatches.GodMode.Modify();
                hexPatches.GodMode2.Modify();
                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.GodMode.Restore();
                hexPatches.GodMode2.Restore();
                //LOGI(OBFUSCATE("Off"));
            }
            break;
        
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

    // Register your class native methods. Build and ecompile the app and see the signature
    // This is to hide function names from disassembler
    // See more: https://developer.android.com/training/articles/perf-jni#native-libraries

    //Your menu class
    jclass c = globalEnv->FindClass("com/Raunak/mods/Menu");
    if (c != nullptr){
        static const JNINativeMethod menuMethods[] = {
              {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
              {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
              {OBFUSCATE("isGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
              {OBFUSCATE("setHeadingText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setHeadingText)},
              {OBFUSCATE("setTitleText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setTitleText)},
              {OBFUSCATE("settingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(settingsList)},
              {OBFUSCATE("getFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(getFeatureList)},
        };

        int mm = globalEnv->RegisterNatives(c, menuMethods, sizeof(menuMethods) / sizeof(JNINativeMethod));
        if (mm != JNI_OK) {
            LOGE(OBFUSCATE("Menu methods error"));
            return mm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    jclass p = globalEnv->FindClass( OBFUSCATE("com/Raunak/mods/Preferences"));
    if (p != nullptr){
        static const JNINativeMethod prefmethods[] = {
                { OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
        };

        int pm = globalEnv->RegisterNatives(p, prefmethods, sizeof(prefmethods) / sizeof(JNINativeMethod));
        if (pm != JNI_OK){
            LOGE(OBFUSCATE("Preferences methods error"));
            return pm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

