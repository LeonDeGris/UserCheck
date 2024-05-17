#include <napi.h>
#include <windows.h>
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

std::string GetUserPrivilege(const std::string& username) {
    LOCALGROUP_USERS_INFO_0 *groupInfo;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    NET_API_STATUS nStatus;
    std::string result = "None";

    nStatus = NetUserGetLocalGroups(
        NULL,
        username.c_str(),
        0,
        LG_INCLUDE_INDIRECT,
        (LPBYTE*)&groupInfo,
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries
    );

    if (nStatus == NERR_Success) {
        for (DWORD i = 0; i < entriesRead; i++) {
            std::wstring groupNameW(groupInfo[i].lgrui0_name);
            std::string groupName(groupNameW.begin(), groupNameW.end());

            if (groupName == "Administrators") {
                result = "Administrator";
                break;
            } else if (groupName == "Users") {
                result = "User";
            } else if (groupName == "Guests") {
                result = "Guest";
            }
        }
        NetApiBufferFree(groupInfo);
    }

    return result;
}

Napi::String CheckUserPrivilege(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    std::string username = info[0].As<Napi::String>();
    std::string privilege = GetUserPrivilege(username);

    return Napi::String::New(env, privilege);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "checkUserPrivilege"),
                Napi::Function::New(env, CheckUserPrivilege));
    return exports;
}

NODE_API_MODULE(user_privilege, Init)
