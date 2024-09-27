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
  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Home Assistant");
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  
  SwtStatus1 = lv_switch_create(lv_scr_act(), nullptr);
  SwtStatus1->user_data = this;
  lv_obj_set_size(SwtStatus1, 80, 40);
  lv_obj_align(SwtStatus1, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 35);
  LblStatus1 = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(LblStatus1, "---");
  lv_obj_align(LblStatus1, SwtStatus1, LV_ALIGN_OUT_RIGHT_MID, 25, 0); 
  lv_obj_set_event_cb(SwtStatus1, event_handler);

  SwtStatus2 = lv_switch_create(lv_scr_act(), nullptr);
  SwtStatus2->user_data = this;
  lv_obj_set_size(SwtStatus2, 80, 40);
  lv_obj_align(SwtStatus2, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 85);
  LblStatus2 = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_fmt(LblStatus2, "---");
  lv_obj_align(LblStatus2, SwtStatus2, LV_ALIGN_OUT_RIGHT_MID, 25, 0); 
  lv_obj_set_event_cb(SwtStatus2, event_handler);

  SwtStatus3 = lv_switch_create(lv_scr_act(), nullptr);
  SwtStatus3->user_data = this;
  lv_obj_set_size(SwtStatus3, 80, 40);
  lv_obj_align(SwtStatus3, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 135);
  LblStatus3 = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_fmt(LblStatus3, "---");
  lv_obj_align(LblStatus3, SwtStatus3, LV_ALIGN_OUT_RIGHT_MID, 25, 0); 
  lv_obj_set_event_cb(SwtStatus3, event_handler);

  SwtStatus4 = lv_switch_create(lv_scr_act(), nullptr);
  SwtStatus4->user_data = this;
  lv_obj_set_size(SwtStatus4, 80, 40);
  lv_obj_align(SwtStatus4, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 185);
  LblStatus4 = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_fmt(LblStatus4, "---");
  lv_obj_align(LblStatus4, SwtStatus4, LV_ALIGN_OUT_RIGHT_MID, 25, 0); 
  lv_obj_set_event_cb(SwtStatus4, event_handler);

  taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

HomeAssistant::~HomeAssistant() {
  lv_obj_clean(lv_scr_act());
}

void HomeAssistant::OnObjectEvent(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    if (obj == SwtStatus1) {
      lv_label_set_text_fmt(LblStatus1, "Status: %d", lv_switch_get_state(SwtStatus1));
      homeAssistantService.setEntityState(1, lv_switch_get_state(SwtStatus1));
      // homeAssistantService.event((char)lv_switch_get_state(swtStatus));
    } else if (obj == SwtStatus2) {
      lv_label_set_text_fmt(LblStatus2, "Status: %d", lv_switch_get_state(SwtStatus2));
      homeAssistantService.setEntityState(2, lv_switch_get_state(SwtStatus2));
      // homeAssistantService.event((char)lv_switch_get_state(swtStatus));
    } else if (obj == SwtStatus3) {
      lv_label_set_text_fmt(LblStatus3, "Status: %d", lv_switch_get_state(SwtStatus3));
      homeAssistantService.setEntityState(3, lv_switch_get_state(SwtStatus3));
      // homeAssistantService.event((char)lv_switch_get_state(swtStatus));
    } else if (obj == SwtStatus4) {
      lv_label_set_text_fmt(LblStatus4, "Status: %d", lv_switch_get_state(SwtStatus4));
      homeAssistantService.setEntityState(4, lv_switch_get_state(SwtStatus4));
      // homeAssistantService.event((char)lv_switch_get_state(swtStatus));
    }
  }
}

void HomeAssistant::Refresh() {
  if (lv_switch_get_state(SwtStatus1) != homeAssistantService.getEntityState(1)) lv_switch_toggle(SwtStatus1, LV_ANIM_ON);
  lv_label_set_text_fmt(LblStatus1, "Status: %d", lv_switch_get_state(SwtStatus1));

  if (lv_switch_get_state(SwtStatus2) != homeAssistantService.getEntityState(2)) lv_switch_toggle(SwtStatus2, LV_ANIM_ON);
  lv_label_set_text_fmt(LblStatus2, "Status: %d", lv_switch_get_state(SwtStatus2));

  if (lv_switch_get_state(SwtStatus3) != homeAssistantService.getEntityState(3)) lv_switch_toggle(SwtStatus3, LV_ANIM_ON);
  lv_label_set_text_fmt(LblStatus3, "Status: %d", lv_switch_get_state(SwtStatus3));

  if (lv_switch_get_state(SwtStatus4) != homeAssistantService.getEntityState(4)) lv_switch_toggle(SwtStatus4, LV_ANIM_ON);
  lv_label_set_text_fmt(LblStatus4, "Status: %d", lv_switch_get_state(SwtStatus4));
    
}