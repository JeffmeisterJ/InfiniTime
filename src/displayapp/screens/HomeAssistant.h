#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Controllers {
    class HomeAssistantService;
  }

  namespace Applications {
    namespace Screens {
      class HomeAssistant : public Screen {
      public:
        HomeAssistant(Pinetime::Controllers::HomeAssistantService& home);
        
        ~HomeAssistant() override;

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);

        void Refresh() override;
      
      private:
        lv_obj_t* lblRoomName;

        lv_obj_t** entitySwitches = new lv_obj_t*[4];
        lv_obj_t** entityLabels = new lv_obj_t*[4];

        Pinetime::Controllers::HomeAssistantService& homeAssistantService;

        lv_task_t* taskRefresh;
      };
    }
    
    template <>
    struct AppTraits<Apps::HomeAssistant> {
      static constexpr Apps app = Apps::HomeAssistant;
      static constexpr const char* icon = Screens::Symbols::home; 
      
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::HomeAssistant(*controllers.homeAssistantService);
      }
    };
  }
}