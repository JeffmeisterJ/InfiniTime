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
#pragma once

#include <cstdint>
#include <string>
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#include <host/ble_uuid.h>
#undef max
#undef min

namespace Pinetime {
  namespace Controllers {

    enum class HAEventType : uint8_t {
      HA_REQ_REFRESH,
      HA_ENTITY_CHANGED,
    };

    enum class HAEntityType : uint8_t {
      HA_TYPE_NONE,
      HA_TYPE_SWITCH,
      HA_TYPE_LAMP,
      HA_TYPE_LAMP_RGB,
    };
    
    struct HAEntity {
      HAEntityType type = HAEntityType::HA_TYPE_NONE;
      std::string name = std::string("---");
      bool state = false;
      uint8_t brightness = 0;
      uint8_t hue = 0;
      uint8_t saturation = 0;
      uint8_t value = 0;
    };

    class NimbleController;

    class HomeAssistantService {
    public:
      explicit HomeAssistantService(NimbleController& nimble);

      void Init();

      int OnCommand(struct ble_gatt_access_ctxt* ctxt);

      void event(char event);

      HAEntity* getEntityState(uint8_t entityNum) const;
      int8_t setEntityState(uint8_t entityNum, HAEntity* entity);
      std::string getRoomName();

    private:
      struct ble_gatt_chr_def characteristicDefinition[12];
      struct ble_gatt_svc_def serviceDefinition[2];

      uint16_t eventHandle {};

      NimbleController& nimble;
      std::string roomName = std::string("Home Assistant");
      uint8_t entityId = 0;
      uint8_t roomId = 0;
      
      
      HAEntity entities[4]; 

    };
  }
}
