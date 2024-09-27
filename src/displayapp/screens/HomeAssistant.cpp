#include "displayapp/screens/HomeAssistant.h"
#include <cstdint>
#include "displayapp/DisplayApp.h"
#include "components/ble/HomeAssistantService.h"

using namespace Pinetime::Applications::Screens;

static void event_handler(lv_obj_t* obj, lv_event_t event) {
  HomeAssistant* screen = static_cast<HomeAssistant*>(obj->user_data);
  screen->OnObjectEvent(obj, event);
}

HomeAssistant::HomeAssistant(Pinetime::Controllers::HomeAssistantService& home) : homeAssistantService(home) {
  lblRoomName = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(lblRoomName, "---");
  lv_obj_align(lblRoomName, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_label_set_align(lblRoomName, LV_LABEL_ALIGN_CENTER);
  
  for (uint8_t idx=0;idx<4;idx++) {
    entitySwitches[idx] = lv_switch_create(lv_scr_act(), nullptr);
    entitySwitches[idx]->user_data = this;
    lv_obj_set_size(entitySwitches[idx], 80, 40);
    lv_obj_align(entitySwitches[idx], lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, (idx*50)+35);
    entityLabels[idx] = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(entityLabels[idx], "---");
    lv_obj_align(entityLabels[idx], entitySwitches[idx], LV_ALIGN_OUT_RIGHT_MID, 25, 0); 
    lv_obj_set_event_cb(entitySwitches[idx], event_handler);  
  }
  taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
  homeAssistantService.event((uint8_t)Pinetime::Controllers::HAEventType::HA_REQ_REFRESH);
}

HomeAssistant::~HomeAssistant() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void HomeAssistant::OnObjectEvent(lv_obj_t* obj, lv_event_t event) {
  Pinetime::Controllers::HAEntity entity;
  if (event == LV_EVENT_CLICKED) {
    for (uint8_t idx=0;idx<sizeof(entitySwitches);idx++) {
      if (obj == entitySwitches[idx]) {
        entity = *homeAssistantService.getEntityState(idx);
        entity.state = (bool)lv_switch_get_state(entitySwitches[idx]);
        //TODO check if set state succesful 
        homeAssistantService.setEntityState(idx, &entity);  
        homeAssistantService.event((((uint8_t)Pinetime::Controllers::HAEventType::HA_ENTITY_CHANGED)<<4)|(idx & 0xF));
      }  
    }
  }
}

void HomeAssistant::Refresh() {
  Pinetime::Controllers::HAEntity entity;

  lv_label_set_text(lblRoomName, homeAssistantService.getRoomName().data());
  lv_obj_align(lblRoomName, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0); // Make sure title text is centered

  for (uint8_t idx=0;idx<sizeof(entitySwitches);idx++) {
    //TODO check if getEntityState returns a valid pointer
    entity = *homeAssistantService.getEntityState(idx);
    if (lv_switch_get_state(entitySwitches[idx]) != entity.state) lv_switch_toggle(entitySwitches[idx], LV_ANIM_ON);
    lv_label_set_text(entityLabels[idx], entity.name.data());
  } 
}