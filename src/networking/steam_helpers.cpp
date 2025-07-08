#include "steam_helpers.h"
#include <steam/steam_api_flat.h>
#include <steam/isteamnetworkingsockets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils/types.h"

extern "C" {

ISteamNetworkingSockets* networking = nullptr;
HSteamListenSocket listenSocket;
HSteamNetConnection connection;
SteamNetworkingIdentity peerIdentity;


int initSteamAPI() {
    if (!SteamAPI_Init()) {
        fprintf(stderr, "Steam init failed\n");
        return 0;
    }

    networking = SteamAPI_SteamNetworkingSockets_SteamAPI();
    if (!networking) {
        fprintf(stderr, "Failed to get SteamNetworkingSockets interface\n");
        return 0;
    }

    // Create listen socket for P2P connections
    listenSocket = SteamAPI_ISteamNetworkingSockets_CreateListenSocketP2P(networking, 0, 0, NULL);
    
    // For now, we'll handle connections manually rather than using callbacks
    // since the callback system requires more complex setup
    
    char partnerSteamID[64];
    printf("Enter peer Steam ID (or leave empty to wait): ");
    if (fgets(partnerSteamID, sizeof(partnerSteamID), stdin)) {
        // Remove newline character
        size_t len = strlen(partnerSteamID);
        if (len > 0 && partnerSteamID[len-1] == '\n') {
            partnerSteamID[len-1] = '\0';
        }
        
        if (strlen(partnerSteamID) > 0) {
            // Convert string to uint64
            uint64_t steamID = strtoull(partnerSteamID, NULL, 10);
            SteamAPI_SteamNetworkingIdentity_SetSteamID64(&peerIdentity, steamID);
            connection = SteamAPI_ISteamNetworkingSockets_ConnectP2P(networking, peerIdentity, 0, 0, NULL);
        }
    }
    
    return 1;
}

// Callback for incoming connections
void OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
    switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_Connecting:
            printf("Accepting incoming connection\n");
            SteamAPI_ISteamNetworkingSockets_AcceptConnection(networking, pInfo->m_hConn);
            connection = pInfo->m_hConn;
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            printf("Connected to peer!\n");
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            printf("Connection closed: %s\n", pInfo->m_info.m_szEndDebug);
            connection = k_HSteamNetConnection_Invalid;
            break;

        default:
            break;
    }
}

void send_message(const char* msg, u32 length) {
    if (connection == k_HSteamNetConnection_Invalid) {
        printf("No active connection to send message\n");
        return;
    }
    
    EResult result = SteamAPI_ISteamNetworkingSockets_SendMessageToConnection(
        networking,
        connection,
        msg,
        length,
        k_nSteamNetworkingSend_Reliable,
        NULL
    );
    
    if (result != k_EResultOK) {
        printf("Failed to send message: %d\n", result);
    }
}

void receive_messages() {
    if (connection == k_HSteamNetConnection_Invalid) {
        return;
    }
    
    SteamNetworkingMessage_t* incoming[32];
    int numMessages = SteamAPI_ISteamNetworkingSockets_ReceiveMessagesOnConnection(
        networking, 
        connection, 
        incoming, 
        32
    );
    
    for (int i = 0; i < numMessages; i++) {
        if (incoming[i] && incoming[i]->m_pData) {
            printf("Received: %s\n", (const char*)incoming[i]->m_pData);
            SteamAPI_SteamNetworkingMessage_t_Release(incoming[i]);
        }
    }
}

void check_messages() {
    if (connection != k_HSteamNetConnection_Invalid) {
        receive_messages();
    } else {
        printf("No active connection.\n");
    }
}


}