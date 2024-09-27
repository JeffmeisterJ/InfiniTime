/*  Copyright (C) 2020-2021 JF, Adam Pigg, Avamander

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "components/ble/HomeAssistantService.h"
#include "components/ble/NimbleController.h"
#include <cstring>

namespace {
  // 0006yyxx-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t CharUuid(uint8_t x, uint8_t y) {
    return ble_uuid128_t {.u = {.type = BLE_UUID_TYPE_128},
                          .value = {0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, x, y, 0x06, 0x00}};
  }

  // 00000000-78fc-48fe-8e23-433b3a1942d0
  constexpr ble_uuid128_t BaseUuid() {
    return CharUuid(0x00, 0x00);
  }

  constexpr ble_uuid128_t haUuid {BaseUuid()};

  constexpr ble_uuid128_t haEventCharUuid {CharUuid(0x01, 0x00)};
  constexpr ble_uuid128_t haEntityId {CharUuid(0x02, 0x00)};
  constexpr ble_uuid128_t haEntityType {CharUuid(0x03, 0x00)};
  constexpr ble_uuid128_t haEntityName {CharUuid(0x04, 0x00)};
  constexpr ble_uuid128_t haEntityState {CharUuid(0x05, 0x00)};
  constexpr ble_uuid128_t haEntityBrightness {CharUuid(0x06, 0x00)};
  constexpr ble_uuid128_t haEntityHue {CharUuid(0x07, 0x00)};
  constexpr ble_uuid128_t haEntitySaturation {CharUuid(0x08, 0x00)};
  constexpr ble_uuid128_t haEntityValue {CharUuid(0x09, 0x00)};
  constexpr ble_uuid128_t haRoomName {CharUuid(0x0a, 0x00)};
  constexpr ble_uuid128_t haRoomId {CharUuid(0x0b, 0x00)};

  constexpr uint8_t MaxStringSize {16};

  int HomeAssistantCallback(uint16_t /*conn_handle*/, uint16_t /*attr_handle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    return static_cast<Pinetime::Controllers::HomeAssistantService*>(arg)->OnCommand(ctxt);
  }
}

Pinetime::Controllers::HomeAssistantService::HomeAssistantService(Pinetime::Controllers::NimbleController& nimble) : nimble(nimble) {
  characteristicDefinition[0] = {.uuid = &haEventCharUuid.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_NOTIFY,
                                 .val_handle = &eventHandle};
  characteristicDefinition[1] = {.uuid = &haEntityId.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[2] = {.uuid = &haEntityType.u, 
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[3] = {.uuid = &haEntityName.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[4] = {.uuid = &haEntityState.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[5] = {.uuid = &haEntityBrightness.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[6] = {.uuid = &haEntityHue.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[7] = {.uuid = &haEntitySaturation.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[8] = {.uuid = &haEntityValue.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[9] = {.uuid = &haRoomName.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[9] = {.uuid = &haRoomId.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[11] = {0};

  serviceDefinition[0] = {.type = BLE_GATT_SVC_TYPE_PRIMARY, .uuid = &haUuid.u, .characteristics = characteristicDefinition};
  serviceDefinition[1] = {0};
}

void Pinetime::Controllers::HomeAssistantService::Init() {
  uint8_t res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);

}

int Pinetime::Controllers::HomeAssistantService::OnCommand(struct ble_gatt_access_ctxt* ctxt) {
  char entState[17];
  uint8_t entSize = 0;
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    size_t notifSize = OS_MBUF_PKTLEN(ctxt->om);
    size_t bufferSize = notifSize;
    if (notifSize > MaxStringSize) {
      bufferSize = MaxStringSize;
    }

    char data[bufferSize + 1];
    memset(data, 0, bufferSize);
    os_mbuf_copydata(ctxt->om, 0, bufferSize, data);

    data[bufferSize] = '\0';

    char* s = &data[0];
    
    if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityId.u) == 0) {
      entityId = (uint8_t)s[0];
      // If non-existent entity ID, select lowest one and return error
      if (entityId > sizeof(entities)) {
        entityId = 0;
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
      }
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityType.u) == 0) {
      entities[entityId].type = static_cast<HAEntityType>((uint8_t)s[0]);
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityName.u) == 0) {
      entities[entityId].name = std::string(s);
      if (entities[entityId].name.size() > 9) entities[entityId].name.resize(9);
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityState.u) == 0) {
      entities[entityId].state = (bool)s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityBrightness.u) == 0) {
      entities[entityId].brightness = (uint8_t)s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntitySaturation.u) == 0) {
      entities[entityId].saturation = (uint8_t)s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityValue.u) == 0) {
      entities[entityId].value = (uint8_t)s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haRoomName.u) == 0) {
      roomName = std::string(s);
      if (roomName.size() > 16) roomName.resize(16);
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haRoomId.u) == 0) {
      roomId = (uint8_t)s[0];
    }

  } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
    
    if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityId.u) == 0) {
      entState[0] = entityId;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityType.u) == 0) {
      entState[0] = (uint8_t)entities[entityId].type;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityName.u) == 0) {
      strncpy(entState, entities[entityId].name.data(), entities[entityId].name.size());
      entSize = entities[entityId].name.size();
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityState.u) == 0) {
      entState[0] = entities[entityId].state;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityBrightness.u) == 0) {
      entState[0] = entities[entityId].brightness;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntitySaturation.u) == 0) {
      entState[0] = entities[entityId].saturation;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityValue.u) == 0) {
      entState[0] = entities[entityId].value;
      entSize = 1;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haRoomName.u) == 0) {
      strncpy(entState, roomName.data(), 16);
      entSize = strlen(entState);
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityId.u) == 0) {
      entState[0] = roomId;
      entSize = 1;
    }
    
    int res = os_mbuf_append(ctxt->om, &entState, entSize);
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}

Pinetime::Controllers::HAEntity* Pinetime::Controllers::HomeAssistantService::getEntityState(uint8_t entityNum) const {
  if (entityNum > (sizeof(entities)-1)) return (HAEntity *)-1;
  return (HAEntity *)&entities[entityNum];
}

int8_t Pinetime::Controllers::HomeAssistantService::setEntityState(uint8_t entityNum, HAEntity* entity) {
  if (entityNum > (sizeof(entities)-1)) return -1;
  entities[entityNum] = *entity;
  return 0;
}

std::string Pinetime::Controllers::HomeAssistantService::getRoomName() {
  return roomName;
}

void Pinetime::Controllers::HomeAssistantService::event(char event) {
  auto* om = ble_hs_mbuf_from_flat(&event, 1);

  uint16_t connectionHandle = nimble.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}
