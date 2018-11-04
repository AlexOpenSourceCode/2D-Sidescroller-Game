#ifndef GroundSpikeScriptH
#define GroundSpikeScriptH


#include "Script.h"

class GroundSpikeScript : public Script {
public:
	GroundSpikeScript();
	GroundSpikeScript(GameObject* go_);

	virtual void update();

	void on_event(const std::string& event_name) override;

};

#endif