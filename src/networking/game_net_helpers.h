#pragma once


#ifdef __cplusplus
extern "C" {
#endif

int initSteamAPI();
void send_message(const char* msg, unsigned int length);
void receive_messages();
void check_messages();

#ifdef __cplusplus
}
#endif