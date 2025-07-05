#include "steam_helpers.h"
#include <steam/steam_api_flat.h>

extern "C" {
int initSteamAPI() {
    char fail[1024];
    if (!SteamAPI_InitFlat(&fail)) {
        fprintf(stderr, fail);
        return 0;
    }
}
}