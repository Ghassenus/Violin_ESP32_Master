#pragma once

#include <Arduino.h>
#include <vector>

struct ScannedDevice {
    String name;
    String mac;
    bool available;
    bool connected;
};

namespace bluetooth_history {

// Initialise la gestion d'historique (lecture depuis la Flash)
void init();

// Ajouter ou mettre à jour un périphérique dans la liste
void add_or_update_device(const String& name, const String& mac);

// Effacer tout l'historique
void clear();

// Sauvegarder manuellement la liste actuelle en Flash
void save();

// Envoyer toute la liste sur UART à ESP1
void send_all_uart();

// Marquer tous les devices comme "non disponibles" avant un nouveau scan
void mark_all_unavailable();

// Mettre à jour le statut "connecté" d'un device
void set_connected(const String& mac, bool connected);

// Retourner la liste en accès interne (si besoin)
const std::vector<ScannedDevice>& get_devices();

} // namespace bluetooth_history
