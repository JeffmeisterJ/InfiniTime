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
  constexpr ble_uuid128_t haEntityOneState {CharUuid(0x02, 0x00)};
  constexpr ble_uuid128_t haEntityTwoState {CharUuid(0x03, 0x00)};
  constexpr ble_uuid128_t haEntityThreeState {CharUuid(0x04, 0x00)};
  constexpr ble_uuid128_t haEntityFourState {CharUuid(0x05, 0x00)};
  

  constexpr uint8_t MaxStringSize {40};

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
  characteristicDefinition[1] = {.uuid = &haEntityOneState.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[2] = {.uuid = &haEntityTwoState.u, 
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[3] = {.uuid = &haEntityThreeState.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};
  characteristicDefinition[4] = {.uuid = &haEntityFourState.u,
                                 .access_cb = HomeAssistantCallback,
                                 .arg = this,
                                 .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ};                                 
  characteristicDefinition[5] = {0};

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
  bool entState;
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    size_t notifSize = OS_MBUF_PKTLEN(ctxt->om);
    size_t bufferSize = notifSize;
    if (notifSize > MaxStringSize) {
      bufferSize = MaxStringSize;
    }

    char data[bufferSize + 1];
    os_mbuf_copydata(ctxt->om, 0, bufferSize, data);

    if (notifSize > bufferSize) {
      data[bufferSize - 1] = '.';
      data[bufferSize - 2] = '.';
      data[bufferSize - 3] = '.';
    }
    data[bufferSize] = '\0';

    char* s = &data[0];
    
    if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityOneState.u) == 0) {
      entityOneState = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityTwoState.u) == 0) {
      entityTwoState = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityThreeState.u) == 0) {
      entityThreeState = s[0];
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityFourState.u) == 0) {
      entityFourState = s[0];
    }

  } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
    
    if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityOneState.u) == 0) {
      entState = entityOneState;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityTwoState.u) == 0) {
      entState = entityTwoState;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityThreeState.u) == 0) {
      entState = entityThreeState;
    } else if (ble_uuid_cmp(ctxt->chr->uuid, &haEntityFourState.u) == 0) {
      entState = entityFourState;
    }
    
    int res = os_mbuf_append(ctxt->om, &entState, sizeof(bool));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}

bool Pinetime::Controllers::HomeAssistantService::getEntityState(uint8_t entityNum) const {
  switch (entityNum)
  {
  case 1:
    return entityOneState;
  case 2:
    return entityTwoState;
  case 3:
    return entityThreeState;
  case 4:
    return entityFourState;

  default:
    return false;
  }
}

void Pinetime::Controllers::HomeAssistantService::setEntityState(uint8_t entityNum, bool state) {
  switch (entityNum)
  {
  case 1:
    entityOneState = state;
    break;
  case 2:
    entityTwoState = state;
    break;
  case 3:
    entityThreeState = state;
    break;
  case 4:
    entityFourState = state;
    break;

  default:
    break;
  }
}

void Pinetime::Controllers::HomeAssistantService::event(char event) {
  auto* om = ble_hs_mbuf_from_flat(&event, 1);

  uint16_t connectionHandle = nimble.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}
