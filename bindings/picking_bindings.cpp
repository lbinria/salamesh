#include "picking_bindings.h"

namespace bindings {

	void PickingBindings::loadBindings(sol::state &lua, IApp &app) {

		lua.new_enum("PickElement", 
			"PICK_VERTEX", PickElement::PICK_VERTEX,
			"PICK_HALFEDGE", PickElement::PICK_HALFEDGE,
			"PICK_FACET", PickElement::PICK_FACET,
			"PICK_CELL", PickElement::PICK_CELL,
			"PICK_MESH", PickElement::PICK_MESH,
			"PICK_ELEMENT_COUNT", PickElement::PICK_ELEMENT_COUNT
		);

		lua.set_function("pick_element_to_string", &pickElementToString);


		type = lua.new_usertype<PickResult>("PickResult",
			"get", &PickResult::get,
			"exists", &PickResult::exists,
			"ids", sol::readonly_property(&PickResult::getIds),
			"count", sol::readonly_property(&PickResult::count)
		);

		pickStateType = lua.new_usertype<PickState>("PickState",
			"get_result", sol::overload(
				sol::resolve<PickResult&(PickElement)>(&PickState::getResult),
				sol::resolve<PickResult(long, PickElement)>(&PickState::getResult)
			),
			"get_ids", sol::overload(
				sol::resolve<std::vector<long>(PickElement)>(&PickState::getIds),
				sol::resolve<std::vector<long>(long, PickElement)>(&PickState::getIds)
			),
			"any", sol::overload(
				sol::resolve<bool(PickElement)>(&PickState::any),
				sol::resolve<bool(long, PickElement)>(&PickState::any)
			),
			"count", sol::overload(
				sol::resolve<long(PickElement)>(&PickState::count),
				sol::resolve<long(long, PickElement)>(&PickState::count)
			)
		);

	}
}