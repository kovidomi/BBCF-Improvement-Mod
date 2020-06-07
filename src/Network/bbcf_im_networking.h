#pragma once
#include "network_manager.h"

#include <steam_api.h>

void Send_BBCFIM_ID(CSteamID opponentSteamID);
void Receive_BBCFIM_ID(im_packet_internal_t *packet);

void Send_customGameMode_request(CSteamID opponentSteamID);
void Receive_customGameMode_request(im_packet_internal_t *packet);